#ifndef UIFLOWSTATEMACHINE_H
#define UIFLOWSTATEMACHINE_H

#include <QObject>
#include <QString>
#include <QList>
#include <QMap>
#include <QJsonObject>
#include "uiinterface.h"

// UI流状态机 - 第一层次：专注于UI界面串联
class UIFlowStateMachine : public QObject
{
    Q_OBJECT

public:
    // UI流状态（对应UI界面）
    struct UIFlowState {
        QString stateId;              // 状态ID
        QString name;                 // 状态名称
        QString description;          // 状态描述
        QString uiInterfaceId;        // 关联的UI界面ID
        bool isInitialState;          // 是否为初始状态
        bool isFinalState;            // 是否为终止状态
        
        UIFlowState() : isInitialState(false), isFinalState(false) {}
        
        QJsonObject toJson() const;
        bool fromJson(const QJsonObject &json);
    };

    // UI流转换（UI事件触发）
    struct UIFlowTransition {
        QString transitionId;         // 转换ID
        QString fromStateId;          // 源状态ID
        QString toStateId;            // 目标状态ID
        QString eventSource;          // 事件源（如按钮ID、菜单项ID等）
        QString eventType;            // 事件类型（click、changed等）
        
        UIFlowTransition() {}
        
        QJsonObject toJson() const;
        bool fromJson(const QJsonObject &json);
    };

    explicit UIFlowStateMachine(const QString &name = "", QObject *parent = nullptr);
    ~UIFlowStateMachine() override;

    // 基本信息
    QString name() const;
    void setName(const QString &name);
    QString id() const;

    // 状态管理
    QList<UIFlowState> states() const;
    UIFlowState* findState(const QString &stateId);
    UIFlowState* findStateByUiInterface(const QString &uiInterfaceId);
    
    bool addState(const UIFlowState &state);
    bool removeState(const QString &stateId);
    bool updateState(const UIFlowState &state);
    
    const UIFlowState* initialState() const;
    void setInitialState(const QString &stateId);

    // 转换管理
    QList<UIFlowTransition> transitions() const;
    QList<UIFlowTransition> transitionsFromState(const QString &stateId) const;
    
    bool addTransition(const UIFlowTransition &transition);
    bool removeTransition(const QString &transitionId);
    bool updateTransition(const UIFlowTransition &transition);

    // 当前状态管理
    const UIFlowState* currentState() const;
    bool setCurrentState(const QString &stateId);
    
    // UI事件处理（简化的接口）
    bool handleUIEvent(const QString &eventSource, const QString &eventType);
    
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
    void stateChanged(const QString &fromStateId, const QString &toStateId);
    void uiInterfaceChanged(const QString &uiInterfaceId);
    void transitionTriggered(const QString &transitionId);

private:
    QString m_id;
    QString m_name;
    QList<UIFlowState> m_states;
    QList<UIFlowTransition> m_transitions;
    QString m_initialStateId;
    QString m_currentStateId;
    
    UIFlowState* findStateById(const QString &stateId);
    const UIFlowState* findStateById(const QString &stateId) const;
    UIFlowTransition* findTransitionById(const QString &transitionId);
};

// UI流状态机管理器
class UIFlowStateMachineManager : public QObject
{
    Q_OBJECT

public:
    explicit UIFlowStateMachineManager(QObject *parent = nullptr);
    ~UIFlowStateMachineManager() override;

    // 状态机管理
    QList<UIFlowStateMachine*> stateMachines() const;
    UIFlowStateMachine* currentStateMachine() const;
    void setCurrentStateMachine(UIFlowStateMachine *stateMachine);
    
    UIFlowStateMachine* createStateMachine(const QString &name = "");
    bool removeStateMachine(UIFlowStateMachine *stateMachine);
    UIFlowStateMachine* findStateMachine(const QString &stateMachineId) const;
    
    // 与UI界面关联
    bool linkStateToUiInterface(const QString &stateId, const QString &uiInterfaceId);
    QString getUiInterfaceForState(const QString &stateId) const;
    QString getStateForUiInterface(const QString &uiInterfaceId) const;
    
    // 序列化和反序列化
    QJsonObject toJson() const;
    bool fromJson(const QJsonObject &json);
    
    // 文件操作
    bool saveToFile(const QString &filePath);
    bool loadFromFile(const QString &filePath);

signals:
    void currentStateMachineChanged(UIFlowStateMachine *stateMachine);

private:
    QList<UIFlowStateMachine*> m_stateMachines;
    UIFlowStateMachine* m_currentStateMachine;
};

#endif // UIFLOWSTATEMACHINE_H