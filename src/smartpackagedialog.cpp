#include "smartpackagedialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QDesktopServices>
#include <QUrl>
#include <QDebug>
#include <QButtonGroup>

SmartPackageDialog::SmartPackageDialog(QWidget *parent)
    : QDialog(parent)
    , m_packageManager(new PackageManager(this))
    , m_smartPackageConfig(new SmartPackageConfig(this))
{
    setupUI();
    setupConnections();
    setWindowTitle("智能软件打包");
    setMinimumSize(600, 500);
    
    // 设置默认MinGW路径为D:\Qt\Tools\mingw1310_64
    QString defaultMingwPath = "D:/Qt/Tools/mingw1310_64";
    if (QDir(defaultMingwPath).exists()) {
        m_smartPackageConfig->setMingwPath(defaultMingwPath);
        m_mingwPathEdit->setText(defaultMingwPath);
        m_mingwPathLabel->setText("默认路径: " + defaultMingwPath);
        m_mingwPathLabel->setStyleSheet("QLabel { color: #2E8B57; }");
    }
    
    // 初始检测开发工具
    onDetectTools();
}

SmartPackageDialog::~SmartPackageDialog()
{
}

void SmartPackageDialog::setProductInfo(const QJsonObject &productInfo)
{
    // 从JSON对象创建Product对象
    Product product;
    product.setName(productInfo["name"].toString());
    product.setVersion(productInfo["version"].toString());
    product.setDeveloper(productInfo["developer"].toString());
    product.setDescription(productInfo["description"].toString());
    product.setIconPath(productInfo["iconPath"].toString());
    
    setProductInfo(product);
}

void SmartPackageDialog::setProductInfo(const Product &product)
{
    // 存储Product对象
    m_product = product;
    
    // 从Product对象创建SmartPackageSettings
    SmartPackageConfig::SmartPackageSettings settings;
    
    settings.name = product.name();
    settings.version = product.version();
    settings.developer = product.developer();
    settings.description = product.description();
    settings.iconPath = product.iconPath();
    
    // 设置默认输出目录
    QString defaultOutputDir = QDir::currentPath() + "/smart_packages/" + product.name();
    settings.outputDir = defaultOutputDir;
    
    // 默认启用依赖包含和安装包创建
    settings.includeDependencies = true;
    settings.createInstaller = true;
    
    // 更新UI
    m_nameEdit->setText(settings.name);
    m_versionEdit->setText(settings.version);
    m_developerEdit->setText(settings.developer);
    m_descriptionEdit->setText(settings.description);
    m_iconPathEdit->setText(settings.iconPath);
    m_outputDirEdit->setText(settings.outputDir);
    m_createInstallerCheck->setChecked(settings.createInstaller);
    m_includeDepsCheck->setChecked(settings.includeDependencies);
    
    // 默认选择智能打包模式
    m_smartPackageRadio->setChecked(true);
    onPackageModeChanged();
}

SmartPackageConfig::SmartPackageSettings SmartPackageDialog::getSmartPackageSettings() const
{
    SmartPackageConfig::SmartPackageSettings settings;
    
    settings.name = m_nameEdit->text();
    settings.version = m_versionEdit->text();
    settings.developer = m_developerEdit->text();
    settings.description = m_descriptionEdit->toPlainText();
    settings.iconPath = m_iconPathEdit->text();
    settings.outputDir = m_outputDirEdit->text();
    settings.createInstaller = m_createInstallerCheck->isChecked();
    settings.includeDependencies = m_includeDepsCheck->isChecked();
    
    // 优先使用手动填入的工具路径，如果手动路径为空，则使用自动检测的路径
    if (!m_qtPathEdit->text().isEmpty()) {
        settings.qtDir = m_qtPathEdit->text();
        m_smartPackageConfig->setQtPath(m_qtPathEdit->text()); // 设置到配置对象中
    } else {
        settings.qtDir = m_smartPackageConfig->getQtPath();
    }
    
    if (!m_mingwPathEdit->text().isEmpty()) {
        settings.mingwDir = m_mingwPathEdit->text();
        m_smartPackageConfig->setMingwPath(m_mingwPathEdit->text()); // 设置到配置对象中
    } else {
        settings.mingwDir = m_smartPackageConfig->getMingwPath();
    }
    
    if (!m_cmakePathEdit->text().isEmpty()) {
        settings.cmakeDir = m_cmakePathEdit->text();
        m_smartPackageConfig->setCmakePath(m_cmakePathEdit->text()); // 设置到配置对象中
    } else {
        settings.cmakeDir = m_smartPackageConfig->getCmakePath();
    }
    
    return settings;
}

void SmartPackageDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // 基本信息组
    QGroupBox *infoGroup = new QGroupBox("软件信息", this);
    QFormLayout *infoLayout = new QFormLayout(infoGroup);
    
    m_nameEdit = new QLineEdit(this);
    m_versionEdit = new QLineEdit(this);
    m_developerEdit = new QLineEdit(this);
    m_descriptionEdit = new QTextEdit(this);
    m_descriptionEdit->setMaximumHeight(80);
    
    infoLayout->addRow("软件名称*:", m_nameEdit);
    infoLayout->addRow("版本号*:", m_versionEdit);
    infoLayout->addRow("开发者*:", m_developerEdit);
    infoLayout->addRow("描述:", m_descriptionEdit);
    
    // 打包模式选择
    QGroupBox *modeGroup = new QGroupBox("打包模式", this);
    QVBoxLayout *modeLayout = new QVBoxLayout(modeGroup);
    
    m_normalPackageRadio = new QRadioButton("标准打包模式", this);
    m_smartPackageRadio = new QRadioButton("智能打包模式（推荐）", this);
    
    modeLayout->addWidget(m_normalPackageRadio);
    modeLayout->addWidget(m_smartPackageRadio);
    
    // 智能打包选项组
    m_smartPackageGroup = new QGroupBox("智能打包配置", this);
    QFormLayout *smartLayout = new QFormLayout(m_smartPackageGroup);
    
    // Qt路径设置
    QLabel *qtLabel = new QLabel("Qt路径:", this);
    m_qtPathEdit = new QLineEdit(this);
    QPushButton *qtBrowseButton = new QPushButton("浏览...", this);
    QHBoxLayout *qtLayout = new QHBoxLayout();
    qtLayout->addWidget(m_qtPathEdit);
    qtLayout->addWidget(qtBrowseButton);
    
    // MinGW路径设置
    QLabel *mingwLabel = new QLabel("MinGW路径:", this);
    m_mingwPathEdit = new QLineEdit(this);
    QPushButton *mingwBrowseButton = new QPushButton("浏览...", this);
    QHBoxLayout *mingwLayout = new QHBoxLayout();
    mingwLayout->addWidget(m_mingwPathEdit);
    mingwLayout->addWidget(mingwBrowseButton);
    
    // CMake路径设置
    QLabel *cmakeLabel = new QLabel("CMake路径:", this);
    m_cmakePathEdit = new QLineEdit(this);
    QPushButton *cmakeBrowseButton = new QPushButton("浏览...", this);
    QHBoxLayout *cmakeLayout = new QHBoxLayout();
    cmakeLayout->addWidget(m_cmakePathEdit);
    cmakeLayout->addWidget(cmakeBrowseButton);
    
    m_qtPathLabel = new QLabel("未检测到", this);
    m_mingwPathLabel = new QLabel("未检测到", this);
    m_cmakePathLabel = new QLabel("未检测到", this);
    m_detectToolsButton = new QPushButton("自动检测工具", this);
    
    smartLayout->addRow("Qt安装目录:", qtLayout);
    smartLayout->addRow("检测状态:", m_qtPathLabel);
    smartLayout->addRow("MinGW安装目录:", mingwLayout);
    smartLayout->addRow("检测状态:", m_mingwPathLabel);
    smartLayout->addRow("CMake安装目录:", cmakeLayout);
    smartLayout->addRow("检测状态:", m_cmakePathLabel);
    smartLayout->addRow("", m_detectToolsButton);
    
    // 打包选项组
    QGroupBox *optionsGroup = new QGroupBox("打包选项", this);
    QFormLayout *optionsLayout = new QFormLayout(optionsGroup);
    
    m_iconPathEdit = new QLineEdit(this);
    QPushButton *iconBrowseButton = new QPushButton("浏览...", this);
    QHBoxLayout *iconLayout = new QHBoxLayout();
    iconLayout->addWidget(m_iconPathEdit);
    iconLayout->addWidget(iconBrowseButton);
    
    m_outputDirEdit = new QLineEdit(this);
    QPushButton *outputBrowseButton = new QPushButton("浏览...", this);
    QHBoxLayout *outputLayout = new QHBoxLayout();
    outputLayout->addWidget(m_outputDirEdit);
    outputLayout->addWidget(outputBrowseButton);
    
    m_createInstallerCheck = new QCheckBox("创建安装包", this);
    m_includeDepsCheck = new QCheckBox("包含依赖文件", this);
    
    optionsLayout->addRow("图标文件:", iconLayout);
    optionsLayout->addRow("输出目录*:", outputLayout);
    optionsLayout->addRow(m_createInstallerCheck);
    optionsLayout->addRow(m_includeDepsCheck);
    
    // 智能打包信息
    m_smartPackageInfoLabel = new QLabel("智能打包模式将自动检测开发工具路径，解决CMake配置错误问题。", this);
    m_smartPackageInfoLabel->setWordWrap(true);
    m_smartPackageInfoLabel->setStyleSheet("QLabel { color: #2E8B57; font-weight: bold; }");
    
    // 进度显示
    QGroupBox *progressGroup = new QGroupBox("打包进度", this);
    QVBoxLayout *progressLayout = new QVBoxLayout(progressGroup);
    
    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    
    m_statusLabel = new QLabel("准备打包...", this);
    
    progressLayout->addWidget(m_progressBar);
    progressLayout->addWidget(m_statusLabel);
    
    // 按钮组
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    m_startButton = new QPushButton("开始打包", this);
    m_cancelButton = new QPushButton("取消", this);
    m_closeButton = new QPushButton("关闭", this);
    
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_startButton);
    buttonLayout->addWidget(m_cancelButton);
    buttonLayout->addWidget(m_closeButton);
    
    // 添加到主布局
    mainLayout->addWidget(infoGroup);
    mainLayout->addWidget(modeGroup);
    mainLayout->addWidget(m_smartPackageGroup);
    mainLayout->addWidget(optionsGroup);
    mainLayout->addWidget(m_smartPackageInfoLabel);
    mainLayout->addWidget(progressGroup);
    mainLayout->addLayout(buttonLayout);
    
    // 连接按钮信号
    connect(iconBrowseButton, &QPushButton::clicked, this, &SmartPackageDialog::onBrowseIcon);
    connect(outputBrowseButton, &QPushButton::clicked, this, &SmartPackageDialog::onBrowseOutputDir);
    connect(qtBrowseButton, &QPushButton::clicked, this, &SmartPackageDialog::onBrowseQtPath);
    connect(mingwBrowseButton, &QPushButton::clicked, this, &SmartPackageDialog::onBrowseMingwPath);
    connect(cmakeBrowseButton, &QPushButton::clicked, this, &SmartPackageDialog::onBrowseCmakePath);
    connect(m_startButton, &QPushButton::clicked, this, &SmartPackageDialog::onStartPackage);
    connect(m_cancelButton, &QPushButton::clicked, this, &SmartPackageDialog::onCancelPackage);
    connect(m_closeButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_detectToolsButton, &QPushButton::clicked, this, &SmartPackageDialog::onDetectTools);
    connect(m_normalPackageRadio, &QRadioButton::toggled, this, &SmartPackageDialog::onPackageModeChanged);
    connect(m_smartPackageRadio, &QRadioButton::toggled, this, &SmartPackageDialog::onPackageModeChanged);
    
    // 初始状态
    updateUIState(false);
    onPackageModeChanged();
}

