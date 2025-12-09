#include "product.h"
#include <QDir>
#include <QFileInfo>

Product::Product()
{
    // Generate a unique ID for new products
    m_uniqueId = generateUniqueId();
}

// Helper function to convert relative path to absolute path
QString Product::toAbsolutePath(const QString &relativePath) const
{
    if (relativePath.isEmpty()) {
        return QString();
    }
    
    // If the path is already absolute, return it as is
    QFileInfo fileInfo(relativePath);
    if (fileInfo.isAbsolute()) {
        return relativePath;
    }
    
    // Convert relative path to absolute path based on current working directory
    return QDir::current().absoluteFilePath(relativePath);
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
    
    // 新结构：使用嵌套的productInfo对象
    QJsonObject productInfo;
    productInfo["name"] = m_name;
    productInfo["version"] = m_version;
    productInfo["description"] = m_description;
    productInfo["category"] = m_category;
    productInfo["developer"] = m_developer;
    productInfo["website"] = m_website;
    productInfo["uniqueId"] = m_uniqueId;
    
    // 在productInfo中添加resources字段
    QJsonObject resources;
    resources["icon"] = m_iconPath;
    resources["screenshot"] = m_screenshotPath;
    productInfo["resources"] = resources;
    
    // 在productInfo中添加features字段
    QJsonArray featuresArray;
    for (const auto &feature : m_features) {
        QJsonObject featureObj;
        featureObj["name"] = feature.name;
        featureObj["description"] = feature.description;
        featuresArray.append(featureObj);
    }
    productInfo["features"] = featuresArray;
    
    json["productInfo"] = productInfo;
    
    // 文件结构信息
    QJsonObject fileStructure;
    if (!m_uiLayoutPath.isEmpty()) {
        // 从UI布局路径中提取基础路径和UI布局路径
        QString basePath = m_uiLayoutPath;
        QString uiLayoutsPath = "ui_layouts";
        
        // 尝试从路径中提取信息
        if (m_uiLayoutPath.contains("/")) {
            QStringList pathParts = m_uiLayoutPath.split("/");
            if (pathParts.size() >= 2) {
                basePath = pathParts[0];
                uiLayoutsPath = pathParts[1];
            }
        }
        
        fileStructure["basePath"] = basePath;
        fileStructure["uiLayoutsPath"] = uiLayoutsPath;
        fileStructure["stateMachinesPath"] = "state_machines";
        fileStructure["resourcesPath"] = "resources";
    } else {
        // 默认文件结构
        fileStructure["basePath"] = "product_configurations/product_" + m_uniqueId.mid(0, 5);
        fileStructure["uiLayoutsPath"] = "ui_layouts";
        fileStructure["stateMachinesPath"] = "state_machines";
        fileStructure["resourcesPath"] = "resources";
    }
    
    // 多UI文件管理 - 适配新结构
    QJsonArray uiFilesArray;
    for (const auto &uiFile : m_uiFiles) {
        QJsonObject uiFileObj;
        uiFileObj["name"] = uiFile.name;
        
        // 保存相对于产品目录的相对路径，确保产品可以移动到任意位置
        QString fileName = uiFile.filePath;
        
        // 如果文件路径是绝对路径，转换为相对于产品目录的相对路径
        if (QFileInfo(fileName).isAbsolute()) {
            QString basePath = fileStructure.value("basePath").toString();
            QString uiLayoutsPath = fileStructure.value("uiLayoutsPath").toString();
            
            if (!basePath.isEmpty()) {
                // 计算相对于产品目录的路径
                QDir baseDir(basePath);
                fileName = baseDir.relativeFilePath(fileName);
            }
        }
        
        // 确保路径格式正确（使用正斜杠）
        fileName = fileName.replace("\\", "/");
        
        uiFileObj["fileName"] = fileName;  // 保存相对于产品目录的相对路径
        
        uiFileObj["type"] = uiFile.type;
        uiFileObj["isMain"] = uiFile.isMain;
        uiFileObj["description"] = uiFile.description;
        uiFileObj["order"] = uiFile.order;
        uiFilesArray.append(uiFileObj);
    }
    json["uiFiles"] = uiFilesArray;
    
    // 状态机文件信息
    QJsonArray stateMachinesArray;
    if (!m_stateMachinePath.isEmpty()) {
        QJsonObject stateMachine;
        
        // 提取文件名
        QString fileName = m_stateMachinePath;
        if (fileName.contains("/")) {
            QStringList pathParts = fileName.split("/");
            fileName = pathParts.last();
        }
        
        stateMachine["name"] = "state_machine_1";
        stateMachine["fileName"] = fileName;
        stateMachine["description"] = "主状态机文件";
        stateMachinesArray.append(stateMachine);
    }
    json["stateMachines"] = stateMachinesArray;

    return json;
}

