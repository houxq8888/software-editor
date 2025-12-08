#include "packageconfig.h"
#include "packageservice.h"
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QDateTime>

PackageConfigManager::PackageConfigManager(QObject *parent)
    : QObject(parent)
    , m_settings(new QSettings(QSettings::IniFormat, QSettings::UserScope, 
                              "SoftwareEditor", "PackageConfig", this))
{
    initializeDefaultConfig();
}

bool PackageConfigManager::loadConfig(const QString &configName)
{
    m_lastError.clear();
    
    QString configPath = getConfigFilePath(configName);
    QSettings configFile(configPath, QSettings::IniFormat);
    
    if (!QFile::exists(configPath)) {
        m_lastError = QString("配置文件不存在: %1").arg(configPath);
        return false;
    }
    
    // 加载基本配置
    m_currentConfig.name = configFile.value("Software/name", "MyApplication").toString();
    m_currentConfig.version = configFile.value("Software/version", "1.0.0").toString();
    m_currentConfig.developer = configFile.value("Software/developer", "Developer").toString();
    m_currentConfig.uniqueId = configFile.value("Software/uniqueId", 
        QDateTime::currentDateTime().toString("yyyyMMddhhmmss")).toString();
    m_currentConfig.outputDir = configFile.value("Software/outputDirectory",
        QDir::homePath() + "/SoftwareOutput").toString();
    m_currentConfig.uiLayoutPath = configFile.value("Software/uiLayoutPath", "").toString();
    m_currentConfig.createInstaller = configFile.value("Software/createInstaller", true).toBool();
    
    // 加载开发工具路径
    m_currentConfig.qtDir = configFile.value("DevelopmentTools/qtDir", "").toString();
    m_currentConfig.mingwDir = configFile.value("DevelopmentTools/mingwDir", "").toString();
    m_currentConfig.cmakeDir = configFile.value("DevelopmentTools/cmakeDir", "").toString();
    
    // 加载高级选项
    m_currentConfig.iconPath = configFile.value("Advanced/iconPath", "").toString();
    m_currentConfig.description = configFile.value("Advanced/description", "").toString();
    m_currentConfig.copyright = configFile.value("Advanced/copyright", "").toString();
    
    // 验证配置
    if (!validateConfig(m_currentConfig)) {
        m_lastError = QString("配置验证失败: %1").arg(m_lastError);
        return false;
    }
    
    emit configLoaded(m_currentConfig);
    return true;
}

bool PackageConfigManager::saveConfig(const QString &configName)
{
    m_lastError.clear();
    
    QString configPath = getConfigFilePath(configName);
    QSettings configFile(configPath, QSettings::IniFormat);
    
    // 确保配置目录存在
    QFileInfo configInfo(configPath);
    QDir configDir = configInfo.absoluteDir();
    if (!configDir.exists()) {
        if (!configDir.mkpath(".")) {
            m_lastError = QString("无法创建配置目录: %1").arg(configDir.absolutePath());
            return false;
        }
    }
    
    // 保存基本配置
    configFile.setValue("Software/name", m_currentConfig.name);
    configFile.setValue("Software/version", m_currentConfig.version);
    configFile.setValue("Software/developer", m_currentConfig.developer);
    configFile.setValue("Software/uniqueId", m_currentConfig.uniqueId);
    configFile.setValue("Software/outputDirectory", m_currentConfig.outputDir);
    configFile.setValue("Software/uiLayoutPath", m_currentConfig.uiLayoutPath);
    configFile.setValue("Software/iconPath", m_currentConfig.iconPath);
    configFile.setValue("DevelopmentTools/qtDir", m_currentConfig.qtDir);
    configFile.setValue("DevelopmentTools/mingwDir", m_currentConfig.mingwDir);
    configFile.setValue("DevelopmentTools/cmakeDir", m_currentConfig.cmakeDir);
    
    // 保存高级选项
    configFile.setValue("Advanced/iconPath", m_currentConfig.iconPath);
    configFile.setValue("Advanced/description", m_currentConfig.description);
    configFile.setValue("Advanced/copyright", m_currentConfig.copyright);
    
    configFile.sync();
    
    if (configFile.status() != QSettings::NoError) {
        m_lastError = "保存配置文件失败";
        return false;
    }
    
    emit configSaved(configName);
    return true;
}

