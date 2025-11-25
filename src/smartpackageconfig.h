#ifndef SMARTPACKAGECONFIG_H
#define SMARTPACKAGECONFIG_H

#include <QObject>
#include <QString>
#include <QProcess>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <QDebug>

class SmartPackageConfig : public QObject
{
    Q_OBJECT

public:
    explicit SmartPackageConfig(QObject *parent = nullptr);
    
    // 智能打包配置结构
    struct SmartPackageSettings {
        QString name;                   // 软件名称
        QString uniqueId;               // 唯一标识符，用于生成文件名（避免中文问题）
        QString version;               // 版本号
        QString developer;             // 开发者
        QString description;           // 描述
        QString iconPath;              // 图标路径
        QString outputDir;            // 输出目录
        QString uiLayoutPath;          // UI布局文件路径
        bool createInstaller;          // 是否创建安装包
        bool includeDependencies;      // 是否包含依赖
        
        // 智能检测的路径
        QString qtDir;                 // Qt安装目录
        QString mingwDir;             // MinGW安装目录
        QString cmakeDir;              // CMake安装目录
        QString detectedConfig;        // 检测到的配置方案
        
        SmartPackageSettings() {
            name = "软件编辑器";
            uniqueId = "software_editor"; // 默认uniqueId
            version = "1.0.0";
            developer = "软件工作室";
            description = "专业的软件UI布局编辑器";
            iconPath = "";
            outputDir = QDir::currentPath() + "/packages/";
            uiLayoutPath = "";
            createInstaller = false;
            includeDependencies = true;
            qtDir = "";
            mingwDir = "";
            cmakeDir = "";
            detectedConfig = "";
        }
    };
    
    // 检测开发工具路径
    bool detectDevelopmentTools();
    
    // 获取检测到的配置
    SmartPackageSettings getDetectedSettings() const;
    
    // 验证配置是否可用
    bool validateConfiguration();
    
    // 生成智能CMakeLists.txt内容
    QString generateSmartCmakeLists(const SmartPackageSettings &settings) const;
    
    // 生成智能main.cpp内容
    QString generateSmartMainCpp(const SmartPackageSettings &settings) const;
    
    // 根据UI布局文件生成main.cpp内容
    QString generateMainCppFromUILayout(const SmartPackageSettings &settings) const;
    
    // 生成构建批处理文件
    QString generateBuildBat(const SmartPackageSettings &settings) const;
    
    // 执行智能打包
    bool executeSmartPackage(const SmartPackageSettings &settings);
    
    // 获取错误信息
    QString getLastError() const;
    
    // 检测工具路径（公开方法）
    QString detectQtPath() const;
    QString detectMingwPath() const;
    QString detectCmakePath() const;
    
    // 手动设置工具路径
    void setQtPath(const QString &path);
    void setMingwPath(const QString &path);
    void setCmakePath(const QString &path);
    
    // 获取当前设置的工具路径
    QString getQtPath() const;
    QString getMingwPath() const;
    QString getCmakePath() const;
    
    // 智能CMake配置
    QStringList detectAvailableCmakeGenerators() const;
    bool testCmakeGenerator(const QString &generator) const;
    
    // NSIS安装包生成功能（公开给packagemanager使用）
    bool createNsisInstaller(const SmartPackageSettings &settings, const QString &exePath) const;

signals:
    void progressChanged(int progress, const QString &message);
    void packageFinished(bool success, const QString &resultPath);
    void errorOccurred(const QString &error);

private:
    SmartPackageSettings m_settings;
    QString m_lastError;
    
    // 检查路径是否存在
    bool checkPathExists(const QString &path) const;
    
    // 执行命令并获取输出
    QString executeCommand(const QString &command, const QStringList &args) const;
    
    // 创建输出目录
    bool createOutputDirectory(const QString &path) const;
    
    // 复制依赖文件
    bool copyDependencies(const QString &exePath, const QString &outputDir) const;
    
    // 设置应用程序图标
    bool setApplicationIcon(const QString &exePath, const QString &iconPath) const;
    
    QString createNsisScript(const SmartPackageSettings &settings, const QString &exePath) const;
    
    // NSIS工具查找和安装包路径生成公共函数
    QString findNsisTool() const;
    QString generateInstallerPath(const SmartPackageSettings &settings) const;
    
    // 统一日志函数
    void logError(const QString &message, bool isCritical = false) const;
    void logInfo(const QString &message) const;
    void logProgress(const QString &message) const;
};

#endif // SMARTPACKAGECONFIG_H