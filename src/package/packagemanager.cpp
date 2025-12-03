#include "packagemanager.h"
#include "packageservice.h"
#include <QApplication>
#include <QDateTime>
#include <QGridLayout>
#include <QDesktopServices>
#include <QFileInfo>

PackageManager::PackageManager(QWidget *parent)
    : QDialog(parent)
    , m_packageService(new PackageService(this))
    , m_isPackaging(false)
    , m_statusTimer(new QTimer(this))
    , m_currentProgressState(Idle)
{
    // 设置窗口标志，确保有关闭按钮
    setWindowFlags(windowFlags() | Qt::WindowCloseButtonHint);
    
    setupUI();
    setupConnections();
    
    // 初始化配置
    m_currentConfig.name = "MyApplication";
    m_currentConfig.version = "1.0.0";
    m_currentConfig.developer = "Developer";
    m_currentConfig.outputDir = QDir::currentPath() + "/output";
    m_currentConfig.uniqueId = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
    
    updateUIFromConfig();
    
    // 设置状态定时器 - 只在打包过程中显示状态
    m_statusTimer->setInterval(1000);
    connect(m_statusTimer, &QTimer::timeout, this, [this]() {
        // 只在打包过程中显示状态，初始状态下不显示"就绪"
        if (m_isPackaging) {
            // 打包过程中可以显示进度状态
            showStatusMessage("打包进行中...");
        }
    });
    m_statusTimer->start();
    
    // 初始状态下显示欢迎信息，而不是"就绪"
    showStatusMessage("欢迎使用软件打包工具，请配置参数后点击'开始打包'按钮");
}

PackageManager::~PackageManager()
{
    // 停止定时器
    m_statusTimer->stop();
    
    // 断开所有信号连接，避免对象销毁后信号仍然被发送
    if (m_packageService) {
        disconnect(m_packageService, &IPackageService::progressChanged, this, &PackageManager::onProgressChanged);
        disconnect(m_packageService, &IPackageService::packageFinished, this, &PackageManager::onPackageFinished);
        disconnect(m_packageService, &IPackageService::errorOccurred, this, &PackageManager::onErrorOccurred);
    }
    
    // 取消正在进行的打包操作
    if (m_isPackaging && m_packageService) {
        m_packageService->cancelPackage();
    }
}

