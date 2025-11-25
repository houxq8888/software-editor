#ifndef EVENTACTIONMODEL_H
#define EVENTACTIONMODEL_H

#include <QObject>
#include <QString>
#include <QList>
#include <QMap>
#include <QVariant>
#include <QJsonObject>
#include <QJsonArray>

// 事件类型枚举
enum class EventType {
    ButtonClick,        // 按钮点击
    TextChanged,        // 文本改变
    ValueChanged,       // 值改变
    SelectionChanged,   // 选择改变
    FocusIn,           // 获得焦点
    FocusOut,          // 失去焦点
    MouseEnter,        // 鼠标进入
    MouseLeave,        // 鼠标离开
    CustomEvent        // 自定义事件
};

// 动作类型枚举
enum class ActionType {
    ShowWidget,        // 显示控件
    HideWidget,        // 隐藏控件
    EnableWidget,      // 启用控件
    DisableWidget,     // 禁用控件
    SetText,          // 设置文本
    SetValue,         // 设置值
    ExecuteScript,    // 执行脚本
    Navigate,         // 导航
    ValidateForm,     // 验证表单
    SubmitForm,       // 提交表单
    CustomAction      // 自定义动作
};

// 事件定义
class EventDefinition
{
public:
    EventDefinition();
    EventDefinition(const QString &id, EventType type, const QString &sourceWidgetId, const QMap<QString, QVariant> &parameters = {});
    
    QString id() const { return m_id; }
    EventType type() const { return m_type; }
    QString sourceWidgetId() const { return m_sourceWidgetId; }
    QMap<QString, QVariant> parameters() const { return m_parameters; }
    
    void setParameters(const QMap<QString, QVariant> &parameters) { m_parameters = parameters; }
    
    QJsonObject toJson() const;
    static EventDefinition fromJson(const QJsonObject &json);
    
private:
    QString m_id;
    EventType m_type;
    QString m_sourceWidgetId;
    QMap<QString, QVariant> m_parameters;
};

// 动作定义
class ActionDefinition
{
public:
    ActionDefinition();
    ActionDefinition(const QString &id, ActionType type, const QString &targetWidgetId, const QMap<QString, QVariant> &parameters = {});
    
    QString id() const { return m_id; }
    ActionType type() const { return m_type; }
    QString targetWidgetId() const { return m_targetWidgetId; }
    QMap<QString, QVariant> parameters() const { return m_parameters; }
    
    void setParameters(const QMap<QString, QVariant> &parameters) { m_parameters = parameters; }
    
    QJsonObject toJson() const;
    static ActionDefinition fromJson(const QJsonObject &json);
    
private:
    QString m_id;
    ActionType m_type;
    QString m_targetWidgetId;
    QMap<QString, QVariant> m_parameters;
};

// 事件-动作规则
class EventActionRule
{
public:
    EventActionRule();
    EventActionRule(const QString &id, const EventDefinition &event, const QList<ActionDefinition> &actions);
    
    QString id() const { return m_id; }
    EventDefinition event() const { return m_event; }
    QList<ActionDefinition> actions() const { return m_actions; }
    
    void setEvent(const EventDefinition &event) { m_event = event; }
    void setActions(const QList<ActionDefinition> &actions) { m_actions = actions; }
    void addAction(const ActionDefinition &action) { m_actions.append(action); }
    void removeAction(const QString &actionId);
    
    QJsonObject toJson() const;
    static EventActionRule fromJson(const QJsonObject &json);
    
private:
    QString m_id;
    EventDefinition m_event;
    QList<ActionDefinition> m_actions;
};

// 交互模板
class InteractionTemplate
{
public:
    InteractionTemplate();
    InteractionTemplate(const QString &name, const QString &description, const QList<EventActionRule> &rules);
    
    QString name() const { return m_name; }
    QString description() const { return m_description; }
    QList<EventActionRule> rules() const { return m_rules; }
    
    QJsonObject toJson() const;
    static InteractionTemplate fromJson(const QJsonObject &json);
    
private:
    QString m_name;
    QString m_description;
    QList<EventActionRule> m_rules;
};

// 事件-动作模型管理器
class EventActionModel : public QObject
{
    Q_OBJECT

public:
    explicit EventActionModel(QObject *parent = nullptr);
    
    // 规则管理
    void addRule(const EventActionRule &rule);
    void removeRule(const QString &ruleId);
    EventActionRule getRule(const QString &ruleId) const;
    QList<EventActionRule> getAllRules() const { return m_rules; }
    
    // 模板管理
    void addTemplate(const InteractionTemplate &interactionTemplate);
    void removeTemplate(const QString &templateName);
    InteractionTemplate getTemplate(const QString &templateName) const;
    QList<InteractionTemplate> getAllTemplates() const { return m_templates; }
    
    // 文件操作
    bool saveToFile(const QString &filePath);
    bool loadFromFile(const QString &filePath);
    
    // 验证和测试
    bool validateRule(const EventActionRule &rule);
    QStringList getValidationErrors() const;
    
signals:
    void ruleAdded(const QString &ruleId);
    void ruleRemoved(const QString &ruleId);
    void ruleModified(const QString &ruleId);
    void templateAdded(const QString &templateName);
    void templateRemoved(const QString &templateName);
    
private:
    QList<EventActionRule> m_rules;
    QList<InteractionTemplate> m_templates;
    QStringList m_validationErrors;
    
    void loadDefaultTemplates();
};

#endif // EVENTACTIONMODEL_H