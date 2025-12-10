#include "statemachineeditor.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolBar>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QMessageBox>
#include <QInputDialog>
#include <QMenu>
#include <QAction>
#include <QUndoCommand>
#include <QGraphicsTextItem>
#include <QGraphicsPolygonItem>
#include <QApplication>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QFormLayout>
#include <cmath>

// StateNode 实现
StateNode::StateNode(const StateMachineState &state, QGraphicsItem *parent)
    : QGraphicsRectItem(parent)
    , m_state(state)
    , m_uiInterfaceIndicator(nullptr)
    , m_uiIcon(nullptr)
    , m_shadowEffect(nullptr)
    , m_selectionAnimation(nullptr)
    , m_isHovered(false)
    , m_isDragging(false)
    , m_isHighlighted(false)
{
    setRect(-60, -40, 120, 80); // 增大节点尺寸
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    setAcceptHoverEvents(true);
    
    // 创建文本元素
    m_nameText = new QGraphicsTextItem(m_state.name, this);
    m_nameText->setPos(-55, -35);
    m_nameText->setTextWidth(110);
    m_nameText->setFont(QFont("Arial", 10, QFont::Bold));
    
    m_descriptionText = new QGraphicsTextItem(m_state.description, this);
    m_descriptionText->setPos(-55, -15);
    m_descriptionText->setTextWidth(110);
    m_descriptionText->setFont(QFont("Arial", 8));
    
    createVisualEffects();
    updateAppearance();
}

StateMachineState StateNode::state() const
{
    return m_state;
}

void StateNode::setState(const StateMachineState &state)
{
    m_state = state;
    m_nameText->setPlainText(state.name);
    m_descriptionText->setPlainText(state.description);
    updateAppearance();
}

void StateNode::animateSelection()
{
    if (m_selectionAnimation) {
        m_selectionAnimation->stop();
        m_selectionAnimation->setStartValue(scale());
        m_selectionAnimation->setEndValue(QPointF(1.1, 1.1));
        m_selectionAnimation->start();
    }
}

void StateNode::animateConnection()
{
    if (m_selectionAnimation) {
        m_selectionAnimation->stop();
        m_selectionAnimation->setStartValue(scale());
        m_selectionAnimation->setEndValue(QPointF(1.05, 1.05));
        m_selectionAnimation->start();
    }
}

void StateNode::setHighlighted(bool highlighted)
{
    m_isHighlighted = highlighted;
    updateAppearance();
}

QPointF StateNode::getConnectionPoint(const QPointF &targetPoint) const
{
    QPointF center = scenePos() + rect().center();
    QLineF line(center, targetPoint);
    
    // 计算与矩形边界的交点
    QRectF rectInScene = sceneBoundingRect();
    QPointF intersection;
    
    // 简化实现：返回中心点
    return center;
}

void StateNode::updateAppearance()
{
    updateNodeStyle();
    updateUiInterfaceIndicator();
}

void StateNode::createVisualEffects()
{
    // 创建阴影效果
    m_shadowEffect = new QGraphicsDropShadowEffect(this);
    m_shadowEffect->setBlurRadius(15);
    m_shadowEffect->setColor(QColor(0, 0, 0, 80));
    m_shadowEffect->setOffset(3, 3);
    setGraphicsEffect(m_shadowEffect);
    
    // 创建选择动画
    m_selectionAnimation = new QPropertyAnimation(this, "scale");
    m_selectionAnimation->setDuration(200);
    m_selectionAnimation->setEasingCurve(QEasingCurve::OutCubic);
}

void StateNode::updateUiInterfaceIndicator()
{
    // 移除旧的UI界面标识
    if (m_uiInterfaceIndicator) {
        scene()->removeItem(m_uiInterfaceIndicator);
        delete m_uiInterfaceIndicator;
        m_uiInterfaceIndicator = nullptr;
    }
    
    // 移除旧的UI图标
    if (m_uiIcon) {
        scene()->removeItem(m_uiIcon);
        delete m_uiIcon;
        m_uiIcon = nullptr;
    }
    
    // 如果有关联的UI界面，添加标识
    if (!m_state.uiInterfaceId.isEmpty()) {
        // 创建UI界面图标（圆形图标）
        m_uiIcon = new QGraphicsEllipseItem(this);
        m_uiIcon->setRect(rect().right() - 20, rect().top() + 5, 15, 15);
        m_uiIcon->setBrush(QBrush(QColor(76, 175, 80))); // 绿色
        m_uiIcon->setPen(QPen(Qt::white, 1));
        
        // 创建UI文字标识
        m_uiInterfaceIndicator = new QGraphicsTextItem("UI", this);
        m_uiInterfaceIndicator->setFont(QFont("Arial", 7, QFont::Bold));
        m_uiInterfaceIndicator->setDefaultTextColor(Qt::white);
        m_uiInterfaceIndicator->setPos(rect().right() - 17, rect().top() + 7);
        
        // 添加工具提示显示具体的UI界面ID
        setToolTip("状态: " + m_state.name + "\n关联UI界面: " + m_state.uiInterfaceId);
    } else {
        setToolTip("状态: " + m_state.name);
    }
}

