#include "statemachine.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QUuid>
#include <QDebug>

// StateMachineCondition 实现
QJsonObject StateMachineCondition::toJson() const
{
    QJsonObject json;
    json["conditionId"] = conditionId;
    json["description"] = description;
    json["expression"] = expression;
    json["isEnabled"] = isEnabled;
    return json;
}

bool StateMachineCondition::fromJson(const QJsonObject &json)
{
    if (json.isEmpty()) return false;
    
    conditionId = json["conditionId"].toString();
    description = json["description"].toString();
    expression = json["expression"].toString();
    isEnabled = json["isEnabled"].toBool(true);
    
    return true;
}

// StateMachineTransition 实现
QJsonObject StateMachineTransition::toJson() const
{
    QJsonObject json;
    json["transitionId"] = transitionId;
    json["fromStateId"] = fromStateId;
    json["toStateId"] = toStateId;
    json["eventName"] = eventName;
    json["eventType"] = static_cast<int>(eventType);
    json["actionScript"] = actionScript;
    json["isEnabled"] = isEnabled;
    
    // 新增条件配置字段
    json["eventSource"] = eventSource;
    json["conditionVariable"] = condition.variable;
    json["conditionOperator"] = static_cast<int>(condition.operatorType);
    json["conditionValue"] = condition.value;
    json["conditionLogic"] = static_cast<int>(condition.logic);
    
    QJsonArray conditionsArray;
    for (const auto &condition : conditions) {
        conditionsArray.append(condition.toJson());
    }
    json["conditions"] = conditionsArray;
    
    return json;
}

bool StateMachineTransition::fromJson(const QJsonObject &json)
{
    if (json.isEmpty()) return false;
    
    transitionId = json["transitionId"].toString();
    fromStateId = json["fromStateId"].toString();
    toStateId = json["toStateId"].toString();
    eventName = json["eventName"].toString();
    eventType = static_cast<StateMachineEventType>(json["eventType"].toInt());
    actionScript = json["actionScript"].toString();
    isEnabled = json["isEnabled"].toBool(true);
    
    // 新增条件配置字段
    eventSource = json["eventSource"].toString();
    condition.variable = json["conditionVariable"].toString();
    condition.operatorType = static_cast<ConditionOperator>(json["conditionOperator"].toInt());
    condition.value = json["conditionValue"].toString();
    condition.logic = static_cast<ConditionLogic>(json["conditionLogic"].toInt());
    
    conditions.clear();
    QJsonArray conditionsArray = json["conditions"].toArray();
    for (const auto &conditionValue : conditionsArray) {
        StateMachineCondition condition;
        if (condition.fromJson(conditionValue.toObject())) {
            conditions.append(condition);
        }
    }
    
    return true;
}

// StateMachineState 实现
QJsonObject StateMachineState::toJson() const
{
    QJsonObject json;
    json["stateId"] = stateId;
    json["name"] = name;
    json["description"] = description;
    json["uiInterfaceId"] = uiInterfaceId;
    json["isInitialState"] = isInitialState;
    json["isFinalState"] = isFinalState;
    json["positionX"] = position.x();
    json["positionY"] = position.y();
    return json;
}

bool StateMachineState::fromJson(const QJsonObject &json)
{
    if (json.isEmpty()) return false;
    
    stateId = json["stateId"].toString();
    name = json["name"].toString();
    description = json["description"].toString();
    uiInterfaceId = json["uiInterfaceId"].toString();
    isInitialState = json["isInitialState"].toBool();
    isFinalState = json["isFinalState"].toBool();
    position.setX(json["positionX"].toDouble());
    position.setY(json["positionY"].toDouble());
    
    return true;
}

// StateMachine 实现
StateMachine::StateMachine(const QString &name, QObject *parent)
    : QObject(parent)
    , m_name(name)
{
    m_id = QUuid::createUuid().toString(QUuid::WithoutBraces);
}

StateMachine::~StateMachine()
{
    m_states.clear();
    m_transitions.clear();
}

QString StateMachine::name() const
{
    return m_name;
}

void StateMachine::setName(const QString &name)
{
    if (m_name != name) {
        m_name = name;
    }
}

QString StateMachine::description() const
{
    return m_description;
}

void StateMachine::setDescription(const QString &description)
{
    m_description = description;
}

QString StateMachine::id() const
{
    return m_id;
}

QList<StateMachineState> StateMachine::states() const
{
    return m_states;
}

StateMachineState* StateMachine::findState(const QString &stateId)
{
    return findStateById(stateId);
}