void PackageManager::setupUI()
{
    // 主布局
    m_mainLayout = new QVBoxLayout(this);
    
    // 配置组
    m_configGroup = new QGroupBox("软件配置");
    QGridLayout *configLayout = new QGridLayout(m_configGroup);
    
    configLayout->addWidget(new QLabel("软件名称:"), 0, 0);
    m_nameEdit = new QLineEdit();
    configLayout->addWidget(m_nameEdit, 0, 1);
    
    configLayout->addWidget(new QLabel("版本号:"), 1, 0);
    m_versionEdit = new QLineEdit();
    configLayout->addWidget(m_versionEdit, 1, 1);
    
    configLayout->addWidget(new QLabel("开发者:"), 2, 0);
    m_developerEdit = new QLineEdit();
    configLayout->addWidget(m_developerEdit, 2, 1);
    
    configLayout->addWidget(new QLabel("输出目录:"), 3, 0);
    m_outputDirEdit = new QLineEdit();
    QPushButton *selectOutputButton = new QPushButton("选择...");
    connect(selectOutputButton, &QPushButton::clicked, this, &PackageManager::onSelectOutputDirClicked);
    
    QHBoxLayout *outputLayout = new QHBoxLayout();
    outputLayout->addWidget(m_outputDirEdit);
    outputLayout->addWidget(selectOutputButton);
    configLayout->addLayout(outputLayout, 3, 1);
    
    configLayout->addWidget(new QLabel("UI布局文件:"), 4, 0);
    m_uiLayoutEdit = new QLineEdit();
    QPushButton *selectUILayoutButton = new QPushButton("选择...");
    connect(selectUILayoutButton, &QPushButton::clicked, this, &PackageManager::onSelectUILayoutClicked);
    
    QHBoxLayout *uiLayout = new QHBoxLayout();
    uiLayout->addWidget(m_uiLayoutEdit);
    uiLayout->addWidget(selectUILayoutButton);
    configLayout->addLayout(uiLayout, 4, 1);
    
    m_createInstallerCheck = new QCheckBox("创建安装包");
    configLayout->addWidget(m_createInstallerCheck, 5, 1);
    
    m_configGroup->setLayout(configLayout);
    
    // 工具组
    m_toolsGroup = new QGroupBox("开发工具");
    QGridLayout *toolsLayout = new QGridLayout(m_toolsGroup);
    
    toolsLayout->addWidget(new QLabel("Qt路径:"), 0, 0);
    m_qtDirEdit = new QLineEdit();
    toolsLayout->addWidget(m_qtDirEdit, 0, 1);
    
    toolsLayout->addWidget(new QLabel("MinGW路径:"), 1, 0);
    m_mingwDirEdit = new QLineEdit();
    toolsLayout->addWidget(m_mingwDirEdit, 1, 1);
    
    toolsLayout->addWidget(new QLabel("CMake路径:"), 2, 0);
    m_cmakeDirEdit = new QLineEdit();
    toolsLayout->addWidget(m_cmakeDirEdit, 2, 1);
    
    m_detectToolsButton = new QPushButton("自动检测工具");
    toolsLayout->addWidget(m_detectToolsButton, 3, 1);
    
    m_toolsGroup->setLayout(toolsLayout);
    
    // 打包组
    m_packageGroup = new QGroupBox("打包控制");
    QVBoxLayout *packageLayout = new QVBoxLayout(m_packageGroup);
    
    m_packageButton = new QPushButton("开始打包");
    m_packageButton->setMinimumHeight(40);
    packageLayout->addWidget(m_packageButton);
    
    m_progressBar = new QProgressBar();
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    packageLayout->addWidget(m_progressBar);
    
    m_logTextEdit = new QTextEdit();
    m_logTextEdit->setMaximumHeight(200);
    m_logTextEdit->setReadOnly(true);
    packageLayout->addWidget(m_logTextEdit);
    
    m_packageGroup->setLayout(packageLayout);
    
    // 添加到主布局
    m_mainLayout->addWidget(m_configGroup);
    m_mainLayout->addWidget(m_toolsGroup);
    m_mainLayout->addWidget(m_packageGroup);
    
    setWindowTitle("软件打包工具");
    resize(600, 700);
}

void PackageManager::setupConnections()
{
    connect(m_detectToolsButton, &QPushButton::clicked, this, &PackageManager::onDetectToolsClicked);
    connect(m_packageButton, &QPushButton::clicked, this, &PackageManager::onPackageClicked);
    
    // 连接逻辑层信号
    connect(m_packageService, &IPackageService::progressChanged, this, &PackageManager::onProgressChanged);
    connect(m_packageService, &IPackageService::packageFinished, this, &PackageManager::onPackageFinished);
    connect(m_packageService, &IPackageService::errorOccurred, this, &PackageManager::onErrorOccurred);
}

void PackageManager::onDetectToolsClicked()
{
    // 设置状态为检测开发工具
    m_currentProgressState = DetectingTools;
    
    // 重置进度条为0，准备开始检测
    m_progressBar->setValue(0);
    
    // 不在这里显示开始消息，因为detectDevelopmentTools内部会通过progressChanged信号发送
    // 这样可以避免"开始检测开发工具路径..."消息重复打印
    
    // 调用逻辑层检测工具
    if (m_packageService->detectDevelopmentTools()) {
        PackageConfig config = m_packageService->getCurrentConfig();
        
        // 更新UI显示
        m_qtDirEdit->setText(config.qtDir);
        m_mingwDirEdit->setText(config.mingwDir);
        m_cmakeDirEdit->setText(config.cmakeDir);
        
        // 检测完成后，进度条显示100%
        m_progressBar->setValue(100);
        showStatusMessage("开发工具检测完成，进度100%");
        
        // 重置状态为空闲
        m_currentProgressState = Idle;
    } else {
        showStatusMessage("开发工具检测失败: " + m_packageService->getLastError(), true);
        
        // 检测失败，重置状态为空闲
        m_currentProgressState = Idle;
    }
}

