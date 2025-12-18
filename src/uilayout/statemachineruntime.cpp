#include "statemachineruntime.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QMessageBox>
#include <QStackedWidget>
#include <QTimer>
#include <QDebug>

// StateMachineRuntime 实现
StateMachineRuntime::StateMachineRuntime(QObject *parent)
    : QObject(parent)
    , m_stateMachine(nullptr)
    , m_isRunning(false)
    , m_currentStateId("")
    , m_interfaceContainer(new QStackedWidget())
    , m_timer(new QTimer(this))
{
    connect(m_timer, &QTimer::timeout, this, &StateMachineRuntime::onTimerTriggered);
}

StateMachineRuntime::~StateMachineRuntime()
{
    stop();
    cleanupRuntime();
    delete m_interfaceContainer;
}

void StateMachineRuntime::setStateMachine(StateMachine *stateMachine)
{
    if (m_stateMachine != stateMachine) {
        if (m_isRunning) {
            stop();
        }
        m_stateMachine = stateMachine;
        cleanupRuntime();
    }
}



void StateMachineRuntime::setProductUiFiles(const QList<ProductUIFile> &uiFiles)
{
    m_productUiFiles = uiFiles;
}

bool StateMachineRuntime::start()
{
    if (m_isRunning) {
        return true;
    }
    
    if (!m_stateMachine) {
        emit runtimeError("没有设置状态机");
        return false;
    }
    
    if (!initializeRuntime()) {
        emit runtimeError("运行时初始化失败");
        return false;
    }
    
    m_isRunning = true;
    
    // 切换到初始状态
    StateMachineState *initialState = m_stateMachine->initialState();
    if (!initialState || initialState->stateId.isEmpty()) {
        emit runtimeError("状态机没有设置初始状态");
        m_isRunning = false;
        return false;
    }
    
    if (!switchToState(initialState->stateId)) {
        emit runtimeError("无法切换到初始状态: " + initialState->stateId);
        m_isRunning = false;
        return false;
    }
    
    emit runtimeStarted();
    return true;
}

void StateMachineRuntime::stop()
{
    if (!m_isRunning) {
        return;
    }
    
    m_isRunning = false;
    m_timer->stop();
    cleanupRuntime();
    emit runtimeStopped();
}

void StateMachineRuntime::reset()
{
    stop();
    start();
}

bool StateMachineRuntime::isRunning() const
{
    return m_isRunning;
}

QString StateMachineRuntime::currentStateId() const
{
    return m_currentStateId;
}

QString StateMachineRuntime::currentStateName() const
{
    if (!m_stateMachine || m_currentStateId.isEmpty()) {
        return "";
    }
    
    StateMachineState *state = m_stateMachine->findState(m_currentStateId);
    return state ? state->name : "";
}

QWidget* StateMachineRuntime::currentInterface() const
{
    return m_interfaceContainer->currentWidget();
}

bool StateMachineRuntime::triggerEvent(const QString &eventName)
{
    return triggerEvent(eventName, QVariant());
}

bool StateMachineRuntime::triggerEvent(const QString &eventName, const QVariant &data)
{
    if (!m_isRunning || m_currentStateId.isEmpty()) {
        emit eventTriggered(eventName, false);
        return false;
    }
    
    // 查找从当前状态出发的转换
    QList<StateMachineTransition> transitions = m_stateMachine->transitionsFromState(m_currentStateId);
    
    for (const auto &transition : transitions) {
        if (transition.eventName == eventName) {
            // 检查条件
            bool conditionsMet = true;
            for (const auto &condition : transition.conditions) {
                if (condition.isEnabled && !condition.expression.isEmpty()) {
                    // 这里可以添加条件表达式求值逻辑
                    // 暂时简单处理：只要表达式不为空就认为条件满足
                }
            }
            
            if (conditionsMet) {
                // 执行动作脚本
                if (!transition.actionScript.isEmpty()) {
                    if (!executeActionScript(transition.actionScript)) {
                        qWarning() << "执行动作脚本失败:" << transition.actionScript;
                    }
                }
                
                // 切换到目标状态
                if (switchToState(transition.toStateId)) {
                    emit eventTriggered(eventName, true);
                    return true;
                }
            }
        }
    }
    
    emit eventTriggered(eventName, false);
    return false;
}

void StateMachineRuntime::onTimerTriggered()
{
    // 定时器触发事件处理
    triggerEvent("timer_triggered");
}



