#ifndef STATEMACHINEEDITOR_V2_H
#define STATEMACHINEEDITOR_V2_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QMenu>
#include <QAction>
#include <QUndoStack>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QScrollBar>
#include <QToolBar>
#include <QGraphicsSceneMouseEvent>
#include <QDockWidget>
#include <QMainWindow>
#include <QList>
#include <QMap>
#include <QPointF>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QGraphicsDropShadowEffect>
#include <QTabWidget>
#include <QGroupBox>
#include <QRadioButton>
#include <QLabel>
#include "statemachine.h"
#include "product.h"
#include "statemachineruntime.h"
#include "uiflowstatemachine.h"
#include "logicsequencestatemachine.h"
#include "statemachineintegrationmanager.h"

// 状态机模式枚举
enum class StateMachineMode {
    UIFlowOnly,           // 仅UI流模式
    LogicSequenceOnly,    // 仅逻辑时序模式
    Integrated            // 集成模式
};

// UI流状态节点
class UIFlowStateNode : public QObject, public QGraphicsRectItem
{
    Q_OBJECT

public:
    explicit UIFlowStateNode(const UIFlowStateMachine::UIFlowState &state, QGraphicsItem *parent = nullptr);
    
    UIFlowStateMachine::UIFlowState state() const;
    void setState(const UIFlowStateMachine::UIFlowState &state);
    
    void updateAppearance();
    void animateSelection();
    void setHighlighted(bool highlighted);
    QPointF getConnectionPoint(const QPointF &targetPoint) const;

signals:
    void connectionRequested(UIFlowStateNode *node);
    void stateSelected(const UIFlowStateMachine::UIFlowState &state);
    
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    
private:
    UIFlowStateMachine::UIFlowState m_state;
    QGraphicsTextItem *m_nameText;
    QGraphicsTextItem *m_descriptionText;
    QGraphicsTextItem *m_uiInterfaceIndicator;
    QGraphicsEllipseItem *m_uiIcon;
    QGraphicsDropShadowEffect *m_shadowEffect;
    QPropertyAnimation *m_selectionAnimation;
    
    bool m_isHovered;
    bool m_isDragging;
    bool m_isHighlighted;
    QPointF m_dragStartPos;
    
    void updateUiInterfaceIndicator();
    void createVisualEffects();
    void updateNodeStyle();
};

// UI流转换线
class UIFlowTransitionEdge : public QObject, public QGraphicsLineItem
{
    Q_OBJECT

public:
    explicit UIFlowTransitionEdge(const UIFlowStateMachine::UIFlowTransition &transition, 
                                 UIFlowStateNode *fromNode, UIFlowStateNode *toNode, 
                                 QGraphicsItem *parent = nullptr);
    
    UIFlowStateMachine::UIFlowTransition transition() const;
    void setTransition(const UIFlowStateMachine::UIFlowTransition &transition);
    
    void updatePosition();
    void animateTransition();
    void setHighlighted(bool highlighted);
    
    UIFlowStateNode* fromNode() const { return m_fromNode; }
    UIFlowStateNode* toNode() const { return m_toNode; }
    
signals:
    void transitionSelected(const UIFlowStateMachine::UIFlowTransition &transition);
    
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    
private:
    UIFlowStateMachine::UIFlowTransition m_transition;
    UIFlowStateNode *m_fromNode;
    UIFlowStateNode *m_toNode;
    QGraphicsTextItem *m_eventText;
    QGraphicsTextItem *m_labelText;
    QGraphicsPolygonItem *m_arrowHead;
    QGraphicsDropShadowEffect *m_shadowEffect;
    QPropertyAnimation *m_highlightAnimation;
    
    bool m_isHovered;
    bool m_isHighlighted;
    
    void updateArrowPosition();
    void updateLineStyle();
    void createVisualEffects();
    void updateLabelPosition(const QPainterPath &path);
    void updatePath();
    void updateArrowHead(const QPainterPath &path);
};

