#include "packagedialog.h"
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

PackageDialog::PackageDialog(QWidget *parent)
    : QDialog(parent)
    , m_packageManager(new PackageManager(this))
    , m_packageConfig(new PackageConfig(this))
{
    setupUI();
    setupConnections();
    setWindowTitle("软件打包");
    setMinimumSize(500, 400);
}

PackageDialog::~PackageDialog()
{
}

void PackageDialog::setProductInfo(const QJsonObject &productInfo)
{
    m_packageConfig->loadFromProduct(productInfo);
    
    PackageConfig::PackageSettings settings = m_packageConfig->getPackageSettings();
    
    m_nameEdit->setText(settings.name);
    m_versionEdit->setText(settings.version);
    m_developerEdit->setText(settings.developer);
    m_descriptionEdit->setText(settings.description);
    m_iconPathEdit->setText(settings.iconPath);
    m_outputDirEdit->setText(settings.outputDir);
    m_createInstallerCheck->setChecked(settings.createInstaller);
    m_includeDepsCheck->setChecked(settings.includeDependencies);
}

void PackageDialog::setProductInfo(const Product &product)
{
    // 存储Product对象
    m_product = product;
    
    // 从Product对象创建PackageSettings
    PackageConfig::PackageSettings settings;
    
    settings.name = product.name();
    settings.version = product.version();
    settings.developer = product.developer();
    settings.description = product.description();
    settings.iconPath = product.iconPath();
    
    // 设置默认输出目录
    QString defaultOutputDir = QDir::currentPath() + "/packages/" + product.name();
    settings.outputDir = defaultOutputDir;
    
    // 默认启用依赖包含
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
}

PackageConfig::PackageSettings PackageDialog::getPackageSettings() const
{
    PackageConfig::PackageSettings settings;
    
    settings.name = m_nameEdit->text();
    settings.version = m_versionEdit->text();
    settings.developer = m_developerEdit->text();
    settings.description = m_descriptionEdit->toPlainText();
    settings.iconPath = m_iconPathEdit->text();
    settings.outputDir = m_outputDirEdit->text();
    settings.createInstaller = m_createInstallerCheck->isChecked();
    settings.includeDependencies = m_includeDepsCheck->isChecked();
    
    return settings;
}

void PackageDialog::setupUI()
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
    
    // 打包信息组
    QGroupBox *packageInfoGroup = new QGroupBox("打包信息", this);
    QVBoxLayout *packageInfoLayout = new QVBoxLayout(packageInfoGroup);
    
    QLabel *infoLabel = new QLabel("打包过程将:", this);
    QLabel *step1 = new QLabel("• 将UI布局转换为C++代码", this);
    QLabel *step2 = new QLabel("• 生成完整的Qt项目文件", this);
    QLabel *step3 = new QLabel("• 使用CMake构建可执行文件", this);
    QLabel *step4 = new QLabel("• 包含必要的依赖文件", this);
    QLabel *step5 = new QLabel("• 可选创建安装包", this);
    
    packageInfoLayout->addWidget(infoLabel);
    packageInfoLayout->addWidget(step1);
    packageInfoLayout->addWidget(step2);
    packageInfoLayout->addWidget(step3);
    packageInfoLayout->addWidget(step4);
    packageInfoLayout->addWidget(step5);
    
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
    mainLayout->addWidget(optionsGroup);
    mainLayout->addWidget(packageInfoGroup);
    mainLayout->addWidget(progressGroup);
    mainLayout->addLayout(buttonLayout);
    
    // 连接按钮信号
    connect(iconBrowseButton, &QPushButton::clicked, this, &PackageDialog::onBrowseIcon);
    connect(outputBrowseButton, &QPushButton::clicked, this, &PackageDialog::onBrowseOutputDir);
    connect(m_startButton, &QPushButton::clicked, this, &PackageDialog::onStartPackage);
    connect(m_cancelButton, &QPushButton::clicked, this, &PackageDialog::onCancelPackage);
    connect(m_closeButton, &QPushButton::clicked, this, &QDialog::reject);
    
    // 初始状态
    updateUIState(false);
}

void PackageDialog::setupConnections()
{
    connect(m_packageManager, &PackageManager::progressChanged, 
            this, &PackageDialog::onPackageProgress);
    connect(m_packageManager, &PackageManager::packageFinished, 
            this, &PackageDialog::onPackageFinished);
    connect(m_packageManager, &PackageManager::errorOccurred, 
            this, &PackageDialog::onPackageError);
}

void PackageDialog::onBrowseOutputDir()
{
    QString dir = QFileDialog::getExistingDirectory(this, "选择输出目录", 
                                                   m_outputDirEdit->text());
    if (!dir.isEmpty()) {
        m_outputDirEdit->setText(dir);
    }
}

void PackageDialog::onBrowseIcon()
{
    QString file = QFileDialog::getOpenFileName(this, "选择图标文件", 
                                               "", "图标文件 (*.ico *.png *.jpg)");
    if (!file.isEmpty()) {
        m_iconPathEdit->setText(file);
    }
}