void SmartPackageDialog::setupConnections()
{
    connect(m_packageManager, &PackageManager::progressChanged, 
            this, &SmartPackageDialog::onPackageProgress);
    connect(m_packageManager, &PackageManager::packageFinished, 
            this, &SmartPackageDialog::onPackageFinished);
    connect(m_packageManager, &PackageManager::errorOccurred, 
            this, &SmartPackageDialog::onPackageError);
}

void SmartPackageDialog::onBrowseOutputDir()
{
    QString dir = QFileDialog::getExistingDirectory(this, "选择输出目录", 
                                                   m_outputDirEdit->text());
    if (!dir.isEmpty()) {
        m_outputDirEdit->setText(dir);
    }
}

void SmartPackageDialog::onBrowseIcon()
{
    QString file = QFileDialog::getOpenFileName(this, "选择图标文件", 
                                               m_iconPathEdit->text(), 
                                               "图标文件 (*.ico);;所有文件 (*.*)");
    if (!file.isEmpty()) {
        m_iconPathEdit->setText(file);
    }
}

void SmartPackageDialog::onBrowseQtPath()
{
    QString dir = QFileDialog::getExistingDirectory(this, "选择Qt安装目录", 
                                                   m_qtPathEdit->text());
    if (!dir.isEmpty()) {
        m_qtPathEdit->setText(dir);
        
        // 验证Qt路径
        if (QDir(dir).exists()) {
            QString qmakePath = dir + "/bin/qmake.exe";
            if (QFile::exists(qmakePath)) {
                m_qtPathLabel->setText("手动设置: " + dir);
                m_qtPathLabel->setStyleSheet("QLabel { color: #2E8B57; }");
            } else {
                m_qtPathLabel->setText("警告: 未找到qmake.exe");
                m_qtPathLabel->setStyleSheet("QLabel { color: #FFA500; }");
            }
        }
    }
}