bool Product::fromJson(const QJsonObject &json, const QString &configFilePath) {
    // 检查新结构（包含productInfo字段）
    if (json.contains("productInfo") && json["productInfo"].isObject()) {
        // 新结构：从productInfo对象中读取基本信息
        QJsonObject productInfo = json["productInfo"].toObject();
        
        if (!productInfo.contains("name") || !productInfo["name"].isString()) return false;
        if (!productInfo.contains("version") || !productInfo["version"].isString()) return false;

        m_name = productInfo["name"].toString();
        m_version = productInfo["version"].toString();
        m_description = productInfo.value("description").toString();
        m_category = productInfo.value("category").toString();
        m_developer = productInfo.value("developer").toString();
        m_website = productInfo.value("website").toString();
        m_uniqueId = productInfo.value("uniqueId").toString();
        
        // 从productInfo中的resources字段读取图标和截图路径
        if (productInfo.contains("resources") && productInfo["resources"].isObject()) {
            QJsonObject resources = productInfo["resources"].toObject();
            m_iconPath = resources.value("icon").toString();
            m_screenshotPath = resources.value("screenshot").toString();
        } else {
            // 如果没有resources字段，尝试从productInfo根级别读取
            m_iconPath = productInfo.value("iconPath").toString();
            m_screenshotPath = productInfo.value("screenshotPath").toString();
        }
        
        // 从stateMachines字段读取状态机路径
        if (json.contains("stateMachines") && json["stateMachines"].isArray()) {
            QJsonArray stateMachines = json["stateMachines"].toArray();
            if (!stateMachines.isEmpty()) {
                QJsonObject firstStateMachine = stateMachines.first().toObject();
                QString fileName = firstStateMachine.value("fileName").toString();
            }
        }
    } else {
        // 旧结构：直接从根对象读取
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
    }
    
    // If uniqueId is empty, generate a new one
    if (m_uniqueId.isEmpty()) {
        m_uniqueId = generateUniqueId();
    }

    // 多UI文件管理 - 适配新结构
    m_uiFiles.clear();
    if (json.contains("uiFiles") && json["uiFiles"].isArray()) {
        // 新结构：从uiFiles数组读取
        const QJsonArray uiFilesArray = json["uiFiles"].toArray();
        for (const auto &uiFileValue : uiFilesArray) {
            if (uiFileValue.isObject()) {
                const QJsonObject uiFileObj = uiFileValue.toObject();
                ProductUIFile uiFile;
                uiFile.name = uiFileObj.value("name").toString();
                uiFile.filePath = uiFileObj.value("fileName").toString(); // 新结构使用fileName
                uiFile.type = uiFileObj.value("type").toString();
                uiFile.isMain = uiFileObj.value("isMain").toBool(false);
                uiFile.description = uiFileObj.value("description").toString();
                uiFile.order = uiFileObj.value("order").toInt(0);
                
                // 构建完整路径并转换为绝对路径
                if (!uiFile.filePath.isEmpty() && QDir::isRelativePath(uiFile.filePath)) {
                    if (!configFilePath.isEmpty()) {
                        QFileInfo configFileInfo(configFilePath);
                        QDir configDir = configFileInfo.dir();
                        uiFile.filePath = configDir.absoluteFilePath(uiFile.filePath);
                    } else {
                        // 如果没有提供配置文件路径，使用当前工作目录作为后备方案
                        uiFile.filePath = QDir::currentPath() + "/" + uiFile.filePath;
                    }
                }
                
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
    // 优先从productInfo对象中读取features
    if (json.contains("productInfo") && json["productInfo"].isObject()) {
        QJsonObject productInfo = json["productInfo"].toObject();
        if (productInfo.contains("features") && productInfo["features"].isArray()) {
            const QJsonArray featuresArray = productInfo["features"].toArray();
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
    }
    // 向后兼容：如果productInfo中没有features，尝试从根级别读取
    else if (json.contains("features") && json["features"].isArray()) {
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