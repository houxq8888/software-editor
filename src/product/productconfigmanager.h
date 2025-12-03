#ifndef PRODUCTCONFIGMANAGER_H
#define PRODUCTCONFIGMANAGER_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QTimer>
#include "product.h"
#include "productstate.h"

class ProductConfigManager : public QObject
{
    Q_OBJECT

public:
    explicit ProductConfigManager(QObject *parent = nullptr);
    
    // 产品信息管理
    void setProduct(const Product &product);
    Product getProduct() const;
    
    // UI布局信息管理
    void setUiLayoutPath(const QString &uiLayoutPath);
    QString getUiLayoutPath() const;
    
    // 状态管理
    bool isProductModified() const;
    bool isUiLayoutModified() const;
    bool isUiLayoutChanged() const;
    bool isFeaturesModified() const;
    
    // 修改状态管理
    void setProductModified(bool modified);
    void setUiLayoutModified(bool modified);
    void setUiLayoutChanged(bool changed);
    
    // 重置所有修改状态
    void resetAllModifiedStates();
    
    // 检查是否需要保存
    bool needsSave() const;
    bool needsUiBinding() const;
    
    // 时间戳管理
    QDateTime getProductLastModified() const;
    QDateTime getUiLayoutLastModified() const;
    
    // 保存操作
    void markProductSaved();
    void markUiLayoutSaved();
    
    // 绑定操作
    void bindUiLayout(const QString &uiLayoutPath);
    
    // 双向绑定机制
    void bindProductFeatureToWidget(const QString &featureName, const QString &widgetId);
    void unbindProductFeatureFromWidget(const QString &featureName, const QString &widgetId);
    QMap<QString, QString> getFeatureWidgetBindings() const;
    QString getWidgetIdForFeature(const QString &featureName) const;
    QString getFeatureForWidget(const QString &widgetId) const;
    
    // 布局模板管理
    void saveLayoutTemplate(const QString &templateName, const QString &uiLayoutPath);
    QStringList getAvailableLayoutTemplates() const;
    QString getLayoutTemplate(const QString &templateName) const;
    
    // 验证状态
    bool isValid() const;
    bool hasUiLayout() const;
    
    // 获取状态描述
    QString getStatusDescription() const;
    
    // 实时同步和冲突解决机制
    void startRealTimeSync();
    void stopRealTimeSync();
    bool isRealTimeSyncActive() const;
    
    // 冲突检测和解决
    bool hasConflicts() const;
    QStringList getConflictList() const;
    void resolveConflict(const QString &conflictId, bool useProductVersion);
    void resolveAllConflicts(bool useProductVersion);
    
    // 自动同步功能
    void enableAutoSync(bool enable);
    bool isAutoSyncEnabled() const;
    
    // 同步状态检查
    bool isSynced() const;
    QString getSyncStatus() const;

signals:
    void productModifiedChanged(bool modified);
    void uiLayoutModifiedChanged(bool modified);
    void uiLayoutChanged(bool changed);
    void needsSaveChanged(bool needsSave);
    void needsUiBindingChanged(bool needsBinding);
    
    // 实时同步相关信号
    void realTimeSyncStarted();
    void realTimeSyncStopped();
    void syncStatusChanged(const QString &status);
    void conflictDetected(const QString &conflictId, const QString &description);
    void conflictResolved(const QString &conflictId);
    void autoSyncToggled(bool enabled);
    void uiLayoutPathChanged(const QString &newPath);

private:
    ProductState *m_productState;  // 产品状态管理器
    Product m_product;             // 当前产品（兼容旧代码）
    QString m_currentUiLayoutPath; // 当前UI布局路径（兼容旧代码）
    
    // 状态标志（兼容旧代码）
    bool m_productModified;
    bool m_uiLayoutModified;
    bool m_uiLayoutChanged;
    
    // 时间戳（兼容旧代码）
    QDateTime m_productLastModified;
    QDateTime m_uiLayoutLastModified;
    QDateTime m_lastUiLayoutChange;
    
    // 实时同步相关成员
    bool m_realTimeSyncActive;
    bool m_autoSyncEnabled;
    QTimer *m_syncTimer;
    QMap<QString, QDateTime> m_conflicts;
    
    // 同步检查方法
    void checkForConflicts();
    void performSync();
    void detectFeatureWidgetConflicts();
    void detectLayoutTemplateConflicts();
};

#endif // PRODUCTCONFIGMANAGER_H