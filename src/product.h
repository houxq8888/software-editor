#ifndef PRODUCT_H
#define PRODUCT_H

#include <QString>
#include <QList>
#include <QJsonObject>
#include <QJsonArray>

struct ProductFeature
{
    QString name;
    QString description;
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

    QList<ProductFeature> features() const;
    void setFeatures(const QList<ProductFeature> &features);
    void addFeature(const ProductFeature &feature);
    void removeFeature(int index);

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
    QList<ProductFeature> m_features;
};

#endif // PRODUCT_H