void StateNode::updateNodeStyle()
{
    QColor baseColor;
    QColor borderColor = Qt::black;
    int borderWidth = 2;
    
    if (m_state.isInitialState) {
        baseColor = QColor(144, 238, 144); // 浅绿色
        borderColor = QColor(34, 139, 34); // 森林绿
    } else if (m_state.isFinalState) {
        baseColor = QColor(255, 182, 193); // 浅粉色
        borderColor = QColor(219, 112, 147); // 深粉色
    } else {
        baseColor = QColor(240, 240, 240); // 浅灰色
    }
    
    // 悬停效果
    if (m_isHovered) {
        baseColor = baseColor.lighter(110); // 变亮10%
        borderWidth = 3;
    }
    
    // 选中效果
    if (isSelected()) {
        borderColor = QColor(30, 144, 255); // 道奇蓝
        borderWidth = 4;
    }
    
    // 高亮效果（连接模式）
    if (m_isHighlighted) {
        borderColor = QColor(255, 165, 0); // 橙色
        borderWidth = 4;
    }
    
    setBrush(QBrush(baseColor));
    setPen(QPen(borderColor, borderWidth));
}

void StateNode::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsRectItem::mousePressEvent(event);
    
    if (event->button() == Qt::LeftButton) {
        m_isDragging = true;
        m_dragStartPos = event->scenePos();
        
        StateMachineScene *scene = qobject_cast<StateMachineScene*>(this->scene());
        if (scene) {
            emit scene->stateSelected(m_state);
            animateSelection();
        }
    }
}

void StateNode::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_isDragging) {
        QGraphicsRectItem::mouseMoveEvent(event);
        
        // 更新状态的位置
        m_state.position = scenePos();
        
        // 通知场景状态位置已更新
        StateMachineScene *scene = qobject_cast<StateMachineScene*>(this->scene());
        if (scene && scene->stateMachine()) {
            scene->stateMachine()->updateState(m_state);
        }
    }
}

void StateNode::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsRectItem::mouseReleaseEvent(event);
    
    if (event->button() == Qt::LeftButton) {
        m_isDragging = false;
        
        // 恢复动画
        if (m_selectionAnimation) {
            m_selectionAnimation->stop();
            m_selectionAnimation->setStartValue(scale());
            m_selectionAnimation->setEndValue(QPointF(1.0, 1.0));
            m_selectionAnimation->start();
        }
    }
}

void StateNode::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsRectItem::hoverEnterEvent(event);
    m_isHovered = true;
    updateAppearance();
}

void StateNode::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsRectItem::hoverLeaveEvent(event);
    m_isHovered = false;
    updateAppearance();
}

void StateNode::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsRectItem::mouseDoubleClickEvent(event);
    // 触发编辑状态属性
    StateMachineScene *scene = qobject_cast<StateMachineScene*>(this->scene());
    if (scene) {
        emit scene->stateSelected(m_state);
    }
}

void StateNode::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QMenu menu;
    QAction *editAction = menu.addAction("编辑状态");
    QAction *deleteAction = menu.addAction("删除状态");
    QAction *setInitialAction = menu.addAction("设为初始状态");
    QAction *connectAction = menu.addAction("创建连接");
    
    QAction *selectedAction = menu.exec(event->screenPos());
    
    StateMachineScene *scene = qobject_cast<StateMachineScene*>(this->scene());
    if (!scene) return;
    
    if (selectedAction == editAction) {
        emit scene->stateSelected(m_state);
    } else if (selectedAction == deleteAction) {
        scene->removeState(m_state.stateId);
    } else if (selectedAction == setInitialAction) {
        scene->stateMachine()->setInitialState(m_state.stateId);
        scene->refreshScene();
    } else if (selectedAction == connectAction) {
        emit connectionRequested(this);
    }
}

// TransitionEdge 实现
TransitionEdge::TransitionEdge(const StateMachineTransition &transition, 
                               StateNode *fromNode, StateNode *toNode, 
                               QGraphicsItem *parent)
    : QGraphicsLineItem(parent)
    , m_transition(transition)
    , m_fromNode(fromNode)
    , m_toNode(toNode)
    , m_eventText(nullptr)
    , m_arrowHead(nullptr)
    , m_shadowEffect(nullptr)
    , m_highlightAnimation(nullptr)
    , m_isHighlighted(false)
    , m_isHovered(false)
{
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setAcceptHoverEvents(true);
    
    m_eventText = new QGraphicsTextItem(m_transition.eventName, this);
    m_eventText->setFont(QFont("Arial", 9));
    
    // 创建箭头
    m_arrowHead = new QGraphicsPolygonItem(this);
    QPolygonF arrow;
    arrow << QPointF(0, 0) << QPointF(-10, -5) << QPointF(-10, 5);
    m_arrowHead->setPolygon(arrow);
    m_arrowHead->setBrush(QBrush(Qt::black));
    
    createVisualEffects();
    updatePosition();
}

StateMachineTransition TransitionEdge::transition() const
{
    return m_transition;
}

void TransitionEdge::setTransition(const StateMachineTransition &transition)
{
    m_transition = transition;
    m_eventText->setPlainText(transition.eventName);
    updatePosition();
}

