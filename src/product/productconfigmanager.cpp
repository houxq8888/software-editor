#include "productconfigmanager.h"
#include <QFileInfo>
#include <QDateTime>
#include <QSettings>
#include <QDir>
#include <QTimer>
#include <QDebug>

ProductConfigManager::ProductConfigManager(QObject *parent)
    : QObject(parent)
    , m_productState(new ProductState(this))
    , m_productModified(false)
    , m_uiLayoutModified(false)
    , m_uiLayoutChanged(false)
    , m_realTimeSyncActive(false)
    , m_autoSyncEnabled(false)
{
    m_productLastModified = QDateTime::currentDateTime();
    m_uiLayoutLastModified = QDateTime::currentDateTime();
    m_lastUiLayoutChange = QDateTime::currentDateTime();
    
    // 连接ProductState的信号
    connect(m_productState, &ProductState::stateChanged, this, [this]() {
        emit needsSaveChanged(needsSave());
        emit needsUiBindingChanged(needsUiBinding());
    });
    
    connect(m_productState, &ProductState::productModifiedChanged, this, &ProductConfigManager::productModifiedChanged);
    connect(m_productState, &ProductState::uiLayoutModifiedChanged, this, &ProductConfigManager::uiLayoutModifiedChanged);
    connect(m_productState, &ProductState::uiLayoutChanged, this, &ProductConfigManager::uiLayoutChanged);
    connect(m_productState, &ProductState::uiBindingChanged, this, [this](bool changed) {
        emit uiLayoutChanged(changed);
        emit needsUiBindingChanged(needsUiBinding());
    });
    
    // 初始化同步定时器
    m_syncTimer = new QTimer(this);
    m_syncTimer->setInterval(5000); // 5秒检查一次
    connect(m_syncTimer, &QTimer::timeout, this, &ProductConfigManager::checkForConflicts);
}

ProductConfigManager::~ProductConfigManager()
{
    // 确保定时器被正确停止和清理
    qDebug() << "ProductConfigManager析构函数开始执行";
    if (m_syncTimer) {
        qDebug() << "停止和清理同步定时器";
        m_syncTimer->stop();
        delete m_syncTimer;
        m_syncTimer = nullptr;
    }
    qDebug() << "ProductConfigManager析构函数执行完成";
}

void ProductConfigManager::setProduct(const Product &product)
{
    // 使用ProductState进行状态管理
    if (m_productState->getWorkingProduct().toJson() != product.toJson()) {
        // 同时设置原始产品和工作产品，确保打开新产品时两者一致
        m_productState->setOriginalProduct(product);
        m_productState->setWorkingProduct(product);
        
        // 检查UI布局路径是否发生变化
        QString oldUiLayoutPath = m_productState->getWorkingUiLayoutPath();
        QString newUiLayoutPath = product.uiLayoutPath();
        
        if (oldUiLayoutPath != newUiLayoutPath) {
            m_productState->setOriginalUiLayoutPath(newUiLayoutPath);
            m_productState->setWorkingUiLayoutPath(newUiLayoutPath);
            m_productState->setWorkingUiLayoutChanged(true);
        }
        
        // 兼容旧代码
        m_product = product;
        m_productLastModified = QDateTime::currentDateTime();
        
        if (oldUiLayoutPath != newUiLayoutPath) {
            m_currentUiLayoutPath = newUiLayoutPath;
            m_uiLayoutChanged = true;
            m_lastUiLayoutChange = QDateTime::currentDateTime();
        }
    }
}

Product ProductConfigManager::getProduct() const
{
    // 优先使用ProductState
    return m_productState->getWorkingProduct();
}

void ProductConfigManager::setUiLayoutPath(const QString &uiLayoutPath)
{
    // 使用ProductState进行状态管理
    if (m_productState->getWorkingUiLayoutPath() != uiLayoutPath) {
        QString oldPath = m_productState->getWorkingUiLayoutPath();
        
        // 更新ProductState中的UI布局路径
        m_productState->setWorkingUiLayoutPath(uiLayoutPath);
        
        // 更新产品中的UI布局路径
        Product workingProduct = m_productState->getWorkingProduct();
        if (workingProduct.uiLayoutPath() != uiLayoutPath) {
            workingProduct.setUiLayoutPath(uiLayoutPath);
            m_productState->setWorkingProduct(workingProduct);
        }
        
        // 标记UI布局已改变
        m_productState->setWorkingUiLayoutChanged(true);
        
        // 兼容旧代码
        m_currentUiLayoutPath = uiLayoutPath;
        m_uiLayoutChanged = true;
        m_lastUiLayoutChange = QDateTime::currentDateTime();
    }
}

QString ProductConfigManager::getUiLayoutPath() const
{
    // 优先使用ProductState
    return m_productState->getWorkingUiLayoutPath();
}

bool ProductConfigManager::isProductModified() const
{
    // 使用ProductState的统一接口
    return m_productState->isProductModified();
}

bool ProductConfigManager::isUiLayoutModified() const
{
    // 使用ProductState的统一接口
    return m_productState->isUiLayoutModified();
}

bool ProductConfigManager::isUiLayoutChanged() const
{
    // 使用ProductState的统一接口
    return m_productState->isUiLayoutChanged();
}

