#include "eventactionmodel.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUuid>
#include <QDebug>

// EventDefinition 实现
EventDefinition::EventDefinition()
    : m_type(EventType::ButtonClick)
{
    m_id = QUuid::createUuid().toString();
}

EventDefinition::EventDefinition(const QString &id, EventType type, const QString &sourceWidgetId, const QMap<QString, QVariant> &parameters)
    : m_id(id.isEmpty() ? QUuid::createUuid().toString() : id)
    , m_type(type)
    , m_sourceWidgetId(sourceWidgetId)
    , m_parameters(parameters)
{
}

QJsonObject EventDefinition::toJson() const
{
    QJsonObject json;
    json["id"] = m_id;
    json["type"] = static_cast<int>(m_type);
    json["sourceWidgetId"] = m_sourceWidgetId;
    
    QJsonObject paramsJson;
    for (auto it = m_parameters.begin(); it != m_parameters.end(); ++it) {
        paramsJson[it.key()] = QJsonValue::fromVariant(it.value());
    }
    json["parameters"] = paramsJson;
    
    return json;
}

EventDefinition EventDefinition::fromJson(const QJsonObject &json)
{
    QString id = json["id"].toString();
    EventType type = static_cast<EventType>(json["type"].toInt());
    QString sourceWidgetId = json["sourceWidgetId"].toString();
    
    QMap<QString, QVariant> parameters;
    QJsonObject paramsJson = json["parameters"].toObject();
    for (auto it = paramsJson.begin(); it != paramsJson.end(); ++it) {
        parameters[it.key()] = it.value().toVariant();
    }
    
    return EventDefinition(id, type, sourceWidgetId, parameters);
}

// ActionDefinition 实现
ActionDefinition::ActionDefinition()
    : m_type(ActionType::ShowWidget)
{
    m_id = QUuid::createUuid().toString();
}

ActionDefinition::ActionDefinition(const QString &id, ActionType type, const QString &targetWidgetId, const QMap<QString, QVariant> &parameters)
    : m_id(id.isEmpty() ? QUuid::createUuid().toString() : id)
    , m_type(type)
    , m_targetWidgetId(targetWidgetId)
    , m_parameters(parameters)
{
}

QJsonObject ActionDefinition::toJson() const
{
    QJsonObject json;
    json["id"] = m_id;
    json["type"] = static_cast<int>(m_type);
    json["targetWidgetId"] = m_targetWidgetId;
    
    QJsonObject paramsJson;
    for (auto it = m_parameters.begin(); it != m_parameters.end(); ++it) {
        paramsJson[it.key()] = QJsonValue::fromVariant(it.value());
    }
    json["parameters"] = paramsJson;
    
    return json;
}

ActionDefinition ActionDefinition::fromJson(const QJsonObject &json)
{
    QString id = json["id"].toString();
    ActionType type = static_cast<ActionType>(json["type"].toInt());
    QString targetWidgetId = json["targetWidgetId"].toString();
    
    QMap<QString, QVariant> parameters;
    QJsonObject paramsJson = json["parameters"].toObject();
    for (auto it = paramsJson.begin(); it != paramsJson.end(); ++it) {
        parameters[it.key()] = it.value().toVariant();
    }
    
    return ActionDefinition(id, type, targetWidgetId, parameters);
}

// EventActionRule 实现
EventActionRule::EventActionRule()
{
    m_id = QUuid::createUuid().toString();
}

EventActionRule::EventActionRule(const QString &id, const EventDefinition &event, const QList<ActionDefinition> &actions)
    : m_id(id.isEmpty() ? QUuid::createUuid().toString() : id)
    , m_event(event)
    , m_actions(actions)
{
}

void EventActionRule::removeAction(const QString &actionId)
{
    m_actions.erase(std::remove_if(m_actions.begin(), m_actions.end(),
        [actionId](const ActionDefinition &action) {
            return action.id() == actionId;
        }), m_actions.end());
}

QJsonObject EventActionRule::toJson() const
{
    QJsonObject json;
    json["id"] = m_id;
    json["event"] = m_event.toJson();
    
    QJsonArray actionsArray;
    for (const auto &action : m_actions) {
        actionsArray.append(action.toJson());
    }
    json["actions"] = actionsArray;
    
    return json;
}

EventActionRule EventActionRule::fromJson(const QJsonObject &json)
{
    QString id = json["id"].toString();
    EventDefinition event = EventDefinition::fromJson(json["event"].toObject());
    
    QList<ActionDefinition> actions;
    QJsonArray actionsArray = json["actions"].toArray();
    for (const auto &actionJson : actionsArray) {
        actions.append(ActionDefinition::fromJson(actionJson.toObject()));
    }
    
    return EventActionRule(id, event, actions);
}

// InteractionTemplate 实现
InteractionTemplate::InteractionTemplate()
{
}

InteractionTemplate::InteractionTemplate(const QString &name, const QString &description, const QList<EventActionRule> &rules)
    : m_name(name)
    , m_description(description)
    , m_rules(rules)
{
}

QJsonObject InteractionTemplate::toJson() const
{
    QJsonObject json;
    json["name"] = m_name;
    json["description"] = m_description;
    
    QJsonArray rulesArray;
    for (const auto &rule : m_rules) {
        rulesArray.append(rule.toJson());
    }
    json["rules"] = rulesArray;
    
    return json;
}

