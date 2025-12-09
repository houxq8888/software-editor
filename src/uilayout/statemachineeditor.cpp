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
{
    setRect(-50, -30, 100, 60);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    
    m_nameText = new QGraphicsTextItem(m_state.name, this);
    m_nameText->setPos(-45, -25);
    m_nameText->setTextWidth(90);
    
    m_descriptionText = new QGraphicsTextItem(m_state.description, this);
    m_descriptionText->setPos(-45, -5);
    m_descriptionText->setTextWidth(90);
    m_descriptionText->setFont(QFont("Arial", 8));
    
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

void StateNode::updateAppearance()
{
    // 基础颜色设置
    if (m_state.isInitialState) {
        setBrush(QBrush(QColor(144, 238, 144))); // 浅绿色
    } else if (m_state.isFinalState) {
        setBrush(QBrush(QColor(255, 182, 193))); // 浅粉色
    } else {
        setBrush(QBrush(QColor(240, 240, 240))); // 浅灰色
    }
    
    setPen(QPen(Qt::black, 2));
    
    if (isSelected()) {
        setPen(QPen(Qt::blue, 3));
    }
    
    // 更新UI界面标识
    updateUiInterfaceIndicator();
}

void StateNode::updateUiInterfaceIndicator()
{
    // 移除旧的UI界面标识
    if (m_uiInterfaceIndicator) {
        scene()->removeItem(m_uiInterfaceIndicator);
        delete m_uiInterfaceIndicator;
        m_uiInterfaceIndicator = nullptr;
    }
    
    // 如果有关联的UI界面，添加标识
    if (!m_state.uiInterfaceId.isEmpty()) {
        // 创建UI界面标识（小图标或文字）
        m_uiInterfaceIndicator = new QGraphicsTextItem("UI", this);
        m_uiInterfaceIndicator->setFont(QFont("Arial", 8, QFont::Bold));
        m_uiInterfaceIndicator->setDefaultTextColor(QColor(0, 100, 0)); // 深绿色
        m_uiInterfaceIndicator->setPos(rect().right() - 25, rect().top() + 5);
        
        // 添加工具提示显示具体的UI界面ID
        m_uiInterfaceIndicator->setToolTip("关联UI界面: " + m_state.uiInterfaceId);
    }
}

void StateNode::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsRectItem::mousePressEvent(event);
    if (event->button() == Qt::LeftButton) {
        StateMachineScene *scene = qobject_cast<StateMachineScene*>(this->scene());
        if (scene) {
            emit scene->stateSelected(m_state);
        }
    }
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
{
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    
    m_eventText = new QGraphicsTextItem(m_transition.eventName, this);
    m_eventText->setFont(QFont("Arial", 9));
    
    // 创建箭头
    m_arrowHead = new QGraphicsPolygonItem(this);
    QPolygonF arrow;
    arrow << QPointF(0, 0) << QPointF(-10, -5) << QPointF(-10, 5);
    m_arrowHead->setPolygon(arrow);
    m_arrowHead->setBrush(QBrush(Qt::black));
    
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

void TransitionEdge::updatePosition()
{
    if (!m_fromNode || !m_toNode) return;
    
    QPointF fromCenter = m_fromNode->scenePos() + m_fromNode->rect().center();
    QPointF toCenter = m_toNode->scenePos() + m_toNode->rect().center();
    
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
    if (isSelected()) {
        setPen(QPen(Qt::blue, 3));
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

void StateMachineScene::createStateNode(const StateMachineState &state)
{
    StateNode *node = new StateNode(state);
    node->setPos(state.position);
    addItem(node);
    m_stateNodes[state.stateId] = node;
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
    
    if (m_stateMachineManager && m_stateMachineManager->currentStateMachine()) {
        m_stateMachineManager->currentStateMachine()->updateTransition(m_currentTransition);
        m_scene->refreshScene();
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
    
    // 转换属性组
    QGroupBox *transitionGroup = new QGroupBox("转换属性");
    QFormLayout *transitionLayout = new QFormLayout(transitionGroup);
    
    m_transitionEventEdit = new QLineEdit();
    m_transitionEventTypeCombo = new QComboBox();
    m_transitionEventTypeCombo->addItems({"按钮点击", "菜单操作", "定时器触发", "数据变更", "自定义事件"});
    m_transitionActionEdit = new QTextEdit();
    m_transitionActionEdit->setMaximumHeight(80);
    
    transitionLayout->addRow("事件名称:", m_transitionEventEdit);
    transitionLayout->addRow("事件类型:", m_transitionEventTypeCombo);
    transitionLayout->addRow("动作脚本:", m_transitionActionEdit);
    
    QPushButton *applyTransitionBtn = new QPushButton("应用");
    transitionLayout->addRow(applyTransitionBtn);
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