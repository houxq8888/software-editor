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
    
    // 优先使用产品配置包根路径进行转换
    if (!m_configPackageRootPath.isEmpty()) {
        QDir configDir(m_configPackageRootPath);
        return configDir.absoluteFilePath(relativePath);
    }
    
    // 如果没有配置包根路径，使用当前工作目录作为后备方案
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
    // 返回当前产品的配置目录路径
    // 优先使用配置包根路径
    if (!m_configPackageRootPath.isEmpty()) {
        return m_configPackageRootPath;
    }
    
    // 如果没有配置包根路径，使用m_stateMachinePath作为后备方案
    if (!m_stateMachinePath.isEmpty()) {
        return m_stateMachinePath;
    }
    
    // 如果都没有设置，返回空字符串
    return QString(); 
}

void Product::setStateMachinePath(const QString &stateMachinePath) { 
    m_stateMachinePath = stateMachinePath; 
}

bool Product::hasStateMachine() const { 
    return !m_stateMachinePath.isEmpty(); 
}

// State machine configuration management
QList<StateMachineConfig> Product::stateMachineConfigs() const { 
    return m_stateMachineConfigs; 
}

void Product::setStateMachineConfigs(const QList<StateMachineConfig> &configs) { 
    m_stateMachineConfigs = configs; 
}

void Product::addStateMachineConfig(const StateMachineConfig &config) {
    m_stateMachineConfigs.append(config);
}

void Product::removeStateMachineConfig(int index) {
    if (index >= 0 && index < m_stateMachineConfigs.size()) {
        m_stateMachineConfigs.removeAt(index);
    }
}

StateMachineConfig Product::getStateMachineConfig(const QString &name) const {
    for (const auto &config : m_stateMachineConfigs) {
        if (config.name == name) {
            return config;
        }
    }
    return StateMachineConfig();
}

bool Product::hasStateMachineConfig(const QString &name) const {
    for (const auto &config : m_stateMachineConfigs) {
        if (config.name == name) {
            return true;
        }
    }
    return false;
}

bool Product::hasStateMachineConfig() const {
    return !m_stateMachineConfigs.isEmpty();
}