void TransitionEdge::animateTransition()
{
    if (m_highlightAnimation) {
        m_highlightAnimation->stop();
        m_highlightAnimation->setStartValue(1.0);
        m_highlightAnimation->setEndValue(1.5);
        m_highlightAnimation->start();
    }
}

void TransitionEdge::setHighlighted(bool highlighted)
{
    m_isHighlighted = highlighted;
    updateLineStyle();
}



void TransitionEdge::updatePosition()
{
    if (!m_fromNode || !m_toNode) return;
    
    QPointF fromCenter = m_fromNode->getConnectionPoint(m_toNode->scenePos() + m_toNode->rect().center());
    QPointF toCenter = m_toNode->getConnectionPoint(m_fromNode->scenePos() + m_fromNode->rect().center());
    
    // 计算连线
    QLineF line(fromCenter, toCenter);
    setLine(line);
    
    // 计算文本位置（连线中点）
    QPointF textPos = line.pointAt(0.5);
    m_eventText->setPos(textPos - QPointF(m_eventText->boundingRect().width() / 2, 
                                         m_eventText->boundingRect().height() / 2));
    
    // 计算箭头位置和方向
    qreal angle = std::atan2(-line.dy(), line.dx());
    QPointF arrowPos = line.pointAt(0.9);
    m_arrowHead->setPos(arrowPos);
    m_arrowHead->setRotation(-angle * 180 / M_PI);
    
    // 设置线条样式
    updateLineStyle();
}

void TransitionEdge::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsLineItem::hoverEnterEvent(event);
    m_isHovered = true;
    updateLineStyle();
}

void TransitionEdge::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsLineItem::hoverLeaveEvent(event);
    m_isHovered = false;
    updateLineStyle();
}

void TransitionEdge::createVisualEffects()
{
    // 创建阴影效果
    m_shadowEffect = new QGraphicsDropShadowEffect(this);
    m_shadowEffect->setBlurRadius(10);
    m_shadowEffect->setColor(QColor(0, 0, 0, 60));
    m_shadowEffect->setOffset(2, 2);
    setGraphicsEffect(m_shadowEffect);
    
    // 创建高亮动画
    m_highlightAnimation = new QPropertyAnimation(this, QByteArray("scale"), this);
    m_highlightAnimation->setDuration(300);
    m_highlightAnimation->setEasingCurve(QEasingCurve::OutCubic);
}

void TransitionEdge::updateLineStyle()
{
    if (m_isHighlighted) {
        setPen(QPen(Qt::red, 4));
    } else if (isSelected()) {
        setPen(QPen(Qt::blue, 3));
    } else if (m_isHovered) {
        setPen(QPen(Qt::darkGreen, 3));
    } else {
        setPen(QPen(Qt::black, 2));
    }
}

void TransitionEdge::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsLineItem::mousePressEvent(event);
    if (event->button() == Qt::LeftButton) {
        StateMachineScene *scene = qobject_cast<StateMachineScene*>(this->scene());
        if (scene) {
            emit scene->transitionSelected(m_transition);
        }
    }
}

void TransitionEdge::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsLineItem::mouseDoubleClickEvent(event);
    StateMachineScene *scene = qobject_cast<StateMachineScene*>(this->scene());
    if (scene) {
        emit scene->transitionSelected(m_transition);
    }
}

void TransitionEdge::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QMenu menu;
    QAction *editAction = menu.addAction("编辑转换");
    QAction *deleteAction = menu.addAction("删除转换");
    
    QAction *selectedAction = menu.exec(event->screenPos());
    
    StateMachineScene *scene = qobject_cast<StateMachineScene*>(this->scene());
    if (!scene) return;
    
    if (selectedAction == editAction) {
        emit scene->transitionSelected(m_transition);
    } else if (selectedAction == deleteAction) {
        scene->removeTransition(m_transition.transitionId);
    }
}

// StateMachineScene 实现
StateMachineScene::StateMachineScene(QObject *parent)
    : QGraphicsScene(parent)
    , m_stateMachine(nullptr)
    , m_isConnecting(false)
    , m_connectionFromNode(nullptr)
    , m_tempConnectionLine(nullptr)
{
    setSceneRect(-1000, -1000, 2000, 2000);
}

void StateMachineScene::setStateMachine(StateMachine *stateMachine)
{
    if (m_stateMachine == stateMachine) return;
    
    m_stateMachine = stateMachine;
    refreshScene();
}

StateMachine* StateMachineScene::stateMachine() const
{
    return m_stateMachine;
}

void StateMachineScene::refreshScene()
{
    clearScene();
    
    if (!m_stateMachine) return;
    
    // 创建状态节点
    for (const auto &state : m_stateMachine->states()) {
        createStateNode(state);
    }
    
    // 创建转换边
    for (const auto &transition : m_stateMachine->transitions()) {
        createTransitionEdge(transition);
    }
}

StateNode* StateMachineScene::findStateNode(const QString &stateId) const
{
    return m_stateNodes.value(stateId, nullptr);
}

