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
    bool isMain = false;    // 是否为主UI文件
    QString description;    // UI文件描述
    int order = 0;          // 显示顺序
};

class Product
{
public:
    Product();
    ~Product() = default;

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
    ProductUIFile getMainUiFile() const;
    ProductUIFile getUiFile(const QString &filePath) const;
    bool hasUiFile(const QString &filePath) const;
    void setMainUiFile(const QString &filePath);

    QList<ProductFeature> features() const;
    void setFeatures(const QList<ProductFeature> &features);
    void addFeature(const ProductFeature &feature);
    void removeFeature(int index);

    // State machine management
    QString stateMachinePath() const;
    void setStateMachinePath(const QString &stateMachinePath);
    bool hasStateMachine() const;

    // Serialization
    QJsonObject toJson() const;
    bool fromJson(const QJsonObject &json);

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
    QList<ProductUIFile> m_uiFiles; // Multiple UI files
    QList<ProductFeature> m_features;
};

#endif // PRODUCT_H