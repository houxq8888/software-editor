#include "uiflowstatemachine.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QUuid>
#include <QDebug>

// UIFlowState 实现
QJsonObject UIFlowStateMachine::UIFlowState::toJson() const
{
    QJsonObject json;
    json["stateId"] = stateId;
    json["name"] = name;
    json["uiInterfaceId"] = uiInterfaceId;
    json["isInitialState"] = isInitialState;
    json["isFinalState"] = isFinalState;
    return json;
}

bool UIFlowStateMachine::UIFlowState::fromJson(const QJsonObject &json)
{
    if (json.isEmpty()) return false;
    
    stateId = json["stateId"].toString();
    name = json["name"].toString();
    uiInterfaceId = json["uiInterfaceId"].toString();
    isInitialState = json["isInitialState"].toBool();
    isFinalState = json["isFinalState"].toBool();
    
    return true;
}

// UIFlowTransition 实现
QJsonObject UIFlowStateMachine::UIFlowTransition::toJson() const
{
    QJsonObject json;
    json["transitionId"] = transitionId;
    json["fromStateId"] = fromStateId;
    json["toStateId"] = toStateId;
    json["eventSource"] = eventSource;
    json["eventType"] = eventType;
    return json;
}

bool UIFlowStateMachine::UIFlowTransition::fromJson(const QJsonObject &json)
{
    if (json.isEmpty()) return false;
    
    transitionId = json["transitionId"].toString();
    fromStateId = json["fromStateId"].toString();
    toStateId = json["toStateId"].toString();
    eventSource = json["eventSource"].toString();
    eventType = json["eventType"].toString();
    
    return true;
}

// UIFlowStateMachine 实现
UIFlowStateMachine::UIFlowStateMachine(const QString &name, QObject *parent)
    : QObject(parent)
    , m_name(name)
{
    m_id = QUuid::createUuid().toString(QUuid::WithoutBraces);
}

UIFlowStateMachine::~UIFlowStateMachine()
{
    m_states.clear();
    m_transitions.clear();
}

QString UIFlowStateMachine::name() const
{
    return m_name;
}

void UIFlowStateMachine::setName(const QString &name)
{
    if (m_name != name) {
        m_name = name;
    }
}

QString UIFlowStateMachine::id() const
{
    return m_id;
}

QList<UIFlowStateMachine::UIFlowState> UIFlowStateMachine::states() const
{
    return m_states;
}

UIFlowStateMachine::UIFlowState* UIFlowStateMachine::findState(const QString &stateId)
{
    return findStateById(stateId);
}

UIFlowStateMachine::UIFlowState* UIFlowStateMachine::findStateByUiInterface(const QString &uiInterfaceId)
{
    for (auto &state : m_states) {
        if (state.uiInterfaceId == uiInterfaceId) {
            return &state;
        }
    }
    return nullptr;
}

bool UIFlowStateMachine::addState(const UIFlowState &state)
{
    if (state.stateId.isEmpty()) {
        qWarning() << "Cannot add state with empty stateId";
        return false;
    }
    
    if (findStateById(state.stateId)) {
        qWarning() << "State with id" << state.stateId << "already exists";
        return false;
    }
    
    m_states.append(state);
    
    // 如果是初始状态，设置初始状态ID
    if (state.isInitialState) {
        m_initialStateId = state.stateId;
        // 如果没有当前状态，设置为初始状态
        if (m_currentStateId.isEmpty()) {
            m_currentStateId = state.stateId;
        }
    }
    
    return true;
}

bool UIFlowStateMachine::removeState(const QString &stateId)
{
    auto it = std::find_if(m_states.begin(), m_states.end(), 
                          [&stateId](const UIFlowState &state) {
                              return state.stateId == stateId;
                          });
    
    if (it == m_states.end()) {
        qWarning() << "State with id" << stateId << "not found";
        return false;
    }
    
    // 移除与该状态相关的所有转换
    m_transitions.erase(std::remove_if(m_transitions.begin(), m_transitions.end(),
                                      [&stateId](const UIFlowTransition &transition) {
                                          return transition.fromStateId == stateId || 
                                                 transition.toStateId == stateId;
                                      }), m_transitions.end());
    
    // 如果移除的是当前状态，重置当前状态
    if (m_currentStateId == stateId) {
        m_currentStateId.clear();
    }
    
    // 如果移除的是初始状态，重置初始状态
    if (m_initialStateId == stateId) {
        m_initialStateId.clear();
    }
    
    m_states.erase(it);
    return true;
}

