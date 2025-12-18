#include "statemachineintegrationmanager.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QUuid>
#include <QDebug>

// StateMachineMapping 实现
QJsonObject StateMachineIntegrationManager::StateMachineMapping::toJson() const
{
    QJsonObject json;
    json["mappingId"] = mappingId;
    json["uiFlowStateId"] = uiFlowStateId;
    json["logicStateId"] = logicStateId;
    return json;
}

bool StateMachineIntegrationManager::StateMachineMapping::fromJson(const QJsonObject &json)
{
    if (json.isEmpty()) return false;
    
    mappingId = json["mappingId"].toString();
    uiFlowStateId = json["uiFlowStateId"].toString();
    logicStateId = json["logicStateId"].toString();
    
    return true;
}

// StateMachineIntegrationManager 实现
StateMachineIntegrationManager::StateMachineIntegrationManager(QObject *parent)
    : QObject(parent)
    , m_integrationMode(IntegrationMode::UIFlowMode)
    , m_uiFlowStateMachine(nullptr)
    , m_logicSequenceStateMachine(nullptr)
{
}

StateMachineIntegrationManager::~StateMachineIntegrationManager()
{
    disconnectConnections();
    m_mappings.clear();
}

StateMachineIntegrationManager::IntegrationMode StateMachineIntegrationManager::integrationMode() const
{
    return m_integrationMode;
}

void StateMachineIntegrationManager::setIntegrationMode(IntegrationMode mode)
{
    if (m_integrationMode != mode) {
        m_integrationMode = mode;
        emit integrationModeChanged(mode);
        
        // 根据模式重新设置连接
        disconnectConnections();
        setupConnections();
    }
}

UIFlowStateMachine* StateMachineIntegrationManager::uiFlowStateMachine() const
{
    return m_uiFlowStateMachine;
}

void StateMachineIntegrationManager::setUiFlowStateMachine(UIFlowStateMachine *stateMachine)
{
    if (m_uiFlowStateMachine != stateMachine) {
        disconnectConnections();
        m_uiFlowStateMachine = stateMachine;
        setupConnections();
    }
}

LogicSequenceStateMachine* StateMachineIntegrationManager::logicSequenceStateMachine() const
{
    return m_logicSequenceStateMachine;
}

void StateMachineIntegrationManager::setLogicSequenceStateMachine(LogicSequenceStateMachine *stateMachine)
{
    if (m_logicSequenceStateMachine != stateMachine) {
        disconnectConnections();
        m_logicSequenceStateMachine = stateMachine;
        setupConnections();
    }
}

QList<StateMachineIntegrationManager::StateMachineMapping> StateMachineIntegrationManager::mappings() const
{
    return m_mappings;
}

bool StateMachineIntegrationManager::addMapping(const StateMachineMapping &mapping)
{
    if (mapping.mappingId.isEmpty()) {
        qWarning() << "Cannot add mapping with empty mappingId";
        return false;
    }
    
    if (findMappingById(mapping.mappingId)) {
        qWarning() << "Mapping with id" << mapping.mappingId << "already exists";
        return false;
    }
    
    // 验证状态存在
    if (m_uiFlowStateMachine && !m_uiFlowStateMachine->findState(mapping.uiFlowStateId)) {
        qWarning() << "UI flow state not found:" << mapping.uiFlowStateId;
        return false;
    }
    
    if (m_logicSequenceStateMachine && !m_logicSequenceStateMachine->findState(mapping.logicStateId)) {
        qWarning() << "Logic state not found:" << mapping.logicStateId;
        return false;
    }
    
    m_mappings.append(mapping);
    return true;
}

bool StateMachineIntegrationManager::removeMapping(const QString &mappingId)
{
    auto it = std::find_if(m_mappings.begin(), m_mappings.end(), 
                          [&mappingId](const StateMachineMapping &mapping) {
                              return mapping.mappingId == mappingId;
                          });
    
    if (it == m_mappings.end()) {
        qWarning() << "Mapping with id" << mappingId << "not found";
        return false;
    }
    
    m_mappings.erase(it);
    return true;
}

bool StateMachineIntegrationManager::updateMapping(const StateMachineMapping &mapping)
{
    auto it = std::find_if(m_mappings.begin(), m_mappings.end(), 
                          [&mapping](const StateMachineMapping &existingMapping) {
                              return existingMapping.mappingId == mapping.mappingId;
                          });
    
    if (it == m_mappings.end()) {
        qWarning() << "Mapping with id" << mapping.mappingId << "not found";
        return false;
    }
    
    // 验证状态存在
    if (m_uiFlowStateMachine && !m_uiFlowStateMachine->findState(mapping.uiFlowStateId)) {
        qWarning() << "UI flow state not found:" << mapping.uiFlowStateId;
        return false;
    }
    
    if (m_logicSequenceStateMachine && !m_logicSequenceStateMachine->findState(mapping.logicStateId)) {
        qWarning() << "Logic state not found:" << mapping.logicStateId;
        return false;
    }
    
    *it = mapping;
    return true;
}

