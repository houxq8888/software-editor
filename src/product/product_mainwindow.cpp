#include "product_mainwindow.h"
#include "ui_product_mainwindow.h"
#include "productconfigmanager.h"
#include "packagemanager.h"
#include "../uilayout/statemachine.h"
#include "../uilayout/statemachineeditor_v2.h"
#include "../uilayout/uiflowstatemachine.h"
#include "../uilayout/logicsequencestatemachine.h"
#include "../uilayout/statemachineintegrationmanager.h"
#include "../uilayout/uiinterface.h"
#include "../uilayout/wizard.h"
#include "../uilayout/uiinterfacemanager.h"

#include "../designer/qdesigner_workbench.h"
#include "../designer/qdesigner_formwindow.h"
#include "../designer/qdesigner.h"
#include "../designer/mainwindow.h"
#include "../designer/qdesigner_settings.h"
#include <QApplication>
#include <QCloseEvent>
#include <QScreen>
#include <QGuiApplication>
#include <QDesignerFormWindowInterface>
#include <QDesignerFormWindowManagerInterface>
#include <QDesignerFormEditorInterface>
#include <QProcess>
#include <QProcessEnvironment>
#include <QFileInfo>

ProductMainWindow::ProductMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ProductMainWindow)
    , m_packageManager(nullptr)
    , m_configManager(nullptr)
    , m_logTimer(nullptr)
    , m_qdesigner_instance(nullptr)
    , m_uiEditorActive(false)
    , m_stateMachineEditor(nullptr)
    , m_stateMachineEditorActive(false)
    , m_productLoaded(false) // 初始时产品未加载
{
    ui->setupUi(this);
    
    // 初始化配置管理器
    m_configManager = new ProductConfigManager(this);
    
    // 初始化产品数据
    m_product = Product();
    
    // 设置窗口标题
    setWindowTitle("软件编辑器 - 产品配置");
    
    // 初始化UI组件
    initUI();
    
    // 连接信号
    connectSignals();
    
    // 加载配置
    loadConfig();
    
    // 程序启动时自动加载默认产品配置
    loadDefaultProductConfig();
}

ProductMainWindow::~ProductMainWindow()
{
    qDebug() << "ProductMainWindow destructor starts";
    
    // 先删除PackageManager对象，确保在UI删除前断开所有信号连接
    qDebug()<<"delete package manager";
    if (m_packageManager) {
        delete m_packageManager;
        m_packageManager = nullptr;
    }
    
    // 清理日志捕获资源
    qDebug() << "cleanup log capture resources";
    cleanupLogCapture();
    
    delete ui;
    qDebug()<<"delete ui";
    
    qDebug() << "ProductMainWindow destructor completes";
}

void ProductMainWindow::initUI()
{
    // 限制窗口大小不超过屏幕分辨率
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->availableGeometry();
    int maxWidth = screenGeometry.width() - 100; // 留出边距
    int maxHeight = screenGeometry.height() - 100;
    
    // 设置窗口最大尺寸
    setMaximumSize(maxWidth, maxHeight);
    
    // 如果当前尺寸超过屏幕，则调整到合适大小
    if (width() > maxWidth || height() > maxHeight) {
        resize(qMin(width(), maxWidth), qMin(height(), maxHeight));
    }
    
    ui->featuresListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->featuresListWidget->setAlternatingRowColors(true);
    
    // 设置日志捕获
    setupLogCapture();
    
    clearProductData();
    
    ui->mainToolBar->setIconSize(QSize(24, 24));
    ui->statusBar->showMessage("就绪");
}

void ProductMainWindow::connectSignals()
{
    // Connect signals
    connect(ui->nameLineEdit, &QLineEdit::textChanged, this, [this]() { setModified(true); });
    connect(ui->versionLineEdit, &QLineEdit::textChanged, this, [this]() { setModified(true); });
    connect(ui->developerLineEdit, &QLineEdit::textChanged, this, [this]() { setModified(true); });
    connect(ui->categoryLineEdit, &QLineEdit::textChanged, this, [this]() { setModified(true); });
    connect(ui->websiteLineEdit, &QLineEdit::textChanged, this, [this]() { setModified(true); });
    connect(ui->iconPathLineEdit, &QLineEdit::textChanged, this, [this]() { setModified(true); });
    connect(ui->screenshotPathLineEdit, &QLineEdit::textChanged, this, [this]() { setModified(true); });
    connect(ui->descriptionTextEdit, &QTextEdit::textChanged, this, [this]() { setModified(true); });
    connect(ui->featureNameLineEdit, &QLineEdit::textChanged, this, [this]() { setModified(true); });
    connect(ui->featureDescriptionTextEdit, &QTextEdit::textChanged, this, [this]() { setModified(true); });
    
    // Connect ProductConfigManager signals
    connect(m_configManager, &ProductConfigManager::productModifiedChanged, this, [this](bool modified) {
        setModified(modified);
    });
    connect(m_configManager, &ProductConfigManager::uiLayoutModifiedChanged, this, [this](bool modified) {
        // UI布局修改状态变化
        updateStatusBar();
    });
    connect(m_configManager, &ProductConfigManager::uiLayoutChanged, this, [this](bool changed) {
        // UI布局改变状态变化
        updateStatusBar();
    });
    connect(m_configManager, &ProductConfigManager::needsSaveChanged, this, [this](bool needsSave) {
        // 需要保存状态变化
        updateStatusBar();
    });
}

void ProductMainWindow::loadConfig()
{
    // 这里可以加载应用程序配置
    // 目前暂时为空实现
}

void ProductMainWindow::on_actionNew_triggered()
{
    if (saveChanges()) {
        clearProductData();
        m_currentFile = "";
        setModified(false);
        setWindowTitle("产品编辑器 - 新建");
    }
}

void ProductMainWindow::on_actionOpen_triggered()
{
    if (saveChanges()) {
        QString fileName = QFileDialog::getOpenFileName(this, "打开产品文件", "", "JSON文件 (*.json)");
        if (!fileName.isEmpty()) {
            QFile file(fileName);
            if (file.open(QIODevice::ReadOnly)) {
                QJsonDocument jsonDoc = QJsonDocument::fromJson(file.readAll());
                file.close();

                if (jsonDoc.isObject()) {
                    Product product;
                    if (product.fromJson(jsonDoc.object(), fileName)) {
                        // 验证产品配置包是否有效
                        if (product.isValidProductConfigPackage()) {
                            // 有效产品配置包，加载产品信息
                            loadProductData(product);
                            m_currentFile = fileName;
                            setModified(false);
                            setWindowTitle(QString("产品编辑器 - %1").arg(QFileInfo(fileName).fileName()));
                        } else {
                            // 无效产品配置包，弹出提示框
                            QMessageBox::warning(this, 
                                "无效的产品配置包", 
                                "当前打开的文件不是一个有效的产品配置包。\n\n"
                                "请确保：\n"
                                "• 产品名称和版本信息完整\n"
                                "• 产品配置文件(product_config.json)存在\n"
                                "• 至少包含一个有效的UI文件\n"
                                "• 主UI文件路径正确",
                                QMessageBox::Ok);
                            
                            // 清空当前产品数据和文件名
                            clearProductData();
                            m_currentFile.clear();
                            setWindowTitle("产品编辑器");
                        }
                    } else {
                        // JSON解析失败
                        QMessageBox::critical(this, 
                            "文件格式错误", 
                            "无法解析产品配置文件，请检查文件格式是否正确。",
                            QMessageBox::Ok);
                        
                        // 清空当前产品数据和文件名
                        clearProductData();
                        m_currentFile.clear();
                        setWindowTitle("产品编辑器");
                    }
                }
            }
        }
    }
}

void ProductMainWindow::on_actionSave_triggered()
{
    if (m_currentFile.isEmpty()) {
        on_actionSave_As_triggered();
    } else {
        saveProductData();
        setModified(false);
    }
}

void ProductMainWindow::on_actionSave_As_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, "保存产品文件", "", "JSON文件 (*.json)");
    if (!fileName.isEmpty()) {
        m_currentFile = fileName;
        saveProductData();
        setModified(false);
        setWindowTitle(QString("产品编辑器 - %1").arg(QFileInfo(fileName).fileName()));
    }
}

