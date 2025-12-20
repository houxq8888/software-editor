#ifndef PRODUCT_H
#define PRODUCT_H

#include <QString>
#include <QList>
#include <QJsonObject>
#include <QJsonArray>
#include <QUuid>

struct ProductFeature
{
    QString name;
    QString description;
    int priority = 5; // 默认优先级为5
};

struct ProductUIFile
{
    QString name;           // UI文件名称（如：主窗口、设置对话框等）
    QString filePath;       // UI文件路径
    QString type;           // UI文件类型（如：main_window、dialog、widget等）
    QString description;    // UI文件描述
    int order = 0;          // 显示顺序
};

// 向导数据结构
struct WizardData
{
    QString name;                   // 向导名称
    QJsonObject jsonData;           // 向导JSON数据
    QString mainInterfaceName;      // 主界面名称
    QString mainInterfacePath;      // 主界面文件路径
};

// 逻辑数据结构
struct LogicData
{
    QString name;                   // 逻辑名称
    QJsonObject jsonData;           // 逻辑JSON数据
};

struct StateMachineConfig
{
    QString name;                   // 状态机名称
    QString description;            // 状态机描述
    QString fileName;               // 状态机配置文件路径
    QString wizardJsonPath;         // wizard JSON文件路径
    QString logicJsonPath;          // 逻辑JSON文件路径
    QJsonObject stateMachineJson;   // 状态机JSON数据
    WizardData wizardData;          // 向导数据
    LogicData logicData;            // 逻辑数据
};

class Product
{
public:
    Product();
    ~Product() = default;
    
    // Path conversion helper
    QString toAbsolutePath(const QString &relativePath) const;

    // Product information
    QString name() const;      void setName(const QString &name);
    QString version() const;   void setVersion(const QString &version);
    QString description() const; void setDescription(const QString &description);
    QString iconPath() const;  void setIconPath(const QString &iconPath);
    QString screenshotPath() const; void setScreenshotPath(const QString &screenshotPath);
    QString category() const;  void setCategory(const QString &category);
    QString developer() const; void setDeveloper(const QString &developer);
    QString website() const;   void setWebsite(const QString &website);

    // Unique identifier for product distinction
    QString uniqueId() const; void setUniqueId(const QString &uniqueId);
    QString generateUniqueId(); // Generate a new unique Id

    // UI layout file management (backward compatibility)
    QString uiLayoutPath() const; void setUiLayoutPath(const QString &uiLayoutPath);
    
    // Multi-UI file management
    QList<ProductUIFile> uiFiles() const;
    void setUiFiles(const QList<ProductUIFile> &uiFiles);
    void addUiFile(const ProductUIFile &uiFile);
    void removeUiFile(int index);
    void removeUiFile(const QString &filePath);
    ProductUIFile getUiFile(const QString &filePath) const;
    bool hasUiFile(const QString &filePath) const;

    QList<ProductFeature> features() const;
    void setFeatures(const QList<ProductFeature> &features);
    void addFeature(const ProductFeature &feature);
    void removeFeature(int index);

    // State machine management
    QString stateMachinePath() const;
    void setStateMachinePath(const QString &stateMachinePath);
    bool hasStateMachine() const;
    
    // State machine configuration management (single configuration per product)
    StateMachineConfig stateMachineConfig() const;
    void setStateMachineConfig(const StateMachineConfig &config);
    bool hasStateMachineConfig() const; // 检查是否有状态机配置
    QString getWizardJsonPath() const; // 获取wizard JSON文件路径
    QString getWizardAbsolutePath() const; // 获取wizard JSON文件的绝对路径
    
    // 只读数据访问接口
    QString getWizardName() const; // 获取向导名称
    QString getWizardMainInterfaceName() const; // 获取向导主界面名称
    QString getWizardMainInterfacePath() const; // 获取向导主界面文件路径
    
    // 向导数据更新接口 - 基于业务逻辑而不是直接操作JSON
    bool updateWizardMainInterface(const QString &uiFileName, const QString &uiFilePath); // 更新向导主界面信息
    
    // 向导文件加载接口
    bool loadWizardFile(const QString &filePath); // 加载向导文件
    QList<QString> getControlEvents() const; // 获取加载的控件事件列表
    QString getCurrentWizardName() const; // 获取当前向导名称
    QString getCurrentWizardFilePath() const; // 获取当前向导文件路径

    // Serialization
    QJsonObject toJson() const;
    bool fromJson(const QJsonObject &json, const QString &configFilePath = QString());

    // Product configuration package root path management
    QString configPackageRootPath() const;
    void setConfigPackageRootPath(const QString &rootPath);

    // Product configuration package validation
    bool isValidProductConfigPackage() const;

private:
    QString m_name;
    QString m_version;
    QString m_description;
    QString m_iconPath;
    QString m_screenshotPath;
    QString m_category;
    QString m_developer;
    QString m_website;
    QString m_uniqueId;      // Unique identifier for product distinction
    QString m_uiLayoutPath;  // UI layout file path (backward compatibility)
    QString m_stateMachinePath; // State machine file path
    QString m_configPackageRootPath; // Product configuration package root path
    QList<ProductUIFile> m_uiFiles; // Multiple UI files
    QList<ProductFeature> m_features;
    StateMachineConfig m_stateMachineConfig; // Single state machine configuration per product
    
    // 向导文件加载相关数据
    QString m_currentWizardFilePath; // 当前加载的向导文件路径
    QList<QString> m_controlEvents; // 加载的控件事件列表
};

#endif // PRODUCT_H