QStringList PackageConfigManager::getConfigList() const
{
    QStringList configList;
    
    QString configDirPath = getDefaultConfigPath();
    QDir configDir(configDirPath);
    
    if (!configDir.exists()) {
        return configList;
    }
    
    QStringList iniFiles = configDir.entryList({"*.ini"}, QDir::Files);
    
    for (const QString &iniFile : iniFiles) {
        QString configName = QFileInfo(iniFile).baseName();
        configList.append(configName);
    }
    
    return configList;
}

bool PackageConfigManager::deleteConfig(const QString &configName)
{
    m_lastError.clear();
    
    QString configPath = getConfigFilePath(configName);
    
    if (!QFile::exists(configPath)) {
        m_lastError = QString("配置文件不存在: %1").arg(configPath);
        return false;
    }
    
    if (!QFile::remove(configPath)) {
        m_lastError = QString("无法删除配置文件: %1").arg(configPath);
        return false;
    }
    
    return true;
}

bool PackageConfigManager::validateConfig(const PackageConfig &config)
{
    // 验证基本配置
    if (config.name.isEmpty()) {
        m_lastError = "软件名称不能为空";
        return false;
    }
    
    if (config.version.isEmpty()) {
        m_lastError = "版本号不能为空";
        return false;
    }
    
    if (config.outputDir.isEmpty()) {
        m_lastError = "输出目录不能为空";
        return false;
    }
    
    // 验证输出目录是否可写
    QDir outputDir(config.outputDir);
    if (!outputDir.exists()) {
        // 尝试创建目录
        if (!outputDir.mkpath(".")) {
            m_lastError = QString("无法创建输出目录: %1").arg(config.outputDir);
            return false;
        }
    }
    
    // 验证输出目录是否可写
    QFile testFile(config.outputDir + "/test_write.tmp");
    if (testFile.open(QIODevice::WriteOnly)) {
        testFile.write("test");
        testFile.close();
        testFile.remove();
    } else {
        m_lastError = QString("输出目录不可写: %1").arg(config.outputDir);
        return false;
    }
    
    // 验证UI布局文件（如果提供了）
    if (!config.uiLayoutPath.isEmpty()) {
        if (!QFile::exists(config.uiLayoutPath)) {
            m_lastError = QString("UI布局文件不存在: %1").arg(config.uiLayoutPath);
            return false;
        }
        
        // 验证文件格式（简单检查）
        QFile uiFile(config.uiLayoutPath);
        if (!uiFile.open(QIODevice::ReadOnly)) {
            m_lastError = QString("无法读取UI布局文件: %1").arg(config.uiLayoutPath);
            return false;
        }
        
        QByteArray content = uiFile.read(100); // 读取前100字节检查
        uiFile.close();
        
        if (!content.contains("<?xml") && !content.contains("<ui")) {
            m_lastError = QString("UI布局文件格式不正确: %1").arg(config.uiLayoutPath);
            return false;
        }
    }
    
    // 验证开发工具路径
    if (!validateDevelopmentTools(config)) {
        return false;
    }
    
    // 验证图标文件（如果提供了）
    if (!config.iconPath.isEmpty()) {
        if (!QFile::exists(config.iconPath)) {
            m_lastError = QString("图标文件不存在: %1").arg(config.iconPath);
            return false;
        }
        
        // 验证图标文件格式
        QString suffix = QFileInfo(config.iconPath).suffix().toLower();
        if (suffix != "ico" && suffix != "png" && suffix != "jpg" && suffix != "jpeg") {
            m_lastError = QString("不支持的图标文件格式: %1").arg(suffix);
            return false;
        }
    }
    
    return true;
}