void PackageManager::onSelectOutputDirClicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, "选择输出目录", QDir::homePath());
    if (!dir.isEmpty()) {
        m_outputDirEdit->setText(dir);
    }
}

void PackageManager::onSelectUILayoutClicked()
{
    QString file = QFileDialog::getOpenFileName(this, "选择UI布局文件", QDir::homePath(), "UI Files (*.ui *.qml)");
    if (!file.isEmpty()) {
        m_uiLayoutEdit->setText(file);
    }
}

void PackageManager::setConfiguration(const Product &product, const PackageConfig &config)
{
    // 更新配置
    m_currentConfig = config;
    updateUIFromConfig();
    
    // 检查必要配置
    if (m_currentConfig.uiLayoutPath.isEmpty()) {
        QMessageBox::warning(this, "警告", "UI布局文件路径为空");
        return;
    }
    
    if (m_currentConfig.outputDir.isEmpty()) {
        QMessageBox::warning(this, "警告", "输出目录为空");
        return;
    }
    
    // 显示配置已加载的消息
    showStatusMessage("配置已加载，请点击'开始打包'按钮启动打包过程");
}

void PackageManager::onPackageClicked()
{
    if (m_isPackaging) {
        // 如果正在打包，显示取消确认
        if (QMessageBox::question(this, "确认取消", "打包正在进行中，确定要取消吗？") == QMessageBox::Yes) {
            // 这里可以添加取消逻辑
            m_isPackaging = false;
            m_packageButton->setText("开始打包");
            showStatusMessage("打包已取消");
            
            // 重置进度条状态
            m_currentProgressState = Idle;
            m_progressBar->setValue(0);
        }
        return;
    }
    
    // 更新配置
    updateConfigFromUI();
    
    // 验证配置
    if (!m_packageService->validateConfiguration()) {
        showStatusMessage("配置验证失败: " + m_packageService->getLastError(), true);
        return;
    }
    
    // 开始打包
    m_isPackaging = true;
    m_currentProgressState = Packaging;
    m_packageButton->setText("取消打包");
    m_logTextEdit->clear();
    
    // 重置进度条为0，准备开始打包
    m_progressBar->setValue(0);
    
    showStatusMessage("开始打包过程...");
    
    // 调用逻辑层打包
    bool success = m_packageService->executePackage(m_currentConfig);
    
    if (!success) {
        m_isPackaging = false;
        m_packageButton->setText("开始打包");
        
        // 打包失败，重置状态为空闲
        m_currentProgressState = Idle;
    }
}

void PackageManager::startSmartPackage(const Product &product, const PackageConfig &config)
{
    // 更新配置
    m_currentConfig = config;
    updateUIFromConfig();
    
    // 检查必要配置
    if (m_currentConfig.uiLayoutPath.isEmpty()) {
        QMessageBox::warning(this, "警告", "UI布局文件路径为空");
        return;
    }
    
    if (m_currentConfig.outputDir.isEmpty()) {
        QMessageBox::warning(this, "警告", "输出目录为空");
        return;
    }
    
    // 显示配置已加载的消息，等待用户点击"开始打包"按钮
    showStatusMessage("智能打包配置已加载，请点击'开始打包'按钮启动打包过程");
}

void PackageManager::startPackage(const PackageConfig &config)
{
    // 更新配置
    m_currentConfig = config;
    updateUIFromConfig();
    
    // 检查必要配置
    if (m_currentConfig.uiLayoutPath.isEmpty()) {
        QMessageBox::warning(this, "警告", "UI布局文件路径为空");
        return;
    }
    
    if (m_currentConfig.outputDir.isEmpty()) {
        QMessageBox::warning(this, "警告", "输出目录为空");
        return;
    }
    
    // 显示配置已加载的消息，等待用户点击"开始打包"按钮
    showStatusMessage("打包配置已加载，请点击'开始打包'按钮启动打包过程");
}

void PackageManager::cancelPackage()
{
    if (m_packageService) {
        m_packageService->cancelPackage();
    }
    
    m_isPackaging = false;
    showStatusMessage("打包已取消");
}

