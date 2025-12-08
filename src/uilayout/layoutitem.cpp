#include "layoutitem.h"

LayoutItem::LayoutItem(const QString &widgetType, const QString &displayName)
    : m_widgetType(widgetType)
    , m_displayName(displayName.isEmpty() ? widgetType : displayName)
    , m_text("")
    , m_pos(0, 0)
    , m_size(100, 30)
    , m_zIndex(0)
    , m_tabIndex(-1)
    , m_dockArea(Qt::LeftDockWidgetArea)
    , m_floating(false)
{
}

QString LayoutItem::widgetType() const
{
    return m_widgetType;
}

QString LayoutItem::displayName() const
{
    return m_displayName;
}

QString LayoutItem::text() const
{
    return m_text;
}

void LayoutItem::setText(const QString &text)
{
    m_text = text;
}

QPoint LayoutItem::pos() const
{
    return m_pos;
}

void LayoutItem::setPos(const QPoint &pos)
{
    m_pos = pos;
}

QSize LayoutItem::size() const
{
    return m_size;
}

void LayoutItem::setSize(const QSize &size)
{
    m_size = size;
}

int LayoutItem::zIndex() const
{
    return m_zIndex;
}

void LayoutItem::setZIndex(int zIndex)
{
    m_zIndex = zIndex;
}

QObject *LayoutItem::createWidget(QWidget *parent) const
{
    // 这是一个占位实现，实际应该根据widgetType创建对应的控件
    return nullptr;
}

void LayoutItem::setTabIndex(int index)
{
    m_tabIndex = index;
}

int LayoutItem::tabIndex() const
{
    return m_tabIndex;
}

Qt::DockWidgetArea LayoutItem::dockArea() const
{
    return m_dockArea;
}

void LayoutItem::setDockArea(Qt::DockWidgetArea area)
{
    m_dockArea = area;
}

bool LayoutItem::isFloating() const
{
    return m_floating;
}

void LayoutItem::setFloating(bool floating)
{
    m_floating = floating;
}