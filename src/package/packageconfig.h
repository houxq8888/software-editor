#ifndef PACKAGECONFIG_H
#define PACKAGECONFIG_H

#include <QString>
#include <QObject>
#include <QSettings>
#include <QDir>
#include <QDateTime>

// 软件打包配置结构体
struct PackageConfig
{
    QString name;               // 软件名称
    QString version;           // 版本号
    QString developer;         // 开发者
    QString uniqueId;          // 唯一标识符
    QString outputDir;         // 输出目录
    QString uiLayoutPath;      // UI布局文件路径
    bool createInstaller;      // 是否创建安装包
    
    // 开发工具路径
    QString qtDir;             // Qt安装路径
    QString mingwDir;          // MinGW路径
    QString cmakeDir;          // CMake路径
    
    // 高级选项
    QString iconPath;          // 应用程序图标路径
    QString description;       // 软件描述
    QString copyright;         // 版权信息
    QString licenseFile;       // 许可证文件路径
    bool includeDependencies; // 是否包含依赖库
    
    PackageConfig()
        : createInstaller(false)
        , includeDependencies(true)
    {
    }
    
    // 验证配置是否完整
    bool isValid() const
    {
        return !name.isEmpty() && !outputDir.isEmpty();
    }
    
    // 获取默认配置
    static PackageConfig defaultConfig()
    {
        PackageConfig config;
        config.name = "MyApplication";
        config.version = "1.0.0";
        config.developer = "Developer";
        config.uniqueId = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
        config.outputDir = QDir::homePath() + "/SoftwareOutput";
        config.uiLayoutPath = "";
        config.iconPath = "";
        config.qtDir = "";
        config.mingwDir = "";
        config.cmakeDir = "";
        config.createInstaller = true;
        config.includeDependencies = true;
        return config;
    }
};

// 配置管理类
class PackageConfigManager : public QObject
{
    Q_OBJECT

public:
    explicit PackageConfigManager(QObject *parent = nullptr);
    
    // 加载配置
    bool loadConfig(const QString &configName = "default");
    
    // 保存配置
    bool saveConfig(const QString &configName = "default");
    
    // 获取配置列表
    QStringList getConfigList() const;
    
    // 删除配置
    bool deleteConfig(const QString &configName);
    
    // 获取当前配置
    PackageConfig getCurrentConfig() const { return m_currentConfig; }
    
    // 设置当前配置
    void setCurrentConfig(const PackageConfig &config) { m_currentConfig = config; }
    
    // 验证配置
    bool validateConfig(const PackageConfig &config);
    
    // 获取配置错误信息
    QString getLastError() const { return m_lastError; }
    
    // 获取默认配置路径
    static QString getDefaultConfigPath();
    
    // 获取配置文件路径
    static QString getConfigFilePath(const QString &configName);

signals:
    void configLoaded(const PackageConfig &config);
    void configSaved(const QString &configName);
    void configError(const QString &error);

private:
    PackageConfig m_currentConfig;
    QString m_lastError;
    QSettings *m_settings;
    
    // 初始化默认配置
    void initializeDefaultConfig();
    
    // 验证路径是否存在
    bool validatePath(const QString &path, const QString &description);
    
    // 验证开发工具路径
    bool validateDevelopmentTools(const PackageConfig &config);
    

};

#endif // PACKAGECONFIG_H