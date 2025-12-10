#ifndef LOGICSEQUENCESTATEMACHINE_H
#define LOGICSEQUENCESTATEMACHINE_H

#include <QObject>
#include <QString>
#include <QList>
#include <QMap>
#include <QJsonObject>
#include <QVariant>

// 逻辑时序状态机 - 第二层次：专注于软件内部逻辑时序
class LogicSequenceStateMachine : public QObject
{
    Q_OBJECT

public:
    // 逻辑事件类型
    enum class LogicEventType {
        DataChange,           // 数据变更
        TimerTrigger,          // 定时器触发
        ExternalSignal,        // 外部信号
        InternalCondition,     // 内部条件满足
        CustomLogic           // 自定义逻辑
    };

    // 条件操作符
    enum class ConditionOperator {
        Equal,                // 等于
        NotEqual,             // 不等于
        Greater,              // 大于
        Less,                 // 小于
        Contains,             // 包含
        IsEmpty               // 为空
    };

    // 逻辑状态（对应软件内部逻辑状态）
    struct LogicState {
        QString stateId;              // 状态ID
        QString name;                 // 状态名称
        QString description;          // 状态描述
        QMap<QString, QVariant> data; // 状态数据
        bool isInitialState;          // 是否为初始状态
        bool isFinalState;            // 是否为终止状态
        
        LogicState() : isInitialState(false), isFinalState(false) {}
        
        QJsonObject toJson() const;
        bool fromJson(const QJsonObject &json);
    };

    // 逻辑转换条件
    struct LogicCondition {
        QString conditionId;          // 条件ID
        QString description;          // 条件描述
        QString variable;             // 条件变量名
        ConditionOperator operatorType; // 操作符类型
        QVariant value;               // 比较值
        bool isEnabled;               // 是否启用
        
        LogicCondition() : isEnabled(true) {}
        
        bool evaluate(const QVariant &currentValue) const;
        QJsonObject toJson() const;
        bool fromJson(const QJsonObject &json);
    };

    // 逻辑转换
    struct LogicTransition {
        QString transitionId;         // 转换ID
        QString fromStateId;          // 源状态ID
        QString toStateId;            // 目标状态ID
        QString eventName;            // 事件名称
        LogicEventType eventType;     // 事件类型
        QList<LogicCondition> conditions; // 转换条件
        QString actionScript;         // 转换时执行的脚本
        bool isEnabled;               // 是否启用
        
        LogicTransition() : isEnabled(true) {}
        
        QJsonObject toJson() const;
        bool fromJson(const QJsonObject &json);
    };

    explicit LogicSequenceStateMachine(const QString &name = "", QObject *parent = nullptr);
    ~LogicSequenceStateMachine() override;

    // 基本信息
    QString name() const;
    void setName(const QString &name);
    QString id() const;

    // 状态管理
    QList<LogicState> states() const;
    LogicState* findState(const QString &stateId);
    
    bool addState(const LogicState &state);
    bool removeState(const QString &stateId);
    bool updateState(const LogicState &state);
    
    const LogicState* initialState() const;
    void setInitialState(const QString &stateId);

    // 转换管理
    QList<LogicTransition> transitions() const;
    QList<LogicTransition> transitionsFromState(const QString &stateId) const;
    
    bool addTransition(const LogicTransition &transition);
    bool removeTransition(const QString &transitionId);
    bool updateTransition(const LogicTransition &transition);

    // 当前状态管理
    const LogicState* currentState() const;
    bool setCurrentState(const QString &stateId);
    
    // 数据管理
    QVariant getData(const QString &key) const;
    void setData(const QString &key, const QVariant &value);
    QMap<QString, QVariant> getAllData() const;
    
    // 逻辑事件处理
    bool processLogicEvent(const QString &eventName, LogicEventType eventType, const QVariantMap &context = QVariantMap());
    
    // 条件检查
    bool checkConditions(const QList<LogicCondition> &conditions) const;
    
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
    void stateChanged(const LogicState &fromState, const LogicState &toState);
    void transitionTriggered(const LogicTransition &transition);
    void dataChanged(const QString &key, const QVariant &value);
    void eventProcessed(const QString &eventName, bool success);

private:
    QString m_id;
    QString m_name;
    QList<LogicState> m_states;
    QList<LogicTransition> m_transitions;
    QString m_initialStateId;
    QString m_currentStateId;
    QMap<QString, QVariant> m_globalData; // 全局数据存储
    
    LogicState* findStateById(const QString &stateId);
    const LogicState* findStateById(const QString &stateId) const;
    LogicTransition* findTransitionById(const QString &transitionId);
    void executeActionScript(const QString &script);
};

// 逻辑时序状态机管理器
class LogicSequenceStateMachineManager : public QObject
{
    Q_OBJECT

public:
    explicit LogicSequenceStateMachineManager(QObject *parent = nullptr);
    ~LogicSequenceStateMachineManager() override;

    // 状态机管理
    QList<LogicSequenceStateMachine*> stateMachines() const;
    LogicSequenceStateMachine* currentStateMachine() const;
    void setCurrentStateMachine(LogicSequenceStateMachine *stateMachine);
    
    LogicSequenceStateMachine* createStateMachine(const QString &name = "");
    bool removeStateMachine(LogicSequenceStateMachine *stateMachine);
    LogicSequenceStateMachine* findStateMachine(const QString &stateMachineId) const;
    
    // 序列化和反序列化
    QJsonObject toJson() const;
    bool fromJson(const QJsonObject &json);
    
    // 文件操作
    bool saveToFile(const QString &filePath);
    bool loadFromFile(const QString &filePath);

signals:
    void currentStateMachineChanged(LogicSequenceStateMachine *stateMachine);

private:
    QList<LogicSequenceStateMachine*> m_stateMachines;
    LogicSequenceStateMachine* m_currentStateMachine;
};

#endif // LOGICSEQUENCESTATEMACHINE_H