void PackageDialog::onStartPackage()
{
    PackageConfig::PackageSettings settings = getPackageSettings();
    
    // 验证设置
    if (settings.name.isEmpty()) {
        QMessageBox::warning(this, "错误", "软件名称不能为空");
        return;
    }
    
    if (settings.version.isEmpty()) {
        QMessageBox::warning(this, "错误", "版本号不能为空");
        return;
    }
    
    if (settings.developer.isEmpty()) {
        QMessageBox::warning(this, "错误", "开发者不能为空");
        return;
    }
    
    if (settings.outputDir.isEmpty()) {
        QMessageBox::warning(this, "错误", "输出目录不能为空");
        return;
    }
    
    // 检查UI布局文件是否存在 - 使用Product对象中的uiLayoutPath字段
    QString uiLayoutPath = m_product.uiLayoutPath();
    if (uiLayoutPath.isEmpty()) {
        QMessageBox::warning(this, "错误", "产品未绑定UI布局文件，请先保存UI布局");
        return;
    }
    
    if (!QFile::exists(uiLayoutPath)) {
        QMessageBox::warning(this, "错误", QString("UI布局文件不存在: %1\n请确保UI布局文件已正确保存").arg(uiLayoutPath));
        return;
    }
    
    // 检查输出目录是否可写
    QDir outputDir(settings.outputDir);
    if (!outputDir.exists()) {
        if (!outputDir.mkpath(".")) {
            QMessageBox::warning(this, "错误", QString("无法创建输出目录: %1").arg(settings.outputDir));
            return;
        }
    }
    
    // 检查打包工具是否可用
    if (!m_packageManager->isPackagingToolsAvailable()) {
        QMessageBox::warning(this, "错误", 
            "打包工具不可用，请确保以下工具已安装:\n"
            "• CMake (版本3.16或更高)\n"
            "• MinGW或Visual Studio构建工具\n"
            "• Qt6开发环境");
        return;
    }
    
    // 确认对话框
    QMessageBox::StandardButton reply = QMessageBox::question(this, "确认打包", 
        QString("即将开始打包:\n"
                "软件名称: %1\n"
                "版本号: %2\n"
                "输出目录: %3\n"
                "\n是否继续?").arg(settings.name, settings.version, settings.outputDir));
    
    if (reply != QMessageBox::Yes) {
        return;
    }
    
    // 开始打包
    updateUIState(true);
    emit packageRequested(m_product, settings);
}

void PackageDialog::onCancelPackage()
{
    if (m_packageManager->isPackaging()) {
        m_packageManager->cancelPackage();
    }
    updateUIState(false);
}

void PackageDialog::onPackageProgress(int progress, const QString &message)
{
    m_progressBar->setValue(progress);
    m_statusLabel->setText(message);
    
    // 更新状态栏显示详细信息
    QString detailedMessage = QString("进度: %1% - %2").arg(progress).arg(message);
    qDebug() << detailedMessage;
}

void PackageDialog::onPackageFinished(bool success, const QString &resultPath)
{
    updateUIState(false);
    
    if (success) {
        // 显示详细的成功信息
        QString successMessage = QString(
            "打包成功完成！\n"
            "软件名称: %1\n"
            "版本号: %2\n"
            "输出目录: %3\n"
            "\n生成的文件包括:\n"
            "• %4.exe (可执行文件)\n"
            "• 依赖的DLL文件\n"
            "• 完整的项目源代码"
        ).arg(m_nameEdit->text(), m_versionEdit->text(), resultPath, m_nameEdit->text());
        
        QMessageBox::information(this, "打包完成", successMessage);
        
        // 询问是否打开输出目录
        QMessageBox::StandardButton openDirReply = QMessageBox::question(this, "打开输出目录", 
            "是否打开输出目录查看生成的文件?");
        
        if (openDirReply == QMessageBox::Yes) {
            QDesktopServices::openUrl(QUrl::fromLocalFile(resultPath));
        }
    } else {
        QMessageBox::critical(this, "打包失败", 
            "打包过程中出现错误。\n"
            "请检查以下可能的问题:\n"
            "• CMake配置是否正确\n"
            "• Qt开发环境是否完整\n"
            "• 构建工具是否可用\n"
            "• 磁盘空间是否充足");
    }
}

void PackageDialog::onPackageError(const QString &error)
{
    // 显示详细的错误信息
    QString errorMessage = QString(
        "打包过程中出现错误:\n"
        "%1\n"
        "\n建议的解决方案:\n"
        "• 检查CMake和Qt安装\n"
        "• 验证构建工具配置\n"
        "• 查看详细日志信息"
    ).arg(error);
    
    QMessageBox::critical(this, "打包错误", errorMessage);
}

void PackageDialog::updateProgress(int progress, const QString &message)
{
    m_progressBar->setValue(progress);
    m_statusLabel->setText(message);
}

void PackageDialog::packageFinished()
{
    updateUIState(false);
}

void PackageDialog::updateUIState(bool packaging)
{
    m_nameEdit->setEnabled(!packaging);
    m_versionEdit->setEnabled(!packaging);
    m_developerEdit->setEnabled(!packaging);
    m_descriptionEdit->setEnabled(!packaging);
    m_iconPathEdit->setEnabled(!packaging);
    m_outputDirEdit->setEnabled(!packaging);
    m_createInstallerCheck->setEnabled(!packaging);
    m_includeDepsCheck->setEnabled(!packaging);
    
    m_startButton->setEnabled(!packaging);
    m_cancelButton->setEnabled(packaging);
    m_closeButton->setEnabled(!packaging);
    
    if (packaging) {
        m_progressBar->setValue(0);
        m_statusLabel->setText("正在打包...");
    }
}