bool UIFlowStateMachine::updateState(const UIFlowState &state)
{
    auto it = std::find_if(m_states.begin(), m_states.end(), 
                          [&state](const UIFlowState &existingState) {
                              return existingState.stateId == state.stateId;
                          });
    
    if (it == m_states.end()) {
        qWarning() << "State with id" << state.stateId << "not found";
        return false;
    }
    
    *it = state;
    
    // 更新初始状态设置
    if (state.isInitialState) {
        m_initialStateId = state.stateId;
    } else if (m_initialStateId == state.stateId) {
        m_initialStateId.clear();
    }
    
    return true;
}

const UIFlowStateMachine::UIFlowState* UIFlowStateMachine::initialState() const
{
    return findStateById(m_initialStateId);
}

void UIFlowStateMachine::setInitialState(const QString &stateId)
{
    if (stateId == m_initialStateId) return;
    
    // 清除之前的初始状态标记
    if (!m_initialStateId.isEmpty()) {
        auto oldState = findStateById(m_initialStateId);
        if (oldState) {
            oldState->isInitialState = false;
        }
    }
    
    // 设置新的初始状态
    auto newState = findStateById(stateId);
    if (newState) {
        newState->isInitialState = true;
        m_initialStateId = stateId;
        
        // 如果没有当前状态，设置为初始状态
        if (m_currentStateId.isEmpty()) {
            m_currentStateId = stateId;
        }
    }
}

QList<UIFlowStateMachine::UIFlowTransition> UIFlowStateMachine::transitions() const
{
    return m_transitions;
}

QList<UIFlowStateMachine::UIFlowTransition> UIFlowStateMachine::transitionsFromState(const QString &stateId) const
{
    QList<UIFlowTransition> result;
    for (const auto &transition : m_transitions) {
        if (transition.fromStateId == stateId) {
            result.append(transition);
        }
    }
    return result;
}

bool UIFlowStateMachine::addTransition(const UIFlowTransition &transition)
{
    if (transition.transitionId.isEmpty()) {
        qWarning() << "Cannot add transition with empty transitionId";
        return false;
    }
    
    if (findTransitionById(transition.transitionId)) {
        qWarning() << "Transition with id" << transition.transitionId << "already exists";
        return false;
    }
    
    // 验证源状态和目标状态存在
    if (!findStateById(transition.fromStateId)) {
        qWarning() << "Source state" << transition.fromStateId << "not found";
        return false;
    }
    
    if (!findStateById(transition.toStateId)) {
        qWarning() << "Target state" << transition.toStateId << "not found";
        return false;
    }
    
    m_transitions.append(transition);
    return true;
}

bool UIFlowStateMachine::removeTransition(const QString &transitionId)
{
    auto it = std::find_if(m_transitions.begin(), m_transitions.end(), 
                          [&transitionId](const UIFlowTransition &transition) {
                              return transition.transitionId == transitionId;
                          });
    
    if (it == m_transitions.end()) {
        qWarning() << "Transition with id" << transitionId << "not found";
        return false;
    }
    
    m_transitions.erase(it);
    return true;
}

bool UIFlowStateMachine::updateTransition(const UIFlowTransition &transition)
{
    auto it = std::find_if(m_transitions.begin(), m_transitions.end(), 
                          [&transition](const UIFlowTransition &existingTransition) {
                              return existingTransition.transitionId == transition.transitionId;
                          });
    
    if (it == m_transitions.end()) {
        qWarning() << "Transition with id" << transition.transitionId << "not found";
        return false;
    }
    
    // 验证源状态和目标状态存在
    if (!findStateById(transition.fromStateId)) {
        qWarning() << "Source state" << transition.fromStateId << "not found";
        return false;
    }
    
    if (!findStateById(transition.toStateId)) {
        qWarning() << "Target state" << transition.toStateId << "not found";
        return false;
    }
    
    *it = transition;
    return true;
}

const UIFlowStateMachine::UIFlowState* UIFlowStateMachine::currentState() const
{
    return findStateById(m_currentStateId);
}

bool UIFlowStateMachine::setCurrentState(const QString &stateId)
{
    if (stateId == m_currentStateId) return true;
    
    auto state = findStateById(stateId);
    if (!state) {
        qWarning() << "State with id" << stateId << "not found";
        return false;
    }
    
    QString oldStateId = m_currentStateId;
    m_currentStateId = stateId;
    
    emit stateChanged(oldStateId, stateId);
    emit uiInterfaceChanged(state->uiInterfaceId);
    
    return true;
}

