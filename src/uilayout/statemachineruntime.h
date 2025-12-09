#ifndef STATEMACHINERUNTIME_H
#define STATEMACHINERUNTIME_H

#include <QObject>
#include <QWidget>
#include <QStackedWidget>
#include <QMap>
#include <QTimer>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QGroupBox>
#include <QMessageBox>
#include "statemachine.h"
#include "product/product.h"

// 前向声明
class StateMachineEditor;

// 状态机运行时引擎
class StateMachineRuntime : public QObject
{
    Q_OBJECT

public:
    explicit StateMachineRuntime(QObject *parent = nullptr);
    ~StateMachineRuntime();

    // 设置状态机和产品UI文件
    void setStateMachine(StateMachine *stateMachine);
    void setProductUiFiles(const QList<ProductUIFile> &uiFiles);

    // 运行时控制
    bool start();
    void stop();
    void reset();
    bool isRunning() const;

    // 状态管理
    QString currentStateId() const;
    QString currentStateName() const;
    QWidget* currentInterface() const;

    // 事件触发
    bool triggerEvent(const QString &eventName);
    bool triggerEvent(const QString &eventName, const QVariant &data);

signals:
    void stateChanged(const QString &fromStateId, const QString &toStateId);
    void eventTriggered(const QString &eventName, bool success);
    void runtimeStarted();
    void runtimeStopped();
    void runtimeError(const QString &errorMessage);

private slots:
    void onTimerTriggered();
    void onButtonClicked(const QString &buttonName);
    void onMenuActionTriggered(const QString &actionName);

private:
    // 内部方法
    bool initializeRuntime();
    void cleanupRuntime();
    bool switchToState(const QString &stateId);
    bool executeActionScript(const QString &script);
    void connectUIInterfaceSignals(QWidget *interface);
    void disconnectUIInterfaceSignals(QWidget *interface);
    QWidget* createUIInterfaceForState(const StateMachineState &state);
    
    // 成员变量
    StateMachine *m_stateMachine;
    QList<ProductUIFile> m_productUiFiles;
    
    // 运行时状态
    bool m_isRunning;
    QString m_currentStateId;
    QMap<QString, QWidget*> m_interfaceCache;
    QStackedWidget *m_interfaceContainer;
    QTimer *m_timer;
    
    // 事件处理
    QMap<QString, QList<StateMachineTransition>> m_eventTransitions;
};

// 状态机运行时预览窗口
class StateMachineRuntimeView : public QWidget
{
    Q_OBJECT

public:
    explicit StateMachineRuntimeView(QWidget *parent = nullptr);
    
    void setRuntime(StateMachineRuntime *runtime);
    void setStateMachineEditor(StateMachineEditor *editor);

public slots:
    void startRuntime();
    void stopRuntime();
    void resetRuntime();
    void triggerEvent(const QString &eventName);

private slots:
    void onStateChanged(const QString &fromStateId, const QString &toStateId);
    void onRuntimeStarted();
    void onRuntimeStopped();
    void onRuntimeError(const QString &errorMessage);

private:
    void createControls();
    void updateControls();
    void updateInterfaceDisplay();
    
    StateMachineRuntime *m_runtime;
    StateMachineEditor *m_editor;
    
    // UI控件
    QPushButton *m_startButton;
    QPushButton *m_stopButton;
    QPushButton *m_resetButton;
    QLabel *m_statusLabel;
    QLabel *m_currentStateLabel;
    QComboBox *m_eventCombo;
    QPushButton *m_triggerEventButton;
    QWidget *m_interfaceContainer;
};

#endif // STATEMACHINERUNTIME_H