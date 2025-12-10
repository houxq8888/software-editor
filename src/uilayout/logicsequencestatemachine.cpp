#include "logicsequencestatemachine.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QUuid>
#include <QDebug>

// LogicState 实现
QJsonObject LogicSequenceStateMachine::LogicState::toJson() const
{
    QJsonObject json;
    json["stateId"] = stateId;
    json["name"] = name;
    json["description"] = description;
    json["isInitialState"] = isInitialState;
    json["isFinalState"] = isFinalState;
    
    // 序列化状态数据
    QJsonObject dataObject;
    for (auto it = data.begin(); it != data.end(); ++it) {
        dataObject[it.key()] = QJsonValue::fromVariant(it.value());
    }
    json["data"] = dataObject;
    
    return json;
}

bool LogicSequenceStateMachine::LogicState::fromJson(const QJsonObject &json)
{
    if (json.isEmpty()) return false;
    
    stateId = json["stateId"].toString();
    name = json["name"].toString();
    description = json["description"].toString();
    isInitialState = json["isInitialState"].toBool();
    isFinalState = json["isFinalState"].toBool();
    
    // 反序列化状态数据
    data.clear();
    QJsonObject dataObject = json["data"].toObject();
    for (auto it = dataObject.begin(); it != dataObject.end(); ++it) {
        data[it.key()] = it.value().toVariant();
    }
    
    return true;
}

// LogicCondition 实现
bool LogicSequenceStateMachine::LogicCondition::evaluate(const QVariant &currentValue) const
{
    if (!isEnabled) return false;
    
    switch (operatorType) {
    case ConditionOperator::Equal:
        return currentValue == value;
    case ConditionOperator::NotEqual:
        return currentValue != value;
    case ConditionOperator::Greater:
        return currentValue.toDouble() > value.toDouble();
    case ConditionOperator::Less:
        return currentValue.toDouble() < value.toDouble();
    case ConditionOperator::Contains:
        return currentValue.toString().contains(value.toString());
    case ConditionOperator::IsEmpty:
        return currentValue.toString().isEmpty();
    default:
        return false;
    }
}

QJsonObject LogicSequenceStateMachine::LogicCondition::toJson() const
{
    QJsonObject json;
    json["conditionId"] = conditionId;
    json["description"] = description;
    json["variable"] = variable;
    json["operatorType"] = static_cast<int>(operatorType);
    json["value"] = QJsonValue::fromVariant(value);
    json["isEnabled"] = isEnabled;
    return json;
}

bool LogicSequenceStateMachine::LogicCondition::fromJson(const QJsonObject &json)
{
    if (json.isEmpty()) return false;
    
    conditionId = json["conditionId"].toString();
    description = json["description"].toString();
    variable = json["variable"].toString();
    operatorType = static_cast<ConditionOperator>(json["operatorType"].toInt());
    value = json["value"].toVariant();
    isEnabled = json["isEnabled"].toBool(true);
    
    return true;
}

// LogicTransition 实现
QJsonObject LogicSequenceStateMachine::LogicTransition::toJson() const
{
    QJsonObject json;
    json["transitionId"] = transitionId;
    json["fromStateId"] = fromStateId;
    json["toStateId"] = toStateId;
    json["eventName"] = eventName;
    json["eventType"] = static_cast<int>(eventType);
    json["actionScript"] = actionScript;
    json["isEnabled"] = isEnabled;
    
    QJsonArray conditionsArray;
    for (const auto &condition : conditions) {
        conditionsArray.append(condition.toJson());
    }
    json["conditions"] = conditionsArray;
    
    return json;
}

