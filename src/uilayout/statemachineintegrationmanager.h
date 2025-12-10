#ifndef STATEMACHINEINTEGRATIONMANAGER_H
#define STATEMACHINEINTEGRATIONMANAGER_H

#include <QObject>
#include <QString>
#include <QMap>
#include "uiflowstatemachine.h"
#include "logicsequencestatemachine.h"

// 状态机集成管理器 - 协调两个层次的状态机
class StateMachineIntegrationManager : public QObject
{
    Q_OBJECT

public:
    // 集成模式
    enum class IntegrationMode {
        UIFlowOnly,           // 仅使用UI流状态机（第一层次）
        LogicSequenceOnly,    // 仅使用逻辑时序状态机（第二层次）
        Integrated            // 集成模式（两个层次协同工作）
    };

    // 状态机关联映射
    struct StateMachineMapping {
        QString uiFlowStateId;        // UI流状态ID
        QString logicStateId;         // 逻辑时序状态ID
        QString mappingId;            // 映射ID
        
        StateMachineMapping() {}
        
        QJsonObject toJson() const;
        bool fromJson(const QJsonObject &json);
    };

    explicit StateMachineIntegrationManager(QObject *parent = nullptr);
    ~StateMachineIntegrationManager() override;

    // 集成模式设置
    IntegrationMode integrationMode() const;
    void setIntegrationMode(IntegrationMode mode);

    // 状态机管理
    UIFlowStateMachine* uiFlowStateMachine() const;
    void setUiFlowStateMachine(UIFlowStateMachine *stateMachine);
    
    LogicSequenceStateMachine* logicSequenceStateMachine() const;
    void setLogicSequenceStateMachine(LogicSequenceStateMachine *stateMachine);

    // 状态机关联映射
    QList<StateMachineMapping> mappings() const;
    bool addMapping(const StateMachineMapping &mapping);
    bool removeMapping(const QString &mappingId);
    bool updateMapping(const StateMachineMapping &mapping);
    
    QString getLogicStateForUIFlowState(const QString &uiFlowStateId) const;
    QString getUIFlowStateForLogicState(const QString &logicStateId) const;

    // UI事件处理（根据集成模式分发）
    bool handleUIEvent(const QString &eventSource, const QString &eventType);
    
    // 逻辑事件处理（根据集成模式分发）
    bool handleLogicEvent(const QString &eventName, LogicSequenceStateMachine::LogicEventType eventType, const QVariantMap &context = QVariantMap());
    
    // 状态同步
    bool synchronizeStates();
    
    // 序列化和反序列化
    QJsonObject toJson() const;
    bool fromJson(const QJsonObject &json);
    
    // 文件操作
    bool saveToFile(const QString &filePath);
    bool loadFromFile(const QString &filePath);

signals:
    void integrationModeChanged(IntegrationMode mode);
    void uiFlowStateChanged(const QString &fromStateId, const QString &toStateId);
    void logicStateChanged(const LogicSequenceStateMachine::LogicState &fromState, const LogicSequenceStateMachine::LogicState &toState);
    void uiInterfaceChanged(const QString &uiInterfaceId);
    void dataChanged(const QString &key, const QVariant &value);
    void stateSynchronized(bool success);

private slots:
    void onUiFlowStateChanged(const QString &fromStateId, const QString &toStateId);
    void onLogicStateChanged(const LogicSequenceStateMachine::LogicState &fromState, const LogicSequenceStateMachine::LogicState &toState);
    void onUiInterfaceChanged(const QString &uiInterfaceId);
    void onLogicDataChanged(const QString &key, const QVariant &value);

private:
    IntegrationMode m_integrationMode;
    UIFlowStateMachine* m_uiFlowStateMachine;
    LogicSequenceStateMachine* m_logicSequenceStateMachine;
    QList<StateMachineMapping> m_mappings;
    
    void setupConnections();
    void disconnectConnections();
    StateMachineMapping* findMappingById(const QString &mappingId);
    StateMachineMapping* findMappingByUIFlowState(const QString &uiFlowStateId);
    StateMachineMapping* findMappingByLogicState(const QString &logicStateId);
    
    const StateMachineMapping* findMappingById(const QString &mappingId) const;
    const StateMachineMapping* findMappingByUIFlowState(const QString &uiFlowStateId) const;
    const StateMachineMapping* findMappingByLogicState(const QString &logicStateId) const;
};

#endif // STATEMACHINEINTEGRATIONMANAGER_H