// UI流状态机场景
class UIFlowStateMachineScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit UIFlowStateMachineScene(QObject *parent = nullptr);
    
    void setStateMachine(UIFlowStateMachine *stateMachine);
    UIFlowStateMachine* stateMachine() const;
    
    void refreshScene();
    void startConnection(UIFlowStateNode *fromNode, const QPointF &startPos);
    void updateConnection(const QPointF &currentPos);
    void finishConnection(UIFlowStateNode *toNode);
    void cancelConnection();
    
    UIFlowStateNode* findStateNode(const QString &stateId) const;
    UIFlowTransitionEdge* findTransitionEdge(const QString &transitionId) const;
    UIFlowStateNode* getNodeAtPosition(const QPointF &scenePos) const;
    
public slots:
    void addState(const QPointF &position);
    void addTransition(const QString &fromStateId, const QString &toStateId);
    void removeState(const QString &stateId);
    void removeTransition(const QString &transitionId);
    void onStateSelected(const UIFlowStateMachine::UIFlowState &state);
    void onTransitionSelected(const UIFlowStateMachine::UIFlowTransition &transition);
    
signals:
    void stateSelected(const UIFlowStateMachine::UIFlowState &state);
    void transitionSelected(const UIFlowStateMachine::UIFlowTransition &transition);
    void stateMachineChanged();
    void connectionStarted(UIFlowStateNode *fromNode);
    void connectionFinished(UIFlowStateNode *fromNode, UIFlowStateNode *toNode);
    
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    
private:
    UIFlowStateMachine *m_stateMachine;
    QMap<QString, UIFlowStateNode*> m_stateNodes;
    QMap<QString, UIFlowTransitionEdge*> m_transitionEdges;
    
    // 当前选中的项
    UIFlowStateNode *m_currentStateNode;
    UIFlowTransitionEdge *m_currentTransitionEdge;
    
    // 连接线相关
    UIFlowStateNode *m_connectionFromNode;
    QGraphicsLineItem *m_tempConnectionLine;
    bool m_isConnecting;
    
    void createStateNode(const UIFlowStateMachine::UIFlowState &state);
    void createTransitionEdge(const UIFlowStateMachine::UIFlowTransition &transition);
    void clearScene();
    void setupConnectionMode();
    void cleanupConnectionMode();
};

// 逻辑时序状态节点
class LogicStateNode : public QObject, public QGraphicsRectItem
{
    Q_OBJECT

public:
    explicit LogicStateNode(const LogicSequenceStateMachine::LogicState &state, QGraphicsItem *parent = nullptr);
    
    LogicSequenceStateMachine::LogicState state() const;
    void setState(const LogicSequenceStateMachine::LogicState &state);
    
    void updateAppearance();
    void animateSelection();
    void setHighlighted(bool highlighted);
    QPointF getConnectionPoint(const QPointF &targetPoint) const;

signals:
    void connectionRequested(LogicStateNode *node);
    
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    
private:
    LogicSequenceStateMachine::LogicState m_state;
    QGraphicsTextItem *m_nameText;
    QGraphicsTextItem *m_descriptionText;
    QGraphicsTextItem *m_logicTypeIndicator;
    QGraphicsEllipseItem *m_logicIcon;
    QGraphicsDropShadowEffect *m_shadowEffect;
    QPropertyAnimation *m_selectionAnimation;
    
    bool m_isHovered;
    bool m_isDragging;
    bool m_isHighlighted;
    QPointF m_dragStartPos;
    
    void updateLogicTypeIndicator();
    void createVisualEffects();
    void updateNodeStyle();
};

// 逻辑时序转换线
class LogicTransitionEdge : public QObject, public QGraphicsLineItem
{
    Q_OBJECT

public:
    explicit LogicTransitionEdge(const LogicSequenceStateMachine::LogicTransition &transition, 
                                LogicStateNode *fromNode, LogicStateNode *toNode, 
                                QGraphicsItem *parent = nullptr);
    