void SmartPackageDialog::onBrowseMingwPath()
{
    QString dir = QFileDialog::getExistingDirectory(this, "选择MinGW安装目录", 
                                                   m_mingwPathEdit->text());
    if (!dir.isEmpty()) {
        m_mingwPathEdit->setText(dir);
        
        // 验证MinGW路径
        if (QDir(dir).exists()) {
            QString gppPath = dir + "/bin/g++.exe";
            if (QFile::exists(gppPath)) {
                m_mingwPathLabel->setText("手动设置: " + dir);
                m_mingwPathLabel->setStyleSheet("QLabel { color: #2E8B57; }");
            } else {
                m_mingwPathLabel->setText("警告: 未找到g++.exe");
                m_mingwPathLabel->setStyleSheet("QLabel { color: #FFA500; }");
            }
        }
    }
}

void SmartPackageDialog::onBrowseCmakePath()
{
    QString dir = QFileDialog::getExistingDirectory(this, "选择CMake安装目录", 
                                                   m_cmakePathEdit->text());
    if (!dir.isEmpty()) {
        m_cmakePathEdit->setText(dir);
        
        // 验证CMake路径
        if (QDir(dir).exists()) {
            QString cmakePath = dir + "/bin/cmake.exe";
            if (QFile::exists(cmakePath)) {
                m_cmakePathLabel->setText("手动设置: " + dir);
                m_cmakePathLabel->setStyleSheet("QLabel { color: #2E8B57; }");
            } else {
                m_cmakePathLabel->setText("警告: 未找到cmake.exe");
                m_cmakePathLabel->setStyleSheet("QLabel { color: #FFA500; }");
            }
        }
    }
}

void SmartPackageDialog::onStartPackage()
{
    // 验证输入
    if (m_nameEdit->text().isEmpty()) {
        QMessageBox::warning(this, "输入错误", "软件名称不能为空");
        return;
    }
    
    if (m_versionEdit->text().isEmpty()) {
        QMessageBox::warning(this, "输入错误", "版本号不能为空");
        return;
    }
    
    if (m_developerEdit->text().isEmpty()) {
        QMessageBox::warning(this, "输入错误", "开发者不能为空");
        return;
    }
    
    if (m_outputDirEdit->text().isEmpty()) {
        QMessageBox::warning(this, "输入错误", "输出目录不能为空");
        return;
    }
    
    // 获取打包配置
    SmartPackageConfig::SmartPackageSettings settings = getSmartPackageSettings();
    
    // 根据选择的模式启动打包
    if (m_smartPackageRadio->isChecked()) {
        // 智能打包模式 - 验证手动填入的路径
        if (!validateManualPaths()) {
            return;
        }
        
        m_packageManager->startSmartPackage(m_product, settings);
    } else {
        // 标准打包模式
        PackageConfig::PackageSettings normalSettings;
        normalSettings.name = settings.name;
        normalSettings.version = settings.version;
        normalSettings.developer = settings.developer;
        normalSettings.description = settings.description;
        normalSettings.iconPath = settings.iconPath;
        normalSettings.outputDir = settings.outputDir;
        normalSettings.createInstaller = settings.createInstaller;
        normalSettings.includeDependencies = settings.includeDependencies;
        
        m_packageManager->startPackage(m_product, normalSettings);
    }
    
    updateUIState(true);
}