QString StateMachineIntegrationManager::getLogicStateForUIFlowState(const QString &uiFlowStateId) const
{
    auto mapping = findMappingByUIFlowState(uiFlowStateId);
    return mapping ? mapping->logicStateId : QString();
}

QString StateMachineIntegrationManager::getUIFlowStateForLogicState(const QString &logicStateId) const
{
    auto mapping = findMappingByLogicState(logicStateId);
    return mapping ? mapping->uiFlowStateId : QString();
}

bool StateMachineIntegrationManager::handleUIEvent(const QString &eventSource, const QString &eventType)
{
    switch (m_integrationMode) {
    case IntegrationMode::UIFlowMode:
        if (m_uiFlowStateMachine) {
            return m_uiFlowStateMachine->handleUIEvent(eventSource, eventType);
        }
        break;
        
    case IntegrationMode::LogicSequenceMode:
        // 在逻辑时序模式下，UI事件可能被忽略或转换为逻辑事件
        qDebug() << "UI event ignored in logic-only mode:" << eventSource << eventType;
        break;
    }
    
    return false;
}

bool StateMachineIntegrationManager::handleLogicEvent(const QString &eventName, LogicSequenceStateMachine::LogicEventType eventType, const QVariantMap &context)
{
    switch (m_integrationMode) {
    case IntegrationMode::UIFlowMode:
        // 在UI流模式下，逻辑事件可能被忽略
        qDebug() << "Logic event ignored in UI-only mode:" << eventName;
        break;
        
    case IntegrationMode::LogicSequenceMode:
        if (m_logicSequenceStateMachine) {
            return m_logicSequenceStateMachine->processLogicEvent(eventName, eventType, context);
        }
        break;
    }
    
    return false;
}

bool StateMachineIntegrationManager::synchronizeStates()
{
    if (!m_uiFlowStateMachine || !m_logicSequenceStateMachine) {
        qWarning() << "Both state machines must be set for synchronization";
        return false;
    }
    
    bool success = true;
    
    // 根据映射关系同步状态
    for (const auto &mapping : m_mappings) {
        auto uiState = m_uiFlowStateMachine->findState(mapping.uiFlowStateId);
        auto logicState = m_logicSequenceStateMachine->findState(mapping.logicStateId);
        
        if (uiState && logicState) {
            // 这里可以实现更复杂的同步逻辑
            // 例如：当逻辑状态满足某些条件时，自动切换到对应的UI状态
        }
    }
    
    emit stateSynchronized(success);
    return success;
}

QJsonObject StateMachineIntegrationManager::toJson() const
{
    QJsonObject json;
    json["integrationMode"] = static_cast<int>(m_integrationMode);
    
    // 序列化状态机
    if (m_uiFlowStateMachine) {
        json["uiFlowStateMachine"] = m_uiFlowStateMachine->toJson();
    }
    
    if (m_logicSequenceStateMachine) {
        json["logicSequenceStateMachine"] = m_logicSequenceStateMachine->toJson();
    }
    
    // 序列化映射
    QJsonArray mappingsArray;
    for (const auto &mapping : m_mappings) {
        mappingsArray.append(mapping.toJson());
    }
    json["mappings"] = mappingsArray;
    
    return json;
}

bool StateMachineIntegrationManager::fromJson(const QJsonObject &json)
{
    if (json.isEmpty()) return false;
    
    // 设置集成模式
    m_integrationMode = static_cast<IntegrationMode>(json["integrationMode"].toInt());
    
    // 解析UI流状态机
    if (json.contains("uiFlowStateMachine")) {
        if (!m_uiFlowStateMachine) {
            m_uiFlowStateMachine = new UIFlowStateMachine("UI流状态机", this);
        }
        m_uiFlowStateMachine->fromJson(json["uiFlowStateMachine"].toObject());
    }
    
    // 解析逻辑时序状态机
    if (json.contains("logicSequenceStateMachine")) {
        if (!m_logicSequenceStateMachine) {
            m_logicSequenceStateMachine = new LogicSequenceStateMachine("逻辑时序状态机", this);
        }
        m_logicSequenceStateMachine->fromJson(json["logicSequenceStateMachine"].toObject());
    }
    
    // 解析映射
    m_mappings.clear();
    QJsonArray mappingsArray = json["mappings"].toArray();
    for (const auto &mappingValue : mappingsArray) {
        StateMachineMapping mapping;
        if (mapping.fromJson(mappingValue.toObject())) {
            m_mappings.append(mapping);
        }
    }
    
    // 重新设置连接
    disconnectConnections();
    setupConnections();
    
    return true;
}

bool StateMachineIntegrationManager::saveToFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Cannot open file for writing:" << filePath;
        return false;
    }
    
    QJsonDocument doc(toJson());
    file.write(doc.toJson());
    file.close();
    
    return true;
}

bool StateMachineIntegrationManager::loadFromFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot open file for reading:" << filePath;
        return false;
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    
    if (doc.isNull()) {
        qWarning() << "Invalid JSON document in file:" << filePath;
        return false;
    }
    
    return fromJson(doc.object());
}