bool StateMachineRuntime::initializeRuntime()
{
    if (!m_stateMachine) {
        return false;
    }
    
    // 清空缓存
    cleanupRuntime();
    
    // 构建事件转换映射
    m_eventTransitions.clear();
    QList<StateMachineTransition> transitions = m_stateMachine->transitions();
    for (const auto &transition : transitions) {
        m_eventTransitions[transition.eventName].append(transition);
    }
    
    return true;
}

void StateMachineRuntime::cleanupRuntime()
{
    // 断开所有UI接口信号连接
    for (auto *interface : m_interfaceCache) {
        disconnectUIInterfaceSignals(interface);
    }
    
    // 清空接口容器
    while (m_interfaceContainer->count() > 0) {
        QWidget *widget = m_interfaceContainer->widget(0);
        m_interfaceContainer->removeWidget(widget);
    }
    
    // 清空缓存
    m_interfaceCache.clear();
    m_currentStateId = "";
}

bool StateMachineRuntime::switchToState(const QString &stateId)
{
    if (!m_stateMachine || !m_isRunning) {
        return false;
    }
    
    StateMachineState *state = m_stateMachine->findState(stateId);
    if (!state || state->stateId.isEmpty()) {
        qWarning() << "状态不存在:" << stateId;
        return false;
    }
    
    QString fromStateId = m_currentStateId;
    m_currentStateId = stateId;
    
    // 获取或创建UI接口
    QWidget *interface = m_interfaceCache.value(stateId, nullptr);
    if (!interface) {
        interface = createUIInterfaceForState(*state);
        if (interface) {
            m_interfaceCache[stateId] = interface;
            m_interfaceContainer->addWidget(interface);
            connectUIInterfaceSignals(interface);
        }
    }
    
    if (interface) {
        // 显示对应的UI界面
        int index = m_interfaceContainer->indexOf(interface);
        if (index >= 0) {
            m_interfaceContainer->setCurrentIndex(index);
        }
    }
    
    emit stateChanged(fromStateId, stateId);
    return true;
}

bool StateMachineRuntime::executeActionScript(const QString &script)
{
    // 这里可以实现动作脚本执行逻辑
    // 暂时简单处理：输出脚本内容
    qDebug() << "Executing action script:" << script;
    return true;
}

// 按钮点击事件处理
void StateMachineRuntime::onButtonClicked(const QString &buttonName)
{
    qDebug() << "StateMachineRuntime: Received button click event:" << buttonName;
    
    if (!m_isRunning || !m_stateMachine) {
        qWarning() << "运行时未启动或状态机未设置";
        return;
    }
    
    // 触发按钮点击事件
    QVariantMap context;
    context["buttonName"] = buttonName;
    context["currentState"] = m_currentStateId;
    
    bool success = m_stateMachine->processEvent(buttonName, StateMachineEventType::ButtonClick, context);
    
    if (success) {
        qDebug() << "Button click event processed successfully, current state:" << m_stateMachine->currentState()->name;
    } else {
        qWarning() << "按钮点击事件处理失败";
    }
    
    emit eventTriggered(buttonName, success);
}

// 菜单操作事件处理
void StateMachineRuntime::onMenuActionTriggered(const QString &actionName)
{
    qDebug() << "StateMachineRuntime: Received menu action event:" << actionName;
    
    if (!m_isRunning || !m_stateMachine) {
        qWarning() << "运行时未启动或状态机未设置";
        return;
    }
    
    // 触发菜单操作事件
    QVariantMap context;
    context["actionName"] = actionName;
    context["currentState"] = m_currentStateId;
    
    bool success = m_stateMachine->processEvent(actionName, StateMachineEventType::MenuAction, context);
    
    if (success) {
        qDebug() << "Menu action event processed successfully, current state:" << m_stateMachine->currentState()->name;
    } else {
        qWarning() << "菜单操作事件处理失败";
    }
    
    emit eventTriggered(actionName, success);
}

void StateMachineRuntime::connectUIInterfaceSignals(QWidget *interface)
{
    if (!interface) return;
    
    // 查找界面中的所有按钮并连接点击事件
    QList<QPushButton*> buttons = interface->findChildren<QPushButton*>();
    for (QPushButton *button : buttons) {
        connect(button, &QPushButton::clicked, this, [this, button]() {
            onButtonClicked(button->text());
        });
    }
    
    qDebug() << "StateMachineRuntime: UI interface signals connected";
}