bool LogicSequenceStateMachine::LogicTransition::fromJson(const QJsonObject &json)
{
    if (json.isEmpty()) return false;
    
    transitionId = json["transitionId"].toString();
    fromStateId = json["fromStateId"].toString();
    toStateId = json["toStateId"].toString();
    eventName = json["eventName"].toString();
    eventType = static_cast<LogicEventType>(json["eventType"].toInt());
    actionScript = json["actionScript"].toString();
    isEnabled = json["isEnabled"].toBool(true);
    
    conditions.clear();
    QJsonArray conditionsArray = json["conditions"].toArray();
    for (const auto &conditionValue : conditionsArray) {
        LogicCondition condition;
        if (condition.fromJson(conditionValue.toObject())) {
            conditions.append(condition);
        }
    }
    
    return true;
}

// LogicSequenceStateMachine 实现
LogicSequenceStateMachine::LogicSequenceStateMachine(const QString &name, QObject *parent)
    : QObject(parent)
    , m_name(name)
{
    m_id = QUuid::createUuid().toString(QUuid::WithoutBraces);
}

LogicSequenceStateMachine::~LogicSequenceStateMachine()
{
    m_states.clear();
    m_transitions.clear();
    m_globalData.clear();
}

QString LogicSequenceStateMachine::name() const
{
    return m_name;
}

void LogicSequenceStateMachine::setName(const QString &name)
{
    if (m_name != name) {
        m_name = name;
    }
}

QString LogicSequenceStateMachine::id() const
{
    return m_id;
}

QList<LogicSequenceStateMachine::LogicState> LogicSequenceStateMachine::states() const
{
    return m_states;
}

LogicSequenceStateMachine::LogicState* LogicSequenceStateMachine::findState(const QString &stateId)
{
    return findStateById(stateId);
}

bool LogicSequenceStateMachine::addState(const LogicState &state)
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

