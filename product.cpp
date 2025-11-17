#include "product.h"

Product::Product()
{
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