void PackageManager::onProgressChanged(int value, const QString &message)
{
    if (value >= 0) {
        m_progressBar->setValue(value);
    }
    
    // 根据当前状态显示不同的进度信息
    QString statusMessage = message;
    
    switch (m_currentProgressState) {
    case DetectingTools:
        statusMessage = "[检测工具] " + message;
        break;
    case Packaging:
        statusMessage = "[打包过程] " + message;
        break;
    case Idle:
    default:
        // 空闲状态下不添加前缀
        break;
    }
    
    // 只显示状态消息，不重复添加日志
    // showStatusMessage方法内部已经会添加日志
    showStatusMessage(statusMessage);
}

void PackageManager::onPackageFinished(bool success, const QString &outputPath)
{
    m_isPackaging = false;
    m_packageButton->setText("开始打包");
    
    if (success) {
        m_progressBar->setValue(100);
        showStatusMessage("[打包过程] 打包完成！输出路径: " + outputPath);
        
        // 显示成功消息
        QMessageBox::information(this, "打包完成", "软件打包成功！\n输出路径: " + outputPath);
        
        // 检查是否为NSIS安装包路径，如果是则提示用户是否打开所在路径
        if (outputPath.endsWith("_Setup.exe") || outputPath.endsWith("_Installer.exe")) {
            QFileInfo fileInfo(outputPath);
            QString installerDir = fileInfo.absolutePath();
            
            QMessageBox::StandardButton reply = QMessageBox::question(this, 
                "打开安装包路径", 
                "NSIS安装包生成成功！\n是否要打开安装包所在路径？",
                QMessageBox::Yes | QMessageBox::No);
            
            if (reply == QMessageBox::Yes) {
                // 使用系统默认文件管理器打开安装包所在路径
                QDesktopServices::openUrl(QUrl::fromLocalFile(installerDir));
            }
        }
        
        // 打包完成后重置状态为空闲
        m_currentProgressState = Idle;
    } else {
        showStatusMessage("打包失败", true);
        
        // 打包失败后重置状态为空闲
        m_currentProgressState = Idle;
    }
}

void PackageManager::onErrorOccurred(const QString &error)
{
    showStatusMessage("错误: " + error, true);
    
    // 添加错误日志
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    m_logTextEdit->append("[" + timestamp + "] 错误: " + error);
}

void PackageManager::updateConfigFromUI()
{
    m_currentConfig.name = m_nameEdit->text();
    m_currentConfig.version = m_versionEdit->text();
    m_currentConfig.developer = m_developerEdit->text();
    m_currentConfig.outputDir = m_outputDirEdit->text();
    m_currentConfig.uiLayoutPath = m_uiLayoutEdit->text();
    m_currentConfig.createInstaller = m_createInstallerCheck->isChecked();
    m_currentConfig.qtDir = m_qtDirEdit->text();
    m_currentConfig.mingwDir = m_mingwDirEdit->text();
    m_currentConfig.cmakeDir = m_cmakeDirEdit->text();
    
    // 更新逻辑层配置
    m_packageService->updateConfig(m_currentConfig);
}

void PackageManager::updateUIFromConfig()
{
    m_nameEdit->setText(m_currentConfig.name);
    m_versionEdit->setText(m_currentConfig.version);
    m_developerEdit->setText(m_currentConfig.developer);
    m_outputDirEdit->setText(m_currentConfig.outputDir);
    m_uiLayoutEdit->setText(m_currentConfig.uiLayoutPath);
    m_createInstallerCheck->setChecked(m_currentConfig.createInstaller);
    m_qtDirEdit->setText(m_currentConfig.qtDir);
    m_mingwDirEdit->setText(m_currentConfig.mingwDir);
    m_cmakeDirEdit->setText(m_currentConfig.cmakeDir);
}

void PackageManager::showStatusMessage(const QString &message, bool isError)
{
    // 这里可以添加状态栏显示逻辑
    if (isError) {
        m_logTextEdit->append("错误: " + message);
    } else {
        // 正常状态消息
        QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
        m_logTextEdit->append("[" + timestamp + "] " + message);
    }
    
    // 确保日志可见
    QTextCursor cursor = m_logTextEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    m_logTextEdit->setTextCursor(cursor);
}