#ifndef PRODUCTSTATE_H
#define PRODUCTSTATE_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include "product.h"

/**
 * @brief ProductState类用于管理产品状态，包含基本信息、特性和UI布局的副本管理
 * 
 * 该类实现了产品状态的正本-副本管理模式：
 * - 打开产品时创建正本（original）和副本（workingCopy）
 * - 所有修改操作只修改副本
 * - 通过比较正本和副本来判断产品是否发生改变
 * - 支持统一的状态判断接口
 */
class ProductState : public QObject
{
    Q_OBJECT

public:
    explicit ProductState(QObject *parent = nullptr);
    
    // 状态管理
    void setOriginalProduct(const Product &product);
    void setWorkingProduct(const Product &product);
    
    // 获取产品信息
    Product getOriginalProduct() const;
    Product getWorkingProduct() const;
    
    // UI布局状态管理
    void setOriginalUiLayoutPath(const QString &path);
    void setWorkingUiLayoutPath(const QString &path);
    void setOriginalUiLayoutModified(bool modified);
    void setWorkingUiLayoutModified(bool modified);
    void setOriginalUiLayoutChanged(bool changed);
    void setWorkingUiLayoutChanged(bool changed);
    
    // 获取UI布局状态
    QString getOriginalUiLayoutPath() const;
    QString getWorkingUiLayoutPath() const;
    bool isOriginalUiLayoutModified() const;
    bool isWorkingUiLayoutModified() const;
    bool isOriginalUiLayoutChanged() const;
    bool isWorkingUiLayoutChanged() const;
    
    // 状态比较接口
    bool isProductModified() const;
    bool isUiLayoutModified() const;
    bool isUiLayoutChanged() const;
    bool isUiBindingChanged() const;
    
    // 统一的状态判断接口
    bool hasChanges() const;
    bool needsSave() const;
    bool needsUiBinding() const;
    
    // 保存操作
    void markSaved();
    void markUiLayoutSaved();
    void markUiBindingSaved();
    
    // 重置状态
    void reset();
    
    // 获取状态描述
    QString getStatusDescription() const;

signals:
    void stateChanged();
    void productModifiedChanged(bool modified);
    void uiLayoutModifiedChanged(bool modified);
    void uiLayoutChanged(bool changed);
    void uiBindingChanged(bool changed);

private:
    Product m_originalProduct;      // 产品正本
    Product m_workingProduct;       // 产品副本
    
    // UI布局状态
    QString m_originalUiLayoutPath;
    QString m_workingUiLayoutPath;
    bool m_originalUiLayoutModified;
    bool m_workingUiLayoutModified;
    bool m_originalUiLayoutChanged;
    bool m_workingUiLayoutChanged;
    
    // 时间戳
    QDateTime m_originalLastModified;
    QDateTime m_workingLastModified;
    QDateTime m_uiLayoutLastModified;
    QDateTime m_uiBindingLastModified;
    
    // 比较产品信息是否相同
    bool isProductInfoEqual(const Product &p1, const Product &p2) const;
    
    // 比较功能特性是否相同
    bool isFeaturesEqual(const QList<ProductFeature> &f1, const QList<ProductFeature> &f2) const;
};

#endif // PRODUCTSTATE_H