    LogicSequenceStateMachine::LogicTransition transition() const;
    void setTransition(const LogicSequenceStateMachine::LogicTransition &transition);
    
    void updatePosition();
    void animateTransition();
    void setHighlighted(bool highlighted);
    
    LogicStateNode* fromNode() const { return m_fromNode; }
    LogicStateNode* toNode() const { return m_toNode; }
    
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    
private:
    LogicSequenceStateMachine::LogicTransition m_transition;
    LogicStateNode *m_fromNode;
    LogicStateNode *m_toNode;
    QGraphicsTextItem *m_eventText;
    QGraphicsPolygonItem *m_arrowHead;
    QGraphicsDropShadowEffect *m_shadowEffect;
    QPropertyAnimation *m_highlightAnimation;
    
    bool m_isHovered;
    bool m_isHighlighted;
    
    void updateArrowPosition();
    void updateLineStyle();
    void createVisualEffects();
};

// 逻辑时序状态机场景
class LogicStateMachineScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit LogicStateMachineScene(QObject *parent = nullptr);
    
    void setStateMachine(LogicSequenceStateMachine *stateMachine);
    LogicSequenceStateMachine* stateMachine() const;
    
    void refreshScene();
    void startConnection(LogicStateNode *fromNode, const QPointF &startPos);
    void updateConnection(const QPointF &currentPos);
    void finishConnection(LogicStateNode *toNode);
    void cancelConnection();
    
    LogicStateNode* findStateNode(const QString &stateId) const;
    LogicTransitionEdge* findTransitionEdge(const QString &transitionId) const;
    LogicStateNode* getNodeAtPosition(const QPointF &scenePos) const;
    
public slots:
    void addState(const QPointF &position);
    void addTransition(const QString &fromStateId, const QString &toStateId);
    void removeState(const QString &stateId);
    void removeTransition(const QString &transitionId);
    
signals:
    void stateSelected(const LogicSequenceStateMachine::LogicState &state);
    void transitionSelected(const LogicSequenceStateMachine::LogicTransition &transition);
    void stateMachineChanged();
    void connectionStarted(LogicStateNode *fromNode);
    void connectionFinished(LogicStateNode *fromNode, LogicStateNode *toNode);
    
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    
private:
    LogicSequenceStateMachine *m_stateMachine;
    QMap<QString, LogicStateNode*> m_stateNodes;
    QMap<QString, LogicTransitionEdge*> m_transitionEdges;
    
    // 连接线相关
    LogicStateNode *m_connectionFromNode;
    QGraphicsLineItem *m_tempConnectionLine;
    bool m_isConnecting;
    
    void createStateNode(const LogicSequenceStateMachine::LogicState &state);
    void createTransitionEdge(const LogicSequenceStateMachine::LogicTransition &transition);
    void clearScene();
    void setupConnectionMode();
    void cleanupConnectionMode();
};

// 状态机编辑器视图（支持两种模式）
class StateMachineViewV2 : public QGraphicsView
{
    Q_OBJECT

public:
    explicit StateMachineViewV2(QWidget *parent = nullptr);
    
    void setUIFlowScene(UIFlowStateMachineScene *scene);
    void setLogicScene(LogicStateMachineScene *scene);
    void setCurrentMode(StateMachineMode mode);
    
    void fitToView();
    void zoomIn();
    void zoomOut();
    void resetZoom();
    
protected:
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    
private:
    bool m_isDragging;
    bool m_isPanning;
    int m_panStartX;
    int m_panStartY;
    QPoint m_lastMousePos;
    StateMachineMode m_currentMode;
    UIFlowStateMachineScene *m_uiFlowScene;
    LogicStateMachineScene *m_logicScene;
    
    void scaleView(qreal scaleFactor);
    void updateCurrentScene();
    void deleteSelectedItems();
};

// 状态机编辑器主窗口（支持模式切换）
class StateMachineEditorV2 : public QWidget
{
    Q_OBJECT

public:
    explicit StateMachineEditorV2(QWidget *parent = nullptr);
    
