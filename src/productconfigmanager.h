#ifndef PRODUCTCONFIGMANAGER_H
#define PRODUCTCONFIGMANAGER_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include "product.h"

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
    
    // 验证状态
    bool isValid() const;
    bool hasUiLayout() const;
    
    // 获取状态描述
    QString getStatusDescription() const;

signals:
    void productModifiedChanged(bool modified);
    void uiLayoutModifiedChanged(bool modified);
    void uiLayoutChanged(bool changed);
    void needsSaveChanged(bool needsSave);
    void needsUiBindingChanged(bool needsBinding);

private:
    Product m_product;
    QString m_currentUiLayoutPath;
    
    // 状态标志
    bool m_productModified;
    bool m_uiLayoutModified;
    bool m_uiLayoutChanged;
    
    // 时间戳
    QDateTime m_productLastModified;
    QDateTime m_uiLayoutLastModified;
    QDateTime m_lastUiLayoutChange;
};

#endif // PRODUCTCONFIGMANAGER_H