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

// State machine configuration management (single configuration per product)
StateMachineConfig Product::stateMachineConfig() const { 
    return m_stateMachineConfig; 
}

void Product::setStateMachineConfig(const StateMachineConfig &config) { 
    m_stateMachineConfig = config; 
}

bool Product::hasStateMachineConfig() const {
    return !m_stateMachineConfig.name.isEmpty();
}

QString Product::getWizardJsonPath() const {
    // 从状态机文件中读取wizard JSON路径
    if (!m_stateMachineConfig.fileName.isEmpty()) {
        QString stateMachinePath = m_stateMachineConfig.fileName;
        
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

QString Product::getWizardAbsolutePath() const {
    // 获取向导JSON文件的绝对路径
    QString wizardPath = getWizardJsonPath();
    
    if (wizardPath.isEmpty()) {
        return QString();
    }
    
    // 如果已经是绝对路径，直接返回
    if (QFileInfo(wizardPath).isAbsolute()) {
        return wizardPath;
    }
    
    // 如果是相对路径，转换为绝对路径
    if (!m_configPackageRootPath.isEmpty()) {
        return QDir(m_configPackageRootPath).absoluteFilePath(wizardPath);
    } else {
        return QDir::currentPath() + "/" + wizardPath;
    }
}

QString Product::getWizardName() const
{
    return m_stateMachineConfig.wizardData.name;
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
        uiFileObj["description"] = uiFile.description;
        uiFileObj["order"] = uiFile.order;
        uiFilesArray.append(uiFileObj);
    }
    json["uiFiles"] = uiFilesArray;
    
    // 状态机文件信息（单个状态机配置）
    QJsonArray stateMachinesArray;
    
    // 使用单个状态机配置
    if (!m_stateMachineConfig.name.isEmpty()) {
        QJsonObject stateMachine;
        stateMachine["name"] = m_stateMachineConfig.name;
        stateMachine["description"] = m_stateMachineConfig.description;
        stateMachine["fileName"] = m_stateMachineConfig.fileName;
        stateMachine["wizardJsonPath"] = m_stateMachineConfig.wizardJsonPath;
        stateMachine["logicJsonPath"] = m_stateMachineConfig.logicJsonPath;
        stateMachinesArray.append(stateMachine);
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
        
        // 从stateMachines字段读取状态机配置（只取第一个配置）
        if (json.contains("stateMachines") && json["stateMachines"].isArray()) {
            QJsonArray stateMachines = json["stateMachines"].toArray();
            
            // 只取第一个状态机配置
            if (!stateMachines.isEmpty()) {
                const auto &stateMachineValue = stateMachines.first();
                if (stateMachineValue.isObject()) {
                    QJsonObject stateMachineObj = stateMachineValue.toObject();
                    m_stateMachineConfig.name = stateMachineObj.value("name").toString();
                    m_stateMachineConfig.description = stateMachineObj.value("description").toString();
                    m_stateMachineConfig.fileName = stateMachineObj.value("fileName").toString();
                    m_stateMachineConfig.wizardJsonPath = stateMachineObj.value("wizardJsonPath").toString();
                    m_stateMachineConfig.logicJsonPath = stateMachineObj.value("logicJsonPath").toString();
                    
                    // 构建完整路径并转换为绝对路径
                    if (!m_stateMachineConfig.fileName.isEmpty() && QDir::isRelativePath(m_stateMachineConfig.fileName)) {
                        if (!configFilePath.isEmpty()) {
                            QFileInfo configFileInfo(configFilePath);
                            QDir configDir = configFileInfo.dir();
                            m_stateMachineConfig.fileName = configDir.absoluteFilePath(m_stateMachineConfig.fileName);
                        } else {
                            // 如果没有提供配置文件路径，使用当前工作目录作为后备方案
                            m_stateMachineConfig.fileName = QDir::currentPath() + "/" + m_stateMachineConfig.fileName;
                        }
                    }
                }
            }
            
            // 向后兼容：如果没有新的状态机配置，但存在单个状态机路径
            if (m_stateMachineConfig.name.isEmpty() && json.contains("stateMachinePath")) {
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
                // uiFile.isMain = false; // 不再从配置文件读取，主界面信息由状态机配置管理
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

    // 一次性加载状态机、向导、逻辑数据（单个配置）
    if (!m_stateMachineConfig.name.isEmpty()) {
        // 加载状态机JSON数据
        if (!m_stateMachineConfig.fileName.isEmpty()) {
            QFile stateMachineFile(m_stateMachineConfig.fileName);
            if (stateMachineFile.exists() && stateMachineFile.open(QIODevice::ReadOnly)) {
                QJsonDocument doc = QJsonDocument::fromJson(stateMachineFile.readAll());
                stateMachineFile.close();
                if (!doc.isNull() && doc.isObject()) {
                    m_stateMachineConfig.stateMachineJson = doc.object();
                    
                    // 从状态机JSON中读取wizardJsonPath字段
                    if (m_stateMachineConfig.stateMachineJson.contains("wizardJsonPath") && 
                        m_stateMachineConfig.stateMachineJson["wizardJsonPath"].isString()) {
                        
                        QString wizardJsonPath = m_stateMachineConfig.stateMachineJson["wizardJsonPath"].toString();
                        qDebug() << "从状态机JSON中读取到wizardJsonPath:" << wizardJsonPath;
                        
                        // 构建完整的wizard文件路径
                        if (!wizardJsonPath.isEmpty() && QDir::isRelativePath(wizardJsonPath)) {
                            // 相对于状态机文件的位置构建路径
                            QFileInfo stateMachineFileInfo(m_stateMachineConfig.fileName);
                            QDir stateMachineDir = stateMachineFileInfo.dir();
                            QString originalPath = wizardJsonPath;
                            m_stateMachineConfig.wizardJsonPath = stateMachineDir.absoluteFilePath(wizardJsonPath);
                            qDebug() << "转换相对路径(相对于状态机文件):" << originalPath << "->" << m_stateMachineConfig.wizardJsonPath;
                        } else {
                            m_stateMachineConfig.wizardJsonPath = wizardJsonPath;
                            qDebug() << "wizardJsonPath是绝对路径:" << m_stateMachineConfig.wizardJsonPath;
                        }
                    } else {
                        qDebug() << "状态机JSON中未找到wizardJsonPath字段或字段类型不正确";
                    }
                }
            }
        }
        
        // 加载向导JSON数据
        qDebug()<<"wizardpath:"<<m_stateMachineConfig.wizardJsonPath;
                if (!m_stateMachineConfig.wizardJsonPath.isEmpty()) {
                    QFile wizardFile(m_stateMachineConfig.wizardJsonPath);
                    if (wizardFile.exists() && wizardFile.open(QIODevice::ReadOnly)) {
                        QJsonDocument doc = QJsonDocument::fromJson(wizardFile.readAll());
                        wizardFile.close();
                        if (!doc.isNull() && doc.isObject()) {
                            m_stateMachineConfig.wizardData.jsonData = doc.object();
                            // 根级别读取name
                            if (m_stateMachineConfig.wizardData.jsonData.contains("name") && m_stateMachineConfig.wizardData.jsonData["name"].isString()) {
                                m_stateMachineConfig.wizardData.name = m_stateMachineConfig.wizardData.jsonData["name"].toString();
                                qDebug() << "Set wizard name from root level:" << m_stateMachineConfig.wizardData.name;
                            } else {
                                qDebug() << "No valid wizard name found in JSON data";
                            }
                            // 解析主界面信息
                            if (m_stateMachineConfig.wizardData.jsonData.contains("mainInterface") && m_stateMachineConfig.wizardData.jsonData["mainInterface"].isObject()) {
                                QJsonObject mainInterface = m_stateMachineConfig.wizardData.jsonData["mainInterface"].toObject();
                                if (mainInterface.contains("name") && mainInterface["name"].isString()) {
                                    m_stateMachineConfig.wizardData.mainInterfaceName = mainInterface["name"].toString();
                                }
                                if (mainInterface.contains("uiFilePath") && mainInterface["uiFilePath"].isString()) {
                                    m_stateMachineConfig.wizardData.mainInterfacePath = mainInterface["uiFilePath"].toString();
                                }
                            }
                        }
                    }
                }
        
        // 加载逻辑JSON数据
        if (!m_stateMachineConfig.logicJsonPath.isEmpty()) {
            QFile logicFile(m_stateMachineConfig.logicJsonPath);
            if (logicFile.exists() && logicFile.open(QIODevice::ReadOnly)) {
                QJsonDocument doc = QJsonDocument::fromJson(logicFile.readAll());
                logicFile.close();
                if (!doc.isNull() && doc.isObject()) {
                    m_stateMachineConfig.logicData.jsonData = doc.object();
                    // 解析逻辑基本信息
                    if (m_stateMachineConfig.logicData.jsonData.contains("name") && m_stateMachineConfig.logicData.jsonData["name"].isString()) {
                        m_stateMachineConfig.logicData.name = m_stateMachineConfig.logicData.jsonData["name"].toString();
                    }
                }
            }
        }
    }

    return true;
}

// 向导文件加载接口实现
bool Product::loadWizardFile(const QString &filePath)
{
    if (filePath.isEmpty()) {
        qWarning() << "向导文件路径为空";
        return false;
    }
    
    QFile wizardFile(filePath);
    if (!wizardFile.exists()) {
        qWarning() << "向导文件不存在:" << filePath;
        return false;
    }
    
    if (!wizardFile.open(QIODevice::ReadOnly)) {
        qWarning() << "无法打开向导文件:" << filePath;
        return false;
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(wizardFile.readAll());
    wizardFile.close();
    
    if (doc.isNull() || !doc.isObject()) {
        qWarning() << "向导文件格式错误:" << filePath;
        return false;
    }
    
    QJsonObject json = doc.object();
    
    // 验证向导类型
    if (!json.contains("type") || json["type"].toString() != "wizard") {
        qWarning() << "文件不是有效的向导类型:" << filePath;
        return false;
    }
    
    // 保存当前向导文件路径
    m_currentWizardFilePath = filePath;
    
    // 更新状态机配置中的向导数据
    m_stateMachineConfig.wizardData.jsonData = json;
    
    // 从根级别读取向导名称
    if (json.contains("name") && json["name"].isString()) {
        m_stateMachineConfig.wizardData.name = json["name"].toString();
        qDebug() << "加载向导名称:" << m_stateMachineConfig.wizardData.name;
    }
    
    // 解析主界面信息
    if (json.contains("mainInterface") && json["mainInterface"].isObject()) {
        QJsonObject mainInterface = json["mainInterface"].toObject();
        if (mainInterface.contains("name") && mainInterface["name"].isString()) {
            m_stateMachineConfig.wizardData.mainInterfaceName = mainInterface["name"].toString();
        }
        if (mainInterface.contains("uiFilePath") && mainInterface["uiFilePath"].isString()) {
            m_stateMachineConfig.wizardData.mainInterfacePath = mainInterface["uiFilePath"].toString();
        }
    }
    
    // 加载控件事件
    m_controlEvents.clear();
    if (json.contains("controlEvents") && json["controlEvents"].isArray()) {
        QJsonArray controlEventsArray = json["controlEvents"].toArray();
        for (const auto &eventValue : controlEventsArray) {
            if (eventValue.isObject()) {
                QJsonObject eventObj = eventValue.toObject();
                
                // 构建事件信息字符串
                QString eventInfo;
                if (eventObj.contains("eventName") && eventObj["eventName"].isString()) {
                    eventInfo += "事件: " + eventObj["eventName"].toString();
                }
                if (eventObj.contains("controlName") && eventObj["controlName"].isString()) {
                    eventInfo += ", 控件: " + eventObj["controlName"].toString();
                }
                if (eventObj.contains("eventType") && eventObj["eventType"].isString()) {
                    eventInfo += ", 类型: " + eventObj["eventType"].toString();
                }
                if (eventObj.contains("targetUIPage") && eventObj["targetUIPage"].isString()) {
                    eventInfo += ", 目标页面: " + eventObj["targetUIPage"].toString();
                }
                
                if (!eventInfo.isEmpty()) {
                    m_controlEvents.append(eventInfo);
                }
            }
        }
        qDebug() << "加载了" << m_controlEvents.size() << "个控件事件";
    }
    
    qDebug() << "向导文件加载成功:" << filePath;
    return true;
}

QList<QString> Product::getControlEvents() const
{
    return m_controlEvents;
}

QString Product::getCurrentWizardName() const
{
    return m_stateMachineConfig.wizardData.name;
}

QString Product::getCurrentWizardFilePath() const
{
    return m_currentWizardFilePath;
}

// 内存缓存管理实现


QString Product::getWizardMainInterfaceName() const
{
    return m_stateMachineConfig.wizardData.mainInterfaceName;
}

QString Product::getWizardMainInterfacePath() const
{
    return m_stateMachineConfig.wizardData.mainInterfacePath;
}

bool Product::updateWizardMainInterface(const QString &uiFileName, const QString &uiFilePath)
{
    if (m_stateMachineConfig.name.isEmpty()) {
        qWarning() << "未找到状态机配置";
        return false;
    }
    
    // 创建主界面信息JSON对象
    QJsonObject mainInterfaceObj;
    mainInterfaceObj["name"] = uiFileName;
    mainInterfaceObj["description"] = QString("应用程序的主界面 - %1").arg(uiFileName);
    mainInterfaceObj["file"] = uiFilePath;
    mainInterfaceObj["type"] = "main_window";
    
    // 更新向导数据中的主界面信息
    m_stateMachineConfig.wizardData.jsonData["mainInterface"] = mainInterfaceObj;
    m_stateMachineConfig.wizardData.mainInterfaceName = uiFileName;
    m_stateMachineConfig.wizardData.mainInterfacePath = uiFilePath;
    
    qDebug() << "向导主界面信息已更新:" << uiFileName;
    
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
    
    // 主界面信息由状态机配置管理，不再检查主UI文件
    
    return true;
}