TransitionEdge* StateMachineScene::findTransitionEdge(const QString &transitionId) const
{
    return m_transitionEdges.value(transitionId, nullptr);
}

void StateMachineScene::addState(const QPointF &position)
{
    if (!m_stateMachine) return;
    
    bool ok;
    QString stateName = QInputDialog::getText(nullptr, "添加状态", "状态名称:", 
                                             QLineEdit::Normal, "", &ok);
    if (!ok || stateName.isEmpty()) return;
    
    StateMachineState state;
    state.stateId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    state.name = stateName;
    state.position = position;
    
    if (m_stateMachine->states().isEmpty()) {
        state.isInitialState = true;
    }
    
    if (m_stateMachine->addState(state)) {
        createStateNode(state);
        emit stateMachineChanged();
    }
}

void StateMachineScene::addTransition(const QString &fromStateId, const QString &toStateId)
{
    if (!m_stateMachine) return;
    
    bool ok;
    QString eventName = QInputDialog::getText(nullptr, "添加转换", "事件名称:", 
                                             QLineEdit::Normal, "", &ok);
    if (!ok || eventName.isEmpty()) return;
    
    StateMachineTransition transition;
    transition.transitionId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    transition.fromStateId = fromStateId;
    transition.toStateId = toStateId;
    transition.eventName = eventName;
    transition.eventType = StateMachineEventType::ButtonClick;
    
    if (m_stateMachine->addTransition(transition)) {
        createTransitionEdge(transition);
        emit stateMachineChanged();
    }
}

void StateMachineScene::removeState(const QString &stateId)
{
    if (!m_stateMachine) return;
    
    if (m_stateMachine->removeState(stateId)) {
        StateNode *node = m_stateNodes.take(stateId);
        if (node) {
            removeItem(node);
            delete node;
        }
        emit stateMachineChanged();
    }
}

void StateMachineScene::removeTransition(const QString &transitionId)
{
    if (!m_stateMachine) return;
    
    if (m_stateMachine->removeTransition(transitionId)) {
        TransitionEdge *edge = m_transitionEdges.take(transitionId);
        if (edge) {
            removeItem(edge);
            delete edge;
        }
        emit stateMachineChanged();
    }
}

void StateMachineScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mousePressEvent(event);
    
    if (event->button() == Qt::RightButton) {
        QMenu menu;
        QAction *addStateAction = menu.addAction("添加状态");
        
        QAction *selectedAction = menu.exec(event->screenPos());
        if (selectedAction == addStateAction) {
            addState(event->scenePos());
        }
    }
}

void StateMachineScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseMoveEvent(event);
    
    // 更新临时连接线
    if (m_isConnecting && m_tempConnectionLine && m_connectionFromNode) {
        updateConnection(event->scenePos());
    }
}

void StateMachineScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseReleaseEvent(event);
    
    if (m_isConnecting && event->button() == Qt::LeftButton) {
        // 检查是否释放到状态节点上
        QGraphicsItem *item = itemAt(event->scenePos(), QTransform());
        if (StateNode *toNode = qgraphicsitem_cast<StateNode*>(item)) {
            finishConnection(toNode);
        } else {
            cancelConnection();
        }
    }
}

void StateMachineScene::createStateNode(const StateMachineState &state)
{
    StateNode *node = new StateNode(state);
    node->setPos(state.position);
    addItem(node);
    m_stateNodes[state.stateId] = node;
    
    // 连接右键菜单信号
    connect(node, &StateNode::connectionRequested, this, [this](StateNode *node) {
        startConnection(node, node->scenePos());
    });
}

void StateMachineScene::createTransitionEdge(const StateMachineTransition &transition)
{
    StateNode *fromNode = findStateNode(transition.fromStateId);
    StateNode *toNode = findStateNode(transition.toStateId);
    
    if (!fromNode || !toNode) return;
    
    TransitionEdge *edge = new TransitionEdge(transition, fromNode, toNode);
    addItem(edge);
    m_transitionEdges[transition.transitionId] = edge;
}

void StateMachineScene::startConnection(StateNode *fromNode, const QPointF &startPos)
{
    if (!fromNode) return;
    
    m_connectionFromNode = fromNode;
    m_isConnecting = true;
    
    // 创建临时连接线
    m_tempConnectionLine = new QGraphicsLineItem();
    m_tempConnectionLine->setPen(QPen(Qt::gray, 2, Qt::DashLine));
    addItem(m_tempConnectionLine);
    
    setupConnectionMode();
}

void StateMachineScene::updateConnection(const QPointF &mousePos)
{
    if (m_isConnecting && m_tempConnectionLine && m_connectionFromNode) {
        QPointF fromPos = m_connectionFromNode->getConnectionPoint(mousePos);
        QLineF line(fromPos, mousePos);
        m_tempConnectionLine->setLine(line);
    }
}

