#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

#include "../src/uilayout/statemachineeditor_v2.h"
#include "../src/uilayout/uiflowstatemachine.h"
#include "../src/uilayout/logicsequencestatemachine.h"
#include "../src/uilayout/statemachineintegrationmanager.h"

class StateMachineTestWindow : public QWidget
{
    Q_OBJECT

public:
    StateMachineTestWindow(QWidget *parent = nullptr)
        : QWidget(parent)
        , m_editor(nullptr)
        , m_uiFlowStateMachine(nullptr)
        , m_logicStateMachine(nullptr)
        , m_integrationManager(nullptr)
    {
        setupUI();
        setupStateMachines();
        connectSignals();
    }

private slots:
    void testUIFlowMode()
    {
        qDebug() << "=== 测试UI流模式 ===";
        
        // 切换到UI流模式
        m_editor->switchToUIFlowMode();
        
        // 创建测试状态
        UIFlowStateMachine::UIFlowState state1;
        state1.stateId = "state1";
        state1.name = "主界面";
        state1.description = "应用程序主界面";
        state1.uiInterfaceId = "main_window";
        state1.isInitialState = true;
        
        UIFlowStateMachine::UIFlowState state2;
        state2.stateId = "state2";
        state2.name = "设置界面";
        state2.description = "应用程序设置界面";
        state2.uiInterfaceId = "settings_window";
        
        UIFlowStateMachine::UIFlowState state3;
        state3.stateId = "state3";
        state3.name = "关于界面";
        state3.description = "应用程序关于界面";
        state3.uiInterfaceId = "about_window";
        state3.isFinalState = true;
        
        // 添加状态
        m_uiFlowStateMachine->addState(state1);
        m_uiFlowStateMachine->addState(state2);
        m_uiFlowStateMachine->addState(state3);
        
        // 创建转换
        UIFlowStateMachine::UIFlowTransition transition1;
        transition1.transitionId = "trans1";
        transition1.fromStateId = "state1";
        transition1.toStateId = "state2";
        transition1.eventType = "UI_EVENT";
        transition1.eventName = "点击设置按钮";
        
        UIFlowStateMachine::UIFlowTransition transition2;
        transition2.transitionId = "trans2";
        transition2.fromStateId = "state2";
        transition2.toStateId = "state3";
        transition2.eventType = "UI_EVENT";
        transition2.eventName = "点击关于按钮";
        
        UIFlowStateMachine::UIFlowTransition transition3;
        transition3.transitionId = "trans3";
        transition3.fromStateId = "state2";
        transition3.toStateId = "state1";
        transition3.eventType = "UI_EVENT";
        transition3.eventName = "点击返回按钮";
        
        // 添加转换
        m_uiFlowStateMachine->addTransition(transition1);
        m_uiFlowStateMachine->addTransition(transition2);
        m_uiFlowStateMachine->addTransition(transition3);
        
        // 验证状态机
        bool valid = m_uiFlowStateMachine->validate();
        qDebug() << "UI流状态机验证结果:" << (valid ? "通过" : "失败");
        
        // 测试状态切换
        QString currentState = m_uiFlowStateMachine->currentStateId();
        qDebug() << "当前状态:" << currentState;
        
        // 触发转换
        bool success = m_uiFlowStateMachine->triggerEvent("点击设置按钮");
        qDebug() << "触发'点击设置按钮'事件:" << (success ? "成功" : "失败");
        
        currentState = m_uiFlowStateMachine->currentStateId();
        qDebug() << "当前状态:" << currentState;
        
        QMessageBox::information(this, "UI流模式测试", 
            QString("UI流模式测试完成！\n"
                   "状态机验证: %1\n"
                   "最终状态: %2")
            .arg(valid ? "通过" : "失败")
            .arg(currentState));
    }
    
