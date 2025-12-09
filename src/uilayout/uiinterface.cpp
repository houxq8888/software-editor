#include "uiinterface.h"
#include <QUuid>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QWidget>

UIInterface::UIInterface(const QString &name, const QString &description, QObject *parent)
    : QObject(parent)
    , m_id(QUuid::createUuid().toString(QUuid::WithoutBraces))
    , m_name(name)
    , m_description(description)
    , m_title(name)
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

// 事件处理方法实现
void UIInterface::triggerButtonClick(const QString &buttonName)
{
    qDebug() << "UIInterface: 触发按钮点击事件:" << buttonName;
    emit buttonClicked(buttonName);
}

void UIInterface::triggerMenuAction(const QString &actionName)
{
    qDebug() << "UIInterface: 触发菜单操作事件:" << actionName;
    emit menuActionTriggered(actionName);
}

// 获取界面控件信息
QList<QString> UIInterface::getButtonNames() const
{
    QList<QString> buttonNames;
    
    // 从布局项中提取按钮名称
    for (LayoutItem *item : m_layoutItems) {
        if (item->widgetType().contains("Button", Qt::CaseInsensitive) ||
            item->text().contains("按钮", Qt::CaseInsensitive)) {
            buttonNames.append(item->text());
        }
    }
    
    return buttonNames;
}

QList<QString> UIInterface::getMenuActionNames() const
{
    QList<QString> actionNames;
    
    // 从布局项中提取菜单操作名称
    for (LayoutItem *item : m_layoutItems) {
        if (item->widgetType().contains("Menu", Qt::CaseInsensitive) ||
            item->widgetType().contains("Action", Qt::CaseInsensitive)) {
            actionNames.append(item->text());
        }
    }
    
    return actionNames;
}

// 创建界面控件
QWidget* UIInterface::widget()
{
    QWidget *widget = new QWidget();
    widget->setWindowTitle(m_title);
    widget->setMinimumSize(m_size);
    
    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(widget);
    
    // 添加标题标签
    QLabel *titleLabel = new QLabel(m_name);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 16px; font-weight: bold; margin: 10px;");
    mainLayout->addWidget(titleLabel);
    
    // 添加描述标签
    if (!m_description.isEmpty()) {
        QLabel *descLabel = new QLabel(m_description);
        descLabel->setAlignment(Qt::AlignCenter);
        descLabel->setStyleSheet("font-size: 12px; color: gray; margin: 5px;");
        mainLayout->addWidget(descLabel);
    }
    
    // 创建控件容器
    QWidget *controlsContainer = new QWidget();
    QVBoxLayout *controlsLayout = new QVBoxLayout(controlsContainer);
    
    // 根据布局项创建控件
    for (LayoutItem *item : m_layoutItems) {
        QString widgetType = item->widgetType();
        QString text = item->text();
        
        if (widgetType.contains("Button", Qt::CaseInsensitive) || 
            text.contains("按钮", Qt::CaseInsensitive)) {
            // 创建按钮
            QPushButton *button = new QPushButton(text.isEmpty() ? "按钮" : text);
            button->setObjectName(text);
            button->setMinimumSize(item->size());
            
            // 连接按钮点击信号
            connect(button, &QPushButton::clicked, [this, text]() {
                emit buttonClicked(text);
            });
            
            controlsLayout->addWidget(button);
        } else if (widgetType.contains("Label", Qt::CaseInsensitive) || 
                   text.contains("标签", Qt::CaseInsensitive)) {
            // 创建标签
            QLabel *label = new QLabel(text.isEmpty() ? "标签" : text);
            label->setObjectName(text);
            label->setMinimumSize(item->size());
            controlsLayout->addWidget(label);
        } else {
            // 默认创建标签显示控件信息
            QLabel *defaultLabel = new QLabel(QString("%1: %2").arg(widgetType, text));
            defaultLabel->setObjectName(text);
            defaultLabel->setMinimumSize(item->size());
            controlsLayout->addWidget(defaultLabel);
        }
    }
    
    // 如果没有布局项，添加提示信息
    if (m_layoutItems.isEmpty()) {
        QLabel *emptyLabel = new QLabel("此界面没有可显示的控件");
        emptyLabel->setAlignment(Qt::AlignCenter);
        emptyLabel->setStyleSheet("color: gray; font-style: italic;");
        controlsLayout->addWidget(emptyLabel);
    }
    
    mainLayout->addWidget(controlsContainer);
    mainLayout->addStretch();
    
    return widget;
}