void StateMachineScene::finishConnection(StateNode *toNode)
{
    if (!m_isConnecting || !m_connectionFromNode || !toNode || m_connectionFromNode == toNode) {
        cancelConnection();
        return;
    }
    
    // 创建转换
    StateMachineTransition transition;
    transition.fromStateId = m_connectionFromNode->state().stateId;
    transition.toStateId = toNode->state().stateId;
    transition.eventName = "点击事件"; // 默认事件
    
    // 添加到状态机
    if (m_stateMachine) {
        m_stateMachine->addTransition(transition);
        
        // 创建图形化转换边
        TransitionEdge *edge = new TransitionEdge(transition, m_connectionFromNode, toNode);
        addItem(edge);
        m_transitionEdges[transition.transitionId] = edge;
        
        // 动画效果
        edge->animateTransition();
        m_connectionFromNode->animateConnection();
        toNode->animateConnection();
        
        emit stateMachineChanged();
    }
    
    cleanupConnectionMode();
}

void StateMachineScene::cancelConnection()
{
    if (m_tempConnectionLine) {
        removeItem(m_tempConnectionLine);
        delete m_tempConnectionLine;
        m_tempConnectionLine = nullptr;
    }
    
    m_connectionFromNode = nullptr;
    m_isConnecting = false;
    
    cleanupConnectionMode();
}

void StateMachineScene::setupConnectionMode()
{
    // 设置连接模式下的光标（通过视图）
    QList<QGraphicsView*> views = this->views();
    for (QGraphicsView *view : views) {
        view->setCursor(Qt::CrossCursor);
    }
    
    // 高亮可连接的状态节点
    for (StateNode *node : m_stateNodes) {
        if (node != m_connectionFromNode) {
            node->setHighlighted(true);
        }
    }
}

void StateMachineScene::cleanupConnectionMode()
{
    // 恢复默认光标（通过视图）
    QList<QGraphicsView*> views = this->views();
    for (QGraphicsView *view : views) {
        view->unsetCursor();
    }
    
    // 取消高亮
    for (StateNode *node : m_stateNodes) {
        node->setHighlighted(false);
    }
    
    m_connectionFromNode = nullptr;
    m_isConnecting = false;
    
    if (m_tempConnectionLine) {
        removeItem(m_tempConnectionLine);
        delete m_tempConnectionLine;
        m_tempConnectionLine = nullptr;
    }
}

void StateMachineScene::clearScene()
{
    for (auto *node : m_stateNodes) {
        removeItem(node);
        delete node;
    }
    m_stateNodes.clear();
    
    for (auto *edge : m_transitionEdges) {
        removeItem(edge);
        delete edge;
    }
    m_transitionEdges.clear();
    
    // 清理连接模式相关资源
    if (m_tempConnectionLine) {
        removeItem(m_tempConnectionLine);
        delete m_tempConnectionLine;
        m_tempConnectionLine = nullptr;
    }
    m_connectionFromNode = nullptr;
    m_isConnecting = false;
}

// StateMachineView 实现
StateMachineView::StateMachineView(QWidget *parent)
    : QGraphicsView(parent)
    , m_isDragging(false)
{
    setRenderHint(QPainter::Antialiasing);
    setDragMode(QGraphicsView::RubberBandDrag);
    setOptimizationFlags(QGraphicsView::DontSavePainterState);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
}

void StateMachineView::setScene(StateMachineScene *scene)
{
    QGraphicsView::setScene(scene);
}

void StateMachineView::wheelEvent(QWheelEvent *event)
{
    qreal scaleFactor = 1.15;
    if (event->angleDelta().y() < 0) {
        scaleFactor = 1.0 / scaleFactor;
    }
    
    scaleView(scaleFactor);
}

void StateMachineView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton) {
        m_isDragging = true;
        m_lastMousePos = event->pos();
        setCursor(Qt::ClosedHandCursor);
        return;
    }
    
    QGraphicsView::mousePressEvent(event);
}

void StateMachineView::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isDragging) {
        QPoint delta = event->pos() - m_lastMousePos;
        m_lastMousePos = event->pos();
        
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
        return;
    }
    
    QGraphicsView::mouseMoveEvent(event);
}

void StateMachineView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton) {
        m_isDragging = false;
        setCursor(Qt::ArrowCursor);
        return;
    }
    
    QGraphicsView::mouseReleaseEvent(event);
}

void StateMachineView::scaleView(qreal scaleFactor)
{
    qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.07 || factor > 100) {
        return;
    }
    
    scale(scaleFactor, scaleFactor);
}

// StateMachineEditor 实现
StateMachineEditor::StateMachineEditor(QWidget *parent)
    : QWidget(parent)
    , m_stateMachineManager(nullptr)
    , m_view(new StateMachineView(this))
    , m_scene(new StateMachineScene(this))
    , m_propertiesPanel(new QWidget(this))
    , m_toolbar(new QToolBar(this))
    , m_undoStack(new QUndoStack(this))
    , m_runtime(new StateMachineRuntime(this))
    , m_runtimeView(new StateMachineRuntimeView(this))
    , m_runtimeDock(nullptr)
    , m_product(nullptr)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // 创建工具栏
    createToolbar();
    mainLayout->addWidget(m_toolbar);
    
    // 创建主区域
    QHBoxLayout *contentLayout = new QHBoxLayout();
    
    // 状态图视图
    m_view->setScene(m_scene);
    contentLayout->addWidget(m_view, 3);
    
    // 属性面板
    createPropertiesPanel();
    contentLayout->addWidget(m_propertiesPanel, 1);
    
    mainLayout->addLayout(contentLayout);
    
    // 设置运行时
    setupRuntime();
    
    // 连接信号
    connect(m_scene, &StateMachineScene::stateSelected, this, &StateMachineEditor::onStateSelected);
    connect(m_scene, &StateMachineScene::transitionSelected, this, &StateMachineEditor::onTransitionSelected);
    connect(m_scene, &StateMachineScene::stateMachineChanged, this, &StateMachineEditor::onStateMachineChanged);
    
    // 连接UI界面选择信号
    connect(m_uiInterfaceCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &StateMachineEditor::onUiInterfaceChanged);
}

