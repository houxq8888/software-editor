#include "uiinterface.h"
#include <QDebug>
#include <QPushButton>
#include <QVBoxLayout>
#include <QUuid>
#include <QLabel>

UIInterface::UIInterface(const QString &name, const QString &description, QObject *parent)
    : QObject(parent)
    , m_id(QUuid::createUuid().toString(QUuid::WithoutBraces))
    , m_name(name)
    , m_description(description)
    , m_nextUIName(QString())
    , m_widget(nullptr)
    , m_isMainWindow(false)
{
    qDebug() << "[UIInterface] Created UIInterface: " << m_name;
}

UIInterface::UIInterface()
    : QObject(nullptr)
    , m_id(QUuid::createUuid().toString(QUuid::WithoutBraces))
    , m_name("Unknown")
    , m_description("Unknown")
    , m_nextUIName(QString())
    , m_widget(nullptr)
    , m_isMainWindow(false)
{}

UIInterface::~UIInterface()
{
    if (m_widget) {
        delete m_widget;
        m_widget = nullptr;
    }
}

UIInterface* UIInterface::clone() const
{
    UIInterface *cloned = new UIInterface(m_name, m_description);
    cloned->setId(m_id);
    cloned->setNextUI(m_nextUIName);
    cloned->setIsMainWindow(m_isMainWindow);
    return cloned;
}

QJsonObject UIInterface::toJson() const
{
    QJsonObject json;
    json["id"] = m_id;
    json["name"] = m_name;
    json["description"] = m_description;
    json["nextUIName"] = m_nextUIName;
    json["isMainWindow"] = m_isMainWindow;
    return json;
}

bool UIInterface::fromJson(const QJsonObject &json)
{
    m_id = json["id"].toString();
    m_name = json["name"].toString();
    m_description = json["description"].toString();
    m_nextUIName = json["nextUIName"].toString();
    m_isMainWindow = json["isMainWindow"].toBool();
    return true;
}

QWidget* UIInterface::widget()
{
    if (m_widget) {
        return m_widget;
    }
    
    // 创建一个示例Widget
    m_widget = new QWidget();
    m_widget->setWindowTitle(m_name);
    
    // 创建布局
    QVBoxLayout *layout = new QVBoxLayout(m_widget);
    
    // 添加一个标签显示UI名称
    QLabel *label = new QLabel(QString("UI界面: %1").arg(m_name));
    layout->addWidget(label);
    
    // 如果有描述，添加描述标签
    if (!m_description.isEmpty()) {
        QLabel *descLabel = new QLabel(QString("描述: %1").arg(m_description));
        layout->addWidget(descLabel);
    }
    
    // 如果有跳转逻辑，添加一个按钮
    if (hasJumpLogic()) {
        QPushButton *nextButton = new QPushButton(QString("跳转到: %1").arg(m_nextUIName));
        layout->addWidget(nextButton);
        
        // 连接按钮点击事件
        connect(nextButton, &QPushButton::clicked, [this]() {
            handleControlEvent("nextButton", "clicked");
        });
    }
    
    return m_widget;
}

void UIInterface::handleControlEvent(const QString &controlName, const QString &eventType)
{
    qDebug() << "[UIInterface] Handle control event: " << controlName << " - " << eventType;
    
    // 这里可以添加具体的事件处理逻辑
    // 例如：根据控件名称和事件类型执行相应的操作
    
    if (controlName == "nextButton" && eventType == "clicked") {
        qDebug() << "[UIInterface] Button clicked, should jump to: " << m_nextUIName;
        
        // 这里可以发送一个信号通知外部需要跳转到下一个UI
        emit jumpToUI(m_nextUIName);
    }
}