StateMachineState* StateMachine::findStateByUiInterface(const QString &uiInterfaceId)
{
    for (auto &state : m_states) {
        if (state.uiInterfaceId == uiInterfaceId) {
            return &state;
        }
    }
    return nullptr;
}

bool StateMachine::addState(const StateMachineState &state)
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
    }
    
    return true;
}

bool StateMachine::removeState(const QString &stateId)
{
    for (int i = 0; i < m_states.size(); ++i) {
        if (m_states[i].stateId == stateId) {
            // 删除与该状态相关的转换
            for (int j = m_transitions.size() - 1; j >= 0; --j) {
                if (m_transitions[j].fromStateId == stateId || m_transitions[j].toStateId == stateId) {
                    m_transitions.removeAt(j);
                }
            }
            
            // 如果是初始状态，清除初始状态
            if (m_initialStateId == stateId) {
                m_initialStateId.clear();
            }
            
            // 如果是当前状态，清除当前状态
            if (m_currentStateId == stateId) {
                m_currentStateId.clear();
            }
            
            m_states.removeAt(i);
            return true;
        }
    }
    
    return false;
}

bool StateMachine::updateState(const StateMachineState &state)
{
    StateMachineState *existingState = findStateById(state.stateId);
    if (!existingState) {
        return false;
    }
    
    *existingState = state;
    
    // 如果是初始状态，更新初始状态ID
    if (state.isInitialState) {
        m_initialStateId = state.stateId;
    }
    
    return true;
}

StateMachineState* StateMachine::initialState() const
{
    return const_cast<StateMachineState*>(findStateById(m_initialStateId));
}

void StateMachine::setInitialState(const QString &stateId)
{
    StateMachineState *state = findStateById(stateId);
    if (state) {
        // 清除其他状态的初始状态标志
        for (auto &s : m_states) {
            s.isInitialState = (s.stateId == stateId);
        }
        m_initialStateId = stateId;
    }
}

QList<StateMachineTransition> StateMachine::transitions() const
{
    return m_transitions;
}

QList<StateMachineTransition> StateMachine::transitionsFromState(const QString &stateId) const
{
    QList<StateMachineTransition> result;
    for (const auto &transition : m_transitions) {
        if (transition.fromStateId == stateId && transition.isEnabled) {
            result.append(transition);
        }
    }
    return result;
}

bool StateMachine::addTransition(const StateMachineTransition &transition)
{
    if (transition.transitionId.isEmpty()) {
        qWarning() << "Cannot add transition with empty transitionId";
        return false;
    }
    
    if (findTransitionById(transition.transitionId)) {
        qWarning() << "Transition with id" << transition.transitionId << "already exists";
        return false;
    }
    
    // 验证源状态和目标状态是否存在
    if (!findStateById(transition.fromStateId) || !findStateById(transition.toStateId)) {
        qWarning() << "Invalid transition: source or target state does not exist";
        return false;
    }
    
    m_transitions.append(transition);
    return true;
}

bool StateMachine::removeTransition(const QString &transitionId)
{
    for (int i = 0; i < m_transitions.size(); ++i) {
        if (m_transitions[i].transitionId == transitionId) {
            m_transitions.removeAt(i);
            return true;
        }
    }
    
    return false;
}

bool StateMachine::updateTransition(const StateMachineTransition &transition)
{
    StateMachineTransition *existingTransition = findTransitionById(transition.transitionId);
    if (!existingTransition) {
        return false;
    }
    
    *existingTransition = transition;
    return true;
}

StateMachineState* StateMachine::currentState() const
{
    return const_cast<StateMachineState*>(findStateById(m_currentStateId));
}

bool StateMachine::setCurrentState(const QString &stateId)
{
    StateMachineState *state = findStateById(stateId);
    if (!state) {
        return false;
    }
    
    StateMachineState *oldState = currentState();
    m_currentStateId = stateId;
    
    emit stateChanged(oldState ? *oldState : StateMachineState(), *state);
    return true;
}