void StateMachineEditor::setStateMachineManager(StateMachineManager *manager)
{
    m_stateMachineManager = manager;
    if (manager && manager->currentStateMachine()) {
        m_scene->setStateMachine(manager->currentStateMachine());
    }
}

void StateMachineEditor::setProductUiFiles(const QList<ProductUIFile> &uiFiles)
{
    m_productUiFiles = uiFiles;
    qDebug() << "状态机编辑器接收到产品UI文件信息:" << m_productUiFiles.size() << "个文件";
    
    // 更新属性面板中的UI界面下拉框
    updatePropertiesPanel();
}

void StateMachineEditor::setProduct(Product *product)
{
    m_product = product;
    if (product) {
        qDebug() << "状态机编辑器已关联产品:" << product->name();
    }
}

void StateMachineEditor::loadStateMachine(const QString &filePath)
{
    if (!m_stateMachineManager) return;
    
    StateMachine *stateMachine = m_stateMachineManager->createStateMachine();
    if (stateMachine->loadFromFile(filePath)) {
        m_stateMachineManager->setCurrentStateMachine(stateMachine);
        m_scene->setStateMachine(stateMachine);
    }
}

void StateMachineEditor::saveStateMachine(const QString &filePath)
{
    if (!m_stateMachineManager || !m_stateMachineManager->currentStateMachine()) return;
    
    // 保存状态机到文件
    if (m_stateMachineManager->currentStateMachine()->saveToFile(filePath)) {
        // 如果关联了产品对象，则更新产品配置中的状态机路径
        if (m_product) {
            m_product->setStateMachinePath(filePath);
            qDebug() << "已更新产品配置中的状态机路径:" << filePath;
        }
    }
}

void StateMachineEditor::createNewStateMachine()
{
    if (!m_stateMachineManager) {
        qDebug()<<"state machine manager is null";
        return;
    }
    
    StateMachine *stateMachine = m_stateMachineManager->createStateMachine("新状态机");
    m_stateMachineManager->setCurrentStateMachine(stateMachine);
    m_scene->setStateMachine(stateMachine);
}

void StateMachineEditor::editStateProperties()
{
    if (m_currentState.stateId.isEmpty()) return;
    
    // 更新状态属性
    m_currentState.name = m_stateNameEdit->text();
    m_currentState.description = m_stateDescriptionEdit->toPlainText();
    m_currentState.uiInterfaceId = m_uiInterfaceCombo->currentData().toString();
    m_currentState.isInitialState = m_initialStateCheck->isChecked();
    m_currentState.isFinalState = m_finalStateCheck->isChecked();
    
    if (m_stateMachineManager && m_stateMachineManager->currentStateMachine()) {
        m_stateMachineManager->currentStateMachine()->updateState(m_currentState);
        m_scene->refreshScene();
    }
}

void StateMachineEditor::editTransitionProperties()
{
    if (m_currentTransition.transitionId.isEmpty()) return;
    
    // 更新转换属性
    m_currentTransition.eventName = m_transitionEventEdit->text();
    m_currentTransition.eventType = static_cast<StateMachineEventType>(m_transitionEventTypeCombo->currentIndex());
    m_currentTransition.actionScript = m_transitionActionEdit->toPlainText();
    
    // 更新条件配置
    m_currentTransition.condition.variable = m_conditionVariableEdit->text();
    m_currentTransition.condition.operatorType = static_cast<ConditionOperator>(m_conditionOperatorCombo->currentIndex());
    m_currentTransition.condition.value = m_conditionValueEdit->text();
    m_currentTransition.condition.logic = static_cast<ConditionLogic>(m_conditionLogicCombo->currentIndex());
    m_currentTransition.eventSource = m_transitionSourceEdit->text();
    
    if (m_stateMachineManager && m_stateMachineManager->currentStateMachine()) {
        m_stateMachineManager->currentStateMachine()->updateTransition(m_currentTransition);
        m_scene->refreshScene();
        
        QMessageBox::information(this, "转换设置", "转换属性已成功更新！");
    }
}

void StateMachineEditor::setInitialState()
{
    if (m_stateMachineManager && m_stateMachineManager->currentStateMachine()) {
        m_stateMachineManager->currentStateMachine()->setInitialState(m_currentState.stateId);
        m_scene->refreshScene();
    }
}