void ProductMainWindow::on_saveButton_clicked()
{
    qDebug()<<"on_saveButton_clicked";
    // 检查是否有未保存的修改
    if (m_configManager->isProductModified()) {
        QMessageBox::StandardButton button = QMessageBox::question(this, 
            "保存更改", 
            "产品数据有未保存的更改。是否保存？",
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        
        if (button == QMessageBox::Save) {
            // 保存产品数据
            saveProductData();
        } else if (button == QMessageBox::Cancel) {
            // 取消保存操作
            return;
        }
        // 如果选择Discard，直接继续而不保存
    } else {
        // 没有修改，直接保存
        saveProductData();
    }
}

void ProductMainWindow::on_loadButton_clicked()
{
    on_actionOpen_triggered();
}

void ProductMainWindow::on_actionExit_triggered()
{
    if (saveChanges()) {
        close();
    }
}

void ProductMainWindow::on_iconBrowseButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "选择图标文件", "", "图像文件 (*.png *.jpg *.jpeg *.ico)");
    if (!fileName.isEmpty()) {
        ui->iconPathLineEdit->setText(fileName);
    }
}

void ProductMainWindow::on_screenshotBrowseButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "选择截图文件", "", "图像文件 (*.png *.jpg *.jpeg)");
    if (!fileName.isEmpty()) {
        ui->screenshotPathLineEdit->setText(fileName);
    }
}



void ProductMainWindow::on_addFeatureButton_clicked()
{
    ProductFeature feature = getCurrentFeatureFromEditors();
    if (!feature.name.isEmpty()) {
        // Save current feature if any
        ProductFeature currentFeature = getCurrentFeatureFromEditors();
        if (!currentFeature.name.isEmpty() && ui->featuresListWidget->currentRow() >= 0) {
            int currentRow = ui->featuresListWidget->currentRow();
            QList<ProductFeature> features = m_product.features();
            if (currentRow >= 0 && currentRow < features.size()) {
                features[currentRow] = currentFeature;
                m_product.setFeatures(features);
            }
        }

        // Add new feature
        m_product.addFeature(feature);
        QListWidgetItem *item = new QListWidgetItem(feature.name);
        ui->featuresListWidget->addItem(item);
        ui->featuresListWidget->setCurrentRow(ui->featuresListWidget->count() - 1);
        ui->featureNameLineEdit->clear();
        ui->featureDescriptionTextEdit->clear();
        setModified(true);
    }
}

void ProductMainWindow::on_removeFeatureButton_clicked()
{
    int row = ui->featuresListWidget->currentRow();
    if (row >= 0) {
        m_product.removeFeature(row);
        delete ui->featuresListWidget->takeItem(row);
        setModified(true);
    }
}

void ProductMainWindow::on_featuresListWidget_itemClicked(QListWidgetItem *item)
{
    int row = ui->featuresListWidget->row(item);
    if (row >= 0 && row < m_product.features().size()) {
        const ProductFeature &feature = m_product.features().at(row);
        ui->featureNameLineEdit->setText(feature.name);
        ui->featureDescriptionTextEdit->setPlainText(feature.description);
    }
}

void ProductMainWindow::clearProductData()
{
    ui->nameLineEdit->clear();
    ui->versionLineEdit->clear();
    ui->developerLineEdit->clear();
    ui->categoryLineEdit->clear();
    ui->websiteLineEdit->clear();
    ui->iconPathLineEdit->clear();
    ui->screenshotPathLineEdit->clear();
    ui->descriptionTextEdit->clear();
    ui->featureNameLineEdit->clear();
    ui->featureDescriptionTextEdit->clear();
    ui->featuresListWidget->clear();
    m_product = Product();
    m_productLoaded = false; // 重置产品加载状态
    
    // 重置配置管理器状态
    m_configManager->setProduct(m_product);
    m_configManager->setUiLayoutPath("");
    m_configManager->resetAllModifiedStates();
    
    updateStatusBar();
}

void ProductMainWindow::updateFeatureEditors()
{
    // Implement feature editor updates if needed
}

void ProductMainWindow::loadProductData(const Product &product)
{
    m_product = product;
    m_productLoaded = true; // 标记产品已从文件加载

    ui->nameLineEdit->setText(product.name());
    ui->versionLineEdit->setText(product.version());
    ui->developerLineEdit->setText(product.developer());
    ui->categoryLineEdit->setText(product.category());
    ui->websiteLineEdit->setText(product.website());
    ui->iconPathLineEdit->setText(product.iconPath());
    ui->screenshotPathLineEdit->setText(product.screenshotPath());
    ui->descriptionTextEdit->setText(product.description());

    // 更新配置管理器
    m_configManager->setProduct(product);
    m_configManager->setUiLayoutPath(product.uiLayoutPath());
    m_configManager->resetAllModifiedStates();
    
    updateFeaturesList();
    updateStatusBar();
}

void ProductMainWindow::saveProductData()
{
    // Save current feature if any
    ProductFeature feature = getCurrentFeatureFromEditors();
    if (!feature.name.isEmpty() && ui->featuresListWidget->currentRow() >= 0) {
        int row = ui->featuresListWidget->currentRow();
        if (row >= 0 && row < m_product.features().size()) {
            QList<ProductFeature> features = m_product.features();
            features[row] = feature;
            m_product.setFeatures(features);
        }
    }

    // Update product data from UI
    m_product.setName(ui->nameLineEdit->text());
    m_product.setVersion(ui->versionLineEdit->text());
    m_product.setDeveloper(ui->developerLineEdit->text());
    m_product.setCategory(ui->categoryLineEdit->text());
    m_product.setWebsite(ui->websiteLineEdit->text());
    m_product.setIconPath(ui->iconPathLineEdit->text());
    m_product.setScreenshotPath(ui->screenshotPathLineEdit->text());
    m_product.setDescription(ui->descriptionTextEdit->toPlainText());

    // UI绑定相关的修改只在关闭UI编辑器时询问用户，这里不再检查

    // Write to file
    QFile file(m_currentFile);
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument jsonDoc(m_product.toJson());
        file.write(jsonDoc.toJson(QJsonDocument::Indented));
        file.close();
        
        // 标记产品已保存
        m_configManager->markProductSaved();
    }
}

void ProductMainWindow::setModified(bool modified)
{
    m_configManager->setProductModified(modified);
    
    QString windowTitle = m_currentFile.isEmpty() ? "产品编辑器 - 新建" : QString("产品编辑器 - %1").arg(QFileInfo(m_currentFile).fileName());
    if (modified) {
        windowTitle += " *";
    }
    setWindowTitle(windowTitle);
    
    updateStatusBar();
}

bool ProductMainWindow::saveChanges()
{
    qDebug()<<"saveChanges";
    if (m_configManager->isProductModified()) {
        QMessageBox::StandardButton button = QMessageBox::question(this, "保存更改", "是否保存当前编辑的产品信息？");
        if (button == QMessageBox::Save) {
            on_actionSave_triggered();
            return true;
        } else if (button == QMessageBox::Cancel) {
            return false;
        }
    }
    return true;
}

void ProductMainWindow::updateFeaturesList()
{
    ui->featuresListWidget->clear();

    for (const auto &feature : m_product.features()) {
        QListWidgetItem *item = new QListWidgetItem(feature.name);
        ui->featuresListWidget->addItem(item);
    }
}

void ProductMainWindow::updateStatusBar()
{
    // 调用带日志显示的状态栏更新方法
    updateStatusBarWithLogs();
}

ProductFeature ProductMainWindow::getCurrentFeatureFromEditors() const
{
    ProductFeature feature;
    feature.name = ui->featureNameLineEdit->text();
    feature.description = ui->featureDescriptionTextEdit->toPlainText();
    return feature;
}

void ProductMainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, "关于产品编辑器", "这是一个基于Qt的产品编辑工具，用于创建和管理产品信息。");
}