    void setStateMachineManager(StateMachineManager *manager);
    void setProductUiFiles(const QList<ProductUIFile> &uiFiles);
    void setProduct(Product *product);
    
    void loadStateMachine(const QString &filePath);
    void saveStateMachine(const QString &filePath);
    
    StateMachineMode currentMode() const;
    
public slots:
    void createNewStateMachine();
    void editStateProperties();
    void editTransitionProperties();
    void setInitialState();
    void validateStateMachine();
    void showRuntimePreview();
    void switchToUIFlowMode();
    void switchToLogicSequenceMode();
    void switchToIntegratedMode();

private slots:
    void onStateSelected(const UIFlowStateMachine::UIFlowState &state);
    void onTransitionSelected(const UIFlowStateMachine::UIFlowTransition &transition);
    void onLogicStateSelected(const LogicSequenceStateMachine::LogicState &state);
    void onLogicTransitionSelected(const LogicSequenceStateMachine::LogicTransition &transition);
    void onStateMachineChanged();
    void onUiInterfaceChanged(int index);
    void onModeChanged();

private:
    void createToolbar();
    void createModeSelector();
    void createPropertiesPanel();
    void createRuntimePreview();
    void updatePropertiesPanel();
    void setupRuntime();
    void switchMode(StateMachineMode newMode);
    void setupUIFlowMode();
    void setupLogicSequenceMode();
    void setupIntegratedMode();
    void updateModeUI();
    void updateToolbarAvailability();
    void updatePropertiesPanelAvailability();
    
    StateMachineManager *m_stateMachineManager;
    StateMachineIntegrationManager *m_integrationManager;
    StateMachineViewV2 *m_view;
    UIFlowStateMachineScene *m_uiFlowScene;
    LogicStateMachineScene *m_logicScene;
    QWidget *m_propertiesPanel;
    QToolBar *m_toolbar;
    QUndoStack *m_undoStack;
    
    // 模式选择器
    QGroupBox *m_modeGroup;
    QRadioButton *m_uiFlowModeRadio;
    QRadioButton *m_logicSequenceModeRadio;
    QRadioButton *m_integratedModeRadio;
    QLabel *m_modeDescription;
    
    // 运行时预览
    StateMachineRuntime *m_runtime;
    StateMachineRuntimeView *m_runtimeView;
    QDockWidget *m_runtimeDock;
    
    // 产品UI文件信息
    QList<ProductUIFile> m_productUiFiles;
    Product *m_product;
    
    // UI元素
    QTabWidget *m_propertiesTab;
    QWidget *m_uiFlowProperties;
    QWidget *m_logicProperties;
    
    // UI流属性面板
    QLineEdit *m_uiFlowStateNameEdit;
    QTextEdit *m_uiFlowStateDescriptionEdit;
    QComboBox *m_uiInterfaceCombo;
    QCheckBox *m_initialStateCheck;
    QCheckBox *m_finalStateCheck;
    QLineEdit *m_uiFlowTransitionEventEdit;
    QComboBox *m_uiFlowTransitionEventTypeCombo;
    
    // 逻辑时序属性面板
    QLineEdit *m_logicStateNameEdit;
    QTextEdit *m_logicStateDescriptionEdit;
    QComboBox *m_logicTypeCombo;
    QLineEdit *m_logicStateDataEdit;
    QLineEdit *m_logicTransitionEventEdit;
    QComboBox *m_logicTransitionEventTypeCombo;
    QTextEdit *m_logicTransitionActionEdit;
    
    // 当前选中状态和转换
    UIFlowStateMachine::UIFlowState m_currentUIFlowState;
    UIFlowStateMachine::UIFlowTransition m_currentUIFlowTransition;
    LogicSequenceStateMachine::LogicState m_currentLogicState;
    LogicSequenceStateMachine::LogicTransition m_currentLogicTransition;
    
    // 当前模式
    StateMachineMode m_currentMode;
};

#endif // STATEMACHINEEDITOR_V2_H