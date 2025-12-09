#include "product.h"

Product::Product()
{
    // Generate a unique ID for new products
    m_uniqueId = generateUniqueId();
}

QString Product::name() const { return m_name; }
void Product::setName(const QString &name) { m_name = name; }

QString Product::version() const { return m_version; }
void Product::setVersion(const QString &version) { m_version = version; }

QString Product::description() const { return m_description; }
void Product::setDescription(const QString &description) { m_description = description; }

QString Product::iconPath() const { return m_iconPath; }
void Product::setIconPath(const QString &iconPath) { m_iconPath = iconPath; }

QString Product::screenshotPath() const { return m_screenshotPath; }
void Product::setScreenshotPath(const QString &screenshotPath) { m_screenshotPath = screenshotPath; }

QString Product::category() const { return m_category; }
void Product::setCategory(const QString &category) { m_category = category; }

QString Product::developer() const { return m_developer; }
void Product::setDeveloper(const QString &developer) { m_developer = developer; }

QString Product::website() const { return m_website; }
void Product::setWebsite(const QString &website) { m_website = website; }

// Unique identifier management
QString Product::uniqueId() const { return m_uniqueId; }
void Product::setUniqueId(const QString &uniqueId) { m_uniqueId = uniqueId; }
QString Product::generateUniqueId() {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

// UI layout file management (backward compatibility)
QString Product::uiLayoutPath() const { return m_uiLayoutPath; }
void Product::setUiLayoutPath(const QString &uiLayoutPath) { 
    m_uiLayoutPath = uiLayoutPath; 
    
    // 向后兼容：如果设置了单个UI文件路径，自动添加到UI文件列表中
    if (!uiLayoutPath.isEmpty()) {
        ProductUIFile mainUiFile;
        mainUiFile.name = "主窗口";
        mainUiFile.filePath = uiLayoutPath;
        mainUiFile.type = "main_window";
        mainUiFile.isMain = true;
        mainUiFile.description = "产品主界面窗口";
        mainUiFile.order = 0;
        
        // 检查是否已存在相同的文件路径
        bool exists = false;
        for (const auto &uiFile : m_uiFiles) {
            if (uiFile.filePath == uiLayoutPath) {
                exists = true;
                break;
            }
        }
        
        if (!exists) {
            m_uiFiles.append(mainUiFile);
        }
    }
}

// Multi-UI file management
QList<ProductUIFile> Product::uiFiles() const { return m_uiFiles; }
void Product::setUiFiles(const QList<ProductUIFile> &uiFiles) { m_uiFiles = uiFiles; }

void Product::addUiFile(const ProductUIFile &uiFile) {
    m_uiFiles.append(uiFile);
}

void Product::removeUiFile(int index) {
    if (index >= 0 && index < m_uiFiles.size()) {
        m_uiFiles.removeAt(index);
    }
}

void Product::removeUiFile(const QString &filePath) {
    for (int i = 0; i < m_uiFiles.size(); i++) {
        if (m_uiFiles[i].filePath == filePath) {
            m_uiFiles.removeAt(i);
            break;
        }
    }
}

ProductUIFile Product::getMainUiFile() const {
    for (const auto &uiFile : m_uiFiles) {
        if (uiFile.isMain) {
            return uiFile;
        }
    }
    
    // 如果没有设置主UI文件，返回第一个文件
    if (!m_uiFiles.isEmpty()) {
        return m_uiFiles.first();
    }
    
    // 返回空的UI文件结构
    return ProductUIFile();
}

ProductUIFile Product::getUiFile(const QString &filePath) const {
    for (const auto &uiFile : m_uiFiles) {
        if (uiFile.filePath == filePath) {
            return uiFile;
        }
    }
    return ProductUIFile();
}

bool Product::hasUiFile(const QString &filePath) const {
    for (const auto &uiFile : m_uiFiles) {
        if (uiFile.filePath == filePath) {
            return true;
        }
    }
    return false;
}

void Product::setMainUiFile(const QString &filePath) {
    // 先取消所有UI文件的主文件标记
    for (auto &uiFile : m_uiFiles) {
        uiFile.isMain = false;
    }
    
    // 设置指定文件为主文件
    for (auto &uiFile : m_uiFiles) {
        if (uiFile.filePath == filePath) {
            uiFile.isMain = true;
            break;
        }
    }
}

QList<ProductFeature> Product::features() const { return m_features; }
void Product::setFeatures(const QList<ProductFeature> &features) { m_features = features; }

void Product::addFeature(const ProductFeature &feature) {
    m_features.append(feature);
}

void Product::removeFeature(int index) {
    if (index >= 0 && index < m_features.size()) {
        m_features.removeAt(index);
    }
}

// State machine management
QString Product::stateMachinePath() const { 
    return m_stateMachinePath; 
}

void Product::setStateMachinePath(const QString &stateMachinePath) { 
    m_stateMachinePath = stateMachinePath; 
}

bool Product::hasStateMachine() const { 
    return !m_stateMachinePath.isEmpty(); 
}

QJsonObject Product::toJson() const {
    QJsonObject json;
    json["name"] = m_name;
    json["version"] = m_version;
    json["description"] = m_description;
    json["iconPath"] = m_iconPath;
    json["screenshotPath"] = m_screenshotPath;
    json["category"] = m_category;
    json["developer"] = m_developer;
    json["website"] = m_website;
    json["uniqueId"] = m_uniqueId;
    
    // 向后兼容：保留单个UI文件路径
    if (!m_uiLayoutPath.isEmpty()) {
        json["uiLayoutPath"] = m_uiLayoutPath;
    }
    
    // State machine file path
    if (!m_stateMachinePath.isEmpty()) {
        json["stateMachinePath"] = m_stateMachinePath;
    }
    
    // 多UI文件管理
    QJsonArray uiFilesArray;
    for (const auto &uiFile : m_uiFiles) {
        QJsonObject uiFileObj;
        uiFileObj["name"] = uiFile.name;
        uiFileObj["filePath"] = uiFile.filePath;
        uiFileObj["type"] = uiFile.type;
        uiFileObj["isMain"] = uiFile.isMain;
        uiFileObj["description"] = uiFile.description;
        uiFileObj["order"] = uiFile.order;
        uiFilesArray.append(uiFileObj);
    }
    json["uiFiles"] = uiFilesArray;

    QJsonArray featuresArray;
    for (const auto &feature : m_features) {
        QJsonObject featureObj;
        featureObj["name"] = feature.name;
        featureObj["description"] = feature.description;
        featuresArray.append(featureObj);
    }
    json["features"] = featuresArray;

    return json;
}

bool Product::fromJson(const QJsonObject &json) {
    if (!json.contains("name") || !json["name"].isString()) return false;
    if (!json.contains("version") || !json["version"].isString()) return false;

    m_name = json["name"].toString();
    m_version = json["version"].toString();
    m_description = json.value("description").toString();
    m_iconPath = json.value("iconPath").toString();
    m_screenshotPath = json.value("screenshotPath").toString();
    m_category = json.value("category").toString();
    m_developer = json.value("developer").toString();
    m_website = json.value("website").toString();
    m_uniqueId = json.value("uniqueId").toString();
    m_uiLayoutPath = json.value("uiLayoutPath").toString();
    m_stateMachinePath = json.value("stateMachinePath").toString();
    
    // If uniqueId is empty, generate a new one
    if (m_uniqueId.isEmpty()) {
        m_uniqueId = generateUniqueId();
    }

    // 多UI文件管理
    m_uiFiles.clear();
    if (json.contains("uiFiles") && json["uiFiles"].isArray()) {
        const QJsonArray uiFilesArray = json["uiFiles"].toArray();
        for (const auto &uiFileValue : uiFilesArray) {
            if (uiFileValue.isObject()) {
                const QJsonObject uiFileObj = uiFileValue.toObject();
                ProductUIFile uiFile;
                uiFile.name = uiFileObj.value("name").toString();
                uiFile.filePath = uiFileObj.value("filePath").toString();
                uiFile.type = uiFileObj.value("type").toString();
                uiFile.isMain = uiFileObj.value("isMain").toBool(false);
                uiFile.description = uiFileObj.value("description").toString();
                uiFile.order = uiFileObj.value("order").toInt(0);
                m_uiFiles.append(uiFile);
            }
        }
    } else if (!m_uiLayoutPath.isEmpty()) {
        // 向后兼容：如果只有单个UI文件路径，自动创建UI文件结构
        ProductUIFile mainUiFile;
        mainUiFile.name = "主窗口";
        mainUiFile.filePath = m_uiLayoutPath;
        mainUiFile.type = "main_window";
        mainUiFile.isMain = true;
        mainUiFile.description = "产品主界面窗口";
        mainUiFile.order = 0;
        m_uiFiles.append(mainUiFile);
    }

    m_features.clear();
    if (json.contains("features") && json["features"].isArray()) {
        const QJsonArray featuresArray = json["features"].toArray();
        for (const auto &featureValue : featuresArray) {
            if (featureValue.isObject()) {
                const QJsonObject featureObj = featureValue.toObject();
                ProductFeature feature;
                feature.name = featureObj.value("name").toString();
                feature.description = featureObj.value("description").toString();
                m_features.append(feature);
            }
        }
    }

    return true;
}