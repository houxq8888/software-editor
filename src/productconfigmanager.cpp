#include "productconfigmanager.h"
#include <QDebug>

ProductConfigManager::ProductConfigManager(QObject *parent)
    : QObject(parent)
    , m_productModified(false)
    , m_uiLayoutModified(false)
    , m_uiLayoutChanged(false)
{
    // 初始化时间戳
    m_productLastModified = QDateTime::currentDateTime();
    m_uiLayoutLastModified = QDateTime::currentDateTime();
    m_lastUiLayoutChange = QDateTime::currentDateTime();
}

void ProductConfigManager::setProduct(const Product &product)
{
    if (m_product.toJson() != product.toJson()) {
        m_product = product;
        m_productLastModified = QDateTime::currentDateTime();
        
        // 检查UI布局路径是否发生变化
        QString oldUiLayoutPath = m_currentUiLayoutPath;
        QString newUiLayoutPath = product.uiLayoutPath();
        
        if (oldUiLayoutPath != newUiLayoutPath) {
            m_currentUiLayoutPath = newUiLayoutPath;
            m_uiLayoutChanged = true;
            m_lastUiLayoutChange = QDateTime::currentDateTime();
            emit uiLayoutChanged(true);
            emit needsUiBindingChanged(needsUiBinding());
        }
        
        emit productModifiedChanged(m_productModified);
        emit needsSaveChanged(needsSave());
    }
}

Product ProductConfigManager::getProduct() const
{
    return m_product;
}

void ProductConfigManager::setUiLayoutPath(const QString &uiLayoutPath)
{
    if (m_currentUiLayoutPath != uiLayoutPath) {
        QString oldPath = m_currentUiLayoutPath;
        m_currentUiLayoutPath = uiLayoutPath;
        
        // 更新产品中的UI布局路径
        if (m_product.uiLayoutPath() != uiLayoutPath) {
            m_product.setUiLayoutPath(uiLayoutPath);
            m_productModified = true;
            emit productModifiedChanged(true);
        }
        
        // 标记UI布局已改变
        m_uiLayoutChanged = true;
        m_lastUiLayoutChange = QDateTime::currentDateTime();
        emit uiLayoutChanged(true);
        emit needsUiBindingChanged(needsUiBinding());
        emit needsSaveChanged(needsSave());
    }
}

QString ProductConfigManager::getUiLayoutPath() const
{
    return m_currentUiLayoutPath;
}

bool ProductConfigManager::isProductModified() const
{
    return m_productModified;
}

bool ProductConfigManager::isUiLayoutModified() const
{
    return m_uiLayoutModified;
}

bool ProductConfigManager::isUiLayoutChanged() const
{
    return m_uiLayoutChanged;
}

void ProductConfigManager::setProductModified(bool modified)
{
    if (m_productModified != modified) {
        m_productModified = modified;
        if (modified) {
            m_productLastModified = QDateTime::currentDateTime();
        }
        emit productModifiedChanged(modified);
        emit needsSaveChanged(needsSave());
    }
}

void ProductConfigManager::setUiLayoutModified(bool modified)
{
    if (m_uiLayoutModified != modified) {
        m_uiLayoutModified = modified;
        if (modified) {
            m_uiLayoutLastModified = QDateTime::currentDateTime();
        }
        emit uiLayoutModifiedChanged(modified);
        emit needsSaveChanged(needsSave());
    }
}

void ProductConfigManager::setUiLayoutChanged(bool changed)
{
    if (m_uiLayoutChanged != changed) {
        m_uiLayoutChanged = changed;
        if (changed) {
            m_lastUiLayoutChange = QDateTime::currentDateTime();
        }
        emit uiLayoutChanged(changed);
        emit needsUiBindingChanged(needsUiBinding());
    }
}

void ProductConfigManager::resetAllModifiedStates()
{
    setProductModified(false);
    setUiLayoutModified(false);
    setUiLayoutChanged(false);
}

bool ProductConfigManager::needsSave() const
{
    return m_productModified || m_uiLayoutModified;
}

bool ProductConfigManager::needsUiBinding() const
{
    // 需要UI绑定的条件：
    // 1. 有UI布局改变
    // 2. 当前UI布局路径与产品绑定的UI布局路径不同
    // 3. 当前UI布局路径不为空
    return m_uiLayoutChanged && 
           m_currentUiLayoutPath != m_product.uiLayoutPath() && 
           !m_currentUiLayoutPath.isEmpty();
}

QDateTime ProductConfigManager::getProductLastModified() const
{
    return m_productLastModified;
}

QDateTime ProductConfigManager::getUiLayoutLastModified() const
{
    return m_uiLayoutLastModified;
}

void ProductConfigManager::markProductSaved()
{
    setProductModified(false);
    m_productLastModified = QDateTime::currentDateTime();
}

void ProductConfigManager::markUiLayoutSaved()
{
    setUiLayoutModified(false);
    m_uiLayoutLastModified = QDateTime::currentDateTime();
}

void ProductConfigManager::bindUiLayout(const QString &uiLayoutPath)
{
    if (!uiLayoutPath.isEmpty()) {
        // 绑定UI布局
        m_product.setUiLayoutPath(uiLayoutPath);
        m_currentUiLayoutPath = uiLayoutPath;
        
        // 重置UI布局改变状态
        setUiLayoutChanged(false);
        
        // 标记产品已修改（因为UI布局路径已更新）
        setProductModified(true);
        
        qDebug() << "UI布局已绑定:" << uiLayoutPath;
    }
}

bool ProductConfigManager::isValid() const
{
    return !m_product.name().isEmpty() && !m_product.version().isEmpty();
}

bool ProductConfigManager::hasUiLayout() const
{
    return !m_currentUiLayoutPath.isEmpty();
}

QString ProductConfigManager::getStatusDescription() const
{
    QString status;
    
    if (m_productModified) {
        status += "产品信息已修改";
    }
    
    if (m_uiLayoutModified) {
        if (!status.isEmpty()) status += ", ";
        status += "UI布局已修改";
    }
    
    if (m_uiLayoutChanged) {
        if (!status.isEmpty()) status += ", ";
        status += "UI布局已改变";
    }
    
    if (needsUiBinding()) {
        if (!status.isEmpty()) status += ", ";
        status += "需要绑定UI布局";
    }
    
    if (status.isEmpty()) {
        status = "无修改";
    }
    
    return status;
}