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
#include <QGuiApplication>
#include <QScreen>
#include <QWindow>
#include <QResizeEvent>

SmartPackageDialog::SmartPackageDialog(QWidget *parent)
    : QDialog(parent)
    , m_packageManager(new PackageManager(this))
    , m_smartPackageConfig(new SmartPackageConfig(this))
{
    // 设置窗口标志，减少拖动时的闪烁
    setWindowFlags(windowFlags() | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    
    // 优化窗口属性，减少重绘但保持背景正常显示
    setAttribute(Qt::WA_StaticContents);
    
    setupUI();
    setupConnections();
    setWindowTitle("智能软件打包");
    
    // 设置最大尺寸限制，避免超过显示器分辨率
    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->availableGeometry();
        int maxWidth = screenGeometry.width() * 0.8;  // 屏幕宽度的80%
        int maxHeight = screenGeometry.height() * 0.6; // 屏幕高度的80%
        
        // setMaximumSize(maxWidth, maxHeight);
    } else {
        // 备用方案：设置合理的最大尺寸
        // setMaximumSize(1200, 950); // 确保高度足够容纳布局
    }
    
    // 设置窗口背景色，确保正常显示
    setStyleSheet("QDialog { background-color: palette(window); }");
    
    // 连接窗口标题变化信号，用于调试
    connect(this, &QDialog::windowTitleChanged, this, [this](const QString &title) {
        qDebug() << "窗口标题变化:" << title << "当前大小:" << size();
    });
    
    // 记录初始窗口大小
    qDebug() << "智能打包对话框创建，初始大小:" << size();
    
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
    
    // 设置默认输出目录 - 使用uniqueId而不是产品名称，避免中文路径问题
    QString defaultOutputDir = QDir::currentPath() + "/smart_packages/" + product.uniqueId();
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
    
    // 从当前产品信息中获取配置，确保uniqueId等关键信息正确
    settings.name = m_product.name();
    settings.version = m_product.version();
    settings.developer = m_product.developer();
    settings.description = m_product.description();
    settings.iconPath = m_product.iconPath();
    settings.uniqueId = m_product.uniqueId(); // 关键：从产品信息获取uniqueId
    settings.uiLayoutPath = m_product.uiLayoutPath(); // 关键：从产品信息获取UI布局文件路径
    
    // 从UI获取用户设置的路径和选项
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
    
    // 设置布局间距和边距，避免信息堆叠
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    
    // 基本信息组
    QGroupBox *infoGroup = new QGroupBox("软件信息", this);
    QFormLayout *infoLayout = new QFormLayout(infoGroup);
    infoLayout->setSpacing(8);
    infoLayout->setContentsMargins(10, 10, 10, 10);
    
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
    modeLayout->setSpacing(8);
    modeLayout->setContentsMargins(10, 10, 10, 10);
    
    m_normalPackageRadio = new QRadioButton("标准打包模式", this);
    m_smartPackageRadio = new QRadioButton("智能打包模式（推荐）", this);
    
    modeLayout->addWidget(m_normalPackageRadio);
    modeLayout->addWidget(m_smartPackageRadio);
    
    // 智能打包选项组
    m_smartPackageGroup = new QGroupBox("智能打包配置", this);
    QFormLayout *smartLayout = new QFormLayout(m_smartPackageGroup);
    smartLayout->setSpacing(8);
    smartLayout->setContentsMargins(10, 10, 10, 10);
    
    // Qt路径设置
    m_qtPathEdit = new QLineEdit(this);
    QPushButton *qtBrowseButton = new QPushButton("浏览...", this);
    QHBoxLayout *qtLayout = new QHBoxLayout();
    qtLayout->setSpacing(5);
    qtLayout->addWidget(m_qtPathEdit);
    qtLayout->addWidget(qtBrowseButton);
    
    // MinGW路径设置
    m_mingwPathEdit = new QLineEdit(this);
    QPushButton *mingwBrowseButton = new QPushButton("浏览...", this);
    QHBoxLayout *mingwLayout = new QHBoxLayout();
    mingwLayout->setSpacing(5);
    mingwLayout->addWidget(m_mingwPathEdit);
    mingwLayout->addWidget(mingwBrowseButton);
    
    // CMake路径设置
    m_cmakePathEdit = new QLineEdit(this);
    QPushButton *cmakeBrowseButton = new QPushButton("浏览...", this);
    QHBoxLayout *cmakeLayout = new QHBoxLayout();
    cmakeLayout->setSpacing(5);
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
    optionsLayout->setSpacing(8);
    optionsLayout->setContentsMargins(10, 10, 10, 10);
    
    m_iconPathEdit = new QLineEdit(this);
    QPushButton *iconBrowseButton = new QPushButton("浏览...", this);
    QHBoxLayout *iconLayout = new QHBoxLayout();
    iconLayout->setSpacing(5);
    iconLayout->addWidget(m_iconPathEdit);
    iconLayout->addWidget(iconBrowseButton);
    
    m_outputDirEdit = new QLineEdit(this);
    QPushButton *outputBrowseButton = new QPushButton("浏览...", this);
    QHBoxLayout *outputLayout = new QHBoxLayout();
    outputLayout->setSpacing(5);
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
    m_smartPackageInfoLabel->setStyleSheet("QLabel { color: #2E8B57; font-weight: bold; margin: 5px; }");
    m_smartPackageInfoLabel->setContentsMargins(10, 5, 10, 5);
    
    // 进度显示
    QGroupBox *progressGroup = new QGroupBox("打包进度", this);
    QVBoxLayout *progressLayout = new QVBoxLayout(progressGroup);
    progressLayout->setSpacing(8);
    progressLayout->setContentsMargins(10, 10, 10, 10);
    
    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    
    m_statusLabel = new QLabel("准备打包...", this);
    
    progressLayout->addWidget(m_progressBar);
    progressLayout->addWidget(m_statusLabel);
    
    // 按钮组
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(10);
    
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
        SmartPackageConfig::SmartPackageSettings normalSettings;
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
    updateProgress(progress, message);
}