bool UIFlowStateMachine::handleUIEvent(const QString &eventSource, const QString &eventType)
{
    if (m_currentStateId.isEmpty()) {
        qWarning() << "No current state set";
        return false;
    }
    
    // 查找当前状态的转换
    for (const auto &transition : m_transitions) {
        if (transition.fromStateId == m_currentStateId &&
            transition.eventSource == eventSource &&
            transition.eventType == eventType) {
            
            // 执行状态转换
            if (setCurrentState(transition.toStateId)) {
                emit transitionTriggered(transition.transitionId);
                return true;
            }
        }
    }
    
    qWarning() << "No matching transition found for event:" << eventSource << eventType;
    return false;
}

QJsonObject UIFlowStateMachine::toJson() const
{
    QJsonObject json;
    json["id"] = m_id;
    json["name"] = m_name;
    json["initialStateId"] = m_initialStateId;
    json["currentStateId"] = m_currentStateId;
    
    QJsonArray statesArray;
    for (const auto &state : m_states) {
        statesArray.append(state.toJson());
    }
    json["states"] = statesArray;
    
    QJsonArray transitionsArray;
    for (const auto &transition : m_transitions) {
        transitionsArray.append(transition.toJson());
    }
    json["transitions"] = transitionsArray;
    
    return json;
}

bool UIFlowStateMachine::fromJson(const QJsonObject &json)
{
    if (json.isEmpty()) return false;
    
    m_id = json["id"].toString();
    m_name = json["name"].toString();
    m_initialStateId = json["initialStateId"].toString();
    m_currentStateId = json["currentStateId"].toString();
    
    // 解析状态
    m_states.clear();
    QJsonArray statesArray = json["states"].toArray();
    for (const auto &stateValue : statesArray) {
        UIFlowState state;
        if (state.fromJson(stateValue.toObject())) {
            m_states.append(state);
        }
    }
    
    // 解析转换
    m_transitions.clear();
    QJsonArray transitionsArray = json["transitions"].toArray();
    for (const auto &transitionValue : transitionsArray) {
        UIFlowTransition transition;
        if (transition.fromJson(transitionValue.toObject())) {
            m_transitions.append(transition);
        }
    }
    
    return true;
}

bool UIFlowStateMachine::saveToFile(const QString &filePath)
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

bool UIFlowStateMachine::loadFromFile(const QString &filePath)
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

bool UIFlowStateMachine::validate() const
{
    // 基本验证逻辑
    if (m_states.isEmpty()) {
        qWarning() << "State machine has no states";
        return false;
    }
    
    // 验证初始状态存在
    if (!m_initialStateId.isEmpty() && !findStateById(m_initialStateId)) {
        qWarning() << "Initial state not found:" << m_initialStateId;
        return false;
    }
    
    // 验证转换的源状态和目标状态存在
    for (const auto &transition : m_transitions) {
        if (!findStateById(transition.fromStateId)) {
            qWarning() << "Transition source state not found:" << transition.fromStateId;
            return false;
        }
        if (!findStateById(transition.toStateId)) {
            qWarning() << "Transition target state not found:" << transition.toStateId;
            return false;
        }
    }
    
    return true;
}

QList<QString> UIFlowStateMachine::validationErrors() const
{
    QList<QString> errors;
    
    if (m_states.isEmpty()) {
        errors.append("状态机没有状态");
    }
    
    if (!m_initialStateId.isEmpty() && !findStateById(m_initialStateId)) {
        errors.append("初始状态不存在: " + m_initialStateId);
    }
    
    for (const auto &transition : m_transitions) {
        if (!findStateById(transition.fromStateId)) {
            errors.append("转换源状态不存在: " + transition.fromStateId);
        }
        if (!findStateById(transition.toStateId)) {
            errors.append("转换目标状态不存在: " + transition.toStateId);
        }
    }
    
    return errors;
}

UIFlowStateMachine::UIFlowState* UIFlowStateMachine::findStateById(const QString &stateId)
{
    for (auto &state : m_states) {
        if (state.stateId == stateId) {
            return &state;
        }
    }
    return nullptr;
}

const UIFlowStateMachine::UIFlowState* UIFlowStateMachine::findStateById(const QString &stateId) const
{
    for (const auto &state : m_states) {
        if (state.stateId == stateId) {
            return &state;
        }
    }
    return nullptr;
}

UIFlowStateMachine::UIFlowTransition* UIFlowStateMachine::findTransitionById(const QString &transitionId)
{
    for (auto &transition : m_transitions) {
        if (transition.transitionId == transitionId) {
            return &transition;
        }
    }
    return nullptr;
}

