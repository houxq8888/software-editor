#include "productconfigmanager.h"
#include <QFileInfo>
#include <QDateTime>
#include <QSettings>
#include <QDir>
#include <QTimer>
#include <QDebug>

ProductConfigManager::ProductConfigManager(QObject *parent)
    : QObject(parent)
    , m_productModified(false)
    , m_uiLayoutModified(false)
    , m_uiLayoutChanged(false)
    , m_realTimeSyncActive(false)
    , m_autoSyncEnabled(false)
{
    m_productLastModified = QDateTime::currentDateTime();
    m_uiLayoutLastModified = QDateTime::currentDateTime();
    m_lastUiLayoutChange = QDateTime::currentDateTime();
    
    // 初始化同步定时器
    m_syncTimer = new QTimer(this);
    m_syncTimer->setInterval(5000); // 5秒检查一次
    connect(m_syncTimer, &QTimer::timeout, this, &ProductConfigManager::checkForConflicts);
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

// 双向绑定机制实现
void ProductConfigManager::bindProductFeatureToWidget(const QString &featureName, const QString &widgetId)
{
    QSettings settings;
    settings.beginGroup("FeatureWidgetBindings");
    settings.setValue(featureName, widgetId);
    settings.endGroup();
    
    emit productModifiedChanged(true);
}

void ProductConfigManager::unbindProductFeatureFromWidget(const QString &featureName, const QString &widgetId)
{
    QSettings settings;
    settings.beginGroup("FeatureWidgetBindings");
    settings.remove(featureName);
    settings.endGroup();
    
    emit productModifiedChanged(true);
}

QMap<QString, QString> ProductConfigManager::getFeatureWidgetBindings() const
{
    QMap<QString, QString> bindings;
    QSettings settings;
    settings.beginGroup("FeatureWidgetBindings");
    
    QStringList keys = settings.allKeys();
    for (const QString &key : keys) {
        bindings[key] = settings.value(key).toString();
    }
    
    settings.endGroup();
    return bindings;
}

QString ProductConfigManager::getWidgetIdForFeature(const QString &featureName) const
{
    QSettings settings;
    settings.beginGroup("FeatureWidgetBindings");
    QString widgetId = settings.value(featureName).toString();
    settings.endGroup();
    return widgetId;
}

QString ProductConfigManager::getFeatureForWidget(const QString &widgetId) const
{
    QSettings settings;
    settings.beginGroup("FeatureWidgetBindings");
    
    QStringList keys = settings.allKeys();
    for (const QString &key : keys) {
        if (settings.value(key).toString() == widgetId) {
            return key;
        }
    }
    
    settings.endGroup();
    return QString();
}

// 布局模板管理
void ProductConfigManager::saveLayoutTemplate(const QString &templateName, const QString &uiLayoutPath)
{
    QSettings settings;
    settings.beginGroup("LayoutTemplates");
    settings.setValue(templateName, uiLayoutPath);
    settings.endGroup();
}

QStringList ProductConfigManager::getAvailableLayoutTemplates() const
{
    QSettings settings;
    settings.beginGroup("LayoutTemplates");
    QStringList templates = settings.allKeys();
    settings.endGroup();
    return templates;
}

QString ProductConfigManager::getLayoutTemplate(const QString &templateName) const
{
    QSettings settings;
    settings.beginGroup("LayoutTemplates");
    QString templatePath = settings.value(templateName).toString();
    settings.endGroup();
    return templatePath;
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
    
    if (m_realTimeSyncActive) {
        if (!status.isEmpty()) status += ", ";
        status += "实时同步中";
    }
    
    if (hasConflicts()) {
        if (!status.isEmpty()) status += ", ";
        status += QString("有%1个冲突").arg(m_conflicts.size());
    }
    
    if (status.isEmpty()) {
        status = "无修改";
    }
    
    return status;
}

// 实时同步和冲突解决机制实现
void ProductConfigManager::startRealTimeSync()
{
    if (m_realTimeSyncActive) {
        return;
    }
    
    m_realTimeSyncActive = true;
    m_syncTimer->start();
    
    qDebug() << "ProductConfigManager: 实时同步已启动";
    emit realTimeSyncStarted();
    emit syncStatusChanged("实时同步已启动");
}

void ProductConfigManager::stopRealTimeSync()
{
    if (!m_realTimeSyncActive) {
        return;
    }
    
    m_realTimeSyncActive = false;
    m_syncTimer->stop();
    
    qDebug() << "ProductConfigManager: 实时同步已停止";
    emit realTimeSyncStopped();
    emit syncStatusChanged("实时同步已停止");
}

bool ProductConfigManager::isRealTimeSyncActive() const
{
    return m_realTimeSyncActive;
}

bool ProductConfigManager::hasConflicts() const
{
    return !m_conflicts.isEmpty();
}

QStringList ProductConfigManager::getConflictList() const
{
    return m_conflicts.keys();
}

void ProductConfigManager::resolveConflict(const QString &conflictId, bool useProductVersion)
{
    if (!m_conflicts.contains(conflictId)) {
        return;
    }
    
    // 根据冲突类型进行解决
    if (conflictId.startsWith("feature_")) {
        // 功能特性冲突
        QString featureName = conflictId.mid(8); // 去掉"feature_"前缀
        
        if (useProductVersion) {
            // 使用产品版本：移除UI布局中的绑定
            QSettings settings;
            settings.beginGroup("FeatureWidgetBindings");
            settings.remove(featureName);
            settings.endGroup();
        } else {
            // 使用UI布局版本：保持绑定不变
            // 这里不需要做任何操作，因为UI布局版本已经是最新的
        }
    } else if (conflictId.startsWith("layout_")) {
        // 布局模板冲突
        QString templateName = conflictId.mid(7); // 去掉"layout_"前缀
        
        if (useProductVersion) {
            // 使用产品版本：恢复产品中的布局模板
            // 这里需要从产品配置中获取模板信息
        } else {
            // 使用UI布局版本：使用当前的布局模板
            // 这里不需要做任何操作
        }
    }
    
    // 移除冲突
    m_conflicts.remove(conflictId);
    
    qDebug() << "ProductConfigManager: 冲突已解决:" << conflictId << "使用" << (useProductVersion ? "产品版本" : "UI布局版本");
    emit conflictResolved(conflictId);
    
    // 检查是否还有冲突
    if (m_conflicts.isEmpty()) {
        emit syncStatusChanged("所有冲突已解决");
    }
}

void ProductConfigManager::resolveAllConflicts(bool useProductVersion)
{
    QStringList conflicts = m_conflicts.keys();
    for (const QString &conflictId : conflicts) {
        resolveConflict(conflictId, useProductVersion);
    }
}

void ProductConfigManager::enableAutoSync(bool enable)
{
    if (m_autoSyncEnabled == enable) {
        return;
    }
    
    m_autoSyncEnabled = enable;
    
    if (enable && !m_realTimeSyncActive) {
        startRealTimeSync();
    } else if (!enable && m_realTimeSyncActive) {
        stopRealTimeSync();
    }
    
    qDebug() << "ProductConfigManager: 自动同步" << (enable ? "已启用" : "已禁用");
    emit autoSyncToggled(enable);
}

bool ProductConfigManager::isAutoSyncEnabled() const
{
    return m_autoSyncEnabled;
}

bool ProductConfigManager::isSynced() const
{
    return !m_productModified && !m_uiLayoutModified && !hasConflicts();
}

QString ProductConfigManager::getSyncStatus() const
{
    if (!m_realTimeSyncActive) {
        return "实时同步未启动";
    }
    
    if (hasConflicts()) {
        return QString("有%1个冲突需要解决").arg(m_conflicts.size());
    }
    
    if (isSynced()) {
        return "已同步";
    }
    
    return "同步中...";
}

// 私有同步检查方法
void ProductConfigManager::checkForConflicts()
{
    if (!m_realTimeSyncActive) {
        return;
    }
    
    // 检测各种类型的冲突
    detectFeatureWidgetConflicts();
    detectLayoutTemplateConflicts();
    
    // 如果有冲突，更新状态
    if (hasConflicts()) {
        emit syncStatusChanged(QString("检测到%1个冲突").arg(m_conflicts.size()));
    } else {
        emit syncStatusChanged("无冲突");
    }
}

void ProductConfigManager::performSync()
{
    if (!m_realTimeSyncActive) {
        return;
    }
    
    // 执行同步操作
    qDebug() << "ProductConfigManager: 执行同步操作";
    
    // 检查是否需要同步产品信息
    if (m_productModified) {
        // 同步产品信息到UI布局
        // 这里可以实现具体的同步逻辑
    }
    
    // 检查是否需要同步UI布局
    if (m_uiLayoutModified) {
        // 同步UI布局到产品信息
        // 这里可以实现具体的同步逻辑
    }
    
    emit syncStatusChanged("同步完成");
}

void ProductConfigManager::detectFeatureWidgetConflicts()
{
    // 检测功能特性与控件绑定的冲突
    QMap<QString, QString> currentBindings = getFeatureWidgetBindings();
    
    // 获取产品功能特性
    QList<ProductFeature> features = m_product.features();
    
    for (const ProductFeature &feature : features) {
        QString featureName = feature.name;
        QString widgetId = getWidgetIdForFeature(featureName);
        
        if (!widgetId.isEmpty()) {
            // 检查绑定是否有效
            QString conflictId = "feature_" + featureName;
            
            // 这里可以添加更复杂的冲突检测逻辑
            // 例如：检查控件是否存在、检查绑定是否过期等
            
            // 暂时简单标记为需要检查
            if (!m_conflicts.contains(conflictId)) {
                m_conflicts[conflictId] = QDateTime::currentDateTime();
                emit conflictDetected(conflictId, QString("功能特性'%1'的控件绑定需要检查").arg(featureName));
            }
        }
    }
}

void ProductConfigManager::detectLayoutTemplateConflicts()
{
    // 检测布局模板的冲突
    QStringList templates = getAvailableLayoutTemplates();
    
    for (const QString &templateName : templates) {
        QString templatePath = getLayoutTemplate(templateName);
        
        // 检查模板文件是否存在
        if (!QFile::exists(templatePath)) {
            QString conflictId = "layout_" + templateName;
            
            if (!m_conflicts.contains(conflictId)) {
                m_conflicts[conflictId] = QDateTime::currentDateTime();
                emit conflictDetected(conflictId, QString("布局模板'%1'的文件不存在").arg(templateName));
            }
        }
    }
}