bool LogicSequenceStateMachine::removeState(const QString &stateId)
{
    auto it = std::find_if(m_states.begin(), m_states.end(), 
                          [&stateId](const LogicState &state) {
                              return state.stateId == stateId;
                          });
    
    if (it == m_states.end()) {
        qWarning() << "State with id" << stateId << "not found";
        return false;
    }
    
    // 移除与该状态相关的所有转换
    m_transitions.erase(std::remove_if(m_transitions.begin(), m_transitions.end(),
                                      [&stateId](const LogicTransition &transition) {
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

bool LogicSequenceStateMachine::updateState(const LogicState &state)
{
    auto it = std::find_if(m_states.begin(), m_states.end(), 
                          [&state](const LogicState &existingState) {
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

const LogicSequenceStateMachine::LogicState* LogicSequenceStateMachine::initialState() const
{
    return findStateById(m_initialStateId);
}

void LogicSequenceStateMachine::setInitialState(const QString &stateId)
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

QList<LogicSequenceStateMachine::LogicTransition> LogicSequenceStateMachine::transitions() const
{
    return m_transitions;
}

QList<LogicSequenceStateMachine::LogicTransition> LogicSequenceStateMachine::transitionsFromState(const QString &stateId) const
{
    QList<LogicTransition> result;
    for (const auto &transition : m_transitions) {
        if (transition.fromStateId == stateId) {
            result.append(transition);
        }
    }
    return result;
}

bool LogicSequenceStateMachine::addTransition(const LogicTransition &transition)
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

bool LogicSequenceStateMachine::removeTransition(const QString &transitionId)
{
    auto it = std::find_if(m_transitions.begin(), m_transitions.end(), 
                          [&transitionId](const LogicTransition &transition) {
                              return transition.transitionId == transitionId;
                          });
    
    if (it == m_transitions.end()) {
        qWarning() << "Transition with id" << transitionId << "not found";
        return false;
    }
    
    m_transitions.erase(it);
    return true;
}

bool LogicSequenceStateMachine::updateTransition(const LogicTransition &transition)
{
    auto it = std::find_if(m_transitions.begin(), m_transitions.end(), 
                          [&transition](const LogicTransition &existingTransition) {
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

const LogicSequenceStateMachine::LogicState* LogicSequenceStateMachine::currentState() const
{
    return findStateById(m_currentStateId);
}

bool LogicSequenceStateMachine::setCurrentState(const QString &stateId)
{
    if (stateId == m_currentStateId) return true;
    
    auto newState = findStateById(stateId);
    if (!newState) {
        qWarning() << "State with id" << stateId << "not found";
        return false;
    }
    
    auto oldState = findStateById(m_currentStateId);
    m_currentStateId = stateId;
    
    if (oldState) {
        emit stateChanged(*oldState, *newState);
    } else {
        emit stateChanged(LogicState(), *newState);
    }
    
    return true;
}

QVariant LogicSequenceStateMachine::getData(const QString &key) const
{
    return m_globalData.value(key);
}

void LogicSequenceStateMachine::setData(const QString &key, const QVariant &value)
{
    if (m_globalData.value(key) != value) {
        m_globalData[key] = value;
        emit dataChanged(key, value);
    }
}

QMap<QString, QVariant> LogicSequenceStateMachine::getAllData() const
{
    return m_globalData;
}

bool LogicSequenceStateMachine::processLogicEvent(const QString &eventName, LogicEventType eventType, const QVariantMap &context)
{
    if (m_currentStateId.isEmpty()) {
        qWarning() << "No current state set";
        return false;
    }
    
    // 查找当前状态的转换
    for (const auto &transition : m_transitions) {
        if (transition.fromStateId == m_currentStateId &&
            transition.eventName == eventName &&
            transition.eventType == eventType &&
            transition.isEnabled) {
            
            // 检查条件
            if (!checkConditions(transition.conditions)) {
                continue;
            }
            
            // 执行动作脚本
            if (!transition.actionScript.isEmpty()) {
                executeActionScript(transition.actionScript);
            }
            
            // 执行状态转换
            if (setCurrentState(transition.toStateId)) {
                emit transitionTriggered(transition);
                emit eventProcessed(eventName, true);
                return true;
            }
        }
    }
    
    qWarning() << "No matching transition found for event:" << eventName;
    emit eventProcessed(eventName, false);
    return false;
}

bool LogicSequenceStateMachine::checkConditions(const QList<LogicCondition> &conditions) const
{
    if (conditions.isEmpty()) return true;
    
    for (const auto &condition : conditions) {
        if (!condition.isEnabled) continue;
        
        QVariant currentValue = m_globalData.value(condition.variable);
        if (!condition.evaluate(currentValue)) {
            return false;
        }
    }
    
    return true;
}

QJsonObject LogicSequenceStateMachine::toJson() const
{
    QJsonObject json;
    json["id"] = m_id;
    json["name"] = m_name;
    json["initialStateId"] = m_initialStateId;
    json["currentStateId"] = m_currentStateId;
    
    // 序列化全局数据
    QJsonObject dataObject;
    for (auto it = m_globalData.begin(); it != m_globalData.end(); ++it) {
        dataObject[it.key()] = QJsonValue::fromVariant(it.value());
    }
    json["globalData"] = dataObject;
    
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

bool LogicSequenceStateMachine::fromJson(const QJsonObject &json)
{
    if (json.isEmpty()) return false;
    
    m_id = json["id"].toString();
    m_name = json["name"].toString();
    m_initialStateId = json["initialStateId"].toString();
    m_currentStateId = json["currentStateId"].toString();
    
    // 反序列化全局数据
    m_globalData.clear();
    QJsonObject dataObject = json["globalData"].toObject();
    for (auto it = dataObject.begin(); it != dataObject.end(); ++it) {
        m_globalData[it.key()] = it.value().toVariant();
    }
    
    // 解析状态
    m_states.clear();
    QJsonArray statesArray = json["states"].toArray();
    for (const auto &stateValue : statesArray) {
        LogicState state;
        if (state.fromJson(stateValue.toObject())) {
            m_states.append(state);
        }
    }
    
    // 解析转换
    m_transitions.clear();
    QJsonArray transitionsArray = json["transitions"].toArray();
    for (const auto &transitionValue : transitionsArray) {
        LogicTransition transition;
        if (transition.fromJson(transitionValue.toObject())) {
            m_transitions.append(transition);
        }
    }
    
    return true;
}

bool LogicSequenceStateMachine::saveToFile(const QString &filePath)
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

bool LogicSequenceStateMachine::loadFromFile(const QString &filePath)
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

bool LogicSequenceStateMachine::validate() const
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

QList<QString> LogicSequenceStateMachine::validationErrors() const
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

LogicSequenceStateMachine::LogicState* LogicSequenceStateMachine::findStateById(const QString &stateId)
{
    for (auto &state : m_states) {
        if (state.stateId == stateId) {
            return &state;
        }
    }
    return nullptr;
}

const LogicSequenceStateMachine::LogicState* LogicSequenceStateMachine::findStateById(const QString &stateId) const
{
    for (const auto &state : m_states) {
        if (state.stateId == stateId) {
            return &state;
        }
    }
    return nullptr;
}

LogicSequenceStateMachine::LogicTransition* LogicSequenceStateMachine::findTransitionById(const QString &transitionId)
{
    for (auto &transition : m_transitions) {
        if (transition.transitionId == transitionId) {
            return &transition;
        }
    }
    return nullptr;
}

void LogicSequenceStateMachine::executeActionScript(const QString &script)
{
    // 简单的脚本执行实现
    // 这里可以使用QScriptEngine或其他脚本引擎
    // 目前先实现简单的变量赋值操作
    
    Q_UNUSED(script);
    qDebug() << "Executing action script:" << script;
    // TODO: 实现脚本执行逻辑
}

// LogicSequenceStateMachineManager 实现
LogicSequenceStateMachineManager::LogicSequenceStateMachineManager(QObject *parent)
    : QObject(parent)
    , m_currentStateMachine(nullptr)
{
}

LogicSequenceStateMachineManager::~LogicSequenceStateMachineManager()
{
    qDeleteAll(m_stateMachines);
    m_stateMachines.clear();
}

QList<LogicSequenceStateMachine*> LogicSequenceStateMachineManager::stateMachines() const
{
    return m_stateMachines;
}

LogicSequenceStateMachine* LogicSequenceStateMachineManager::currentStateMachine() const
{
    return m_currentStateMachine;
}

void LogicSequenceStateMachineManager::setCurrentStateMachine(LogicSequenceStateMachine *stateMachine)
{
    if (m_currentStateMachine != stateMachine) {
        m_currentStateMachine = stateMachine;
        emit currentStateMachineChanged(stateMachine);
    }
}

LogicSequenceStateMachine* LogicSequenceStateMachineManager::createStateMachine(const QString &name)
{
    auto stateMachine = new LogicSequenceStateMachine(name, this);
    m_stateMachines.append(stateMachine);
    
    // 如果没有当前状态机，设置为当前
    if (!m_currentStateMachine) {
        setCurrentStateMachine(stateMachine);
    }
    
    return stateMachine;
}

bool LogicSequenceStateMachineManager::removeStateMachine(LogicSequenceStateMachine *stateMachine)
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

LogicSequenceStateMachine* LogicSequenceStateMachineManager::findStateMachine(const QString &stateMachineId) const
{
    for (auto stateMachine : m_stateMachines) {
        if (stateMachine->id() == stateMachineId) {
            return stateMachine;
        }
    }
    return nullptr;
}

QJsonObject LogicSequenceStateMachineManager::toJson() const
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

bool LogicSequenceStateMachineManager::fromJson(const QJsonObject &json)
{
    if (json.isEmpty()) return false;
    
    // 清除现有状态机
    qDeleteAll(m_stateMachines);
    m_stateMachines.clear();
    m_currentStateMachine = nullptr;
    
    // 解析状态机
    QJsonArray stateMachinesArray = json["stateMachines"].toArray();
    for (const auto &stateMachineValue : stateMachinesArray) {
        auto stateMachine = new LogicSequenceStateMachine();
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

bool LogicSequenceStateMachineManager::saveToFile(const QString &filePath)
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

bool LogicSequenceStateMachineManager::loadFromFile(const QString &filePath)
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