    void testLogicSequenceMode()
    {
        qDebug() << "=== 测试逻辑时序模式 ===";
        
        // 切换到逻辑时序模式
        m_editor->switchToLogicSequenceMode();
        
        // 创建测试状态
        LogicSequenceStateMachine::LogicState state1;
        state1.stateId = "logic_state1";
        state1.name = "初始化";
        state1.description = "系统初始化状态";
        state1.stateType = "INIT";
        state1.stateData = "{\"config\": \"default\"}";
        state1.isInitialState = true;
        
        LogicSequenceStateMachine::LogicState state2;
        state2.stateId = "logic_state2";
        state2.name = "数据处理";
        state2.description = "数据处理状态";
        state2.stateType = "PROCESS";
        state2.stateData = "{\"operation\": \"calculate\"}";
        
        LogicSequenceStateMachine::LogicState state3;
        state3.stateId = "logic_state3";
        state3.name = "完成";
        state3.description = "处理完成状态";
        state3.stateType = "FINISH";
        state3.stateData = "{\"result\": \"success\"}";
        state3.isFinalState = true;
        
        // 添加状态
        m_logicStateMachine->addState(state1);
        m_logicStateMachine->addState(state2);
        m_logicStateMachine->addState(state3);
        
        // 创建转换
        LogicSequenceStateMachine::LogicTransition transition1;
        transition1.transitionId = "logic_trans1";
        transition1.fromStateId = "logic_state1";
        transition1.toStateId = "logic_state2";
        transition1.eventType = "SYSTEM_EVENT";
        transition1.eventName = "初始化完成";
        transition1.condition = "true";
        transition1.actionScript = "console.log('初始化完成，开始数据处理');";
        
        LogicSequenceStateMachine::LogicTransition transition2;
        transition2.transitionId = "logic_trans2";
        transition2.fromStateId = "logic_state2";
        transition2.toStateId = "logic_state3";
        transition2.eventType = "SYSTEM_EVENT";
        transition2.eventName = "数据处理完成";
        transition2.condition = "data.processed == true";
        transition2.actionScript = "console.log('数据处理完成，进入完成状态');";
        
        // 添加转换
        m_logicStateMachine->addTransition(transition1);
        m_logicStateMachine->addTransition(transition2);
        
        // 验证状态机
        bool valid = m_logicStateMachine->validate();
        qDebug() << "逻辑时序状态机验证结果:" << (valid ? "通过" : "失败");
        
        // 测试状态切换
        QString currentState = m_logicStateMachine->currentStateId();
        qDebug() << "当前状态:" << currentState;
        
        // 触发转换
        bool success = m_logicStateMachine->triggerEvent("初始化完成");
        qDebug() << "触发'初始化完成'事件:" << (success ? "成功" : "失败");
        
        currentState = m_logicStateMachine->currentStateId();
        qDebug() << "当前状态:" << currentState;
        
        QMessageBox::information(this, "逻辑时序模式测试", 
            QString("逻辑时序模式测试完成！\n"
                   "状态机验证: %1\n"
                   "最终状态: %2")
            .arg(valid ? "通过" : "失败")
            .arg(currentState));
    }
    
    void testIntegratedMode()
    {
        qDebug() << "=== 测试集成模式 ===";
        
        // 切换到集成模式
        m_editor->switchToIntegratedMode();
        
        // 设置集成管理器
        m_integrationManager->setUiFlowStateMachine(m_uiFlowStateMachine);
        m_integrationManager->setLogicSequenceStateMachine(m_logicStateMachine);
        
        // 创建状态映射
        StateMachineIntegrationManager::StateMapping mapping1;
        mapping1.uiFlowStateId = "state1";
        mapping1.logicStateId = "logic_state1";
        mapping1.description = "主界面与初始化状态映射";
        
        StateMachineIntegrationManager::StateMapping mapping2;
        mapping2.uiFlowStateId = "state2";
        mapping2.logicStateId = "logic_state2";
        mapping2.description = "设置界面与数据处理状态映射";
        
        StateMachineIntegrationManager::StateMapping mapping3;
        mapping3.uiFlowStateId = "state3";
        mapping3.logicStateId = "logic_state3";
        mapping3.description = "关于界面与完成状态映射";
        
        // 添加映射
        m_integrationManager->addMapping(mapping1);
        m_integrationManager->addMapping(mapping2);
        m_integrationManager->addMapping(mapping3);
        
        // 测试状态同步
        bool syncSuccess = m_integrationManager->synchronizeStates();
        qDebug() << "状态同步结果:" << (syncSuccess ? "成功" : "失败");
        
        // 测试集成事件处理
        bool success = m_integrationManager->handleEvent("UI_EVENT", "点击设置按钮");
        qDebug() << "处理UI事件'点击设置按钮':" << (success ? "成功" : "失败");
        
        QString uiCurrentState = m_uiFlowStateMachine->currentStateId();
        QString logicCurrentState = m_logicStateMachine->currentStateId();
        
        qDebug() << "UI流当前状态:" << uiCurrentState;
        qDebug() << "逻辑时序当前状态:" << logicCurrentState;
        
        QMessageBox::information(this, "集成模式测试", 
            QString("集成模式测试完成！\n"
                   "状态同步: %1\n"
                   "UI流状态: %2\n"
                   "逻辑状态: %3")
            .arg(syncSuccess ? "成功" : "失败")
            .arg(uiCurrentState)
            .arg(logicCurrentState));
    }
    