void SmartPackageDialog::updateProgress(int progress, const QString &message)
{
    m_progressBar->setValue(progress);
    
    // 优先使用传入的详细消息，如果没有则根据进度值显示默认步骤信息
    QString stepInfo = message;
    if (message.isEmpty()) {
        if (progress <= 10) {
            stepInfo = "正在准备打包环境...";
        } else if (progress <= 20) {
            stepInfo = "正在验证开发工具路径...";
        } else if (progress <= 30) {
            stepInfo = "正在创建输出目录...";
        } else if (progress <= 40) {
            stepInfo = "正在生成CMake项目文件...";
        } else if (progress <= 50) {
            stepInfo = "正在生成主程序代码...";
        } else if (progress <= 60) {
            stepInfo = "正在生成构建脚本...";
        } else if (progress <= 70) {
            stepInfo = "正在执行CMake配置...";
        } else if (progress <= 80) {
            stepInfo = "正在编译项目...";
        } else if (progress <= 85) {
            stepInfo = "正在检查可执行文件生成...";
        } else if (progress <= 90) {
            stepInfo = "正在复制依赖文件...";
        } else if (progress <= 95) {
            stepInfo = "正在设置应用程序图标...";
        } else if (progress <= 99) {
            stepInfo = "正在创建安装包...";
        } else {
            stepInfo = "打包完成！";
        }
    }
    
    m_statusLabel->setText(QString("%1 (%2%)").arg(stepInfo).arg(progress));
}