InteractionTemplate InteractionTemplate::fromJson(const QJsonObject &json)
{
    QString name = json["name"].toString();
    QString description = json["description"].toString();
    
    QList<EventActionRule> rules;
    QJsonArray rulesArray = json["rules"].toArray();
    for (const auto &ruleJson : rulesArray) {
        rules.append(EventActionRule::fromJson(ruleJson.toObject()));
    }
    
    return InteractionTemplate(name, description, rules);
}

// EventActionModel 实现
EventActionModel::EventActionModel(QObject *parent)
    : QObject(parent)
{
    loadDefaultTemplates();
}

void EventActionModel::addRule(const EventActionRule &rule)
{
    m_rules.append(rule);
    emit ruleAdded(rule.id());
}

void EventActionModel::removeRule(const QString &ruleId)
{
    m_rules.erase(std::remove_if(m_rules.begin(), m_rules.end(),
        [ruleId](const EventActionRule &rule) {
            return rule.id() == ruleId;
        }), m_rules.end());
    emit ruleRemoved(ruleId);
}

EventActionRule EventActionModel::getRule(const QString &ruleId) const
{
    auto it = std::find_if(m_rules.begin(), m_rules.end(),
        [ruleId](const EventActionRule &rule) {
            return rule.id() == ruleId;
        });
    
    if (it != m_rules.end()) {
        return *it;
    }
    return EventActionRule();
}

void EventActionModel::addTemplate(const InteractionTemplate &interactionTemplate)
{
    m_templates.append(interactionTemplate);
    emit templateAdded(interactionTemplate.name());
}

void EventActionModel::removeTemplate(const QString &templateName)
{
    m_templates.erase(std::remove_if(m_templates.begin(), m_templates.end(),
        [templateName](const InteractionTemplate &interactionTemplate) {
            return interactionTemplate.name() == templateName;
        }), m_templates.end());
    emit templateRemoved(templateName);
}

InteractionTemplate EventActionModel::getTemplate(const QString &templateName) const
{
    auto it = std::find_if(m_templates.begin(), m_templates.end(),
        [templateName](const InteractionTemplate &interactionTemplate) {
            return interactionTemplate.name() == templateName;
        });
    
    if (it != m_templates.end()) {
        return *it;
    }
    return InteractionTemplate();
}

bool EventActionModel::saveToFile(const QString &filePath)
{
    QJsonObject root;
    
    // 保存规则
    QJsonArray rulesArray;
    for (const auto &rule : m_rules) {
        rulesArray.append(rule.toJson());
    }
    root["rules"] = rulesArray;
    
    // 保存模板
    QJsonArray templatesArray;
    for (const auto &interactionTemplate : m_templates) {
        templatesArray.append(interactionTemplate.toJson());
    }
    root["templates"] = templatesArray;
    
    QJsonDocument doc(root);
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    
    file.write(doc.toJson());
    file.close();
    return true;
}

bool EventActionModel::loadFromFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    
    if (doc.isNull()) {
        return false;
    }
    
    QJsonObject root = doc.object();
    
    // 加载规则
    m_rules.clear();
    QJsonArray rulesArray = root["rules"].toArray();
    for (const auto &ruleJson : rulesArray) {
        m_rules.append(EventActionRule::fromJson(ruleJson.toObject()));
    }
    
    // 加载模板
    m_templates.clear();
    QJsonArray templatesArray = root["templates"].toArray();
    for (const auto &templateJson : templatesArray) {
        m_templates.append(InteractionTemplate::fromJson(templateJson.toObject()));
    }
    
    return true;
}

bool EventActionModel::validateRule(const EventActionRule &rule)
{
    m_validationErrors.clear();
    
    // 验证事件源控件存在
    if (rule.event().sourceWidgetId().isEmpty()) {
        m_validationErrors.append("事件源控件ID不能为空");
    }
    
    // 验证动作目标控件存在
    for (const auto &action : rule.actions()) {
        if (action.targetWidgetId().isEmpty()) {
            m_validationErrors.append("动作目标控件ID不能为空");
        }
    }
    
    return m_validationErrors.isEmpty();
}

QStringList EventActionModel::getValidationErrors() const
{
    return m_validationErrors;
}

void EventActionModel::loadDefaultTemplates()
{
    // 按钮点击模板
    EventActionRule buttonClickRule("button_click_template",
        EventDefinition("", EventType::ButtonClick, "button_widget"),
        {
            ActionDefinition("", ActionType::ShowWidget, "target_widget"),
            ActionDefinition("", ActionType::SetText, "label_widget", {{"text", "按钮被点击了！"}})
        }
    );
    
    InteractionTemplate buttonTemplate("按钮点击交互", "按钮点击时显示目标控件并更新文本", {buttonClickRule});
    m_templates.append(buttonTemplate);
    
    // 表单验证模板
    EventActionRule formValidationRule("form_validation_template",
        EventDefinition("", EventType::TextChanged, "input_field"),
        {
            ActionDefinition("", ActionType::ValidateForm, "form_container"),
            ActionDefinition("", ActionType::EnableWidget, "submit_button", {{"enabled", true}})
        }
    );
    
    InteractionTemplate formTemplate("表单验证", "输入框内容改变时验证表单并启用提交按钮", {formValidationRule});
    m_templates.append(formTemplate);
    
    // 导航模板
    EventActionRule navigationRule("navigation_template",
        EventDefinition("", EventType::ButtonClick, "nav_button"),
        {
            ActionDefinition("", ActionType::Navigate, "content_area", {{"page", "home"}}),
            ActionDefinition("", ActionType::SetText, "title_label", {{"text", "首页"}})
        }
    );
    
    InteractionTemplate navTemplate("页面导航", "点击导航按钮切换页面内容", {navigationRule});
    m_templates.append(navTemplate);
}