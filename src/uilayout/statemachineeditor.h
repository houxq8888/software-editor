#ifndef STATEMACHINEEDITOR_H
#define STATEMACHINEEDITOR_H

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
#include "statemachine.h"
#include "product.h"
#include "statemachineruntime.h"

// 状态图节点
class StateNode : public QObject, public QGraphicsRectItem
{
    Q_OBJECT

public:
    explicit StateNode(const StateMachineState &state, QGraphicsItem *parent = nullptr);
    
    StateMachineState state() const;
    void setState(const StateMachineState &state);
    
    void updateAppearance();
    void animateSelection();
    void animateConnection();
    void setHighlighted(bool highlighted);
    
    QPointF getConnectionPoint(const QPointF &targetPoint) const;

signals:
    void connectionRequested(StateNode *node);
    
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    
private:
    StateMachineState m_state;
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

// 状态图转换线
class TransitionEdge : public QObject, public QGraphicsLineItem
{
    Q_OBJECT

public:
    explicit TransitionEdge(const StateMachineTransition &transition, 
                           StateNode *fromNode, StateNode *toNode, 
                           QGraphicsItem *parent = nullptr);
    
    StateMachineTransition transition() const;
    void setTransition(const StateMachineTransition &transition);
    
    void updatePosition();
    void animateTransition();
    void setHighlighted(bool highlighted);
    
    StateNode* fromNode() const { return m_fromNode; }
    StateNode* toNode() const { return m_toNode; }
    
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    
private:
    StateMachineTransition m_transition;
    StateNode *m_fromNode;
    StateNode *m_toNode;
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

// 状态机编辑器场景
class StateMachineScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit StateMachineScene(QObject *parent = nullptr);
    
    void setStateMachine(StateMachine *stateMachine);
    StateMachine* stateMachine() const;
    
    void refreshScene();
    void startConnection(StateNode *fromNode, const QPointF &startPos);
    void updateConnection(const QPointF &currentPos);
    void finishConnection(StateNode *toNode);
    void cancelConnection();
    
    StateNode* findStateNode(const QString &stateId) const;
    TransitionEdge* findTransitionEdge(const QString &transitionId) const;
    StateNode* getNodeAtPosition(const QPointF &scenePos) const;
    
public slots:
    void addState(const QPointF &position);
    void addTransition(const QString &fromStateId, const QString &toStateId);
    void removeState(const QString &stateId);
    void removeTransition(const QString &transitionId);
    
signals:
    void stateSelected(const StateMachineState &state);
    void transitionSelected(const StateMachineTransition &transition);
    void stateMachineChanged();
    void connectionStarted(StateNode *fromNode);
    void connectionFinished(StateNode *fromNode, StateNode *toNode);
    
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    
private:
    StateMachine *m_stateMachine;
    QMap<QString, StateNode*> m_stateNodes;
    QMap<QString, TransitionEdge*> m_transitionEdges;
    
    // 连接线相关
    StateNode *m_connectionFromNode;
    QGraphicsLineItem *m_tempConnectionLine;
    bool m_isConnecting;
    
    void createStateNode(const StateMachineState &state);
    void createTransitionEdge(const StateMachineTransition &transition);
    void clearScene();
    void setupConnectionMode();
    void cleanupConnectionMode();
};

// 状态机编辑器视图
class StateMachineView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit StateMachineView(QWidget *parent = nullptr);
    
    void setScene(StateMachineScene *scene);
    
protected:
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    
private:
    bool m_isDragging;
    QPoint m_lastMousePos;
    
    void scaleView(qreal scaleFactor);
};

// 状态机编辑器主窗口
class StateMachineEditor : public QWidget
{
    Q_OBJECT

public:
    explicit StateMachineEditor(QWidget *parent = nullptr);
    
    void setStateMachineManager(StateMachineManager *manager);
    void setProductUiFiles(const QList<ProductUIFile> &uiFiles);
    void setProduct(Product *product);
    
    void loadStateMachine(const QString &filePath);
    void saveStateMachine(const QString &filePath);

public slots:
    void createNewStateMachine();
    void editStateProperties();
    void editTransitionProperties();
    void setInitialState();
    void validateStateMachine();
    void showRuntimePreview();

private slots:
    void onStateSelected(const StateMachineState &state);
    void onTransitionSelected(const StateMachineTransition &transition);
    void onStateMachineChanged();
    void onUiInterfaceChanged(int index);

private:
    void createToolbar();
    void createPropertiesPanel();
    void createRuntimePreview();
    void updatePropertiesPanel();
    void setupRuntime();
    
    StateMachineManager *m_stateMachineManager;
    StateMachineView *m_view;
    StateMachineScene *m_scene;
    QWidget *m_propertiesPanel;
    QToolBar *m_toolbar;
    QUndoStack *m_undoStack;
    
    // 运行时预览
    StateMachineRuntime *m_runtime;
    StateMachineRuntimeView *m_runtimeView;
    QDockWidget *m_runtimeDock;
    
    // 产品UI文件信息
    QList<ProductUIFile> m_productUiFiles;
    Product *m_product;
    
    // UI元素
    QLineEdit *m_stateNameEdit;
    QTextEdit *m_stateDescriptionEdit;
    QComboBox *m_uiInterfaceCombo;
    QCheckBox *m_initialStateCheck;
    QCheckBox *m_finalStateCheck;
    
    QLineEdit *m_transitionEventEdit;
    QComboBox *m_transitionEventTypeCombo;
    QTextEdit *m_transitionActionEdit;
    
    // 条件配置UI组件
    QLineEdit *m_transitionSourceEdit;
    QLineEdit *m_conditionVariableEdit;
    QComboBox *m_conditionOperatorCombo;
    QLineEdit *m_conditionValueEdit;
    QComboBox *m_conditionLogicCombo;
    
    StateMachineState m_currentState;
    StateMachineTransition m_currentTransition;
};

#endif // STATEMACHINEEDITOR_H