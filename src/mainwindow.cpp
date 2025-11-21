#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "uilayoutwindow.h"
#include <QApplication>
#include <QCloseEvent>
#include <QScreen>
#include <QGuiApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_currentFile("")
    , m_isModified(false)
    , m_packageManager(nullptr)
    , m_packageDialog(nullptr)
    , m_configManager(new ProductConfigManager(this))
{
    ui->setupUi(this);
    
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
    connect(m_configManager, &ProductConfigManager::needsUiBindingChanged, this, [this](bool needsBinding) {
        // 需要UI绑定状态变化
        updateStatusBar();
    });
    
    clearProductData();

    ui->mainToolBar->setIconSize(QSize(24, 24));
    ui->statusBar->showMessage("就绪");
    
    // 初始化智能打包对话框为nullptr
    m_smartPackageDialog = nullptr;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionNew_triggered()
{
    if (saveChanges()) {
        clearProductData();
        m_currentFile = "";
        setModified(false);
        setWindowTitle("产品编辑器 - 新建");
    }
}

void MainWindow::on_actionOpen_triggered()
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

void MainWindow::on_actionSave_triggered()
{
    if (m_currentFile.isEmpty()) {
        on_actionSave_As_triggered();
    } else {
        saveProductData();
        setModified(false);
    }
}

void MainWindow::on_actionSave_As_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, "保存产品文件", "", "JSON文件 (*.json)");
    if (!fileName.isEmpty()) {
        m_currentFile = fileName;
        saveProductData();
        setModified(false);
        setWindowTitle(QString("产品编辑器 - %1").arg(QFileInfo(fileName).fileName()));
    }
}

void MainWindow::on_saveButton_clicked()
{
    // 检查是否有未保存的修改
    if (m_isModified) {
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

void MainWindow::on_loadButton_clicked()
{
    on_actionOpen_triggered();
}

void MainWindow::on_actionExit_triggered()
{
    if (saveChanges()) {
        close();
    }
}

void MainWindow::on_iconBrowseButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "选择图标文件", "", "图像文件 (*.png *.jpg *.jpeg *.ico)");
    if (!fileName.isEmpty()) {
        ui->iconPathLineEdit->setText(fileName);
    }
}

void MainWindow::on_screenshotBrowseButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "选择截图文件", "", "图像文件 (*.png *.jpg *.jpeg)");
    if (!fileName.isEmpty()) {
        ui->screenshotPathLineEdit->setText(fileName);
    }
}



void MainWindow::on_addFeatureButton_clicked()
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

void MainWindow::on_removeFeatureButton_clicked()
{
    int row = ui->featuresListWidget->currentRow();
    if (row >= 0) {
        m_product.removeFeature(row);
        delete ui->featuresListWidget->takeItem(row);
        setModified(true);
    }
}

void MainWindow::on_featuresListWidget_itemClicked(QListWidgetItem *item)
{
    int row = ui->featuresListWidget->row(item);
    if (row >= 0 && row < m_product.features().size()) {
        const ProductFeature &feature = m_product.features().at(row);
        ui->featureNameLineEdit->setText(feature.name);
        ui->featureDescriptionTextEdit->setPlainText(feature.description);
    }
}

void MainWindow::clearProductData()
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
    
    // 重置配置管理器状态
    m_configManager->setProduct(m_product);
    m_configManager->setUiLayoutPath("");
    m_configManager->resetAllModifiedStates();
    
    updateStatusBar();
}

void MainWindow::updateFeatureEditors()
{
    // Implement feature editor updates if needed
}

