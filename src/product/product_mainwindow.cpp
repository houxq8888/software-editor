#include "product_mainwindow.h"
#include "ui_product_mainwindow.h"
#include "uilayoutwindow.h"
#include "../designer/qdesigner.h"
#include "../designer/mainwindow.h"
#include <QApplication>
#include <QCloseEvent>
#include <QScreen>
#include <QGuiApplication>
#include <QDesignerFormWindowInterface>
#include <QDesignerFormWindowManagerInterface>
#include <QDesignerFormEditorInterface>
#include "qdesigner_workbench.h"
#include <QProcess>
#include <QProcessEnvironment>

ProductMainWindow::ProductMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ProductMainWindow)
    , m_packageManager(nullptr)
    , m_configManager(nullptr)
    , m_logTimer(nullptr)
    , m_qdesigner_instance(nullptr)
    , m_uiEditorActive(false)
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
}

ProductMainWindow::~ProductMainWindow()
{
    qDebug() << "ProductMainWindow析构函数开始执行";
    
    // 先删除PackageManager对象，确保在UI删除前断开所有信号连接
    qDebug()<<"delete package manager";
    if (m_packageManager) {
        delete m_packageManager;
        m_packageManager = nullptr;
    }
    
    // 清理日志捕获资源
    qDebug() << "清理日志捕获资源";
    cleanupLogCapture();
    
    delete ui;
    qDebug()<<"delete ui";
    
    qDebug() << "ProductMainWindow析构函数执行完成";
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
                    if (product.fromJson(jsonDoc.object())) {
                        loadProductData(product);
                        m_currentFile = fileName;
                        setModified(false);
                        setWindowTitle(QString("产品编辑器 - %1").arg(QFileInfo(fileName).fileName()));
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
    
    // 如果UI编辑器已经在运行，则直接激活
    if (m_uiEditorActive && m_qdesigner_instance) {
        qDebug()<<"ui editor already active";
        // 获取QDesigner的主窗口并激活
        QMainWindow *mainWindow = qobject_cast<QMainWindow*>(m_qdesigner_instance->mainWindow());
        if (mainWindow) {
            mainWindow->show();
            mainWindow->raise();
            mainWindow->activateWindow();
            return;
        }
    }
    
    // 启动UI编辑器
    startUIEditor();
}

void ProductMainWindow::startUIEditor()
{
    if (m_uiEditorActive) {
        qDebug() << "UI编辑器已经在运行";
        return;
    }
    
    qDebug() << "启动UI编辑器";
    
    // 创建QDesigner实例
    static int argc = 1;
    static const char *argv[] = {"software-editor", nullptr};
    
    m_qdesigner_instance = new QDesigner(argc, const_cast<char**>(argv));
    
    // 设置启动标记属性，标识这是从产品配置页面启动的
    m_qdesigner_instance->setProperty("launchedFromProductMainWindow", true);
    
    // 连接QDesigner的退出信号
    connect(m_qdesigner_instance, &QDesigner::aboutToQuit, this, &ProductMainWindow::onQDesignerFinished);
    
    // 关键修改：在解析命令行参数之前设置预定义文件路径
    // 获取UI文件路径
    QString uiPath = m_configManager->getUiLayoutPath();
    if (uiPath.isEmpty()) {
        // 如果没有UI文件，使用默认路径
        uiPath = QDir::currentPath() + "/default.ui";
        qDebug() << "使用默认UI文件路径:" << uiPath;
    }
    
    // 如果UI文件不存在，创建一个空的UI文件
    if (!QFile::exists(uiPath)) {
        qDebug() << "UI文件不存在，创建空文件:" << uiPath;
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
    
    // 关键修复：在解析命令行参数之前设置预定义文件路径
    // 这样在parseCommandLineArguments()中就能识别到有文件要打开，从而抑制NewForm窗口
    QStringList filesToOpen;
    filesToOpen << uiPath;
    
    qDebug() << "调用setPredefinedFiles方法，设置预定义UI文件路径:" << uiPath;
    m_qdesigner_instance->setPredefinedFiles(filesToOpen);
    
    // 解析命令行参数
    QDesigner::ParseArgumentsResult result = m_qdesigner_instance->parseCommandLineArguments();
    
    if (result != QDesigner::ParseArgumentsSuccess) {
        qDebug() << "QDesigner参数解析失败";
        delete m_qdesigner_instance;
        m_qdesigner_instance = nullptr;
        return;
    }
}

void ProductMainWindow::stopUIEditor()
{
    if (m_uiEditorActive && m_qdesigner_instance) {
        // 断开信号连接
        disconnect(m_qdesigner_instance, &QDesigner::aboutToQuit, this, &ProductMainWindow::onQDesignerFinished);
        
        // 重要：不要调用quit()，因为QDesigner继承自QApplication，调用quit()会导致整个程序退出
        // 只需要断开连接并置空指针，让QDesigner自然关闭
        // m_qdesigner_instance->quit(); // 这行代码会导致整个应用程序退出
        
        // 重要：不要删除QDesigner实例，因为它继承自QApplication
        // 只需要将指针置为nullptr，让QDesigner自然退出
        m_qdesigner_instance = nullptr;
        
        m_uiEditorActive = false;
        qDebug() << "UI编辑器已停止";
    }
}



void ProductMainWindow::onQDesignerFinished()
{
    qDebug() << "QDesigner finish quit";
    
    // 保存当前打开的UI文件路径到产品配置
    if (m_qdesigner_instance && m_qdesigner_instance->workbench()) {
        // 获取当前打开的UI文件路径 - 通过formWindowManager获取activeFormWindow
        QDesignerFormWindowInterface *currentForm = m_qdesigner_instance->workbench()->core()->formWindowManager()->activeFormWindow();
        if (currentForm) {
            QString currentUiPath = currentForm->fileName();
            if (!currentUiPath.isEmpty()) {
                qDebug() << "detect current open ui layout file:" << currentUiPath;
                
                // 保存UI路径到产品配置
                m_product.setUiLayoutPath(currentUiPath);
                m_configManager->setUiLayoutPath(currentUiPath);
                
                qDebug() << "save current open ui layout file path to product config:" << currentUiPath;
            }
        }
    }
    
    // 清理QDesigner实例 - 注意：不能删除QApplication实例，否则会导致整个程序退出
    if (m_qdesigner_instance) {
        qDebug() << "cleanup QDesigner instance connections";
        disconnect(m_qdesigner_instance, &QDesigner::aboutToQuit, this, &ProductMainWindow::onQDesignerFinished);
        
        // 重要：不要删除QDesigner实例，因为它继承自QApplication
        // 只需要将指针置为nullptr，让QDesigner自然退出
        m_qdesigner_instance = nullptr;
    }
    
    m_uiEditorActive = false;
    
    // 确保产品配置页面正确显示
    // 先确保窗口没有被最小化
    if (this->isMinimized()) {
        this->showNormal();
    }
    
    // 显示窗口
    this->show();
    qDebug()<<"show mainwindow";
    // 确保窗口获得焦点
    this->raise();
    this->activateWindow();
    
    // 强制窗口重绘以确保可见
    this->repaint();
    
    // 添加延迟处理，确保窗口状态正确恢复
    QTimer::singleShot(100, this, [this]() {
        this->raise();
        this->activateWindow();
        qDebug() << "延迟激活窗口完成";
    });
    
    qDebug() << "return to product config page, ui layout path saved";
}

void ProductMainWindow::onUILayoutWindowClosed()
{
    // UI布局窗口关闭时的处理逻辑
    qDebug() << "UI布局窗口已关闭";
    
    // 注意：此时UI布局窗口已经被销毁，无法通过sender()获取实例
    // 我们需要在UI布局窗口关闭前保存布局路径信息
    
    // 获取产品文件路径
    QString productFilePath = m_currentFile;
    if (productFilePath.isEmpty()) {
        qDebug() << "当前产品文件路径为空，无法绑定UI布局";
        return;
    }
    
    // 检查是否是UI绑定相关的修改（新文件加载）
    // 关键逻辑：只有当UI布局路径发生变化时，才认为是新文件加载，需要触发UI绑定询问
    QString currentUiLayoutPath = m_configManager->getUiLayoutPath();
    QString productUiLayoutPath = m_product.uiLayoutPath();
    
    qDebug() << "当前UI布局路径:" << currentUiLayoutPath;
    qDebug() << "产品UI布局路径:" << productUiLayoutPath;
    
    // 检查UI布局路径是否发生变化
    if (!currentUiLayoutPath.isEmpty() && currentUiLayoutPath != productUiLayoutPath) {
        // 检查UI布局内容是否被修改过
            qDebug() << "检测到UI布局内容被修改，触发UI绑定询问";
            
            // 询问用户是否要绑定UI布局
            QMessageBox::StandardButton reply = QMessageBox::question(this,
                "UI布局绑定",
                QString("检测到新的UI布局文件：%1\n是否要为当前产品绑定这个UI布局？").arg(QFileInfo(currentUiLayoutPath).fileName()),
                QMessageBox::Yes | QMessageBox::No);
            
            if (reply == QMessageBox::Yes) {
                // 绑定UI布局
                m_configManager->bindUiLayout(currentUiLayoutPath);
                
                // 更新产品的UI布局路径
                m_product.setUiLayoutPath(currentUiLayoutPath);
                qDebug() << "已将UI布局文件绑定到产品:" << currentUiLayoutPath;
                
                // 标记产品已修改
                setModified(true);
            } else {
                // 用户选择不绑定，将UI布局路径改回原来的路径
                m_configManager->setUiLayoutPath(productUiLayoutPath);
                qDebug() << "用户选择不绑定UI布局，已将UI布局路径改回:" << productUiLayoutPath;
            }
    } else {
        // UI绑定和UI内容都没有修改
        qDebug() << "UI绑定没有修改";
    }
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
        qDebug()<<"产品名称:"<<m_product.name();
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
    
    qDebug()<<"关闭事件检查 - 产品基本信息修改:"<<basicInfoModified
            <<", 产品功能特性修改:"<<featuresTabModified;
    
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
                    qDebug() << "产品功能特性修改已保存";
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