    void testSaveLoad()
    {
        qDebug() << "=== 测试保存和加载功能 ===";
        
        // 测试UI流状态机保存
        QJsonObject uiFlowJson = m_uiFlowStateMachine->toJson();
        QJsonDocument uiFlowDoc(uiFlowJson);
        QString uiFlowJsonStr = uiFlowDoc.toJson(QJsonDocument::Indented);
        
        qDebug() << "UI流状态机JSON:";
        qDebug().noquote() << uiFlowJsonStr;
        
        // 测试逻辑时序状态机保存
        QJsonObject logicJson = m_logicStateMachine->toJson();
        QJsonDocument logicDoc(logicJson);
        QString logicJsonStr = logicDoc.toJson(QJsonDocument::Indented);
        
        qDebug() << "逻辑时序状态机JSON:";
        qDebug().noquote() << logicJsonStr;
        
        // 测试集成管理器保存
        QJsonObject integrationJson = m_integrationManager->toJson();
        QJsonDocument integrationDoc(integrationJson);
        QString integrationJsonStr = integrationDoc.toJson(QJsonDocument::Indented);
        
        qDebug() << "集成管理器JSON:";
        qDebug().noquote() << integrationJsonStr;
        
        // 测试加载功能
        UIFlowStateMachine loadedUIFlow;
        loadedUIFlow.fromJson(uiFlowJson);
        
        LogicSequenceStateMachine loadedLogic;
        loadedLogic.fromJson(logicJson);
        
        StateMachineIntegrationManager loadedIntegration;
        loadedIntegration.fromJson(integrationJson);
        
        qDebug() << "加载UI流状态机状态数量:" << loadedUIFlow.states().size();
        qDebug() << "加载逻辑时序状态机状态数量:" << loadedLogic.states().size();
        qDebug() << "加载集成管理器映射数量:" << loadedIntegration.mappings().size();
        
        QMessageBox::information(this, "保存加载测试", 
            QString("保存加载测试完成！\n"
                   "UI流状态: %1\n"
                   "逻辑状态: %2\n"
                   "集成映射: %3")
            .arg(loadedUIFlow.states().size())
            .arg(loadedLogic.states().size())
            .arg(loadedIntegration.mappings().size()));
    }
    
    void testEditorFunctionality()
    {
        qDebug() << "=== 测试编辑器功能 ===";
        
        // 测试模式切换
        m_editor->switchToUIFlowMode();
        qDebug() << "切换到UI流模式";
        
        m_editor->switchToLogicSequenceMode();
        qDebug() << "切换到逻辑时序模式";
        
        m_editor->switchToIntegratedMode();
        qDebug() << "切换到集成模式";
        
        // 测试属性面板
        m_editor->editStateProperties();
        qDebug() << "测试状态属性编辑";
        
        m_editor->editTransitionProperties();
        qDebug() << "测试转换属性编辑";
        
        // 测试验证功能
        m_editor->validateStateMachine();
        qDebug() << "测试状态机验证";
        
        QMessageBox::information(this, "编辑器功能测试", 
            "编辑器功能测试完成！\n"
            "所有基本功能均已测试。");
    }
    