void StateMachineEditor::validateStateMachine()
{
    if (!m_stateMachineManager || !m_stateMachineManager->currentStateMachine()) return;
    
    if (m_stateMachineManager->currentStateMachine()->validate()) {
        QMessageBox::information(this, "验证成功", "状态机验证通过！");
    } else {
        QList<QString> errors = m_stateMachineManager->currentStateMachine()->validationErrors();
        QMessageBox::warning(this, "验证失败", "状态机验证失败：\n" + errors.join("\n"));
    }
}

void StateMachineEditor::onStateSelected(const StateMachineState &state)
{
    m_currentState = state;
    updatePropertiesPanel();
}

void StateMachineEditor::onTransitionSelected(const StateMachineTransition &transition)
{
    m_currentTransition = transition;
    updatePropertiesPanel();
}

void StateMachineEditor::onStateMachineChanged()
{
    // 状态机发生变化时的处理
}

void StateMachineEditor::onUiInterfaceChanged(int index)
{
    if (index < 0) return;
    
    // 获取选中的UI界面ID
    QString uiInterfaceId = m_uiInterfaceCombo->currentData().toString();
    
    // 检查是否有选中的状态
    if (m_currentState.stateId.isEmpty()) {
        qDebug() << "current state is empty, ui interface selection will not take effect";
        return;
    }
    
    // 更新当前状态的UI界面ID
    m_currentState.uiInterfaceId = uiInterfaceId;
    
    // 如果状态机存在，则更新状态机中的状态信息
    if (m_stateMachineManager && m_stateMachineManager->currentStateMachine()) {
        m_stateMachineManager->currentStateMachine()->updateState(m_currentState);
        
        // 刷新场景显示
        m_scene->refreshScene();
        
        qDebug() << "state" << m_currentState.name << "ui interface has been updated to:" 
                 << m_uiInterfaceCombo->currentText() << "(" << uiInterfaceId << ")";
    }
}

void StateMachineEditor::createToolbar()
{
    QAction *newAction = m_toolbar->addAction("新建");
    QAction *openAction = m_toolbar->addAction("打开");
    QAction *saveAction = m_toolbar->addAction("保存");
    QAction *validateAction = m_toolbar->addAction("验证");
    QAction *runtimeAction = m_toolbar->addAction("运行时预览");
    
    connect(newAction, &QAction::triggered, this, &StateMachineEditor::createNewStateMachine);
    connect(validateAction, &QAction::triggered, this, &StateMachineEditor::validateStateMachine);
    connect(runtimeAction, &QAction::triggered, this, &StateMachineEditor::showRuntimePreview);
}

void StateMachineEditor::createPropertiesPanel()
{
    QVBoxLayout *layout = new QVBoxLayout(m_propertiesPanel);
    
    // 状态属性组
    QGroupBox *stateGroup = new QGroupBox("状态属性");
    QFormLayout *stateLayout = new QFormLayout(stateGroup);
    
    m_stateNameEdit = new QLineEdit();
    m_stateDescriptionEdit = new QTextEdit();
    m_stateDescriptionEdit->setMaximumHeight(80);
    m_uiInterfaceCombo = new QComboBox();
    m_initialStateCheck = new QCheckBox();
    m_finalStateCheck = new QCheckBox();
    
    stateLayout->addRow("名称:", m_stateNameEdit);
    stateLayout->addRow("描述:", m_stateDescriptionEdit);
    stateLayout->addRow("UI界面:", m_uiInterfaceCombo);
    stateLayout->addRow("初始状态:", m_initialStateCheck);
    stateLayout->addRow("终止状态:", m_finalStateCheck);
    
    QPushButton *applyStateBtn = new QPushButton("应用");
    stateLayout->addRow(applyStateBtn);
    connect(applyStateBtn, &QPushButton::clicked, this, &StateMachineEditor::editStateProperties);
    
    // 转换属性组 - 增强版
    QGroupBox *transitionGroup = new QGroupBox("转换属性");
    QVBoxLayout *transitionMainLayout = new QVBoxLayout(transitionGroup);
    
    // 基础事件设置
    QGroupBox *eventGroup = new QGroupBox("事件设置");
    QFormLayout *eventLayout = new QFormLayout(eventGroup);
    
    m_transitionEventEdit = new QLineEdit();
    m_transitionEventTypeCombo = new QComboBox();
    m_transitionEventTypeCombo->addItems({"按钮点击", "菜单操作", "定时器触发", "数据变更", "自定义事件"});
    m_transitionSourceEdit = new QLineEdit();
    m_transitionSourceEdit->setPlaceholderText("例如：button1, menuItem2");
    
    eventLayout->addRow("事件名称:", m_transitionEventEdit);
    eventLayout->addRow("事件类型:", m_transitionEventTypeCombo);
    eventLayout->addRow("事件源:", m_transitionSourceEdit);
    
    // 条件配置
    QGroupBox *conditionGroup = new QGroupBox("条件配置");
    QFormLayout *conditionLayout = new QFormLayout(conditionGroup);
    
    m_conditionVariableEdit = new QLineEdit();
    m_conditionOperatorCombo = new QComboBox();
    m_conditionOperatorCombo->addItems({"等于", "不等于", "大于", "小于", "包含", "为空"});
    m_conditionValueEdit = new QLineEdit();
    m_conditionLogicCombo = new QComboBox();
    m_conditionLogicCombo->addItems({"与", "或"});
    
    conditionLayout->addRow("条件变量:", m_conditionVariableEdit);
    conditionLayout->addRow("操作符:", m_conditionOperatorCombo);
    conditionLayout->addRow("比较值:", m_conditionValueEdit);
    conditionLayout->addRow("逻辑关系:", m_conditionLogicCombo);
    
    // 动作脚本
    QGroupBox *actionGroup = new QGroupBox("动作脚本");
    QVBoxLayout *actionLayout = new QVBoxLayout(actionGroup);
    
    m_transitionActionEdit = new QTextEdit();
    m_transitionActionEdit->setMaximumHeight(120);
    m_transitionActionEdit->setPlaceholderText("输入转换时执行的脚本代码...");
    
    QLabel *actionHint = new QLabel("支持JavaScript语法，可访问全局变量和UI组件");
    actionHint->setStyleSheet("color: gray; font-size: 10px;");
    
    actionLayout->addWidget(m_transitionActionEdit);
    actionLayout->addWidget(actionHint);
    
    // 应用按钮
    QPushButton *applyTransitionBtn = new QPushButton("应用转换设置");
    
    transitionMainLayout->addWidget(eventGroup);
    transitionMainLayout->addWidget(conditionGroup);
    transitionMainLayout->addWidget(actionGroup);
    transitionMainLayout->addWidget(applyTransitionBtn);
    
    connect(applyTransitionBtn, &QPushButton::clicked, this, &StateMachineEditor::editTransitionProperties);
    
    layout->addWidget(stateGroup);
    layout->addWidget(transitionGroup);
    layout->addStretch();
}