void SmartPackageDialog::onCancelPackage()
{
    m_packageManager->cancelPackage();
    updateUIState(false);
}

void SmartPackageDialog::onPackageProgress(int progress, const QString &message)
{
    m_progressBar->setValue(progress);
    m_statusLabel->setText(message);
}

void SmartPackageDialog::onPackageFinished(bool success, const QString &resultPath)
{
    updateUIState(false);
    
    if (success) {
        m_progressBar->setValue(100);
        m_statusLabel->setText("打包完成！");
        
        QMessageBox::information(this, "打包成功", 
                                QString("软件打包完成！\n输出目录: %1").arg(resultPath));
        
        // 打开输出目录
        QDesktopServices::openUrl(QUrl::fromLocalFile(resultPath));
    } else {
        m_statusLabel->setText("打包失败");
    }
}

void SmartPackageDialog::onPackageError(const QString &error)
{
    updateUIState(false);
    m_statusLabel->setText("打包错误");
    QMessageBox::critical(this, "打包错误", error);
}

void SmartPackageDialog::onDetectTools()
{
    bool success = m_smartPackageConfig->detectDevelopmentTools();
    
    if (success) {
        // 获取检测到的路径
        QString qtPath = m_smartPackageConfig->detectQtPath();
        QString mingwPath = m_smartPackageConfig->detectMingwPath();
        QString cmakePath = m_smartPackageConfig->detectCmakePath();
        
        // 更新界面标签显示
        m_qtPathLabel->setText(qtPath);
        m_mingwPathLabel->setText(mingwPath);
        m_cmakePathLabel->setText(cmakePath);
        
        // 如果手动输入框为空，则自动填入检测到的路径
        if (m_qtPathEdit->text().isEmpty()) {
            m_qtPathEdit->setText(qtPath);
        }
        if (m_mingwPathEdit->text().isEmpty()) {
            m_mingwPathEdit->setText(mingwPath);
        }
        if (m_cmakePathEdit->text().isEmpty()) {
            m_cmakePathEdit->setText(cmakePath);
        }
        
        m_qtPathLabel->setStyleSheet("QLabel { color: #2E8B57; }");
        m_mingwPathLabel->setStyleSheet("QLabel { color: #2E8B57; }");
        m_cmakePathLabel->setStyleSheet("QLabel { color: #2E8B57; }");
        
        updateSmartPackageInfo();
    } else {
        m_qtPathLabel->setText("未检测到");
        m_mingwPathLabel->setText("未检测到");
        m_cmakePathLabel->setText("未检测到");
        
        m_qtPathLabel->setStyleSheet("QLabel { color: #FF0000; }");
        m_mingwPathLabel->setStyleSheet("QLabel { color: #FF0000; }");
        m_cmakePathLabel->setStyleSheet("QLabel { color: #FF0000; }");
        
        QMessageBox::warning(this, "工具检测", "无法检测到必要的开发工具，请确保Qt、MinGW和CMake已正确安装。");
    }
}

void SmartPackageDialog::onPackageModeChanged()
{
    bool smartMode = m_smartPackageRadio->isChecked();
    
    m_smartPackageGroup->setVisible(smartMode);
    m_smartPackageInfoLabel->setVisible(smartMode);
    
    if (smartMode) {
        updateSmartPackageInfo();
    }
}

void SmartPackageDialog::updateUIState(bool packaging)
{
    m_nameEdit->setEnabled(!packaging);
    m_versionEdit->setEnabled(!packaging);
    m_developerEdit->setEnabled(!packaging);
    m_descriptionEdit->setEnabled(!packaging);
    m_iconPathEdit->setEnabled(!packaging);
    m_outputDirEdit->setEnabled(!packaging);
    m_createInstallerCheck->setEnabled(!packaging);
    m_includeDepsCheck->setEnabled(!packaging);
    m_normalPackageRadio->setEnabled(!packaging);
    m_smartPackageRadio->setEnabled(!packaging);
    m_detectToolsButton->setEnabled(!packaging);
    
    m_startButton->setEnabled(!packaging);
    m_cancelButton->setEnabled(packaging);
    m_closeButton->setEnabled(!packaging);
    
    if (packaging) {
        m_progressBar->setValue(0);
        m_statusLabel->setText("正在打包...");
    }
}