void StateMachineIntegrationManager::setupConnections()
{
    if (m_uiFlowStateMachine) {
        connect(m_uiFlowStateMachine, &UIFlowStateMachine::stateChanged,
                this, &StateMachineIntegrationManager::onUiFlowStateChanged);
        connect(m_uiFlowStateMachine, &UIFlowStateMachine::uiInterfaceChanged,
                this, &StateMachineIntegrationManager::onUiInterfaceChanged);
    }
    
    if (m_logicSequenceStateMachine) {
        connect(m_logicSequenceStateMachine, &LogicSequenceStateMachine::stateChanged,
                this, &StateMachineIntegrationManager::onLogicStateChanged);
        connect(m_logicSequenceStateMachine, &LogicSequenceStateMachine::dataChanged,
                this, &StateMachineIntegrationManager::onLogicDataChanged);
    }
}

void StateMachineIntegrationManager::disconnectConnections()
{
    if (m_uiFlowStateMachine) {
        disconnect(m_uiFlowStateMachine, &UIFlowStateMachine::stateChanged,
                   this, &StateMachineIntegrationManager::onUiFlowStateChanged);
        disconnect(m_uiFlowStateMachine, &UIFlowStateMachine::uiInterfaceChanged,
                   this, &StateMachineIntegrationManager::onUiInterfaceChanged);
    }
    
    if (m_logicSequenceStateMachine) {
        disconnect(m_logicSequenceStateMachine, &LogicSequenceStateMachine::stateChanged,
                   this, &StateMachineIntegrationManager::onLogicStateChanged);
        disconnect(m_logicSequenceStateMachine, &LogicSequenceStateMachine::dataChanged,
                   this, &StateMachineIntegrationManager::onLogicDataChanged);
    }
}

void StateMachineIntegrationManager::onUiFlowStateChanged(const QString &fromStateId, const QString &toStateId)
{
    emit uiFlowStateChanged(fromStateId, toStateId);
    
    // 在逻辑时序模式下，UI状态变化可能触发逻辑状态同步
    if (m_integrationMode == IntegrationMode::LogicSequenceMode && m_logicSequenceStateMachine) {
        QString logicStateId = getLogicStateForUIFlowState(toStateId);
        if (!logicStateId.isEmpty()) {
            m_logicSequenceStateMachine->setCurrentState(logicStateId);
        }
    }
}

void StateMachineIntegrationManager::onLogicStateChanged(const LogicSequenceStateMachine::LogicState &fromState, const LogicSequenceStateMachine::LogicState &toState)
{
    emit logicStateChanged(fromState, toState);
    
    // 在逻辑时序模式下，逻辑状态变化可能触发UI状态同步
    if (m_integrationMode == IntegrationMode::LogicSequenceMode && m_uiFlowStateMachine) {
        QString uiFlowStateId = getUIFlowStateForLogicState(toState.stateId);
        if (!uiFlowStateId.isEmpty()) {
            m_uiFlowStateMachine->setCurrentState(uiFlowStateId);
        }
    }
}

void StateMachineIntegrationManager::onUiInterfaceChanged(const QString &uiInterfaceId)
{
    emit uiInterfaceChanged(uiInterfaceId);
}

void StateMachineIntegrationManager::onLogicDataChanged(const QString &key, const QVariant &value)
{
    emit dataChanged(key, value);
}

StateMachineIntegrationManager::StateMachineMapping* StateMachineIntegrationManager::findMappingById(const QString &mappingId)
{
    for (auto &mapping : m_mappings) {
        if (mapping.mappingId == mappingId) {
            return &mapping;
        }
    }
    return nullptr;
}

StateMachineIntegrationManager::StateMachineMapping* StateMachineIntegrationManager::findMappingByUIFlowState(const QString &uiFlowStateId)
{
    for (auto &mapping : m_mappings) {
        if (mapping.uiFlowStateId == uiFlowStateId) {
            return &mapping;
        }
    }
    return nullptr;
}

StateMachineIntegrationManager::StateMachineMapping* StateMachineIntegrationManager::findMappingByLogicState(const QString &logicStateId)
{
    for (auto &mapping : m_mappings) {
        if (mapping.logicStateId == logicStateId) {
            return &mapping;
        }
    }
    return nullptr;
}

const StateMachineIntegrationManager::StateMachineMapping* StateMachineIntegrationManager::findMappingById(const QString &mappingId) const
{
    for (const auto &mapping : m_mappings) {
        if (mapping.mappingId == mappingId) {
            return &mapping;
        }
    }
    return nullptr;
}

const StateMachineIntegrationManager::StateMachineMapping* StateMachineIntegrationManager::findMappingByUIFlowState(const QString &uiFlowStateId) const
{
    for (const auto &mapping : m_mappings) {
        if (mapping.uiFlowStateId == uiFlowStateId) {
            return &mapping;
        }
    }
    return nullptr;
}

const StateMachineIntegrationManager::StateMachineMapping* StateMachineIntegrationManager::findMappingByLogicState(const QString &logicStateId) const
{
    for (const auto &mapping : m_mappings) {
        if (mapping.logicStateId == logicStateId) {
            return &mapping;
        }
    }
    return nullptr;
}