QString PackageConfigManager::getDefaultConfigPath()
{
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    if (configDir.isEmpty()) {
        configDir = QDir::homePath() + "/.config/SoftwareEditor";
    }
    
    return configDir;
}

QString PackageConfigManager::getConfigFilePath(const QString &configName)
{
    QString configDir = getDefaultConfigPath();
    return configDir + "/" + configName + ".ini";
}

void PackageConfigManager::initializeDefaultConfig()
{
    m_currentConfig = PackageConfig::defaultConfig();
    
    // 使用PackageService检测Qt路径
    PackageService packageService;
    QString qtPath = packageService.detectQtPath();
    if (!qtPath.isEmpty()) {
        m_currentConfig.qtDir = qtPath;
        qDebug() << "Qt path detected successfully:" << qtPath;
    }
    
    // 从Qt路径推断MinGW路径
    if (!m_currentConfig.qtDir.isEmpty()) {
        QString mingwPath = m_currentConfig.qtDir + "/../../Tools/mingw1310_64";
        if (QDir(mingwPath).exists()) {
            m_currentConfig.mingwDir = mingwPath;
            qDebug() << "MinGW path detected successfully:" << mingwPath;
        }
    }
    
    // 使用PackageService检测CMake路径
    QString cmakePath = packageService.detectCmakePath();
    if (!cmakePath.isEmpty()) {
        m_currentConfig.cmakeDir = cmakePath;
        qDebug() << "CMake path detected successfully:" << cmakePath;
    }
}

bool PackageConfigManager::validateDevelopmentTools(const PackageConfig &config)
{
    // 验证Qt路径
    if (!config.qtDir.isEmpty()) {
        if (!QDir(config.qtDir).exists()) {
            m_lastError = QString("Qt路径不存在: %1").arg(config.qtDir);
            return false;
        }
        
        // 检查Qt核心文件
        QStringList requiredFiles = {
            "/bin/qmake.exe",
            "/bin/moc.exe", 
            "/bin/uic.exe",
            "/bin/rcc.exe"
        };
        
        for (const QString &file : requiredFiles) {
            if (!QFile::exists(config.qtDir + file)) {
                m_lastError = QString("Qt路径不完整，缺少文件: %1").arg(file);
                return false;
            }
        }
    }
    
    // 验证MinGW路径
    if (!config.mingwDir.isEmpty()) {
        if (!QDir(config.mingwDir).exists()) {
            m_lastError = QString("MinGW路径不存在: %1").arg(config.mingwDir);
            return false;
        }
        
        // 检查MinGW核心文件
        QStringList requiredFiles = {
            "/bin/g++.exe",
            "/bin/gcc.exe",
            "/bin/ld.exe"
        };
        
        for (const QString &file : requiredFiles) {
            if (!QFile::exists(config.mingwDir + file)) {
                m_lastError = QString("MinGW路径不完整，缺少文件: %1").arg(file);
                return false;
            }
        }
    }
    
    // 验证CMake路径
    if (!config.cmakeDir.isEmpty()) {
        if (!QDir(config.cmakeDir).exists()) {
            m_lastError = QString("CMake路径不存在: %1").arg(config.cmakeDir);
            return false;
        }
        
        // 检查CMake可执行文件
        if (!QFile::exists(config.cmakeDir + "/cmake.exe")) {
            m_lastError = "CMake路径不完整，缺少cmake.exe";
            return false;
        }
    }
    
    return true;
}

bool PackageConfigManager::validatePath(const QString &path, const QString &description)
{
    if (path.isEmpty()) {
        return true; // 空路径视为有效（可选）
    }
    
    if (!QDir(path).exists()) {
        m_lastError = QString("%1路径不存在: %2").arg(description, path);
        return false;
    }
    
    return true;
}

