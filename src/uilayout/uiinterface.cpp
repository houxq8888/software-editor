#include "uiinterface.h"
#include <QUuid>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>

UIInterface::UIInterface(const QString &name, const QString &description)
    : m_id(QUuid::createUuid().toString())
    , m_name(name.isEmpty() ? "新界面" : name)
    , m_description(description)
    , m_title("")
    , m_size(800, 600)
    , m_isMainWindow(false)
{
}

QString UIInterface::name() const
{
    return m_name;
}

void UIInterface::setName(const QString &name)
{
    m_name = name;
}

QString UIInterface::description() const
{
    return m_description;
}

void UIInterface::setDescription(const QString &description)
{
    m_description = description;
}

QString UIInterface::id() const
{
    return m_id;
}

QList<LayoutItem*> UIInterface::layoutItems() const
{
    return m_layoutItems;
}

void UIInterface::addLayoutItem(LayoutItem *item)
{
    if (item && !m_layoutItems.contains(item)) {
        m_layoutItems.append(item);
    }
}

void UIInterface::removeLayoutItem(LayoutItem *item)
{
    if (item) {
        m_layoutItems.removeOne(item);
        delete item;
    }
}

void UIInterface::clearLayoutItems()
{
    qDeleteAll(m_layoutItems);
    m_layoutItems.clear();
}

LayoutItem* UIInterface::findLayoutItem(const QString &widgetId) const
{
    for (LayoutItem *item : m_layoutItems) {
        // 这里需要LayoutItem有获取控件ID的方法
        // 暂时使用控件文本作为标识
        if (item->text() == widgetId) {
            return item;
        }
    }
    return nullptr;
}

QJsonObject UIInterface::toJson() const
{
    QJsonObject json;
    json["id"] = m_id;
    json["name"] = m_name;
    json["description"] = m_description;
    json["title"] = m_title;
    json["width"] = m_size.width();
    json["height"] = m_size.height();
    json["isMainWindow"] = m_isMainWindow;
    
    // 序列化布局项
    QJsonArray itemsArray;
    for (LayoutItem *item : m_layoutItems) {
        QJsonObject itemJson;
        itemJson["type"] = item->widgetType();
        itemJson["text"] = item->text();
        itemJson["x"] = item->pos().x();
        itemJson["y"] = item->pos().y();
        itemJson["width"] = item->size().width();
        itemJson["height"] = item->size().height();
        itemsArray.append(itemJson);
    }
    json["layoutItems"] = itemsArray;
    
    return json;
}

bool UIInterface::fromJson(const QJsonObject &json)
{
    if (json.isEmpty()) {
        return false;
    }
    
    // 清除现有数据
    clearLayoutItems();
    
    // 加载基本信息
    m_id = json["id"].toString();
    m_name = json["name"].toString();
    m_description = json["description"].toString();
    m_title = json["title"].toString();
    m_size = QSize(json["width"].toInt(800), json["height"].toInt(600));
    m_isMainWindow = json["isMainWindow"].toBool(false);
    
    // 加载布局项
    QJsonArray itemsArray = json["layoutItems"].toArray();
    for (const QJsonValue &itemValue : itemsArray) {
        QJsonObject itemJson = itemValue.toObject();
        LayoutItem *item = new LayoutItem(
            itemJson["type"].toString(),
            itemJson["text"].toString()
        );
        item->setPos(QPoint(itemJson["x"].toInt(), itemJson["y"].toInt()));
        item->setSize(QSize(itemJson["width"].toInt(), itemJson["height"].toInt()));
        m_layoutItems.append(item);
    }
    
    return true;
}

UIInterface* UIInterface::clone() const
{
    UIInterface *cloneInterface = new UIInterface(m_name + "_副本", m_description);
    cloneInterface->m_title = m_title;
    cloneInterface->m_size = m_size;
    cloneInterface->m_isMainWindow = m_isMainWindow;
    
    // 复制布局项
    for (LayoutItem *item : m_layoutItems) {
        LayoutItem *cloneItem = new LayoutItem(item->widgetType(), item->text());
        cloneItem->setPos(item->pos());
        cloneItem->setSize(item->size());
        cloneInterface->m_layoutItems.append(cloneItem);
    }
    
    return cloneInterface;
}

QSize UIInterface::size() const
{
    return m_size;
}

void UIInterface::setSize(const QSize &size)
{
    m_size = size;
}

QString UIInterface::title() const
{
    return m_title;
}

void UIInterface::setTitle(const QString &title)
{
    m_title = title;
}

bool UIInterface::isMainWindow() const
{
    return m_isMainWindow;
}

void UIInterface::setIsMainWindow(bool isMain)
{
    m_isMainWindow = isMain;
}