bool StateMachine::processEvent(const QString &eventName, StateMachineEventType eventType, const QVariantMap &context)
{
    StateMachineState *currentState = this->currentState();
    if (!currentState) {
        // 如果没有当前状态，尝试设置为初始状态
        if (!m_initialStateId.isEmpty()) {
            setCurrentState(m_initialStateId);
            currentState = this->currentState();
        }
        if (!currentState) {
            qWarning() << "No current state and no initial state set";
            return false;
        }
    }
    
    // 查找匹配的转换
    for (const auto &transition : transitionsFromState(currentState->stateId)) {
        if (transition.eventName == eventName && transition.eventType == eventType) {
            // 检查条件
            bool allConditionsMet = true;
            for (const auto &condition : transition.conditions) {
                if (condition.isEnabled) {
                    // 这里可以实现条件评估逻辑
                    // 目前简单实现：所有条件都满足
                    if (!condition.expression.isEmpty()) {
                        // 可以集成表达式引擎来评估条件
                        // 暂时返回false表示需要实现条件评估
                        allConditionsMet = false;
                        break;
                    }
                }
            }
            
            if (allConditionsMet) {
                // 执行转换
                StateMachineState *targetState = findStateById(transition.toStateId);
                if (targetState) {
                    // 执行转换动作脚本（如果有）
                    if (!transition.actionScript.isEmpty()) {
                        // 这里可以实现脚本执行逻辑
                        qDebug() << "Executing action script:" << transition.actionScript;
                    }
                    
                    // 切换到目标状态
                    setCurrentState(targetState->stateId);
                    emit transitionTriggered(transition);
                    emit eventProcessed(eventName, true);
                    return true;
                }
            }
        }
    }
    
    emit eventProcessed(eventName, false);
    return false;
}

QJsonObject StateMachine::toJson() const
{
    QJsonObject json;
    json["id"] = m_id;
    json["name"] = m_name;
    json["description"] = m_description;
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

bool StateMachine::fromJson(const QJsonObject &json)
{
    if (json.isEmpty()) return false;
    
    m_id = json["id"].toString();
    m_name = json["name"].toString();
    m_description = json["description"].toString();
    m_initialStateId = json["initialStateId"].toString();
    m_currentStateId = json["currentStateId"].toString();
    
    m_states.clear();
    QJsonArray statesArray = json["states"].toArray();
    for (const auto &stateValue : statesArray) {
        StateMachineState state;
        if (state.fromJson(stateValue.toObject())) {
            m_states.append(state);
        }
    }
    
    m_transitions.clear();
    QJsonArray transitionsArray = json["transitions"].toArray();
    for (const auto &transitionValue : transitionsArray) {
        StateMachineTransition transition;
        if (transition.fromJson(transitionValue.toObject())) {
            m_transitions.append(transition);
        }
    }
    
    return true;
}

bool StateMachine::saveToFile(const QString &filePath)
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

bool StateMachine::loadFromFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot open file for reading:" << filePath;
        return false;
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    
    return fromJson(doc.object());
}

bool StateMachine::validate() const
{
    // 验证状态机的基本规则
    // 1. 必须有至少一个状态
    if (m_states.isEmpty()) {
        return false;
    }
    
    // 2. 必须有且只有一个初始状态
    int initialStates = 0;
    for (const auto &state : m_states) {
        if (state.isInitialState) {
            initialStates++;
        }
    }
    if (initialStates != 1) {
        return false;
    }
    
    // 3. 所有转换的源状态和目标状态必须存在
    for (const auto &transition : m_transitions) {
        if (!findStateById(transition.fromStateId) || !findStateById(transition.toStateId)) {
            return false;
        }
    }
    
    return true;
}

QList<QString> StateMachine::validationErrors() const
{
    QList<QString> errors;
    
    if (m_states.isEmpty()) {
        errors.append("状态机必须包含至少一个状态");
    }
    
    int initialStates = 0;
    for (const auto &state : m_states) {
        if (state.isInitialState) {
            initialStates++;
        }
    }
    
    if (initialStates == 0) {
        errors.append("状态机必须包含一个初始状态");
    } else if (initialStates > 1) {
        errors.append("状态机只能包含一个初始状态");
    }
    
    for (const auto &transition : m_transitions) {
        if (!findStateById(transition.fromStateId)) {
            errors.append(QString("转换 %1 的源状态 %2 不存在").arg(transition.transitionId).arg(transition.fromStateId));
        }
        if (!findStateById(transition.toStateId)) {
            errors.append(QString("转换 %1 的目标状态 %2 不存在").arg(transition.transitionId).arg(transition.toStateId));
        }
    }
    
    return errors;
}

// 私有方法实现
StateMachineState* StateMachine::findStateById(const QString &stateId)
{
    for (auto &state : m_states) {
        if (state.stateId == stateId) {
            return &state;
        }
    }
    return nullptr;
}

const StateMachineState* StateMachine::findStateById(const QString &stateId) const
{
    for (const auto &state : m_states) {
        if (state.stateId == stateId) {
            return &state;
        }
    }
    return nullptr;
}