void ProductMainWindow::on_actionOpen_UI_Layout_Editor_triggered()
{
    // 检查产品基本信息是否完整
    if (m_product.name().isEmpty()) {
        QMessageBox::warning(this, "打开UI布局编辑器", "请先填写产品基本信息。");
        return;
    }
    
    // 如果UI编辑器已经在运行，则直接激活并加载多个UI文件
    if (m_uiEditorActive && m_qdesigner_instance) {
        qDebug()<<"ui editor already active";
        // 获取QDesigner的主窗口并激活
        QMainWindow *mainWindow = qobject_cast<QMainWindow*>(m_qdesigner_instance->mainWindow());
        if (mainWindow) {
            mainWindow->show();
            mainWindow->raise();
            mainWindow->activateWindow();

            // 关键修改：加载多个UI文件，而不仅仅是单个文件
            QList<ProductUIFile> uiFiles = m_product.uiFiles();
            
            if (!uiFiles.isEmpty()) {
                qDebug() << "Loading multiple UI files in active QDesigner:" << uiFiles.size();
                
                // 遍历所有UI文件并加载到QDesigner中
                for (const auto &uiFile : uiFiles) {
                    QString uiPath = uiFile.filePath;
                    
                    // 确保文件存在
                    if (QFile::exists(uiPath)) {
                        qDebug() << "Loading UI file in active QDesigner:" << uiPath;
                        m_qdesigner_instance->workbench()->readInForm(uiPath);
                    } else {
                        qDebug() << "UI file does not exist, skip loading:" << uiPath;
                    }
                }
            } else {
                // 向后兼容：如果没有uiFiles数组，使用单个UI文件路径
                QString uiPath = m_product.uiLayoutPath();
                if (!uiPath.isEmpty() && QFile::exists(uiPath)) {
                    qDebug() << "Loading single UI file in active QDesigner:" << uiPath;
                    m_qdesigner_instance->workbench()->readInForm(uiPath);
                }
            }
            
            return;
        }
    }
    
    // 启动UI编辑器
    startUIEditor();
}

