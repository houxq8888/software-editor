#include "productstate.h"
#include <QDebug>

ProductState::ProductState(QObject *parent)
    : QObject(parent)
    , m_originalUiLayoutModified(false)
    , m_workingUiLayoutModified(false)
    , m_originalUiLayoutChanged(false)
    , m_workingUiLayoutChanged(false)
{
    // 初始化时间戳
    m_originalLastModified = QDateTime::currentDateTime();
    m_workingLastModified = QDateTime::currentDateTime();
    m_uiLayoutLastModified = QDateTime::currentDateTime();
    m_uiBindingLastModified = QDateTime::currentDateTime();
}

void ProductState::setOriginalProduct(const Product &product)
{
    m_originalProduct = product;
    m_originalLastModified = QDateTime::currentDateTime();
    emit stateChanged();
}

void ProductState::setWorkingProduct(const Product &product)
{
    m_workingProduct = product;
    m_workingLastModified = QDateTime::currentDateTime();
    emit stateChanged();
}

Product ProductState::getOriginalProduct() const
{
    return m_originalProduct;
}

Product ProductState::getWorkingProduct() const
{
    return m_workingProduct;
}

void ProductState::setOriginalUiLayoutPath(const QString &path)
{
    m_originalUiLayoutPath = path;
    emit stateChanged();
}

void ProductState::setWorkingUiLayoutPath(const QString &path)
{
    m_workingUiLayoutPath = path;
    emit stateChanged();
}

void ProductState::setOriginalUiLayoutModified(bool modified)
{
    m_originalUiLayoutModified = modified;
    if (modified) {
        m_uiLayoutLastModified = QDateTime::currentDateTime();
    }
    emit uiLayoutModifiedChanged(modified);
    emit stateChanged();
}

void ProductState::setWorkingUiLayoutModified(bool modified)
{
    m_workingUiLayoutModified = modified;
    if (modified) {
        m_uiLayoutLastModified = QDateTime::currentDateTime();
    }
    emit uiLayoutModifiedChanged(modified);
    emit stateChanged();
}

void ProductState::setOriginalUiLayoutChanged(bool changed)
{
    m_originalUiLayoutChanged = changed;
    if (changed) {
        m_uiBindingLastModified = QDateTime::currentDateTime();
    }
    emit uiLayoutChanged(changed);
    emit stateChanged();
}

void ProductState::setWorkingUiLayoutChanged(bool changed)
{
    m_workingUiLayoutChanged = changed;
    if (changed) {
        m_uiBindingLastModified = QDateTime::currentDateTime();
    }
    emit uiLayoutChanged(changed);
    emit stateChanged();
}

QString ProductState::getOriginalUiLayoutPath() const
{
    return m_originalUiLayoutPath;
}

QString ProductState::getWorkingUiLayoutPath() const
{
    return m_workingUiLayoutPath;
}

bool ProductState::isOriginalUiLayoutModified() const
{
    return m_originalUiLayoutModified;
}

bool ProductState::isWorkingUiLayoutModified() const
{
    return m_workingUiLayoutModified;
}

bool ProductState::isOriginalUiLayoutChanged() const
{
    return m_originalUiLayoutChanged;
}

bool ProductState::isWorkingUiLayoutChanged() const
{
    return m_workingUiLayoutChanged;
}

bool ProductState::isProductModified() const
{
    // 比较产品基本信息是否相同
    if (!isProductInfoEqual(m_originalProduct, m_workingProduct)) {
        return true;
    }
    
    // 比较功能特性是否相同
    if (!isFeaturesEqual(m_originalProduct.features(), m_workingProduct.features())) {
        return true;
    }
    
    return false;
}

bool ProductState::isUiLayoutModified() const
{
    // UI布局内容发生改变（新增控件、删除控件、修改布局等）
    return m_workingUiLayoutModified;
}

bool ProductState::isUiLayoutChanged() const
{
    // UI布局绑定发生改变（加载了不同的UI文件）
    return m_workingUiLayoutChanged || (m_originalUiLayoutPath != m_workingUiLayoutPath);
}