void StateMachineEditor::updatePropertiesPanel()
{
    // 更新状态属性
    m_stateNameEdit->setText(m_currentState.name);
    m_stateDescriptionEdit->setPlainText(m_currentState.description);
    m_initialStateCheck->setChecked(m_currentState.isInitialState);
    m_finalStateCheck->setChecked(m_currentState.isFinalState);
    
    // 更新UI界面下拉框
    m_uiInterfaceCombo->clear();
    
    // 添加产品配置中的UI文件信息
    if (!m_productUiFiles.isEmpty()) {
        for (const auto &uiFile : m_productUiFiles) {
            QString displayName = uiFile.name;
            // 使用文件路径作为唯一标识符
            m_uiInterfaceCombo->addItem(displayName, uiFile.filePath);
        }
    }
    
    // 设置当前选中的UI界面
    int index = m_uiInterfaceCombo->findData(m_currentState.uiInterfaceId);
    if (index >= 0) {
        m_uiInterfaceCombo->setCurrentIndex(index);
    }
    
    // 更新转换属性
    m_transitionEventEdit->setText(m_currentTransition.eventName);
    m_transitionEventTypeCombo->setCurrentIndex(static_cast<int>(m_currentTransition.eventType));
    m_transitionActionEdit->setPlainText(m_currentTransition.actionScript);
    
    // 更新条件配置
    m_transitionSourceEdit->setText(m_currentTransition.eventSource);
    m_conditionVariableEdit->setText(m_currentTransition.condition.variable);
    m_conditionOperatorCombo->setCurrentIndex(static_cast<int>(m_currentTransition.condition.operatorType));
    m_conditionValueEdit->setText(m_currentTransition.condition.value);
    m_conditionLogicCombo->setCurrentIndex(static_cast<int>(m_currentTransition.condition.logic));
}

void StateMachineEditor::setupRuntime()
{
    // 设置运行时引擎
    m_runtime->setProductUiFiles(m_productUiFiles);
    
    // 设置运行时视图
    m_runtimeView->setRuntime(m_runtime);
    m_runtimeView->setStateMachineEditor(this);
}

void StateMachineEditor::showRuntimePreview()
{
    if (!m_stateMachineManager || !m_stateMachineManager->currentStateMachine()) {
        QMessageBox::warning(this, "运行时预览", "请先创建或加载一个状态机");
        return;
    }
    
    // 设置当前状态机到运行时引擎
    m_runtime->setStateMachine(m_stateMachineManager->currentStateMachine());
    
    // 创建或显示运行时预览窗口
    if (!m_runtimeDock) {
        m_runtimeDock = new QDockWidget("状态机运行时预览", this);
        m_runtimeDock->setWidget(m_runtimeView);
        m_runtimeDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
        
        // 获取主窗口并添加停靠窗口
        QWidget *mainWindow = this->window();
        
        // 使用更安全的方法检查是否为QMainWindow
        if (mainWindow && mainWindow->inherits("QMainWindow")) {
            QMainWindow *mainWin = static_cast<QMainWindow*>(mainWindow);
            mainWin->addDockWidget(Qt::RightDockWidgetArea, m_runtimeDock);
        } else {
            // 如果没有主窗口，则显示为独立窗口
            m_runtimeDock->setWindowFlags(Qt::Window);
            m_runtimeDock->show();
        }
    }
    
    m_runtimeDock->show();
    m_runtimeDock->raise();
}