void MainWindow::loadProductData(const Product &product)
{
    m_product = product;

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

void MainWindow::saveProductData()
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

    // 使用配置管理器检查UI绑定状态
    if (m_configManager->needsUiBinding()) {
        // 查找当前打开的UI布局窗口
        QWidgetList topLevelWidgets = QApplication::topLevelWidgets();
        QString currentUiLayoutPath;
        
        for (QWidget *widget : topLevelWidgets) {
            UILayoutWindow *layoutWindow = qobject_cast<UILayoutWindow*>(widget);
            if (layoutWindow) {
                currentUiLayoutPath = layoutWindow->getCurrentLayoutPath();
                if (!currentUiLayoutPath.isEmpty()) {
                    break;
                }
            }
        }
        
        if (!currentUiLayoutPath.isEmpty()) {
            // 弹出提示框询问用户是否要绑定UI
            QMessageBox::StandardButton reply = QMessageBox::question(
                this,
                "绑定UI布局",
                QString("检测到UI布局文件：%1\n是否要为当前产品绑定这个UI布局？").arg(QFileInfo(currentUiLayoutPath).fileName()),
                QMessageBox::Yes | QMessageBox::No,
                QMessageBox::Yes
            );
            
            if (reply == QMessageBox::Yes) {
                // 用户选择绑定
                m_configManager->bindUiLayout(currentUiLayoutPath);
                // 只有在产品没有UI布局路径或用户明确要更新时才设置
                if (m_product.uiLayoutPath().isEmpty()) {
                    m_product.setUiLayoutPath(currentUiLayoutPath);
                    qDebug() << "已将UI布局文件绑定到产品:" << currentUiLayoutPath;
                } else {
                    qDebug() << "产品已有UI布局路径，保持原有路径:" << m_product.uiLayoutPath();
                }
            }
        }
    }

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

void MainWindow::setModified(bool modified)
{
    m_isModified = modified;
    m_configManager->setProductModified(modified);
    
    QString windowTitle = m_currentFile.isEmpty() ? "产品编辑器 - 新建" : QString("产品编辑器 - %1").arg(QFileInfo(m_currentFile).fileName());
    if (modified) {
        windowTitle += " *";
    }
    setWindowTitle(windowTitle);
    
    updateStatusBar();
}

bool MainWindow::saveChanges()
{
    if (m_isModified) {
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

void MainWindow::updateFeaturesList()
{
    ui->featuresListWidget->clear();

    for (const auto &feature : m_product.features()) {
        QListWidgetItem *item = new QListWidgetItem(feature.name);
        ui->featuresListWidget->addItem(item);
    }
}

void MainWindow::updateStatusBar()
{
    QString status = m_configManager->getStatusDescription();
    ui->statusBar->showMessage(status);
}

ProductFeature MainWindow::getCurrentFeatureFromEditors() const
{
    ProductFeature feature;
    feature.name = ui->featureNameLineEdit->text();
    feature.description = ui->featureDescriptionTextEdit->toPlainText();
    return feature;
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, "关于产品编辑器", "这是一个基于Qt的产品编辑工具，用于创建和管理产品信息。");
}

void MainWindow::on_actionOpen_UI_Layout_Editor_triggered()
{
    // Save current product data first
    saveProductData();
    
    // 判断产品是否为新建状态（通过检查当前文件路径是否为空）
    bool isNewProduct = m_currentFile.isEmpty();
    
    // Create UI layout editor window
    UILayoutWindow *layoutWindow = new UILayoutWindow(this, isNewProduct, m_currentFile, m_configManager);
    layoutWindow->setAttribute(Qt::WA_DeleteOnClose);
    
    // 如果产品已有绑定的UI布局文件，自动加载
    if (!isNewProduct && !m_product.uiLayoutPath().isEmpty()) {
        QString uiLayoutPath = m_product.uiLayoutPath();
        if (QFile::exists(uiLayoutPath)) {
            layoutWindow->loadLayout(uiLayoutPath);
            qDebug() << "已自动加载关联的UI布局文件:" << uiLayoutPath;
        } else {
            qDebug() << "关联的UI布局文件不存在:" << uiLayoutPath;
        }
    } else if (!isNewProduct) {
        // 产品没有UI路径，但用户可能会加载UI文件
        // 注意：这里不应该标记为已修改状态，只有在用户实际修改时才应该设置
        qDebug() << "产品没有UI路径，等待用户操作";
    }
    
    layoutWindow->show();
    
    // 连接窗口关闭信号
    connect(layoutWindow, &UILayoutWindow::destroyed, this, &MainWindow::onUILayoutWindowClosed);
}

void MainWindow::onUILayoutWindowClosed()
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
    
    // 检查产品目录下是否有UI布局文件
    QDir productDir = QFileInfo(productFilePath).dir();
    
    // 查找产品目录下所有的.ui文件
    QStringList uiFiles = productDir.entryList(QStringList() << "*.ui", QDir::Files);
    
    if (!uiFiles.isEmpty()) {
        // 如果有多个UI文件，选择最近修改的一个
        QString latestUiFile;
        QDateTime latestModified;
        
        for (const QString &uiFile : uiFiles) {
            QString uiFilePath = productDir.filePath(uiFile);
            QFileInfo fileInfo(uiFilePath);
            if (!latestModified.isValid() || fileInfo.lastModified() > latestModified) {
                latestModified = fileInfo.lastModified();
                latestUiFile = uiFilePath;
            }
        }
        
        // 更新配置管理器中的UI布局路径
        m_configManager->setUiLayoutPath(latestUiFile);
        
        // 检查是否需要UI绑定
        if (m_configManager->needsUiBinding()) {
            // 弹出提示框询问用户是否要绑定UI
            QMessageBox::StandardButton reply = QMessageBox::question(
                this,
                "绑定UI布局",
                QString("检测到UI布局文件：%1\n是否要为当前产品绑定这个UI布局？").arg(QFileInfo(latestUiFile).fileName()),
                QMessageBox::Yes | QMessageBox::No,
                QMessageBox::Yes
            );
            
            if (reply == QMessageBox::Yes) {
                // 用户选择绑定
                m_configManager->bindUiLayout(latestUiFile);
                // 只有在产品没有UI布局路径或用户明确要更新时才设置
                if (m_product.uiLayoutPath().isEmpty()) {
                    m_product.setUiLayoutPath(latestUiFile);
                    qDebug() << "已将UI布局文件绑定到产品:" << latestUiFile;
                } else {
                    qDebug() << "产品已有UI布局路径，保持原有路径:" << m_product.uiLayoutPath();
                }
                
                // 只有在UI布局编辑器有实际修改时才自动保存产品数据
                if (m_configManager->isUiLayoutModified()) {
                    // 自动保存产品数据以保存绑定关系
                    saveProductData();
                    qDebug() << "产品数据已保存，UI布局绑定完成";
                    
                    // 显示成功提示
                    QMessageBox::information(this, "绑定成功", "UI布局已成功绑定到当前产品！");
                } else {
                    // UI布局编辑器没有修改，只绑定但不自动保存
                    qDebug() << "UI布局编辑器没有修改，只绑定但不自动保存";
                    
                    // 重置产品修改状态，因为只是绑定操作，没有实际修改
                    m_configManager->setProductModified(false);
                }
            } else {
                // 用户选择不绑定
                qDebug() << "用户选择不绑定UI布局文件";
                
                // 重置UI布局改变状态
                m_configManager->setUiLayoutChanged(false);
            }
        } else {
            qDebug() << "UI布局文件未改变，跳过绑定提示";
        }
    } else {
        qDebug() << "产品目录下未找到UI布局文件，跳过绑定";
    }
}

void MainWindow::on_actionPackage_Software_triggered()
{
    // 检查是否有有效的产品数据
    if (m_product.name().isEmpty()) {
        QMessageBox::warning(this, "打包软件", "请先创建或加载一个产品项目。");
        return;
    }

    // 保存当前编辑的数据
    if (m_isModified) {
        QMessageBox::StandardButton button = QMessageBox::question(this, "保存更改", "打包前需要保存当前的产品信息。是否保存？");
        if (button == QMessageBox::Save) {
            on_actionSave_triggered();
        } else if (button == QMessageBox::Cancel) {
            return;
        }
    }

    // 创建打包对话框
    if (!m_packageDialog) {
        m_packageDialog = new PackageDialog(this);
        connect(m_packageDialog, &PackageDialog::packageRequested, this, &MainWindow::startPackageProcess);
    }

    // 设置产品信息到打包对话框
    m_packageDialog->setProductInfo(m_product);
    
    // 显示打包对话框
    m_packageDialog->exec();
}

void MainWindow::on_actionSmart_Package_Software_triggered()
{
    // 检查是否有有效的产品数据
    if (m_product.name().isEmpty()) {
        QMessageBox::warning(this, "智能打包软件", "请先创建或加载一个产品项目。");
        return;
    }

    // 保存当前编辑的数据
    if (m_isModified) {
        QMessageBox::StandardButton button = QMessageBox::question(this, "保存更改", "打包前需要保存当前的产品信息。是否保存？");
        if (button == QMessageBox::Save) {
            on_actionSave_triggered();
        } else if (button == QMessageBox::Cancel) {
            return;
        }
    }

    // 创建智能打包对话框
    if (!m_smartPackageDialog) {
        m_smartPackageDialog = new SmartPackageDialog(this);
        connect(m_smartPackageDialog, &SmartPackageDialog::smartPackageRequested, this, &MainWindow::startSmartPackageProcess);
    }

    // 设置产品信息到智能打包对话框
    m_smartPackageDialog->setProductInfo(m_product);
    
    // 显示智能打包对话框
    m_smartPackageDialog->exec();
}

void MainWindow::startPackageProcess(const Product &product, const PackageConfig::PackageSettings &settings)
{
    // 创建打包管理器
    if (!m_packageManager) {
        m_packageManager = new PackageManager(this);
        connect(m_packageManager, &PackageManager::progressChanged, this, &MainWindow::onPackageProgress);
        connect(m_packageManager, &PackageManager::packageFinished, this, &MainWindow::onPackageFinished);
        connect(m_packageManager, &PackageManager::errorOccurred, this, &MainWindow::onPackageError);
    }

    // 开始打包过程 - 使用传递的Product对象
    m_packageManager->startPackage(product, settings);
}

void MainWindow::startSmartPackageProcess(const Product &product, const SmartPackageConfig::SmartPackageSettings &settings)
{
    // 创建打包管理器
    if (!m_packageManager) {
        m_packageManager = new PackageManager(this);
        connect(m_packageManager, &PackageManager::progressChanged, this, &MainWindow::onPackageProgress);
        connect(m_packageManager, &PackageManager::packageFinished, this, &MainWindow::onPackageFinished);
        connect(m_packageManager, &PackageManager::errorOccurred, this, &MainWindow::onPackageError);
    }

    // 开始智能打包过程 - 使用传递的Product对象
    m_packageManager->startSmartPackage(product, settings);
}

void MainWindow::onPackageProgress(int progress, const QString &message)
{
    ui->statusBar->showMessage(message);
    
    // 如果打包对话框存在，更新进度
    if (m_packageDialog) {
        m_packageDialog->updateProgress(progress, message);
    }
}

void MainWindow::onPackageFinished(bool success, const QString &message)
{
    ui->statusBar->showMessage(message);
    
    if (success) {
        QMessageBox::information(this, "打包完成", message);
    } else {
        QMessageBox::warning(this, "打包失败", message);
    }
    
    // 关闭打包对话框
    if (m_packageDialog) {
        m_packageDialog->packageFinished();
    }
}

void MainWindow::onPackageError(const QString &error)
{
    ui->statusBar->showMessage("打包错误: " + error);
    QMessageBox::critical(this, "打包错误", error);
    
    // 关闭打包对话框
    if (m_packageDialog) {
        m_packageDialog->packageFinished();
    }
}

// 处理主窗口关闭事件
void MainWindow::closeEvent(QCloseEvent *event)
{
    // 检查是否有未保存的修改（包括产品数据和UI布局）
    bool hasUnsavedChanges = m_isModified || (m_configManager && m_configManager->needsSave());
    
    if (hasUnsavedChanges) {
        QString message = "检测到未保存的更改：\n";
        
        if (m_isModified) {
            message += "• 产品信息已修改\n";
        }
        
        if (m_configManager && m_configManager->isProductModified()) {
            message += "• 产品配置已修改\n";
        }
        
        if (m_configManager && m_configManager->isUiLayoutModified()) {
            message += "• UI布局已修改\n";
        }
        
        message += "\n是否保存这些更改？";
        
        QMessageBox::StandardButton button = QMessageBox::question(this, 
            "保存更改", 
            message,
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        
        if (button == QMessageBox::Save) {
            // 保存产品数据
            saveProductData();
            
            // 如果UI布局已修改，也需要保存UI布局文件
            if (m_configManager && m_configManager->isUiLayoutModified()) {
                // 这里可以添加保存UI布局文件的逻辑
                qDebug() << "UI布局修改需要保存，但当前实现中UI布局文件在关闭UI布局窗口时已自动保存";
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
        event->accept(); // 接受关闭事件
    }
}