void SmartPackageDialog::updateSmartPackageInfo()
{
    QString infoText = "智能打包模式已启用。\n";
    
    if (m_smartPackageConfig->detectQtPath().isEmpty()) {
        infoText += "• Qt路径: 未检测到（请重新检测工具）\n";
    } else {
        infoText += QString("• Qt路径: %1\n").arg(m_smartPackageConfig->detectQtPath());
    }
    
    if (m_smartPackageConfig->detectMingwPath().isEmpty()) {
        infoText += "• MinGW路径: 未检测到（请重新检测工具）\n";
    } else {
        infoText += QString("• MinGW路径: %1\n").arg(m_smartPackageConfig->detectMingwPath());
    }
    
    if (m_smartPackageConfig->detectCmakePath().isEmpty()) {
        infoText += "• CMake路径: 未检测到（请重新检测工具）\n";
    } else {
        infoText += QString("• CMake路径: %1\n").arg(m_smartPackageConfig->detectCmakePath());
    }
    
    infoText += "\n智能打包将自动解决CMake配置错误，提高打包成功率。";
    
    m_smartPackageInfoLabel->setText(infoText);
}

bool SmartPackageDialog::validateManualPaths()
{
    // 检查手动填入的Qt路径
    if (!m_qtPathEdit->text().isEmpty()) {
        QString qtPath = m_qtPathEdit->text();
        if (!QDir(qtPath).exists()) {
            QMessageBox::warning(this, "路径错误", QString("Qt路径不存在: %1").arg(qtPath));
            return false;
        }
        
        QString qmakePath = qtPath + "/bin/qmake.exe";
        if (!QFile::exists(qmakePath)) {
            qmakePath = qtPath + "/qmake.exe";
            if (!QFile::exists(qmakePath)) {
                QMessageBox::warning(this, "路径错误", 
                    QString("Qt路径无效，未找到qmake.exe: %1\n请检查Qt安装路径是否正确").arg(qtPath));
                return false;
            }
        }
    }
    
    // 检查手动填入的MinGW路径
    if (!m_mingwPathEdit->text().isEmpty()) {
        QString mingwPath = m_mingwPathEdit->text();
        if (!QDir(mingwPath).exists()) {
            QMessageBox::warning(this, "路径错误", QString("MinGW路径不存在: %1").arg(mingwPath));
            return false;
        }
        
        QString gppPath = mingwPath + "/bin/g++.exe";
        if (!QFile::exists(gppPath)) {
            gppPath = mingwPath + "/g++.exe";
            if (!QFile::exists(gppPath)) {
                QMessageBox::warning(this, "路径错误", 
                    QString("MinGW路径无效，未找到g++.exe: %1\n请检查MinGW安装路径是否正确").arg(mingwPath));
                return false;
            }
        }
    }
    
    // 检查手动填入的CMake路径
    if (!m_cmakePathEdit->text().isEmpty()) {
        QString cmakePath = m_cmakePathEdit->text();
        if (!QDir(cmakePath).exists()) {
            QMessageBox::warning(this, "路径错误", QString("CMake路径不存在: %1").arg(cmakePath));
            return false;
        }
        
        QString cmakeExePath = cmakePath + "/bin/cmake.exe";
        if (!QFile::exists(cmakeExePath)) {
            cmakeExePath = cmakePath + "/cmake.exe";
            if (!QFile::exists(cmakeExePath)) {
                QMessageBox::warning(this, "路径错误", 
                    QString("CMake路径无效，未找到cmake.exe: %1\n请检查CMake安装路径是否正确").arg(cmakePath));
                return false;
            }
        }
    }
    
    return true;
}

void SmartPackageDialog::updateProgress(int progress, const QString &message)
{
    onPackageProgress(progress, message);
}

void SmartPackageDialog::packageFinished()
{
    onPackageFinished(true, m_outputDirEdit->text());
}