StateMachineTransition* StateMachine::findTransitionById(const QString &transitionId)
{
    for (auto &transition : m_transitions) {
        if (transition.transitionId == transitionId) {
            return &transition;
        }
    }
    return nullptr;
}

// StateMachineManager 实现
StateMachineManager::StateMachineManager(QObject *parent)
    : QObject(parent)
    , m_currentStateMachine(nullptr)
    , m_uiInterfaceManager(nullptr)
{
}

StateMachineManager::~StateMachineManager()
{
    qDeleteAll(m_stateMachines);
    m_stateMachines.clear();
}

QList<StateMachine*> StateMachineManager::stateMachines() const
{
    return m_stateMachines;
}

StateMachine* StateMachineManager::currentStateMachine() const
{
    return m_currentStateMachine;
}

void StateMachineManager::setCurrentStateMachine(StateMachine *stateMachine)
{
    if (m_currentStateMachine != stateMachine) {
        m_currentStateMachine = stateMachine;
        emit currentStateMachineChanged(stateMachine);
    }
}

StateMachine* StateMachineManager::createStateMachine(const QString &name)
{
    StateMachine *stateMachine = new StateMachine(name, this);
    m_stateMachines.append(stateMachine);
    emit stateMachineAdded(stateMachine);
    return stateMachine;
}

bool StateMachineManager::removeStateMachine(StateMachine *stateMachine)
{
    if (m_stateMachines.contains(stateMachine)) {
        m_stateMachines.removeOne(stateMachine);
        if (m_currentStateMachine == stateMachine) {
            m_currentStateMachine = nullptr;
        }
        emit stateMachineRemoved(stateMachine);
        delete stateMachine;
        return true;
    }
    return false;
}

StateMachine* StateMachineManager::findStateMachine(const QString &stateMachineId) const
{
    for (auto *stateMachine : m_stateMachines) {
        if (stateMachine->id() == stateMachineId) {
            return stateMachine;
        }
    }
    return nullptr;
}

void StateMachineManager::setUiInterfaceManager(UIInterfaceManager *manager)
{
    m_uiInterfaceManager = manager;
}

UIInterfaceManager* StateMachineManager::uiInterfaceManager() const
{
    return m_uiInterfaceManager;
}

bool StateMachineManager::linkStateToUiInterface(const QString &stateId, const QString &uiInterfaceId)
{
    if (!m_currentStateMachine) {
        return false;
    }
    
    StateMachineState *state = m_currentStateMachine->findState(stateId);
    if (!state) {
        return false;
    }
    
    state->uiInterfaceId = uiInterfaceId;
    return true;
}

QString StateMachineManager::getUiInterfaceForState(const QString &stateId) const
{
    if (!m_currentStateMachine) {
        return QString();
    }
    
    StateMachineState *state = m_currentStateMachine->findState(stateId);
    return state ? state->uiInterfaceId : QString();
}

QString StateMachineManager::getStateForUiInterface(const QString &uiInterfaceId) const
{
    if (!m_currentStateMachine) {
        return QString();
    }
    
    StateMachineState *state = m_currentStateMachine->findStateByUiInterface(uiInterfaceId);
    return state ? state->stateId : QString();
}

QJsonObject StateMachineManager::toJson() const
{
    QJsonObject json;
    
    QJsonArray stateMachinesArray;
    for (const auto *stateMachine : m_stateMachines) {
        stateMachinesArray.append(stateMachine->toJson());
    }
    json["stateMachines"] = stateMachinesArray;
    
    if (m_currentStateMachine) {
        json["currentStateMachineId"] = m_currentStateMachine->id();
    }
    
    return json;
}

bool StateMachineManager::fromJson(const QJsonObject &json)
{
    if (json.isEmpty()) return false;
    
    qDeleteAll(m_stateMachines);
    m_stateMachines.clear();
    
    QJsonArray stateMachinesArray = json["stateMachines"].toArray();
    for (const auto &stateMachineValue : stateMachinesArray) {
        StateMachine *stateMachine = new StateMachine("", this);
        if (stateMachine->fromJson(stateMachineValue.toObject())) {
            m_stateMachines.append(stateMachine);
        } else {
            delete stateMachine;
        }
    }
    
    QString currentStateMachineId = json["currentStateMachineId"].toString();
    if (!currentStateMachineId.isEmpty()) {
        m_currentStateMachine = findStateMachine(currentStateMachineId);
    }
    
    return true;
}

bool StateMachineManager::saveToFile(const QString &filePath)
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

bool StateMachineManager::loadFromFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot open file for reading:" << filePath;
        return false;
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    
    return fromJson(doc.object());
}