    void runAllTests()
    {
        qDebug() << "开始运行所有状态机测试...";
        
        testUIFlowMode();
        testLogicSequenceMode();
        testIntegratedMode();
        testSaveLoad();
        testEditorFunctionality();
        
        qDebug() << "所有测试完成！";
        
        QMessageBox::information(this, "测试完成", 
            "所有状态机测试已完成！\n"
            "请查看控制台输出了解详细测试结果。");
    }

private:
    void setupUI()
    {
        setWindowTitle("状态机解耦测试");
        setMinimumSize(400, 300);
        
        QVBoxLayout *layout = new QVBoxLayout(this);
        
        // 创建测试按钮
        QPushButton *uiFlowTestBtn = new QPushButton("测试UI流模式", this);
        QPushButton *logicTestBtn = new QPushButton("测试逻辑时序模式", this);
        QPushButton *integratedTestBtn = new QPushButton("测试集成模式", this);
        QPushButton *saveLoadTestBtn = new QPushButton("测试保存加载", this);
        QPushButton *editorTestBtn = new QPushButton("测试编辑器功能", this);
        QPushButton *runAllBtn = new QPushButton("运行所有测试", this);
        
        layout->addWidget(uiFlowTestBtn);
        layout->addWidget(logicTestBtn);
        layout->addWidget(integratedTestBtn);
        layout->addWidget(saveLoadTestBtn);
        layout->addWidget(editorTestBtn);
        layout->addWidget(runAllBtn);
        
        // 连接按钮信号
        connect(uiFlowTestBtn, &QPushButton::clicked, this, &StateMachineTestWindow::testUIFlowMode);
        connect(logicTestBtn, &QPushButton::clicked, this, &StateMachineTestWindow::testLogicSequenceMode);
        connect(integratedTestBtn, &QPushButton::clicked, this, &StateMachineTestWindow::testIntegratedMode);
        connect(saveLoadTestBtn, &QPushButton::clicked, this, &StateMachineTestWindow::testSaveLoad);
        connect(editorTestBtn, &QPushButton::clicked, this, &StateMachineTestWindow::testEditorFunctionality);
        connect(runAllBtn, &QPushButton::clicked, this, &StateMachineTestWindow::runAllTests);
        
        // 创建状态机编辑器
        m_editor = new StateMachineEditorV2(this);
        layout->addWidget(m_editor);
    }
    
    void setupStateMachines()
    {
        // 创建状态机实例
        m_uiFlowStateMachine = new UIFlowStateMachine(this);
        m_logicStateMachine = new LogicSequenceStateMachine(this);
        m_integrationManager = new StateMachineIntegrationManager(this);
        
        // 设置编辑器状态机管理器
        m_editor->setStateMachineManager(nullptr); // 暂时设置为nullptr
        
        // 设置集成管理器
        m_integrationManager->setUiFlowStateMachine(m_uiFlowStateMachine);
        m_integrationManager->setLogicSequenceStateMachine(m_logicStateMachine);
    }
    
    void connectSignals()
    {
        // 连接状态机变化信号
        connect(m_uiFlowStateMachine, &UIFlowStateMachine::stateChanged, 
                this, [this](const QString &stateId) {
                    qDebug() << "UI流状态变化:" << stateId;
                });
        
        connect(m_logicStateMachine, &LogicSequenceStateMachine::stateChanged, 
                this, [this](const QString &stateId) {
                    qDebug() << "逻辑时序状态变化:" << stateId;
                });
        
        connect(m_integrationManager, &StateMachineIntegrationManager::statesSynchronized, 
                this, [this]() {
                    qDebug() << "状态同步完成";
                });
    }

private:
    StateMachineEditorV2 *m_editor;
    UIFlowStateMachine *m_uiFlowStateMachine;
    LogicSequenceStateMachine *m_logicStateMachine;
    StateMachineIntegrationManager *m_integrationManager;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    StateMachineTestWindow window;
    window.show();
    
    return app.exec();
}

#include "statemachine_test.moc"