void SmartPackageDialog::onPackageFinished(bool success, const QString &resultPath)
{
    updateUIState(false);
    
    if (success) {
        m_statusLabel->setText("打包完成");
        m_progressBar->setValue(100);
        
        // resultPath 可能是EXE文件或安装包的完整路径
        QString filePath = resultPath;
        QString outputDir = QFileInfo(filePath).absolutePath();
        qDebug()<<"filePath:"<<filePath;
        qDebug()<<"outputDir:"<<outputDir;
        QString successMessage;
        
        // 检查是否生成了NSIS安装包
        bool installerGenerated = filePath.endsWith("_Setup.exe");
        
        if (installerGenerated) {
            // NSIS安装包生成成功
            successMessage = QString(
                "智能打包完成！\n"
                "软件名称: %1\n"
                "版本号: %2\n"
                "输出目录: %3\n"
                "安装包文件: %4\n"
                "\n生成的文件包括:\n"
                "• NSIS安装包 (.exe)\n"
                "• 可执行文件 (.exe)\n"
                "• CMakeLists.txt (项目配置文件)\n"
                "• main.cpp (主程序文件)\n"
                "• build.bat (构建脚本)\n"
                "• 完整的项目源代码"
            ).arg(m_nameEdit->text(), m_versionEdit->text(), outputDir, filePath);
        } else if (QFile::exists(filePath) && filePath.endsWith(".exe")) {
            // 智能打包成功生成了可执行文件，但没有生成安装包
            successMessage = QString(
                "智能打包完成！\n"
                "软件名称: %1\n"
                "版本号: %2\n"
                "输出目录: %3\n"
                "可执行文件: %4\n"
                "\n生成的文件包括:\n"
                "• 可执行文件 (.exe)\n"
                "• CMakeLists.txt (项目配置文件)\n"
                "• main.cpp (主程序文件)\n"
                "• build.bat (构建脚本)\n"
                "• 完整的项目源代码"
            ).arg(m_nameEdit->text(), m_versionEdit->text(), outputDir, filePath);
        } else {
            // 智能打包生成了项目文件，但EXE文件不存在，这是正常情况
            // 智能打包只负责生成项目文件，不包含自动构建流程
            successMessage = QString(
                "智能打包项目生成完成！\n"
                "软件名称: %1\n"
                "版本号: %2\n"
                "输出目录: %3\n"
                "\n生成的项目文件包括:\n"
                "• CMakeLists.txt (项目配置文件)\n"
                "• main.cpp (主程序文件)\n"
                "• build.bat (构建脚本)\n"
                "• 完整的项目源代码"
            ).arg(m_nameEdit->text(), m_versionEdit->text(), outputDir);
        }
        
        QMessageBox::information(this, "智能打包完成", successMessage);
        
        // 询问是否打开输出目录
        QMessageBox::StandardButton openDirReply = QMessageBox::question(this, "打开输出目录", 
            "是否打开输出目录查看生成的文件?");
        
        if (openDirReply == QMessageBox::Yes) {
            QDesktopServices::openUrl(QUrl::fromLocalFile(outputDir));
        }
    } else {
        QMessageBox::critical(this, "智能打包失败", 
            "智能打包过程中出现错误。\n"
            "请检查以下可能的问题:\n"
            "• 开发工具路径配置是否正确\n"
            "• Qt、MinGW、CMake是否已正确安装\n"
            "• 磁盘空间是否充足\n"
            "• 项目名称是否包含特殊字符\n"
            "• NSIS安装包生成是否失败（如果启用了安装包生成）");
    }
}

void SmartPackageDialog::onPackageError(const QString &error)
{
    updateUIState(false);
    m_statusLabel->setText("打包错误");
    
    // 分析错误信息，确定当前卡在哪个步骤
    QString stepInfo = "未知步骤";
    QString detailedError = error;
    
    if (error.contains("CMake")) {
        stepInfo = "CMake项目生成阶段";
    } else if (error.contains("main.cpp")) {
        stepInfo = "主程序文件生成阶段";
    } else if (error.contains("build.bat")) {
        stepInfo = "构建脚本生成阶段";
    } else if (error.contains("UI布局")) {
        stepInfo = "UI布局文件处理阶段";
    } else if (error.contains("输出目录")) {
        stepInfo = "输出目录创建阶段";
    } else if (error.contains("依赖")) {
        stepInfo = "依赖文件处理阶段";
    } else if (error.contains("图标")) {
        stepInfo = "应用程序图标设置阶段";
    }
    
    // 显示详细的错误信息
    QString errorMessage = QString(
        "智能打包过程中出现错误！\n"
        "当前卡在: %1\n"
        "\n错误详情:\n"
        "%2\n"
        "\n建议的解决方案:\n"
        "• 检查开发工具路径配置\n"
        "• 验证Qt、MinGW、CMake安装\n"
        "• 确保磁盘空间充足\n"
        "• 检查项目名称是否包含特殊字符\n"
        "• 查看详细构建日志"
    ).arg(stepInfo, detailedError);
    
    QMessageBox::critical(this, "智能打包错误", errorMessage);
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



void SmartPackageDialog::packageFinished()
{
    onPackageFinished(true, m_outputDirEdit->text());
}

void SmartPackageDialog::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    
    // 记录窗口大小变化
    qDebug() << "窗口大小变化事件触发:";
    qDebug() << "  旧大小:" << event->oldSize();
    qDebug() << "  新大小:" << event->size();
    qDebug() << "  最小大小:" << minimumSize();
    qDebug() << "  最大大小:" << maximumSize();
    qDebug() << "  实际大小:" << size();
    
    // 检查是否有布局变化
    if (layout()) {
        qDebug() << "  布局大小提示:" << layout()->sizeHint();
        qDebug() << "  布局最小大小:" << layout()->minimumSize();
        qDebug() << "  布局最大大小:" << layout()->maximumSize();
    }
}