void StateMachineRuntime::disconnectUIInterfaceSignals(QWidget *interface)
{
    if (!interface) return;
    
    // 断开界面中所有按钮的信号连接
    QList<QPushButton*> buttons = interface->findChildren<QPushButton*>();
    for (QPushButton *button : buttons) {
        disconnect(button, &QPushButton::clicked, this, nullptr);
    }
    
    qDebug() << "StateMachineRuntime: UI interface signals disconnected";
}

QWidget* StateMachineRuntime::createUIInterfaceForState(const StateMachineState &state)
{
    // 从产品UI文件创建界面
    for (const auto &uiFile : m_productUiFiles) {
        if (uiFile.filePath == state.uiInterfaceId) {
            // 这里可以实现从UI文件创建界面的逻辑
            // 暂时创建一个简单的占位界面
            QWidget *widget = new QWidget();
            QVBoxLayout *layout = new QVBoxLayout(widget);
            QLabel *label = new QLabel(uiFile.name + " - " + state.name);
            layout->addWidget(label);
            
            // 添加一些示例按钮用于测试
            QPushButton *testButton = new QPushButton("测试按钮");
            layout->addWidget(testButton);
            
            qDebug() << "Creating UI interface from product UI file:" << uiFile.name << uiFile.filePath;
            return widget;
        }
    }
    
    // 如果没有找到对应的UI文件，创建一个默认界面
    QWidget *widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(widget);
    QLabel *label = new QLabel("默认界面 - " + state.name);
    layout->addWidget(label);
    
    return widget;
}

// StateMachineRuntimeView 实现
StateMachineRuntimeView::StateMachineRuntimeView(QWidget *parent)
    : QWidget(parent)
    , m_runtime(nullptr)
    , m_editor(nullptr)
    , m_startButton(new QPushButton("启动运行时"))
    , m_stopButton(new QPushButton("停止运行时"))
    , m_resetButton(new QPushButton("重置运行时"))
    , m_statusLabel(new QLabel("状态: 未启动"))
    , m_currentStateLabel(new QLabel("当前状态: 无"))
    , m_eventCombo(new QComboBox())
    , m_triggerEventButton(new QPushButton("触发事件"))
    , m_interfaceContainer(new QWidget())
{
    createControls();
    updateControls();
}

void StateMachineRuntimeView::setRuntime(StateMachineRuntime *runtime)
{
    if (m_runtime) {
        disconnect(m_runtime, &StateMachineRuntime::stateChanged, this, &StateMachineRuntimeView::onStateChanged);
        disconnect(m_runtime, &StateMachineRuntime::runtimeStarted, this, &StateMachineRuntimeView::onRuntimeStarted);
        disconnect(m_runtime, &StateMachineRuntime::runtimeStopped, this, &StateMachineRuntimeView::onRuntimeStopped);
        disconnect(m_runtime, &StateMachineRuntime::runtimeError, this, &StateMachineRuntimeView::onRuntimeError);
    }
    
    m_runtime = runtime;
    
    if (m_runtime) {
        connect(m_runtime, &StateMachineRuntime::stateChanged, this, &StateMachineRuntimeView::onStateChanged);
        connect(m_runtime, &StateMachineRuntime::runtimeStarted, this, &StateMachineRuntimeView::onRuntimeStarted);
        connect(m_runtime, &StateMachineRuntime::runtimeStopped, this, &StateMachineRuntimeView::onRuntimeStopped);
        connect(m_runtime, &StateMachineRuntime::runtimeError, this, &StateMachineRuntimeView::onRuntimeError);
    }
    
    updateControls();
}

void StateMachineRuntimeView::setStateMachineEditor(StateMachineEditor *editor)
{
    m_editor = editor;
}

void StateMachineRuntimeView::startRuntime()
{
    if (m_runtime && m_runtime->start()) {
        updateControls();
    }
}

void StateMachineRuntimeView::stopRuntime()
{
    if (m_runtime) {
        m_runtime->stop();
        updateControls();
    }
}

void StateMachineRuntimeView::resetRuntime()
{
    if (m_runtime) {
        m_runtime->reset();
        updateControls();
    }
}

void StateMachineRuntimeView::triggerEvent(const QString &eventName)
{
    if (m_runtime) {
        m_runtime->triggerEvent(eventName);
    }
}

void StateMachineRuntimeView::onStateChanged(const QString &fromStateId, const QString &toStateId)
{
    Q_UNUSED(fromStateId)
    
    if (m_runtime) {
        m_currentStateLabel->setText("当前状态: " + m_runtime->currentStateName());
        updateInterfaceDisplay();
    }
}

