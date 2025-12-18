#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <QObject>
#include <QString>
#include <QList>
#include <QMap>
#include <QJsonObject>
#include "uiinterface.h"
#include "uiinterfacemanager.h"

// 前向声明
class WizardManager;

// 状态机事件类型
enum class StateMachineEventType {
    ButtonClick,      // 按钮点击
    MenuAction,       // 菜单操作
    TimerTrigger,     // 定时器触发
    DataChange,       // 数据变更
    CustomEvent       // 自定义事件
};

// 条件操作符
enum class ConditionOperator {
    Equal,           // 等于
    NotEqual,        // 不等于
    Greater,         // 大于
    Less,            // 小于
    Contains,        // 包含
    IsEmpty          // 为空
};

// 条件逻辑关系
enum class ConditionLogic {
    And,             // 与
    Or               // 或
};

// 状态机转换条件
struct StateMachineCondition {
    QString conditionId;          // 条件ID
    QString description;          // 条件描述
    QString expression;           // 条件表达式
    bool isEnabled;               // 是否启用
    
    StateMachineCondition() : isEnabled(true) {}
    
    QJsonObject toJson() const;
    bool fromJson(const QJsonObject &json);
};

// 状态机转换
struct StateMachineTransition {
    QString transitionId;         // 转换ID
    QString fromStateId;          // 源状态ID
    QString toStateId;            // 目标状态ID
    QString eventName;            // 事件名称
    StateMachineEventType eventType; // 事件类型
    QList<StateMachineCondition> conditions; // 转换条件
    QString actionScript;         // 转换时执行的脚本
    bool isEnabled;               // 是否启用
    
    // 新增条件配置字段
    QString eventSource;          // 事件源（如按钮ID、菜单项ID等）
    struct {
        QString variable;         // 条件变量名
        ConditionOperator operatorType; // 操作符类型
        QString value;            // 比较值
        ConditionLogic logic;     // 逻辑关系
    } condition;                  // 简单条件配置
    
    StateMachineTransition() : isEnabled(true) {}
    
    QJsonObject toJson() const;
    bool fromJson(const QJsonObject &json);
};

// 状态机状态（对应UI界面）
struct StateMachineState {
    QString stateId;              // 状态ID
    QString name;                 // 状态名称
    QString description;          // 状态描述
    QString uiInterfaceId;        // 关联的UI界面ID
    bool isInitialState;          // 是否为初始状态
    bool isFinalState;            // 是否为终止状态
    QPointF position;             // 在状态图中的位置
    
    StateMachineState() : isInitialState(false), isFinalState(false), position(0, 0) {}
    
    QJsonObject toJson() const;
    bool fromJson(const QJsonObject &json);
};

// 状态机类
class StateMachine : public QObject
{
    Q_OBJECT

public:
    explicit StateMachine(const QString &name = "", QObject *parent = nullptr);
    ~StateMachine() override;

    // 基本信息
    QString name() const;
    void setName(const QString &name);
    
    QString description() const;
    void setDescription(const QString &description);
    
    QString id() const;
    
    // 状态管理
    QList<StateMachineState> states() const;
    StateMachineState* findState(const QString &stateId);
    StateMachineState* findStateByUiInterface(const QString &uiInterfaceId);
    
    bool addState(const StateMachineState &state);
    bool removeState(const QString &stateId);
    bool updateState(const StateMachineState &state);
    
    StateMachineState* initialState() const;
    void setInitialState(const QString &stateId);
    
    // 转换管理
    QList<StateMachineTransition> transitions() const;
    QList<StateMachineTransition> transitionsFromState(const QString &stateId) const;
    
    bool addTransition(const StateMachineTransition &transition);
    bool removeTransition(const QString &transitionId);
    bool updateTransition(const StateMachineTransition &transition);
    
    // 当前状态管理
    StateMachineState* currentState() const;
    bool setCurrentState(const QString &stateId);
    
    // 事件处理
    bool processEvent(const QString &eventName, StateMachineEventType eventType, const QVariantMap &context = QVariantMap());
    
    // 序列化和反序列化
    QJsonObject toJson() const;
    bool fromJson(const QJsonObject &json);
    
    // 文件操作
    bool saveToFile(const QString &filePath);
    bool loadFromFile(const QString &filePath);
    
    // 验证状态机
    bool validate() const;
    QList<QString> validationErrors() const;

signals:
    void stateChanged(const StateMachineState &fromState, const StateMachineState &toState);
    void transitionTriggered(const StateMachineTransition &transition);
    void eventProcessed(const QString &eventName, bool success);

private:
    QString m_id;
    QString m_name;
    QString m_description;
    QList<StateMachineState> m_states;
    QList<StateMachineTransition> m_transitions;
    QString m_initialStateId;
    QString m_currentStateId;
    
    StateMachineState* findStateById(const QString &stateId);
    const StateMachineState* findStateById(const QString &stateId) const;
    StateMachineTransition* findTransitionById(const QString &transitionId);
};

// 状态机管理器
class StateMachineManager : public QObject
{
    Q_OBJECT

public:
    explicit StateMachineManager(QObject *parent = nullptr);
    ~StateMachineManager() override;

    // 状态机管理
    QList<StateMachine*> stateMachines() const;
    StateMachine* currentStateMachine() const;
    void setCurrentStateMachine(StateMachine *stateMachine);
    
    StateMachine* createStateMachine(const QString &name = "");
    bool removeStateMachine(StateMachine *stateMachine);
    StateMachine* findStateMachine(const QString &stateMachineId) const;
    
    // 与UIInterfaceManager集成
    void setUiInterfaceManager(UIInterfaceManager *manager);
    UIInterfaceManager* uiInterfaceManager() const;
    
    // 状态机与UI界面关联
    bool linkStateToUiInterface(const QString &stateId, const QString &uiInterfaceId);
    QString getUiInterfaceForState(const QString &stateId) const;
    QString getStateForUiInterface(const QString &uiInterfaceId) const;
    
    // 向导管理（新增）
    WizardManager* wizardManager() const;
    void setWizardManager(WizardManager *manager);
    
    // 序列化和反序列化
    QJsonObject toJson() const;
    bool fromJson(const QJsonObject &json);
    
    // 文件操作
    bool saveToFile(const QString &filePath);
    bool loadFromFile(const QString &filePath);

signals:
    void stateMachineAdded(StateMachine *stateMachine);
    void stateMachineRemoved(StateMachine *stateMachine);
    void currentStateMachineChanged(StateMachine *stateMachine);

private:
    QList<StateMachine*> m_stateMachines;
    StateMachine *m_currentStateMachine;
    UIInterfaceManager *m_uiInterfaceManager;
    WizardManager *m_wizardManager; // 向导管理器（新增）
};

#endif // STATEMACHINE_H