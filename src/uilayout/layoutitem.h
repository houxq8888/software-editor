#ifndef LAYOUTITEM_H
#define LAYOUTITEM_H

#include <QString>
#include <QPoint>
#include <QSize>
#include <QObject>
#include <QList>

// 用于表示布局控件的结构
class LayoutItem {
public:
    explicit LayoutItem(const QString &widgetType, const QString &displayName = "", const QString &name = "");
    ~LayoutItem() = default;
    
    QString widgetType() const;
    QString displayName() const;
    QString text() const;
    void setText(const QString &text);
    QPoint pos() const;
    void setPos(const QPoint &pos);
    QSize size() const;
    void setSize(const QSize &size);
    int zIndex() const;
    void setZIndex(int zIndex);
    QString name() const;
    void setName(const QString &name);
    QObject *createWidget(QWidget *parent) const;
    
    // 设置和获取控件所属的Tab页索引
    void setTabIndex(int index);
    int tabIndex() const;
    
    // DockWidget相关属性
    Qt::DockWidgetArea dockArea() const;
    void setDockArea(Qt::DockWidgetArea area);
    bool isFloating() const;
    void setFloating(bool floating);
    
    // 布局相关属性
    bool isLayout() const;
    void addChild(LayoutItem *child);
    QList<LayoutItem*> children() const;
    void setParentLayout(LayoutItem *parent);
    LayoutItem* parentLayout() const;

private:
    QString m_widgetType;
    QString m_displayName;
    QString m_text;
    QString m_name;
    QPoint m_pos;
    QSize m_size;
    int m_zIndex;
    int m_tabIndex; // 控件所属的Tab页索引，-1表示不在任何Tab页中
    Qt::DockWidgetArea m_dockArea; // DockWidget的停靠区域
    bool m_floating; // DockWidget是否浮动
    
    // 布局相关成员
    QList<LayoutItem*> m_children;
    LayoutItem* m_parentLayout = nullptr;
};

#endif // LAYOUTITEM_H