bool ProductConfigManager::isFeaturesModified() const
{
    // 检查功能特性是否被修改
    // 通过比较正本和副本的功能特性列表来判断
    Product originalProduct = m_productState->getOriginalProduct();
    Product workingProduct = m_productState->getWorkingProduct();
    
    QList<ProductFeature> originalFeatures = originalProduct.features();
    QList<ProductFeature> workingFeatures = workingProduct.features();
    
    // 如果功能特性数量不同，说明有修改
    if (originalFeatures.size() != workingFeatures.size()) {
        return true;
    }
    
    // 逐个比较功能特性
    for (int i = 0; i < originalFeatures.size(); ++i) {
        if (originalFeatures[i].name != workingFeatures[i].name || 
            originalFeatures[i].description != workingFeatures[i].description) {
            return true;
        }
    }
    
    return false;
}

void ProductConfigManager::setProductModified(bool modified)
{
    // 使用ProductState进行状态管理
    if (m_productState->isProductModified() != modified) {
        // 通过修改产品副本来触发状态变化
        if (modified) {
            // 标记产品已修改
            Product workingProduct = m_productState->getWorkingProduct();
            workingProduct.setName(workingProduct.name() + " "); // 微小修改
            m_productState->setWorkingProduct(workingProduct);
        }
        
        // 兼容旧代码
        m_productModified = modified;
        if (modified) {
            m_productLastModified = QDateTime::currentDateTime();
        }
    }
}

void ProductConfigManager::setUiLayoutModified(bool modified)
{
    // 使用ProductState进行状态管理
    if (m_productState->isUiLayoutModified() != modified) {
        m_productState->setWorkingUiLayoutModified(modified);
        
        // 兼容旧代码
        m_uiLayoutModified = modified;
        if (modified) {
            m_uiLayoutLastModified = QDateTime::currentDateTime();
        }
    }
}

void ProductConfigManager::setUiLayoutChanged(bool changed)
{
    // 使用ProductState进行状态管理
    if (m_productState->isUiLayoutChanged() != changed) {
        m_productState->setWorkingUiLayoutChanged(changed);
        
        // 兼容旧代码
        m_uiLayoutChanged = changed;
        if (changed) {
            m_lastUiLayoutChange = QDateTime::currentDateTime();
        }
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
    // 使用ProductState的统一接口
    return m_productState->needsSave();
}

bool ProductConfigManager::needsUiBinding() const
{
    // 使用ProductState的统一接口
    return m_productState->needsUiBinding();
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
    // 使用ProductState进行状态管理
    m_productState->markSaved();
    
    // 兼容旧代码
    setProductModified(false);
    m_productLastModified = QDateTime::currentDateTime();
}

void ProductConfigManager::markUiLayoutSaved()
{
    // 使用ProductState进行状态管理
    m_productState->markUiLayoutSaved();
    
    // 同步产品中的UI布局路径
    Product workingProduct = m_productState->getWorkingProduct();
    QString workingUiLayoutPath = m_productState->getWorkingUiLayoutPath();
    if (workingProduct.uiLayoutPath() != workingUiLayoutPath) {
        workingProduct.setUiLayoutPath(workingUiLayoutPath);
        m_productState->setWorkingProduct(workingProduct);
    }
    
    // 兼容旧代码
    setUiLayoutModified(false);
    setUiLayoutChanged(false);
    m_uiLayoutLastModified = QDateTime::currentDateTime();
}

void ProductConfigManager::bindUiLayout(const QString &uiLayoutPath)
{
    if (!uiLayoutPath.isEmpty()) {
        // 使用ProductState进行状态管理
        
        // 更新产品中的UI布局路径
        Product workingProduct = m_productState->getWorkingProduct();
        workingProduct.setUiLayoutPath(uiLayoutPath);
        m_productState->setWorkingProduct(workingProduct);
        
        // 更新UI布局路径
        m_productState->setWorkingUiLayoutPath(uiLayoutPath);
        
        // 重置UI布局改变状态
        m_productState->setWorkingUiLayoutChanged(false);
        
        // 标记产品已修改（因为UI布局路径已更新）
        // 通过修改产品副本来触发状态变化
        workingProduct.setName(workingProduct.name() + " "); // 微小修改
        m_productState->setWorkingProduct(workingProduct);
        
        // 兼容旧代码
        m_product.setUiLayoutPath(uiLayoutPath);
        m_currentUiLayoutPath = uiLayoutPath;
        setUiLayoutChanged(false);
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
    
    // UI绑定修改应该触发UI布局修改状态，而不是产品信息修改状态
    setUiLayoutModified(true);
    qDebug() << "ProductConfigManager: 功能特性绑定，设置UI布局修改状态";
}

void ProductConfigManager::unbindProductFeatureFromWidget(const QString &featureName, const QString &widgetId)
{
    QSettings settings;
    settings.beginGroup("FeatureWidgetBindings");
    settings.remove(featureName);
    settings.endGroup();
    
    // UI绑定修改应该触发UI布局修改状态，而不是产品信息修改状态
    setUiLayoutModified(true);
    qDebug() << "ProductConfigManager: 功能特性解绑，设置UI布局修改状态";
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
    
    // 使用ProductState的统一接口
    if (m_productState->isProductModified()) {
        status += "产品信息已修改";
    }
    
    if (m_productState->isUiLayoutModified()) {
        if (!status.isEmpty()) status += ", ";
        status += "UI布局已修改";
    }
    
    if (m_productState->isUiLayoutChanged()) {
        if (!status.isEmpty()) status += ", ";
        status += "UI布局已改变";
    }
    
    if (m_productState->needsUiBinding()) {
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