void StateMachineRuntimeView::onRuntimeStarted()
{
    m_statusLabel->setText("状态: 运行中");
    updateControls();
}

void StateMachineRuntimeView::onRuntimeStopped()
{
    m_statusLabel->setText("状态: 已停止");
    m_currentStateLabel->setText("当前状态: 无");
    updateControls();
}

void StateMachineRuntimeView::onRuntimeError(const QString &errorMessage)
{
    QMessageBox::warning(this, "运行时错误", errorMessage);
}

void StateMachineRuntimeView::createControls()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // 控制面板
    QGroupBox *controlGroup = new QGroupBox("运行时控制");
    QHBoxLayout *controlLayout = new QHBoxLayout(controlGroup);
    
    controlLayout->addWidget(m_startButton);
    controlLayout->addWidget(m_stopButton);
    controlLayout->addWidget(m_resetButton);
    controlLayout->addWidget(m_statusLabel);
    controlLayout->addWidget(m_currentStateLabel);
    
    // 事件触发面板
    QGroupBox *eventGroup = new QGroupBox("事件触发");
    QHBoxLayout *eventLayout = new QHBoxLayout(eventGroup);
    
    m_eventCombo->setEditable(true);
    m_eventCombo->addItems({"button_click", "menu_action", "timer_triggered"});
    
    eventLayout->addWidget(new QLabel("事件名称:"));
    eventLayout->addWidget(m_eventCombo);
    eventLayout->addWidget(m_triggerEventButton);
    
    // 界面容器
    QGroupBox *interfaceGroup = new QGroupBox("界面预览");
    QVBoxLayout *interfaceLayout = new QVBoxLayout(interfaceGroup);
    
    // 创建界面预览容器
    m_interfaceContainer = new QWidget();
    QVBoxLayout *containerLayout = new QVBoxLayout(m_interfaceContainer);
    
    // 初始显示提示信息
    QLabel *placeholderLabel = new QLabel("运行时未启动或没有可显示的界面");
    placeholderLabel->setAlignment(Qt::AlignCenter);
    placeholderLabel->setStyleSheet("color: gray; font-style: italic; padding: 20px;");
    containerLayout->addWidget(placeholderLabel);
    
    interfaceLayout->addWidget(m_interfaceContainer);
    
    mainLayout->addWidget(controlGroup);
    mainLayout->addWidget(eventGroup);
    mainLayout->addWidget(interfaceGroup);
    
    // 连接信号
    connect(m_startButton, &QPushButton::clicked, this, &StateMachineRuntimeView::startRuntime);
    connect(m_stopButton, &QPushButton::clicked, this, &StateMachineRuntimeView::stopRuntime);
    connect(m_resetButton, &QPushButton::clicked, this, &StateMachineRuntimeView::resetRuntime);
    connect(m_triggerEventButton, &QPushButton::clicked, this, [this]() {
        triggerEvent(m_eventCombo->currentText());
    });
}

void StateMachineRuntimeView::updateInterfaceDisplay()
{
    if (!m_runtime) {
        return;
    }
    
    // 清空当前界面容器 - 使用Qt自动清理机制
    QLayout *existingLayout = m_interfaceContainer->layout();
    if (existingLayout) {
        QLayoutItem *item;
        while ((item = existingLayout->takeAt(0)) != nullptr) {
            if (item->widget()) {
                item->widget()->deleteLater();
            }
            delete item;
        }
        delete existingLayout;
    }
    
    // 获取当前界面
    QWidget *currentInterface = m_runtime->currentInterface();
    if (currentInterface) {
        // 显示当前界面
        QVBoxLayout *containerLayout = new QVBoxLayout(m_interfaceContainer);
        containerLayout->addWidget(currentInterface);
    } else {
        // 显示提示信息
        QLabel *placeholderLabel = new QLabel("当前状态没有可显示的界面");
        placeholderLabel->setAlignment(Qt::AlignCenter);
        placeholderLabel->setStyleSheet("color: gray; font-style: italic; padding: 20px;");
        QVBoxLayout *containerLayout = new QVBoxLayout(m_interfaceContainer);
        containerLayout->addWidget(placeholderLabel);
    }
}

void StateMachineRuntimeView::updateControls()
{
    bool isRunning = m_runtime && m_runtime->isRunning();
    
    m_startButton->setEnabled(!isRunning);
    m_stopButton->setEnabled(isRunning);
    m_resetButton->setEnabled(isRunning);
    m_eventCombo->setEnabled(isRunning);
    m_triggerEventButton->setEnabled(isRunning);
}