void ProductMainWindow::startUIEditor()
{
    if (m_uiEditorActive) {
        qDebug() << "UI editor already active";
        return;
    }
    
    qDebug() << "start UI editor";
    
    // 创建QDesigner实例
    static int argc = 1;
    static const char *argv[] = {"software-editor", nullptr};
    
    m_qdesigner_instance = new QDesigner(argc, const_cast<char**>(argv));
    
    // 设置启动标记属性，标识这是从产品配置页面启动的
    m_qdesigner_instance->setProperty("launchedFromProductMainWindow", true);

    // 关键修改：支持从JSON加载多个UI文件
    QStringList filesToOpen;
    
    // 只有在QDesigner实例已经创建并初始化后，才能访问其core()方法
    if (m_qdesigner_instance && m_qdesigner_instance->workbench()) {
        QDesignerSettings settings(m_qdesigner_instance->workbench()->core());
        QStringList previouslyOpenedFiles = settings.openFormFiles();
        
        if (!previouslyOpenedFiles.isEmpty()) {
            qDebug() << "Found previously opened UI files:" << previouslyOpenedFiles;
            
            // 清除保存的文件列表，避免重复加载
            settings.setOpenFormFiles(QStringList());
            qDebug() << "Cleared previously opened UI files list";
            
            QDesigner::ParseArgumentsResult result = m_qdesigner_instance->parseCommandLineArguments();
            
            if (result != QDesigner::ParseArgumentsSuccess) {
                qDebug() << "QDesigner command line argument parsing failed";
                delete m_qdesigner_instance;
                m_qdesigner_instance = nullptr;
                return;
            }
        } else {
            // 关键修改：从产品配置的uiFiles数组中加载多个UI文件
            QList<ProductUIFile> uiFiles = m_product.uiFiles();
            
            if (!uiFiles.isEmpty()) {
                qDebug() << "Loading multiple UI files from product configuration:" << uiFiles.size();
                
                // 遍历所有UI文件，确保文件存在，如果不存在则创建空文件
                for (const auto &uiFile : uiFiles) {
                    QString uiPath = uiFile.filePath;
                    
                    // 如果UI文件不存在，创建一个空的UI文件
                    if (!QFile::exists(uiPath)) {
                        qDebug() << "UI file does not exist, create empty file:" << uiPath;
                        QFile file(uiPath);
                        if (file.open(QIODevice::WriteOnly)) {
                            file.write("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
                            file.write("<ui version=\"4.0\">\n");
                            file.write(" <class>Form</class>\n");
                            file.write(" <widget class=\"QWidget\" name=\"Form\">\n");
                            file.write("  <property name=\"geometry\">\n");
                            file.write("   <rect>\n");
                            file.write("    <x>0</x>\n");
                            file.write("    <y>0</y>\n");
                            file.write("    <width>400</width>\n");
                            file.write("    <height>300</height>\n");
                            file.write("   </rect>\n");
                            file.write("  </property>\n");
                            file.write("  <property name=\"windowTitle\">\n");
                            file.write("   <string>Form</string>\n");
                            file.write("  </property>\n");
                            file.write(" </widget>\n");
                            file.write(" <resources/>\n");
                            file.write(" <connections/>\n");
                            file.write("</ui>\n");
                            file.close();
                        }
                    }
                    
                    filesToOpen << uiPath;
                    qDebug() << "Add UI file to open list:" << uiPath << "(Main:" << uiFile.isMain << ")";
                }
            } else {
                // 向后兼容：如果没有uiFiles数组，使用单个UI文件路径
                QString uiPath = m_configManager->getUiLayoutPath();
                if (uiPath.isEmpty()) {
                    // 如果没有UI文件，使用默认路径
                    uiPath = QDir::currentPath() + "/default.ui";
                    qDebug() << "use default UI file path:" << uiPath;
                }
                
                // 如果UI文件不存在，创建一个空的UI文件
                if (!QFile::exists(uiPath)) {
                    qDebug() << "UI file does not exist, create empty file:" << uiPath;
                    QFile file(uiPath);
                    if (file.open(QIODevice::WriteOnly)) {
                        file.write("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
                        file.write("<ui version=\"4.0\">\n");
                        file.write(" <class>Form</class>\n");
                        file.write(" <widget class=\"QWidget\" name=\"Form\">\n");
                        file.write("  <property name=\"geometry\">\n");
                        file.write("   <rect>\n");
                        file.write("    <x>0</x>\n");
                        file.write("    <y>0</y>\n");
                        file.write("    <width>400</width>\n");
                        file.write("    <height>300</height>\n");
                        file.write("   </rect>\n");
                        file.write("  </property>\n");
                        file.write("  <property name=\"windowTitle\">\n");
                        file.write("   <string>Form</string>\n");
                        file.write("  </property>\n");
                        file.write(" </widget>\n");
                        file.write(" <resources/>\n");
                        file.write(" <connections/>\n");
                        file.write("</ui>\n");
                        file.close();
                    }
                }
                
                filesToOpen << uiPath;
            }
            
            qDebug() << "Files to open:" << filesToOpen;
            
            // 关键修复：在解析命令行参数之前设置预定义文件路径
            // 这样在parseCommandLineArguments()中就能识别到有文件要打开，从而抑制NewForm窗口
            qDebug() << "call setPredefinedFiles method, set predefined UI file paths:" << filesToOpen;
            m_qdesigner_instance->setPredefinedFiles(filesToOpen);
            
            // 解析命令行参数
            QDesigner::ParseArgumentsResult result = m_qdesigner_instance->parseCommandLineArguments();
            
            if (result != QDesigner::ParseArgumentsSuccess) {
                qDebug() << "QDesigner command line argument parsing failed";
                delete m_qdesigner_instance;
                m_qdesigner_instance = nullptr;
                return;
            }
            
            // 重要修复：在解析命令行参数后，再次调用setFilesToOpen确保UI文件被正确打开
            // 因为parseCommandLineArguments()会清空m_predefinedFiles
            qDebug() << "call setFilesToOpen method, ensure UI files are opened correctly:" << filesToOpen;
            m_qdesigner_instance->setFilesToOpen(filesToOpen);
        }
    } else {
        // 如果QDesigner实例尚未完全初始化，使用产品配置中的多个UI文件
        QList<ProductUIFile> uiFiles = m_product.uiFiles();
        
        if (!uiFiles.isEmpty()) {
            qDebug() << "Loading multiple UI files from product configuration:" << uiFiles.size();
            
            // 遍历所有UI文件
            for (const auto &uiFile : uiFiles) {
                QString uiPath = uiFile.filePath;
                filesToOpen << uiPath;
                qDebug() << "Add UI file to open list:" << uiPath << "(Main:" << uiFile.isMain << ")";
            }
        } else {
            // 向后兼容：如果没有uiFiles数组，使用单个UI文件路径
            QString uiPath = m_configManager->getUiLayoutPath();
            if (uiPath.isEmpty()) {
                uiPath = QDir::currentPath() + "/default.ui";
            }
            filesToOpen << uiPath;
            qDebug() << "QDesigner not fully initialized, using default UI file:" << uiPath;
        }
        
        qDebug() << "Files to open:" << filesToOpen;
        
        // 关键修复：在解析命令行参数之前设置预定义文件路径
        qDebug() << "call setPredefinedFiles method, set predefined UI file paths:" << filesToOpen;
        m_qdesigner_instance->setPredefinedFiles(filesToOpen);
        
        // 解析命令行参数
        QDesigner::ParseArgumentsResult result = m_qdesigner_instance->parseCommandLineArguments();
        
        if (result != QDesigner::ParseArgumentsSuccess) {
            qDebug() << "QDesigner command line argument parsing failed";
            delete m_qdesigner_instance;
            m_qdesigner_instance = nullptr;
            return;
        }
        
        // 重要修复：在解析命令行参数后，再次调用setFilesToOpen确保UI文件被正确打开
        qDebug() << "call setFilesToOpen method, ensure UI files are opened correctly:" << filesToOpen;
        m_qdesigner_instance->setFilesToOpen(filesToOpen);
    }
    
    // 重要：设置UI编辑器为活动状态
    m_uiEditorActive = true;
    qDebug() << "UI editor start up complete, status set to active";
    
    connect(m_qdesigner_instance->workbench(), &QDesignerWorkbench::closeQDesignerUI, this, &ProductMainWindow::onQDesignerFinished);
    
    // 重要：QDesigner继承自QApplication，Qt框架已经自动启动了事件循环
    // 不需要再次调用exec()，否则会导致"The event loop is already running"错误
    qDebug() << "QDesigner application started successfully, event loop is running";
}

void ProductMainWindow::stopUIEditor()
{
    if (m_uiEditorActive && m_qdesigner_instance) {
        // 重要修复：在QDesigner关闭前断开信号连接
        // 注意：这里可以访问workbench()，因为QDesigner还没有完全关闭
        disconnect(m_qdesigner_instance->workbench(), &QDesignerWorkbench::closeQDesignerUI, this, &ProductMainWindow::onQDesignerFinished);
        
        // 重要：不要调用quit()，因为QDesigner继承自QApplication，调用quit()会导致整个程序退出
        // 只需要断开连接并置空指针，让QDesigner自然关闭
        // m_qdesigner_instance->quit(); // 这行代码会导致整个应用程序退出
        
        // 重要：不要删除QDesigner实例，因为它继承自QApplication
        // 只需要将指针置为nullptr，让QDesigner自然退出
        m_qdesigner_instance = nullptr;
        
        m_uiEditorActive = false;
        qDebug() << "UI editor stopped";
    }
}



void ProductMainWindow::onQDesignerFinished()
{
    qDebug() << "QDesigner finish quit";
    
    // 在QDesigner关闭前保存当前打开的UI页面信息到产品配置
    if (m_qdesigner_instance && m_qdesigner_instance->workbench()) {
        QDesignerWorkbench *workbench = m_qdesigner_instance->workbench();
        
        // 获取当前打开的UI文件列表
        QList<ProductUIFile> currentUiFiles;
        int formWindowCount = workbench->formWindowCount();
        
        qDebug() << "before close QDesigner, current open UI window count:" << formWindowCount;
        
        for (int i = 0; i < formWindowCount; i++) {
            QDesignerFormWindow *formWindow = workbench->formWindow(i);
            if (formWindow && formWindow->editor()) {
                QString fileName = formWindow->editor()->fileName();
                
                // 重要修复：保存所有UI文件，包括untitled.ui文件
                // 如果文件名为空，使用默认名称
                if (fileName.isEmpty()) {
                    fileName = QString("untitled_%1.ui").arg(i + 1);
                }
                
                ProductUIFile uiFile;
                
                // 从文件名提取UI文件名称
                QFileInfo fileInfo(fileName);
                uiFile.name = fileInfo.baseName();
                uiFile.filePath = fileName;
                uiFile.type = "ui_layout";
                
                // 检查是否是主UI文件（基于文件路径匹配）
                // 这里可以根据业务逻辑调整判断条件
                if (fileName.contains("main") || i == 0) {
                    uiFile.isMain = true;
                } else {
                    uiFile.isMain = false;
                }
                
                uiFile.description = QString("UI布局文件: %1").arg(uiFile.name);
                uiFile.order = i;
                
                currentUiFiles.append(uiFile);
                
                qDebug() << "before close QDesigner, save UI file info:" << uiFile.name << "path:" << uiFile.filePath << "is main file:" << uiFile.isMain;
            }
        }
        
        // 重要修改：根据UI页面数量比较来决定是否标记UI布局已修改
        // 获取内存中JSON文件的UI页面数量（在更新之前）
        int originalUiFileCount = m_product.uiFiles().size();
        
        qDebug() << "before close QDesigner, original UI file count in JSON file:" << originalUiFileCount;
        qDebug() << "before close QDesigner, current open UI window count in QDesigner:" << formWindowCount;
        
        // 只有当页面数量不相等时才标记UI布局已修改
        bool hasUiPageChanges = (originalUiFileCount != formWindowCount);
        
        // 更新产品配置中的UI文件列表
        if (!currentUiFiles.isEmpty()) {
            m_product.setUiFiles(currentUiFiles);
            qDebug() << "before close QDesigner, successfully save" << currentUiFiles.size() << "UI files to product config";
        } else {
            // 如果没有打开的UI文件，清空UI文件列表
            m_product.setUiFiles(QList<ProductUIFile>());
            qDebug() << "before close QDesigner, no open UI file, clear UI file list in product config";
        }
        
        if (hasUiPageChanges) {
            m_configManager->setUiLayoutModified(true);
            qDebug() << "before close QDesigner, UI layout marked as modified (UI page count changed: " 
                     << originalUiFileCount << "->" << formWindowCount << ")";
        } else {
            qDebug() << "before close QDesigner, UI page count not changed, no need to mark UI layout modified";
        }
        
        // 额外检查：如果页面数量相等，但存在布局修改，也标记为已修改
        bool hasLayoutChanges = false;
        if (!hasUiPageChanges) {
            for (int i = 0; i < formWindowCount; i++) {
                QDesignerFormWindow *formWindow = workbench->formWindow(i);
                if (formWindow && formWindow->editor() && formWindow->editor()->isDirty()) {
                    hasLayoutChanges = true;
                    qDebug() << "before close QDesigner, detect UI layout change, window" << i << "has unsaved changes";
                    
                    // 重要修复：实际保存UI文件内容到磁盘
                    QString fileName = formWindow->editor()->fileName();
                    if (fileName.isEmpty()) {
                        fileName = QString("untitled_%1.ui").arg(i + 1);
                    }
                    
                    // 确保文件路径是绝对路径
                    QFileInfo fileInfo(fileName);
                    if (!fileInfo.isAbsolute()) {
                        // 使用产品配置的基础路径（从产品UI布局路径推断）
                        QString uiLayoutPath = m_product.uiLayoutPath();
                        if (!uiLayoutPath.isEmpty()) {
                            QFileInfo layoutInfo(uiLayoutPath);
                            QString configDir = layoutInfo.absolutePath();
                            fileName = configDir + "/ui_layouts/" + fileInfo.fileName();
                        } else {
                            // 如果UI布局路径为空，使用当前工作目录
                            fileName = QDir::currentPath() + "/ui_layouts/" + fileInfo.fileName();
                        }
                    }
                    
                    // 创建目录（如果不存在）
                    QDir dir = QFileInfo(fileName).absoluteDir();
                    if (!dir.exists()) {
                        dir.mkpath(".");
                    }
                    
                    // 保存UI文件内容到磁盘
                    if (workbench->writeOutForm(formWindow->editor(), fileName)) {
                        qDebug() << "before close QDesigner, successfully saved UI file:" << fileName;
                        
                        // 更新UI文件信息中的路径
                        for (ProductUIFile &uiFile : currentUiFiles) {
                            if (uiFile.order == i) {
                                uiFile.filePath = fileName;
                                break;
                            }
                        }
                    } else {
                        qDebug() << "before close QDesigner, failed to save UI file:" << fileName;
                    }
                    
                    break;
                }
            }
            
            if (hasLayoutChanges) {
                m_configManager->setUiLayoutModified(true);
                qDebug() << "before close QDesigner, UI layout marked as modified (QDesigner has actual layout changes)";
            }
        }
    } else {
        qDebug() << "before close QDesigner, QDesigner instance or workbench not available, cannot save UI file info";
        
        // 清空现有的UI文件列表
        m_product.setUiFiles(QList<ProductUIFile>());
        qDebug() << "before close QDesigner, clear UI file list in product config";
    }

    qDebug() << "before close QDesigner, return to product config page, UI layout path saved";
}

void ProductMainWindow::on_actionSmart_Package_Software_triggered()
{
    // 检查是否有有效的产品数据
    if (m_product.name().isEmpty()) {
        QMessageBox::warning(this, "智能打包软件", "请先创建或加载一个产品项目。");
        return;
    }

    // 保存当前编辑的数据 - 只有当产品名称不为空且确实有修改时才提示保存
    if (!m_product.name().isEmpty() && m_configManager->isProductModified()) {
        qDebug()<<"product name:"<<m_product.name();
        QMessageBox::StandardButton button = QMessageBox::question(this, "保存更改", "打包前需要保存当前的产品信息。是否保存？");
        if (button == QMessageBox::Save) {
            on_actionSave_triggered();
        } else if (button == QMessageBox::Cancel) {
            return;
        }
    }

    // 创建PackageManager对话框
    if (!m_packageManager) {
        m_packageManager = new PackageManager(this);
        // 连接PackageManager的信号
        connect(m_packageManager, &PackageManager::progressChanged, this, &ProductMainWindow::onPackageProgress);
        connect(m_packageManager, &PackageManager::packageFinished, this, &ProductMainWindow::onPackageFinished);
        connect(m_packageManager, &PackageManager::errorOccurred, this, &ProductMainWindow::onPackageError);
    }

    // 创建默认的PackageConfig
    PackageConfig config;
    config.name = m_product.name();
    config.version = m_product.version();
    config.developer = m_product.developer();
    config.uniqueId = m_product.uniqueId();
    config.outputDir = QDir::currentPath() + "/packages/" + m_product.uniqueId();
    config.uiLayoutPath = m_product.uiLayoutPath();
    config.createInstaller = true;
    
    // 设置配置到PackageManager，但不立即开始打包
    // 打包过程将在用户点击"开始打包"按钮时启动
    m_packageManager->setConfiguration(m_product, config);
    
    // 显示PackageManager对话框
    m_packageManager->show();
}





void ProductMainWindow::onPackageProgress(int progress, const QString &message)
{
    ui->statusBar->showMessage(message);
}

void ProductMainWindow::onPackageFinished(bool success, const QString &message)
{
    ui->statusBar->showMessage(message);
    
    if (success) {
        QMessageBox::information(this, "打包完成", message);
    } else {
        QMessageBox::warning(this, "打包失败", message);
    }
}

void ProductMainWindow::onPackageError(const QString &error)
{
    ui->statusBar->showMessage("打包错误: " + error);
    QMessageBox::critical(this, "打包错误", error);
}

// 检查基本信息TAB页是否被修改
bool ProductMainWindow::isBasicInfoModified() const
{
    // 通过ProductConfigManager的统一接口来判断产品基本信息是否被修改
    if (!m_configManager) {
        return false;
    }
    
    // 使用ProductConfigManager的isProductModified()方法，该方法内部通过ProductState的正副本比较实现
    qDebug()<<"isBasicInfoModified";
    return m_configManager->isProductModified();
}

// 检查功能特性TAB页是否被修改
bool ProductMainWindow::isFeaturesTabModified() const
{
    // 通过ProductConfigManager的统一接口来判断产品功能特性是否被修改
    if (!m_configManager) {
        return false;
    }
    
    // 直接检查功能特性是否被修改，而不是整个产品的修改状态
    // 这样可以避免UI布局路径改变时误报功能特性修改
    return m_configManager->isFeaturesModified();
}

// 处理主窗口关闭事件
void ProductMainWindow::closeEvent(QCloseEvent *event)
{
    // 检查是否有未保存的修改，正确区分不同类型的修改
    bool hasUnsavedChanges = false;
    QString message = "检测到未保存的更改：\n";
    
    // 检查产品基本信息修改（只检查基本信息TAB页）
    bool basicInfoModified = isBasicInfoModified();
    if (basicInfoModified) {
        hasUnsavedChanges = true;
        message += "• 产品基本信息已修改\n";
    }
    
    // 检查产品功能特性修改（只检查功能特性TAB页）
    bool featuresTabModified = isFeaturesTabModified();
    if (featuresTabModified) {
        hasUnsavedChanges = true;
        message += "• 产品功能特性已修改\n";
    }
    
    // 检查UI布局修改（只检查QDesigner中的布局修改）
    bool uiLayoutModified = m_configManager ? m_configManager->isUiLayoutModified() : false;
    if (uiLayoutModified) {
        hasUnsavedChanges = true;
        message += "• UI布局已修改\n";
    }
    
    qDebug()<<"close event check - product basic info modified:"<<basicInfoModified
            <<", product features tab modified:"<<featuresTabModified
            <<", UI layout modified:"<<uiLayoutModified;
    
    if (hasUnsavedChanges) {
        message += "\n是否保存这些更改？";
        
        QMessageBox::StandardButton button = QMessageBox::question(this, 
            "保存更改", 
            message,
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        
        if (button == QMessageBox::Save) {
            // 保存产品基本信息（如果基本信息TAB页已修改）
            if (basicInfoModified) {
                saveProductData();
            }
            
            // 保存产品功能特性修改、UI布局修改和UI绑定修改
            if (m_configManager) {
                // 保存产品功能特性修改
                if (featuresTabModified) {
                    // 这里可以添加保存产品功能特性的逻辑
                    qDebug() << "product features tab modified saved";
                }
                
                // 保存UI布局修改（QDesigner中的布局改变）
                if (uiLayoutModified) {
                    // UI布局修改需要保存产品数据，因为UI文件列表已经更新
                    saveProductData();
                    // 重置UI布局修改状态
                    m_configManager->setUiLayoutModified(false);
                    qDebug() << "UI layout modified saved to JSON file";
                }
            }
            
            event->accept(); // 接受关闭事件
        } else if (button == QMessageBox::Discard) {
            // 放弃更改，直接关闭
            event->accept(); // 接受关闭事件
        } else {
            // 取消关闭
            event->ignore(); // 忽略关闭事件
            return;
        }
    } else {
        // 没有修改，直接关闭
        qDebug()<<"event accept";
        event->accept(); // 接受关闭事件
    }
    
    // 确保在窗口关闭时清理资源
    qDebug()<<"cleanup log capture";
    cleanupLogCapture();
    
    qDebug()<<"closeEvent function over, ready to quit app";
    
    // 检查QApplication实例是否存在
    if (qApp) {
        qDebug()<<"QApplication instance has, ready to quit";
        // 显式退出应用程序，确保所有资源被正确释放
        qDebug()<<"explicitly quit app";
        QApplication::quit();
        qDebug()<<"QApplication::quit() called";
    } else {
        qDebug()<<"error: QApplication instance not found";
    }
    
    qDebug()<<"closeEvent function over";
}

// 日志捕获相关函数实现
void ProductMainWindow::setupLogCapture()
{
    // 安装自定义消息处理器来捕获qDebug等日志输出
    qInstallMessageHandler(customMessageHandler);
    
    // 创建定时器用于定期更新状态栏中的日志显示
    m_logTimer = new QTimer(this);
    connect(m_logTimer, &QTimer::timeout, this, &ProductMainWindow::updateStatusBarWithLogs);
    m_logTimer->start(1000); // 每秒更新一次
    
    // 连接Qt日志信号到自定义槽函数
    connect(qApp, &QApplication::aboutToQuit, this, &ProductMainWindow::cleanupLogCapture);
}

void ProductMainWindow::addLogMessage(const QString &message)
{
    // 添加时间戳
    QString timestamp = QDateTime::currentDateTime().toString("[hh:mm:ss]");
    QString logMessage = timestamp + " " + message;
    
    // 添加到日志列表
    m_logMessages.append(logMessage);
    
    // 限制日志数量
    if (m_logMessages.size() > MAX_LOG_MESSAGES) {
        m_logMessages.removeFirst();
    }
}

void ProductMainWindow::updateStatusBarWithLogs()
{
    if (m_logMessages.isEmpty()) {
        // 如果没有日志，显示默认状态
        QString status = m_configManager->getStatusDescription();
        
        // 只有在产品真正被加载时才显示产品ID
        if (m_productLoaded && !m_product.uniqueId().isEmpty()) {
            status += QString(" | 产品ID: %1").arg(m_product.uniqueId());
        }
        
        ui->statusBar->showMessage(status);
        return;
    }
    
    // 获取最新的日志消息
    QString latestLog = m_logMessages.last();
    
    // 构建状态栏显示文本
    QString status = m_configManager->getStatusDescription();
    
    // 只有在产品真正被加载时才显示产品ID
    if (m_productLoaded && !m_product.uniqueId().isEmpty()) {
        status += QString(" | 产品ID: %1").arg(m_product.uniqueId());
    }
    
    // 添加日志显示
    status += " | 日志: " + latestLog;
    
    ui->statusBar->showMessage(status);
}

void ProductMainWindow::cleanupLogCapture()
{
    // 清理日志捕获资源
    if (m_logTimer) {
        qDebug()<<"cleanup log timer";
        m_logTimer->stop();
        delete m_logTimer;
        m_logTimer = nullptr;
    }
    
    // 恢复默认的消息处理器
    qDebug()<<"restore default message handler";
    qInstallMessageHandler(nullptr);
}

// 静态消息处理器函数
void ProductMainWindow::customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    // 根据消息类型添加前缀
    QString level;
    switch (type) {
    case QtDebugMsg:
        level = "[DEBUG]";
        break;
    case QtInfoMsg:
        level = "[INFO]";
        break;
    case QtWarningMsg:
        level = "[WARNING]";
        break;
    case QtCriticalMsg:
        level = "[CRITICAL]";
        break;
    case QtFatalMsg:
        level = "[FATAL]";
        break;
    default:
        level = "[UNKNOWN]";
        break;
    }
    
    // 构建完整的日志消息（包含时间戳用于控制台输出）
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    QString formattedMessage = QString("%1 %2 %3").arg(timestamp, level, msg);
    
    // 输出到标准输出（保持原有行为）
    QByteArray localMsg = formattedMessage.toLocal8Bit();
    fprintf(stdout, "%s\n", localMsg.constData());
    fflush(stdout);
    
    // 查找主窗口实例并转发日志
    QWidgetList topLevelWidgets = QApplication::topLevelWidgets();
    for (QWidget *widget : topLevelWidgets) {
        ProductMainWindow *mainWindow = qobject_cast<ProductMainWindow*>(widget);
        if (mainWindow) {
            // 转发到主窗口的状态栏显示（不包含时间戳，因为状态栏已有时间戳）
            QString statusBarMessage = QString("%1 %2").arg(level, msg);
            mainWindow->addLogMessage(statusBarMessage);
            break;
        }
    }
}

// 状态机编辑器相关函数实现
void ProductMainWindow::on_actionOpen_State_Machine_Editor_triggered()
{
    qDebug() << "open state machine editor menu triggered";
    
    // 检查产品是否已加载
    if (m_product.name().isEmpty()) {
        QMessageBox::warning(this, "open state machine editor", "please create or load a product project first.");
        return;
    }
    
    startStateMachineEditor();
}

void ProductMainWindow::startStateMachineEditor()
{
    qDebug() << "Starting state machine editor";
    
    // 如果状态机编辑器已经打开，则将其置顶并重新传递UI文件信息
    if (m_stateMachineEditorActive && m_stateMachineEditor) {
        qDebug() << "[DEBUG] State machine editor already exists, re-passing UI files";
        
        // 重新传递UI文件信息
        passUiFilesToStateMachineEditor();
        
        m_stateMachineEditor->raise();
        m_stateMachineEditor->activateWindow();
        qDebug() << "State machine editor raised and activated";
        return;
    }
    
    // 创建新的状态机编辑器实例
    createStateMachineEditor();
    qDebug() << "[DEBUG] State machine editor created";
    m_stateMachineEditor->printUiInterfaceComboCount();

    // 传递产品对象到状态机编辑器
    m_stateMachineEditor->setProduct(&m_product);
    qDebug() << "[DEBUG] Product passed to state machine editor";
    m_stateMachineEditor->printUiInterfaceComboCount();

    // 传递UI文件信息并设置UI流状态
    passUiFilesToStateMachineEditor();
    qDebug() << "[DEBUG] UI files passed to state machine editor";
    m_stateMachineEditor->printUiInterfaceComboCount();

    // 处理状态机文件
    QString stateMachinePath = handleStateMachineFile();
    qDebug() << "[DEBUG] State machine file path:" << stateMachinePath;
    m_stateMachineEditor->printUiInterfaceComboCount();

    // 设置窗口属性并显示
    setupStateMachineEditorWindow();
    qDebug()<<"[DEBUG] before pass ui files to state machine editor";
    m_stateMachineEditor->printUiInterfaceComboCount();
    
    qDebug() << "State machine editor V2 started with decoupled architecture";
    m_stateMachineEditor->printUiInterfaceComboCount();
}

// 辅助函数：传递UI文件信息到状态机编辑器
void ProductMainWindow::passUiFilesToStateMachineEditor()
{
    QList<ProductUIFile> uiFiles = m_product.uiFiles();
    qDebug() << "[DEBUG] Product UI files count:" << uiFiles.size();
    
    if (!uiFiles.isEmpty()) {
        qDebug() << "[DEBUG] Before calling setProductUiFiles, m_stateMachineEditor pointer:" << m_stateMachineEditor;
        if (m_stateMachineEditor) {
            qDebug() << "[DEBUG] State machine editor is valid, calling setProductUiFiles with" << uiFiles.size() << "UI files";
            m_stateMachineEditor->setProductUiFiles(uiFiles);
            qDebug() << "[DEBUG] State machine editor received" << uiFiles.size() << "product UI file information";
        } else {
            qDebug() << "[ERROR] State machine editor is null, cannot set UI files";
        }
        
        // 如果是新创建的状态机编辑器，还需要设置UI流状态
        if (!m_stateMachineEditorActive) {
            // 在createStateMachineEditor()中已经创建了状态机实例，这里直接使用
            // 状态机编辑器内部会处理UI流状态的设置
            qDebug() << "[DEBUG] UI flow states will be handled by state machine editor internally";
        }
    } else {
        qDebug() << "[DEBUG] No UI files found in product configuration";
    }
}

// 辅助函数：创建状态机编辑器实例
void ProductMainWindow::createStateMachineEditor()
{
    // 创建新的状态机编辑器V2实例（独立窗口，不设置父窗口）
    m_stateMachineEditor = new StateMachineEditorV2(nullptr);
    
    // 创建并设置状态机管理器
    StateMachineManager *stateMachineManager = new StateMachineManager(m_stateMachineEditor);
    
    // 创建UI接口管理器
    UIInterfaceManager *uiInterfaceManager = new UIInterfaceManager(m_stateMachineEditor);
    
    // 创建向导管理器并设置到状态机管理器中
    WizardManager *wizardManager = new WizardManager(m_stateMachineEditor);
    stateMachineManager->setWizardManager(wizardManager);
    stateMachineManager->setUiInterfaceManager(uiInterfaceManager);
    
    // 创建解耦的状态机实例
    UIFlowStateMachine *uiFlowStateMachine = new UIFlowStateMachine("UIFlowStateMachine", m_stateMachineEditor);
    LogicSequenceStateMachine *logicStateMachine = new LogicSequenceStateMachine("LogicSequenceStateMachine", m_stateMachineEditor);
    StateMachineIntegrationManager *integrationManager = new StateMachineIntegrationManager(m_stateMachineEditor);
    
    // 设置集成管理器
    integrationManager->setUiFlowStateMachine(uiFlowStateMachine);
    integrationManager->setLogicSequenceStateMachine(logicStateMachine);
    
    // 设置编辑器状态机管理器
    m_stateMachineEditor->setStateMachineManager(stateMachineManager);
    
    // 连接关闭信号
    connect(m_stateMachineEditor, &StateMachineEditorV2::destroyed, this, &ProductMainWindow::onStateMachineEditorClosed);
}

// 辅助函数：处理状态机文件
QString ProductMainWindow::handleStateMachineFile()
{
    QString productConfigDir = m_product.configPackageRootPath();
    QString stateMachinePath;
    
    qDebug() << "Product config package root path:" << productConfigDir;
    qDebug() << "Current working directory:" << QDir::currentPath();
    
    // 确定状态机文件路径
    if (!productConfigDir.isEmpty()) {
        // 使用产品配置目录
        QDir stateMachineDir(productConfigDir + "/state_machines");
        if (!stateMachineDir.exists()) {
            stateMachineDir.mkpath(".");
        }
        stateMachinePath = stateMachineDir.absolutePath() + "/state_machine.json";
        qDebug() << "State machine file path (product config):" << stateMachinePath;
    } else {
        // 使用默认目录
        qWarning() << "无法确定产品配置目录，使用默认状态机路径";
        QDir stateMachineDir(QDir::currentPath() + "/state_machines");
        if (!stateMachineDir.exists()) {
            stateMachineDir.mkpath(".");
        }
        stateMachinePath = stateMachineDir.absolutePath() + "/state_machine.json";
        qDebug() << "State machine file path (default):" << stateMachinePath;
    }
    
    // 处理状态机文件
    QFileInfo fileInfo(stateMachinePath);
    
    if (fileInfo.exists() && fileInfo.isFile()) {
        // 文件存在，尝试加载状态机配置
        if (loadDecoupledStateMachine(stateMachinePath, nullptr, nullptr, nullptr)) {
            qDebug() << "Loaded decoupled state machine file:" << stateMachinePath;
        } else {
            // 如果加载失败，创建新的状态机文件
            createNewDecoupledStateMachine(stateMachinePath, nullptr, nullptr, nullptr);
            qDebug() << "Created new decoupled state machine file (replaced old format):" << stateMachinePath;
        }
    } else {
        // 文件不存在，创建新的状态机文件
        createNewDecoupledStateMachine(stateMachinePath, nullptr, nullptr, nullptr);
        qDebug() << "Created new decoupled state machine file:" << stateMachinePath;
    }
    
    // 更新产品配置中的状态机路径
    m_product.setStateMachinePath(stateMachinePath);
    
    return stateMachinePath;
}

// 辅助函数：设置状态机编辑器窗口属性
void ProductMainWindow::setupStateMachineEditorWindow()
{
    // 设置窗口标题
    QString windowTitle = "状态机编辑器V2 - " + (m_product.name().isEmpty() ? "未命名产品" : m_product.name());
    m_stateMachineEditor->setWindowTitle(windowTitle);
    m_stateMachineEditor->setAttribute(Qt::WA_DeleteOnClose);
    
    // 设置窗口大小和位置
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->availableGeometry();
    
    // 设置窗口大小为屏幕的70%
    int width = screenGeometry.width() * 0.7;
    int height = screenGeometry.height() * 0.7;
    m_stateMachineEditor->resize(width, height);
    
    // 设置窗口位置为屏幕中央
    int x = screenGeometry.x() + (screenGeometry.width() - width) / 2;
    int y = screenGeometry.y() + (screenGeometry.height() - height) / 2;
    m_stateMachineEditor->move(x, y);
    
    // 显示状态机编辑器
    m_stateMachineEditor->show();
    m_stateMachineEditorActive = true;
}

void ProductMainWindow::stopStateMachineEditor()
{
    if (m_stateMachineEditorActive && m_stateMachineEditor) {
        // 断开信号连接
        disconnect(m_stateMachineEditor, &StateMachineEditorV2::destroyed, this, &ProductMainWindow::onStateMachineEditorClosed);
        
        // 关闭状态机编辑器
        m_stateMachineEditor->close();
        m_stateMachineEditor = nullptr;
        m_stateMachineEditorActive = false;
        
        qDebug() << "state machine editor stopped";
    }
}

void ProductMainWindow::onStateMachineEditorClosed()
{
    qDebug() << "state machine editor closed";
    
    // 重置状态机编辑器相关状态
    m_stateMachineEditor = nullptr;
    m_stateMachineEditorActive = false;
    
    // 更新状态栏
    updateStatusBar();
}

// 根据产品UI文件自动创建UI流状态机状态
void ProductMainWindow::setupUIFlowStatesFromProduct(UIFlowStateMachine *uiFlowStateMachine, const QList<ProductUIFile> &uiFiles)
{
    if (!uiFlowStateMachine || uiFiles.isEmpty()) {
        return;
    }
    
    qDebug() << "Starting to create UI flow state machine states from product UI files";
    
    // 清空现有状态 - 通过移除所有状态和转换
    QList<UIFlowStateMachine::UIFlowState> states = uiFlowStateMachine->states();
    for (const auto &state : states) {
        uiFlowStateMachine->removeState(state.stateId);
    }
    
    QList<UIFlowStateMachine::UIFlowTransition> transitions = uiFlowStateMachine->transitions();
    for (const auto &transition : transitions) {
        uiFlowStateMachine->removeTransition(transition.transitionId);
    }
    
    // 为每个UI文件创建状态
    for (int i = 0; i < uiFiles.size(); i++) {
        const ProductUIFile &uiFile = uiFiles[i];
        UIFlowStateMachine::UIFlowState state;
        state.stateId = QString("ui_state_%1").arg(QString::number(qHash(uiFile.filePath)));
        state.name = uiFile.name;
        state.uiInterfaceId = uiFile.filePath;
        
        // 设置初始状态（第一个文件）
        if (i == 0) {
            state.isInitialState = true;
        }
        
        // 设置最终状态（最后一个文件）
        if (i == uiFiles.size() - 1) {
            state.isFinalState = true;
        }
        
        uiFlowStateMachine->addState(state);
        qDebug() << "Creating UI flow state:" << state.name;
    }
    
    // 创建状态之间的转换
    for (int i = 0; i < uiFiles.size() - 1; i++) {
        UIFlowStateMachine::UIFlowTransition transition;
        transition.transitionId = QString("trans_%1_to_%2").arg(i).arg(i+1);
        transition.fromStateId = QString("ui_state_%1").arg(QString::number(qHash(uiFiles[i].filePath)));
        transition.toStateId = QString("ui_state_%1").arg(QString::number(qHash(uiFiles[i+1].filePath)));
        transition.eventSource = "next_button";
        transition.eventType = "click";
        
        uiFlowStateMachine->addTransition(transition);
        qDebug() << "Creating UI flow transition:" << transition.transitionId;
    }
    
    qDebug() << "UI flow state machine states creation completed, total" << uiFiles.size() << "states";
}

// 加载状态机配置
bool ProductMainWindow::loadDecoupledStateMachine(const QString &filePath, 
                                                  UIFlowStateMachine *uiFlowStateMachine,
                                                  LogicSequenceStateMachine *logicStateMachine,
                                                  StateMachineIntegrationManager *integrationManager)
{
    if (!uiFlowStateMachine || !logicStateMachine || !integrationManager) {
        qWarning() << "无法加载状态机：状态机实例为空";
        return false;
    }
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "无法打开状态机文件:" << filePath;
        return false;
    }
    
    QJsonDocument jsonDoc = QJsonDocument::fromJson(file.readAll());
    file.close();
    
    if (!jsonDoc.isObject()) {
        qWarning() << "状态机文件格式无效:" << filePath;
        return false;
    }
    
    QJsonObject rootObj = jsonDoc.object();
    
    // 检查是否为状态机格式
    if (rootObj.contains("uiFlowStateMachine") && 
        rootObj.contains("logicSequenceStateMachine") &&
        rootObj.contains("integrationManager")) {
        
        // 加载UI流状态机
        QJsonObject uiFlowObj = rootObj["uiFlowStateMachine"].toObject();
        if (!uiFlowStateMachine->fromJson(uiFlowObj)) {
            qWarning() << "加载UI流状态机失败";
            return false;
        }
        
        // 加载逻辑时序状态机
        QJsonObject logicObj = rootObj["logicSequenceStateMachine"].toObject();
        if (!logicStateMachine->fromJson(logicObj)) {
            qWarning() << "加载逻辑时序状态机失败";
            return false;
        }
        
        // 加载集成管理器
        QJsonObject integrationObj = rootObj["integrationManager"].toObject();
        if (!integrationManager->fromJson(integrationObj)) {
            qWarning() << "加载集成管理器失败";
            return false;
        }
        
        qDebug() << "Successfully loaded decoupled state machine configuration";
        return true;
    }
    
    qDebug() << "State machine file is not in decoupled format, only decoupled format is supported";
    return false;
}

// 创建新的状态机
void ProductMainWindow::createNewDecoupledStateMachine(const QString &filePath,
                                                       UIFlowStateMachine *uiFlowStateMachine,
                                                       LogicSequenceStateMachine *logicStateMachine,
                                                       StateMachineIntegrationManager *integrationManager)
{
    if (!uiFlowStateMachine || !logicStateMachine || !integrationManager) {
        qWarning() << "无法创建状态机：状态机实例为空";
        return;
    }
    
    // 创建默认的UI流状态机状态
    UIFlowStateMachine::UIFlowState defaultState;
    defaultState.stateId = "default_ui_state";
    defaultState.name = "默认界面";
    defaultState.uiInterfaceId = "main_window";
    defaultState.isInitialState = true;
    defaultState.isFinalState = true;
    
    uiFlowStateMachine->addState(defaultState);
    
    // 创建默认的逻辑时序状态机状态
    LogicSequenceStateMachine::LogicState logicState;
    logicState.stateId = "default_logic_state";
    logicState.name = "默认逻辑状态";
    logicState.description = "应用程序默认逻辑状态";
    logicState.isInitialState = true;
    logicState.isFinalState = true;
    
    logicStateMachine->addState(logicState);
    
    // 创建默认的状态映射
    StateMachineIntegrationManager::StateMachineMapping mapping;
    mapping.uiFlowStateId = "default_ui_state";
    mapping.logicStateId = "default_logic_state";
    mapping.mappingId = "default_mapping";
    
    integrationManager->addMapping(mapping);
    
    // 保存到文件
    saveDecoupledStateMachine(filePath, uiFlowStateMachine, logicStateMachine, integrationManager);
}

// 保存状态机配置
bool ProductMainWindow::saveDecoupledStateMachine(const QString &filePath,
                                                  UIFlowStateMachine *uiFlowStateMachine,
                                                  LogicSequenceStateMachine *logicStateMachine,
                                                  StateMachineIntegrationManager *integrationManager)
{
    if (!uiFlowStateMachine || !logicStateMachine || !integrationManager) {
        qWarning() << "无法保存状态机：状态机实例为空";
        return false;
    }
    
    QJsonObject rootObj;
    
    // 添加UI流状态机配置
    rootObj["uiFlowStateMachine"] = uiFlowStateMachine->toJson();
    
    // 添加逻辑时序状态机配置
    rootObj["logicSequenceStateMachine"] = logicStateMachine->toJson();
    
    // 添加集成管理器配置
    rootObj["integrationManager"] = integrationManager->toJson();
    
    // 添加元数据
    rootObj["version"] = "2.0";
    rootObj["created"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    rootObj["type"] = "decoupled_state_machine";
    
    QJsonDocument jsonDoc(rootObj);
    
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "无法创建状态机文件:" << filePath;
        return false;
    }
    
    file.write(jsonDoc.toJson(QJsonDocument::Indented));
    file.close();
    
    return true;
}

// UI文件类型识别函数
QString ProductMainWindow::detectUiFileType(const QString &filePath) const
{
    QFileInfo fileInfo(filePath);
    QString fileName = fileInfo.fileName().toLower();
    QString baseName = fileInfo.baseName().toLower();
    
    // 基于文件名关键字识别UI类型
    if (fileName.contains("main") || fileName.contains("主窗口") || 
        fileName.contains("primary") || baseName.contains("main") ||
        baseName.contains("主窗口") || baseName.contains("primary")) {
        return "main_window";
    }
    else if (fileName.contains("dialog") || fileName.contains("对话框") || 
             baseName.contains("dialog") || baseName.contains("对话框")) {
        return "dialog";
    }
    else if (fileName.contains("widget") || fileName.contains("控件") || 
             baseName.contains("widget") || baseName.contains("控件")) {
        return "widget";
    }
    else if (fileName.contains("settings") || fileName.contains("设置") || 
             baseName.contains("settings") || baseName.contains("设置")) {
        return "settings";
    }
    else if (fileName.contains("config") || fileName.contains("配置") || 
             baseName.contains("config") || baseName.contains("配置")) {
        return "config";
    }
    else if (fileName.contains("about") || fileName.contains("关于") || 
             baseName.contains("about") || baseName.contains("关于")) {
        return "about";
    }
    else if (fileName.contains("login") || fileName.contains("登录") || 
             baseName.contains("login") || baseName.contains("登录")) {
        return "login";
    }
    else if (fileName.contains("register") || fileName.contains("注册") || 
             baseName.contains("register") || baseName.contains("注册")) {
        return "register";
    }
    else if (fileName.contains("wizard") || fileName.contains("向导") || 
             baseName.contains("wizard") || baseName.contains("向导")) {
        return "wizard";
    }
    else if (fileName.contains("toolbar") || fileName.contains("工具栏") || 
             baseName.contains("toolbar") || baseName.contains("工具栏")) {
        return "toolbar";
    }
    else if (fileName.contains("statusbar") || fileName.contains("状态栏") || 
             baseName.contains("statusbar") || baseName.contains("状态栏")) {
        return "statusbar";
    }
    else if (fileName.contains("menu") || fileName.contains("菜单") || 
             baseName.contains("menu") || baseName.contains("菜单")) {
        return "menu";
    }
    else {
        // 默认类型
        return "custom";
    }
}

// UI文件描述生成函数
QString ProductMainWindow::generateUiFileDescription(const QString &filePath, const QString &type) const
{
    QFileInfo fileInfo(filePath);
    QString baseName = fileInfo.baseName();
    
    // 基于类型生成描述
    if (type == "main_window") {
        return QString("主窗口界面 - %1").arg(baseName);
    }
    else if (type == "dialog") {
        return QString("对话框界面 - %1").arg(baseName);
    }
    else if (type == "widget") {
        return QString("控件界面 - %1").arg(baseName);
    }
    else if (type == "settings") {
        return QString("设置界面 - %1").arg(baseName);
    }
    else if (type == "config") {
        return QString("配置界面 - %1").arg(baseName);
    }
    else if (type == "about") {
        return QString("关于界面 - %1").arg(baseName);
    }
    else if (type == "login") {
        return QString("登录界面 - %1").arg(baseName);
    }
    else if (type == "register") {
        return QString("注册界面 - %1").arg(baseName);
    }
    else if (type == "wizard") {
        return QString("向导界面 - %1").arg(baseName);
    }
    else if (type == "toolbar") {
        return QString("工具栏界面 - %1").arg(baseName);
    }
    else if (type == "statusbar") {
        return QString("状态栏界面 - %1").arg(baseName);
    }
    else if (type == "menu") {
        return QString("菜单界面 - %1").arg(baseName);
    }
    else {
        return QString("自定义界面 - %1").arg(baseName);
    }
}

// 加载默认产品配置
void ProductMainWindow::loadDefaultProductConfig()
{
    // 检查是否已经有产品配置加载
    if (m_productLoaded) {
        qDebug() << "Product configuration already loaded, skipping default configuration loading";
        return;
    }
    
    // 尝试多个可能的默认配置文件路径
    QStringList possibleConfigPaths;
    possibleConfigPaths << QDir::currentPath() + "/product_12345.json"
                        << QDir::currentPath() + "/product_configurations/product_12345/product_config.json";
    
    QString defaultProductConfigPath;
    
    // 查找第一个存在的配置文件
    for (const QString &path : possibleConfigPaths) {
        if (QFile::exists(path)) {
            defaultProductConfigPath = path;
            break;
        }
    }
    
    // 如果没有找到任何配置文件，则返回
    if (defaultProductConfigPath.isEmpty()) {
        qDebug() << "No default product configuration files found";
        return;
    }
    
    qDebug() << "Attempting to load default product configuration file:" << defaultProductConfigPath;
    
    // 加载默认产品配置
    QFile file(defaultProductConfigPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "无法打开默认产品配置文件:" << defaultProductConfigPath;
        return;
    }
    
    QByteArray jsonData = file.readAll();
    file.close();
    
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);
    
    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "默认产品配置文件JSON解析错误:" << parseError.errorString();
        return;
    }
    
    if (!jsonDoc.isObject()) {
        qWarning() << "默认产品配置文件不是有效的JSON对象";
        return;
    }
    
    // 从JSON创建产品对象
    Product defaultProduct;
    if (!defaultProduct.fromJson(jsonDoc.object(), defaultProductConfigPath)) {
        qWarning() << "无法从JSON创建默认产品对象";
        return;
    }
    
    // 如果产品配置中没有向导文件路径，尝试添加默认的向导文件路径
    if (defaultProduct.stateMachineConfigs().isEmpty()) {
        qDebug() << "No state machine configuration found in product configuration, adding default state machine configuration";
        
        // 创建默认的状态机配置
        StateMachineConfig defaultStateMachine;
        defaultStateMachine.name = "default_state_machine";
        defaultStateMachine.description = "默认状态机";
        defaultStateMachine.fileName = "state_machines/state_machine.json";
        
        // 添加默认的向导文件路径
        QString defaultWizardPath = QDir::currentPath() + "/wizards/wizard.json";
        if (QFile::exists(defaultWizardPath)) {
            defaultStateMachine.wizardJsonPath = defaultWizardPath;
        }
        
        QList<StateMachineConfig> stateMachineConfigs;
        stateMachineConfigs.append(defaultStateMachine);
        defaultProduct.setStateMachineConfigs(stateMachineConfigs);
    }
    
    // 加载产品数据
    loadProductData(defaultProduct);
    m_currentFile = defaultProductConfigPath;
    
    qDebug() << "Default product configuration loaded successfully:" << defaultProduct.name();
    
    // 更新窗口标题
    setWindowTitle(QString("软件编辑器 - 产品配置 - %1").arg(QFileInfo(defaultProductConfigPath).fileName()));
}