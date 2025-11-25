#ifndef PACKAGEMANAGER_H
#define PACKAGEMANAGER_H

#include <QObject>
#include <QProcess>
#include <QThread>
#include <QJsonObject>
#include "smartpackageconfig.h"
#include "product.h"

class PackageWorker : public QObject
{
    Q_OBJECT

public:
    explicit PackageWorker(QObject *parent = nullptr);

public slots:
    void smartPackageSoftware(const SmartPackageConfig::SmartPackageSettings &settings);
    void smartPackageSoftware(const Product &product, const SmartPackageConfig::SmartPackageSettings &settings);

signals:
    void progressChanged(int progress, const QString &message);
    void finished(bool success, const QString &resultPath);
    void errorOccurred(const QString &error);

public:
    bool copyDependencies(const QString &exePath, const QString &outputDir);
    void setApplicationIcon(const QString &exePath, const QString &iconPath);

private:
    bool executeSmartPackageLogic(const SmartPackageConfig::SmartPackageSettings &settings, const QString &uiLayoutPath);
    QString generateCppCodeFromUILayout(const QString &uiLayoutPath, const SmartPackageConfig::SmartPackageSettings &settings);
    void generateCppProjectFiles(const SmartPackageConfig::SmartPackageSettings &settings, const QString &cppCode, const QString &outputDir);
    QString generateCmakeLists(const SmartPackageConfig::SmartPackageSettings &settings);
    void createInstaller(const SmartPackageConfig::SmartPackageSettings &settings, const QString &exePath);
    bool copyDirectory(const QString &sourceDir, const QString &destinationDir);
    QString generateDependencyReport(const QString &exePath, const QString &outputDir);
    QString findMingwBinDir();
    QString findQtPluginsDir();
};

class PackageManager : public QObject
{
    Q_OBJECT

public:
    explicit PackageManager(QObject *parent = nullptr);
    ~PackageManager();
    
    // 开始打包
    void startPackage(const SmartPackageConfig::SmartPackageSettings &settings);
    void startPackage(const Product &product, const SmartPackageConfig::SmartPackageSettings &settings);
    void startSmartPackage(const SmartPackageConfig::SmartPackageSettings &settings);
    void startSmartPackage(const Product &product, const SmartPackageConfig::SmartPackageSettings &settings);
    
    // 取消打包
    void cancelPackage();
    
    // 检查打包工具是否可用
    bool isPackagingToolsAvailable() const;
    
    // 获取打包状态
    bool isPackaging() const;

signals:
    void progressChanged(int progress, const QString &message);
    void packageFinished(bool success, const QString &resultPath);
    void errorOccurred(const QString &error);

private slots:
    void onWorkerProgress(int progress, const QString &message);
    void onWorkerFinished(bool success, const QString &resultPath);
    void onWorkerError(const QString &error);

private:
    PackageWorker *m_worker;
    QThread *m_workerThread;
    bool m_isPackaging;
    
    void setupWorker();
    void cleanupWorker();
};

#endif // PACKAGEMANAGER_H