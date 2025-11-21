#ifndef PACKAGECONFIG_H
#define PACKAGECONFIG_H

#include <QString>
#include <QObject>
#include <QProcess>
#include <QJsonObject>

class PackageConfig : public QObject
{
    Q_OBJECT

public:
    explicit PackageConfig(QObject *parent = nullptr);
    
    // 打包配置
    struct PackageSettings {
        QString name;           // 软件名称
        QString version;        // 版本号
        QString developer;      // 开发者
        QString description;    // 描述
        QString iconPath;       // 图标路径
        QString outputDir;      // 输出目录
        bool createInstaller;   // 是否创建安装包
        bool includeDependencies; // 是否包含依赖
    };
    
    // 设置打包配置
    void setPackageSettings(const PackageSettings &settings);
    PackageSettings getPackageSettings() const;
    
    // 从产品信息加载配置
    void loadFromProduct(const QJsonObject &productData);
    
    // 验证配置
    bool validate() const;
    
    // 获取默认配置
    static PackageSettings defaultSettings();

private:
    PackageSettings m_settings;
};

#endif // PACKAGECONFIG_H