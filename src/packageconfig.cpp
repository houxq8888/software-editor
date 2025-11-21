#include "packageconfig.h"
#include <QDir>
#include <QFileInfo>

PackageConfig::PackageConfig(QObject *parent)
    : QObject(parent)
    , m_settings(defaultSettings())
{
}

void PackageConfig::setPackageSettings(const PackageConfig::PackageSettings &settings)
{
    m_settings = settings;
}

PackageConfig::PackageSettings PackageConfig::getPackageSettings() const
{
    return m_settings;
}

void PackageConfig::loadFromProduct(const QJsonObject &productData)
{
    m_settings.name = productData.value("name").toString();
    m_settings.version = productData.value("version").toString();
    m_settings.developer = productData.value("developer").toString();
    m_settings.description = productData.value("description").toString();
    m_settings.iconPath = productData.value("iconPath").toString();
    
    // 设置默认输出目录
    QDir currentDir;
    m_settings.outputDir = currentDir.absoluteFilePath("dist");
}

bool PackageConfig::validate() const
{
    if (m_settings.name.isEmpty()) {
        return false;
    }
    
    if (m_settings.version.isEmpty()) {
        return false;
    }
    
    // 检查输出目录是否可写
    QDir outputDir(m_settings.outputDir);
    if (!outputDir.exists()) {
        if (!outputDir.mkpath(".")) {
            return false;
        }
    }
    
    // 检查输出目录是否可写
    QFile testFile(outputDir.absoluteFilePath("test_write.tmp"));
    if (!testFile.open(QIODevice::WriteOnly)) {
        return false;
    }
    testFile.close();
    testFile.remove();
    
    return true;
}

PackageConfig::PackageSettings PackageConfig::defaultSettings()
{
    PackageSettings settings;
    settings.name = "软件编辑器";
    settings.version = "1.0.0";
    settings.developer = "软件工作室";
    settings.description = "专业的软件UI布局编辑器";
    settings.iconPath = "";
    settings.outputDir = "dist";
    settings.createInstaller = true;
    settings.includeDependencies = true;
    
    return settings;
}