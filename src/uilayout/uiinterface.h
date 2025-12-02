#ifndef UIINTERFACE_H
#define UIINTERFACE_H

#include <QString>
#include <QList>
#include <QJsonObject>
#include "layoutitem.h"

// UI界面类，表示一个完整的UI界面
class UIInterface
{
public:
    explicit UIInterface(const QString &name = "", const QString &description = "");
    ~UIInterface() = default;

    // 基本信息
    QString name() const;
    void setName(const QString &name);
    
    QString description() const;
    void setDescription(const QString &description);
    
    QString id() const;
    
    // 布局项管理
    QList<LayoutItem*> layoutItems() const;
    void addLayoutItem(LayoutItem *item);
    void removeLayoutItem(LayoutItem *item);
    void clearLayoutItems();
    
    // 查找布局项
    LayoutItem* findLayoutItem(const QString &widgetId) const;
    
    // 序列化和反序列化
    QJsonObject toJson() const;
    bool fromJson(const QJsonObject &json);
    
    // 复制功能
    UIInterface* clone() const;
    
    // 界面属性
    QSize size() const;
    void setSize(const QSize &size);
    
    QString title() const;
    void setTitle(const QString &title);
    
    bool isMainWindow() const;
    void setIsMainWindow(bool isMain);

private:
    QString m_id;
    QString m_name;
    QString m_description;
    QString m_title;
    QSize m_size;
    bool m_isMainWindow;
    QList<LayoutItem*> m_layoutItems;
};

#endif // UIINTERFACE_H