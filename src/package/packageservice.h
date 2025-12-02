#ifndef PACKAGESERVICE_H
#define PACKAGESERVICE_H

#include "ipackageservice.h"
#include "packageconfig.h"
#include <QObject>
#include <QString>
#include <QProcess>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <QDebug>
#include <QDomDocument>

// 逻辑层实现类 - 负责核心打包业务逻辑
class PackageService : public IPackageService
{
    Q_OBJECT

public:
    explicit PackageService(QObject *parent = nullptr);
    ~PackageService();

    // 开发工具检测
    bool detectDevelopmentTools() override;
    QString detectQtPath() const override;
    QString detectMingwPath() const override;
    QString detectCmakePath() const override;
    
    // 配置管理
    PackageConfig getCurrentConfig() const override;
    void updateConfig(const PackageConfig &config) override;
    bool validateConfiguration() override;
    
    // 配置管理增强
    bool loadConfig(const QString &configName = "default");
    bool saveConfig(const QString &configName = "default");
    QStringList getConfigList() const;
    bool deleteConfig(const QString &configName);
    
    // 代码生成
    QString generateCmakeLists(const PackageConfig &config) const override;
    QString generateMainCpp(const PackageConfig &config) const override;
    QString generateMainCppFromUILayout(const PackageConfig &config) const override;
    QString generateBuildBat(const PackageConfig &config) const;
    
    // 打包执行
    bool executePackage(const PackageConfig &config) override;
    bool createNsisInstaller(const PackageConfig &config, const QString &exePath) const override;
    void cancelPackage() override;
    
    // 工具函数
    bool copyDependencies(const QString &exePath, const QString &outputDir) override;
    bool setApplicationIcon(const QString &exePath, const QString &iconPath) override;
    
    // 错误处理
    QString getLastError() const override;

private:
    PackageConfig m_config;
    QString m_lastError;
    
    // 内部工具函数
    bool checkPathExists(const QString &path) const;
    QString executeCommand(const QString &command, const QStringList &args) const;
    bool createOutputDirectory(const QString &path) const;
    bool copyDirectory(const QString &sourceDir, const QString &destinationDir);
    
    // NSIS相关函数
    QString createNsisScript(const PackageConfig &config, const QString &exePath) const;
    QString findNsisTool() const;
    QString generateInstallerPath(const PackageConfig &config) const;
    
    // 依赖管理
    QString generateDependencyReport(const QString &exePath, const QString &outputDir);
    QString findMingwBinDir();
    QString findQtPluginsDir();
    
    // 日志函数
    void logError(const QString &message, bool isCritical = false) const;
    void logInfo(const QString &message) const;
    void logProgress(const QString &message) const;
    
    // 核心打包逻辑
    bool executeSmartPackageLogic(const PackageConfig &config, const QString &uiLayoutPath);
    QString generateCppCodeFromUILayout(const QString &uiLayoutPath, const PackageConfig &config);
    void generateCppProjectFiles(const PackageConfig &config, const QString &cppCode, const QString &outputDir);
};

#endif // PACKAGESERVICE_H