// UIFlowStateMachineManager 实现
UIFlowStateMachineManager::UIFlowStateMachineManager(QObject *parent)
    : QObject(parent)
    , m_currentStateMachine(nullptr)
{
}

UIFlowStateMachineManager::~UIFlowStateMachineManager()
{
    qDeleteAll(m_stateMachines);
    m_stateMachines.clear();
}

QList<UIFlowStateMachine*> UIFlowStateMachineManager::stateMachines() const
{
    return m_stateMachines;
}

UIFlowStateMachine* UIFlowStateMachineManager::currentStateMachine() const
{
    return m_currentStateMachine;
}

void UIFlowStateMachineManager::setCurrentStateMachine(UIFlowStateMachine *stateMachine)
{
    if (m_currentStateMachine != stateMachine) {
        m_currentStateMachine = stateMachine;
        emit currentStateMachineChanged(stateMachine);
    }
}

UIFlowStateMachine* UIFlowStateMachineManager::createStateMachine(const QString &name)
{
    auto stateMachine = new UIFlowStateMachine(name, this);
    m_stateMachines.append(stateMachine);
    
    // 如果没有当前状态机，设置为当前
    if (!m_currentStateMachine) {
        setCurrentStateMachine(stateMachine);
    }
    
    return stateMachine;
}

bool UIFlowStateMachineManager::removeStateMachine(UIFlowStateMachine *stateMachine)
{
    if (!m_stateMachines.contains(stateMachine)) {
        return false;
    }
    
    m_stateMachines.removeOne(stateMachine);
    
    // 如果移除的是当前状态机，重置当前状态机
    if (m_currentStateMachine == stateMachine) {
        setCurrentStateMachine(m_stateMachines.isEmpty() ? nullptr : m_stateMachines.first());
    }
    
    delete stateMachine;
    return true;
}

UIFlowStateMachine* UIFlowStateMachineManager::findStateMachine(const QString &stateMachineId) const
{
    for (auto stateMachine : m_stateMachines) {
        if (stateMachine->id() == stateMachineId) {
            return stateMachine;
        }
    }
    return nullptr;
}

bool UIFlowStateMachineManager::linkStateToUiInterface(const QString &stateId, const QString &uiInterfaceId)
{
    if (!m_currentStateMachine) {
        qWarning() << "No current state machine";
        return false;
    }
    
    auto state = m_currentStateMachine->findState(stateId);
    if (!state) {
        qWarning() << "State not found:" << stateId;
        return false;
    }
    
    state->uiInterfaceId = uiInterfaceId;
    return true;
}

QString UIFlowStateMachineManager::getUiInterfaceForState(const QString &stateId) const
{
    if (!m_currentStateMachine) {
        return QString();
    }
    
    auto state = m_currentStateMachine->findState(stateId);
    return state ? state->uiInterfaceId : QString();
}

QString UIFlowStateMachineManager::getStateForUiInterface(const QString &uiInterfaceId) const
{
    if (!m_currentStateMachine) {
        return QString();
    }
    
    auto state = m_currentStateMachine->findStateByUiInterface(uiInterfaceId);
    return state ? state->stateId : QString();
}

QJsonObject UIFlowStateMachineManager::toJson() const
{
    QJsonObject json;
    
    QJsonArray stateMachinesArray;
    for (auto stateMachine : m_stateMachines) {
        stateMachinesArray.append(stateMachine->toJson());
    }
    json["stateMachines"] = stateMachinesArray;
    
    json["currentStateMachineId"] = m_currentStateMachine ? m_currentStateMachine->id() : QString();
    
    return json;
}

bool UIFlowStateMachineManager::fromJson(const QJsonObject &json)
{
    if (json.isEmpty()) return false;
    
    // 清除现有状态机
    qDeleteAll(m_stateMachines);
    m_stateMachines.clear();
    m_currentStateMachine = nullptr;
    
    // 解析状态机
    QJsonArray stateMachinesArray = json["stateMachines"].toArray();
    for (const auto &stateMachineValue : stateMachinesArray) {
        auto stateMachine = new UIFlowStateMachine();
        if (stateMachine->fromJson(stateMachineValue.toObject())) {
            m_stateMachines.append(stateMachine);
        } else {
            delete stateMachine;
        }
    }
    
    // 设置当前状态机
    QString currentStateMachineId = json["currentStateMachineId"].toString();
    if (!currentStateMachineId.isEmpty()) {
        m_currentStateMachine = findStateMachine(currentStateMachineId);
    }
    
    return true;
}

bool UIFlowStateMachineManager::saveToFile(const QString &filePath)
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

bool UIFlowStateMachineManager::loadFromFile(const QString &filePath)
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