QString Product::getWizardJsonPath() const {
    // 从状态机文件中读取wizard JSON路径
    if (!m_stateMachineConfigs.isEmpty()) {
        // 获取第一个状态机配置的文件路径
        QString stateMachinePath = m_stateMachineConfigs.first().fileName;
        
        // 如果路径是相对路径，转换为绝对路径
        if (QDir::isRelativePath(stateMachinePath)) {
            if (!m_configPackageRootPath.isEmpty()) {
                stateMachinePath = QDir(m_configPackageRootPath).absoluteFilePath(stateMachinePath);
            } else {
                stateMachinePath = QDir::currentPath() + "/" + stateMachinePath;
            }
        }
        
        // 读取状态机文件并获取wizardJsonPath
        QFile file(stateMachinePath);
        if (file.open(QIODevice::ReadOnly)) {
            QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
            file.close();
            
            if (!doc.isNull()) {
                QJsonObject json = doc.object();
                if (json.contains("wizardJsonPath") && json["wizardJsonPath"].isString()) {
                    QString wizardPath = json["wizardJsonPath"].toString();
                    
                    // 如果向导路径是相对路径，转换为相对于状态机文件的路径
                    if (QDir::isRelativePath(wizardPath)) {
                        QFileInfo stateMachineFileInfo(stateMachinePath);
                        QDir stateMachineDir = stateMachineFileInfo.dir();
                        wizardPath = stateMachineDir.absoluteFilePath(wizardPath);
                    }
                    
                    return wizardPath;
                }
            }
        }
    }
    
    return QString();
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
        // 默认文件结构 - 使用配置包根路径
        QString basePath = m_configPackageRootPath.isEmpty() ? 
            QString("product_" + m_uniqueId.mid(0, 5)) : m_configPackageRootPath;
        
        fileStructure["basePath"] = basePath;
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
    
    // 优先使用新的状态机配置格式
    if (!m_stateMachineConfigs.isEmpty()) {
        for (const auto &config : m_stateMachineConfigs) {
            QJsonObject stateMachine;
            stateMachine["name"] = config.name;
            stateMachine["description"] = config.description;
            stateMachine["fileName"] = config.fileName;
            stateMachine["wizardJsonPath"] = config.wizardJsonPath;
            stateMachine["logicJsonPath"] = config.logicJsonPath;
            stateMachinesArray.append(stateMachine);
        }
    }
    // 向后兼容：如果只有单个状态机路径
    else if (!m_stateMachinePath.isEmpty()) {
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
    // 设置产品配置包根路径
    if (!configFilePath.isEmpty()) {
        QFileInfo configFileInfo(configFilePath);
        m_configPackageRootPath = configFileInfo.dir().absolutePath();
    }
    
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
        
        // 从stateMachines字段读取状态机配置
        if (json.contains("stateMachines") && json["stateMachines"].isArray()) {
            QJsonArray stateMachines = json["stateMachines"].toArray();
            m_stateMachineConfigs.clear();
            
            for (const auto &stateMachineValue : stateMachines) {
                if (stateMachineValue.isObject()) {
                    QJsonObject stateMachineObj = stateMachineValue.toObject();
                    StateMachineConfig config;
                    config.name = stateMachineObj.value("name").toString();
                    config.description = stateMachineObj.value("description").toString();
                    config.fileName = stateMachineObj.value("fileName").toString();
                    config.wizardJsonPath = stateMachineObj.value("wizardJsonPath").toString();
                    config.logicJsonPath = stateMachineObj.value("logicJsonPath").toString();
                    
                    // 构建完整路径并转换为绝对路径
                    if (!config.fileName.isEmpty() && QDir::isRelativePath(config.fileName)) {
                        if (!configFilePath.isEmpty()) {
                            QFileInfo configFileInfo(configFilePath);
                            QDir configDir = configFileInfo.dir();
                            config.fileName = configDir.absoluteFilePath(config.fileName);
                        } else {
                            // 如果没有提供配置文件路径，使用当前工作目录作为后备方案
                            config.fileName = QDir::currentPath() + "/" + config.fileName;
                        }
                    }
                    
                    m_stateMachineConfigs.append(config);
                }
            }
            
            // 向后兼容：如果没有新的状态机配置，但存在单个状态机路径
            if (m_stateMachineConfigs.isEmpty() && json.contains("stateMachinePath")) {
                m_stateMachinePath = json.value("stateMachinePath").toString();
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
        qDebug() << "Found" << uiFilesArray.size() << "UI files in product configuration";
        
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
                
                qDebug() << "Processing UI file:" << uiFile.name << "with path:" << uiFile.filePath;
                
                // 构建完整路径并转换为绝对路径
                if (!uiFile.filePath.isEmpty() && QDir::isRelativePath(uiFile.filePath)) {
                    if (!configFilePath.isEmpty()) {
                        QFileInfo configFileInfo(configFilePath);
                        QDir configDir = configFileInfo.dir();
                        QString originalPath = uiFile.filePath;
                        uiFile.filePath = configDir.absoluteFilePath(uiFile.filePath);
                        qDebug() << "Converted relative path:" << originalPath << "->" << uiFile.filePath;
                    } else {
                        // 如果没有提供配置文件路径，使用当前工作目录作为后备方案
                        QString originalPath = uiFile.filePath;
                        uiFile.filePath = QDir::currentPath() + "/" + uiFile.filePath;
                        qDebug() << "Converted relative path (fallback):" << originalPath << "->" << uiFile.filePath;
                    }
                } else {
                    qDebug() << "UI file path is absolute:" << uiFile.filePath;
                }
                
                // 检查文件是否存在
                QFileInfo fileInfo(uiFile.filePath);
                if (fileInfo.exists()) {
                    qDebug() << "UI file exists:" << uiFile.filePath;
                } else {
                    qWarning() << "UI file does not exist:" << uiFile.filePath;
                }
                
                m_uiFiles.append(uiFile);
            }
        }
        
        qDebug() << "Successfully loaded" << m_uiFiles.size() << "UI files from product configuration";
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

// Product configuration package root path management
QString Product::configPackageRootPath() const {
    return m_configPackageRootPath;
}

void Product::setConfigPackageRootPath(const QString &rootPath) {
    m_configPackageRootPath = rootPath;
}

// Product configuration package validation
bool Product::isValidProductConfigPackage() const {
    // 检查基本产品信息是否完整
    if (m_name.isEmpty() || m_version.isEmpty()) {
        return false;
    }
    
    // 检查产品配置包根路径是否有效
    if (m_configPackageRootPath.isEmpty()) {
        return false;
    }
    
    QDir configDir(m_configPackageRootPath);
    if (!configDir.exists()) {
        return false;
    }
    
    // 检查产品配置文件是否存在
    QString configFilePath = configDir.absoluteFilePath("product_config.json");
    if (!QFile::exists(configFilePath)) {
        return false;
    }
    
    // 检查是否有至少一个UI文件
    if (m_uiFiles.isEmpty()) {
        return false;
    }
    
    // 检查主UI文件是否存在
    ProductUIFile mainUiFile = getMainUiFile();
    if (!mainUiFile.filePath.isEmpty() && !QFile::exists(mainUiFile.filePath)) {
        return false;
    }
    
    return true;
}