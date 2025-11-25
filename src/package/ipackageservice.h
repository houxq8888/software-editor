#ifndef IPACKAGESERVICE_H
#define IPACKAGESERVICE_H

#include <QObject>
#include <QString>
#include "packageconfig.h"

// 打包服务接口类
class IPackageService : public QObject
{
    Q_OBJECT

public:
    explicit IPackageService(QObject *parent = nullptr) : QObject(parent) {}
    virtual ~IPackageService() {}

    // 开发工具检测
    virtual bool detectDevelopmentTools() = 0;
    virtual QString detectQtPath() const = 0;
    virtual QString detectMingwPath() const = 0;
    virtual QString detectCmakePath() const = 0;
    
    // 配置管理
    virtual PackageConfig getCurrentConfig() const = 0;
    virtual void updateConfig(const PackageConfig &config) = 0;
    virtual bool validateConfiguration() = 0;
    
    // 代码生成
    virtual QString generateCmakeLists(const PackageConfig &config) const = 0;
    virtual QString generateMainCpp(const PackageConfig &config) const = 0;
    virtual QString generateMainCppFromUILayout(const PackageConfig &config) const = 0;
    
    // 打包执行
    virtual bool executePackage(const PackageConfig &config) = 0;
    virtual bool createNsisInstaller(const PackageConfig &config, const QString &exePath) const = 0;
    virtual void cancelPackage() = 0;
    
    // 工具函数
    virtual bool copyDependencies(const QString &exePath, const QString &outputDir) = 0;
    virtual bool setApplicationIcon(const QString &exePath, const QString &iconPath) = 0;
    
    // 错误处理
    virtual QString getLastError() const = 0;

signals:
    void progressChanged(int progress, const QString &message);
    void packageFinished(bool success, const QString &resultPath);
    void errorOccurred(const QString &error);
};

#endif // IPACKAGESERVICE_H