bool ProductState::isUiBindingChanged() const
{
    // UI绑定发生改变（UI布局文件路径改变）
    return m_originalUiLayoutPath != m_workingUiLayoutPath;
}

bool ProductState::hasChanges() const
{
    return isProductModified() || isUiLayoutModified() || isUiLayoutChanged();
}

bool ProductState::needsSave() const
{
    // 需要保存的条件：产品信息改变或UI布局内容改变
    return isProductModified() || isUiLayoutModified();
}

bool ProductState::needsUiBinding() const
{
    // 需要UI绑定的条件：UI布局绑定发生改变
    return isUiBindingChanged();
}

void ProductState::markSaved()
{
    // 保存后，将副本状态同步到正本
    m_originalProduct = m_workingProduct;
    m_originalUiLayoutPath = m_workingUiLayoutPath;
    m_originalUiLayoutModified = m_workingUiLayoutModified;
    m_originalUiLayoutChanged = m_workingUiLayoutChanged;
    
    // 重置修改状态
    m_workingUiLayoutModified = false;
    m_workingUiLayoutChanged = false;
    
    m_originalLastModified = QDateTime::currentDateTime();
    emit stateChanged();
}

void ProductState::markUiLayoutSaved()
{
    // UI布局保存后，同步UI布局状态
    m_originalUiLayoutModified = m_workingUiLayoutModified;
    m_workingUiLayoutModified = false;
    emit stateChanged();
}

void ProductState::markUiBindingSaved()
{
    // UI绑定保存后，同步UI绑定状态
    m_originalUiLayoutPath = m_workingUiLayoutPath;
    m_originalUiLayoutChanged = m_workingUiLayoutChanged;
    m_workingUiLayoutChanged = false;
    emit stateChanged();
}

void ProductState::reset()
{
    // 重置所有状态
    m_originalProduct = Product();
    m_workingProduct = Product();
    m_originalUiLayoutPath.clear();
    m_workingUiLayoutPath.clear();
    m_originalUiLayoutModified = false;
    m_workingUiLayoutModified = false;
    m_originalUiLayoutChanged = false;
    m_workingUiLayoutChanged = false;
    
    m_originalLastModified = QDateTime::currentDateTime();
    m_workingLastModified = QDateTime::currentDateTime();
    m_uiLayoutLastModified = QDateTime::currentDateTime();
    m_uiBindingLastModified = QDateTime::currentDateTime();
    
    emit stateChanged();
}

QString ProductState::getStatusDescription() const
{
    QStringList statuses;
    
    if (isProductModified()) {
        statuses.append("产品信息已修改");
    }
    
    if (isUiLayoutModified()) {
        statuses.append("UI布局内容已修改");
    }
    
    if (isUiBindingChanged()) {
        statuses.append("UI绑定已改变");
    }
    
    if (statuses.isEmpty()) {
        return "无修改";
    }
    
    return statuses.join(", ");
}

bool ProductState::isProductInfoEqual(const Product &p1, const Product &p2) const
{
    return p1.name() == p2.name() &&
           p1.version() == p2.version() &&
           p1.description() == p2.description() &&
           p1.iconPath() == p2.iconPath() &&
           p1.screenshotPath() == p2.screenshotPath() &&
           p1.category() == p2.category() &&
           p1.developer() == p2.developer() &&
           p1.website() == p2.website() &&
           p1.uniqueId() == p2.uniqueId() &&
           p1.uiLayoutPath() == p2.uiLayoutPath();
}

bool ProductState::isFeaturesEqual(const QList<ProductFeature> &f1, const QList<ProductFeature> &f2) const
{
    if (f1.size() != f2.size()) {
        return false;
    }
    
    for (int i = 0; i < f1.size(); ++i) {
        if (f1[i].name != f2[i].name || f1[i].description != f2[i].description) {
            return false;
        }
    }
    
    return true;
}