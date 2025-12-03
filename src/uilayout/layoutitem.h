#ifndef LAYOUTITEM_H
#define LAYOUTITEM_H

#include <QString>
#include <QPoint>
#include <QSize>
#include <QObject>

// 用于表示布局控件的结构
class LayoutItem {
public:
    explicit LayoutItem(const QString &widgetType, const QString &displayName = "");
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
    QObject *createWidget(QWidget *parent) const;
    
    // 设置和获取控件所属的Tab页索引
    void setTabIndex(int index);
    int tabIndex() const;
    
    // DockWidget相关属性
    Qt::DockWidgetArea dockArea() const;
    void setDockArea(Qt::DockWidgetArea area);
    bool isFloating() const;
    void setFloating(bool floating);

private:
    QString m_widgetType;
    QString m_displayName;
    QString m_text;
    QPoint m_pos;
    QSize m_size;
    int m_zIndex;
    int m_tabIndex; // 控件所属的Tab页索引，-1表示不在任何Tab页中
    Qt::DockWidgetArea m_dockArea; // DockWidget的停靠区域
    bool m_floating; // DockWidget是否浮动
};

#endif // LAYOUTITEM_H