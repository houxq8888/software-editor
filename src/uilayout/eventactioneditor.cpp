#include "eventactioneditor.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolBar>
#include <QToolButton>
#include <QSpinBox>
#include <QLabel>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>
#include <QJsonDocument>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsLineItem>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QDebug>
#include <QUndoCommand>

// FlowNode 实现
FlowNode::FlowNode(NodeType type, const QString &title, const QString &description, QGraphicsItem *parent)
    : QGraphicsItem(parent)
    , m_type(type)
    , m_title(title)
    , m_description(description)
{
    m_nodeId = QUuid::createUuid().toString();
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
}

QRectF FlowNode::boundingRect() const
{
    return getNodeRect().adjusted(-5, -5, 5, 5);
}

void FlowNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    
    QRectF rect = getNodeRect();
    
    // 绘制节点背景
    painter->setBrush(QBrush(getNodeColor()));
    painter->setPen(QPen(Qt::black, 2));
    painter->drawRoundedRect(rect, 10, 10);
    
    // 绘制标题
    painter->setPen(Qt::black);
    QFont titleFont = painter->font();
    titleFont.setBold(true);
    painter->setFont(titleFont);
    painter->drawText(rect.adjusted(5, 5, -5, -20), Qt::AlignCenter, m_title);
    
    // 绘制描述
    painter->setPen(Qt::darkGray);
    QFont descFont = painter->font();
    descFont.setPointSize(descFont.pointSize() - 2);
    painter->setFont(descFont);
    painter->drawText(rect.adjusted(5, 30, -5, -5), Qt::AlignCenter, m_description);
    
    // 绘制选中状态
    if (isSelected()) {
        painter->setPen(QPen(Qt::blue, 3, Qt::DashLine));
        painter->setBrush(Qt::NoBrush);
        painter->drawRoundedRect(rect.adjusted(-2, -2, 2, 2), 12, 12);
    }
}

void FlowNode::addConnection(FlowNode *targetNode)
{
    if (!m_connections.contains(targetNode)) {
        m_connections.append(targetNode);
    }
}

void FlowNode::removeConnection(FlowNode *targetNode)
{
    m_connections.removeAll(targetNode);
}

void FlowNode::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    m_dragStartPos = pos();
    QGraphicsItem::mousePressEvent(event);
}

void FlowNode::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseMoveEvent(event);
}

void FlowNode::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseReleaseEvent(event);
}

void FlowNode::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    Q_UNUSED(event)
    // 上下文菜单在编辑器主类中处理
}

QColor FlowNode::getNodeColor() const
{
    switch (m_type) {
    case NodeType::EventNode:
        return QColor(255, 200, 200); // 浅红色
    case NodeType::ActionNode:
        return QColor(200, 255, 200); // 浅绿色
    case NodeType::ConditionNode:
        return QColor(255, 255, 200); // 浅黄色
    case NodeType::StartNode:
        return QColor(200, 200, 255); // 浅蓝色
    case NodeType::EndNode:
        return QColor(255, 200, 255); // 浅紫色
    default:
        return Qt::white;
    }
}

QRectF FlowNode::getNodeRect() const
{
    return QRectF(-50, -30, 100, 60);
}

// ConnectionLine 实现
ConnectionLine::ConnectionLine(FlowNode *sourceNode, FlowNode *targetNode, QGraphicsItem *parent)
    : QGraphicsItem(parent)
    , m_sourceNode(sourceNode)
    , m_targetNode(targetNode)
{
    setZValue(-1); // 确保连接线在节点下方
    updatePosition();
}

QRectF ConnectionLine::boundingRect() const
{
    qreal penWidth = 1;
    qreal extra = (penWidth + 10) / 2.0;
    
    return QRectF(m_sourcePoint, QSizeF(m_targetPoint.x() - m_sourcePoint.x(),
                                      m_targetPoint.y() - m_sourcePoint.y()))
        .normalized()
        .adjusted(-extra, -extra, extra, extra);
}

void ConnectionLine::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    
    QPen pen(Qt::black);
    pen.setWidth(2);
    painter->setPen(pen);
    
    // 绘制带箭头的直线
    painter->drawLine(m_sourcePoint, m_targetPoint);
    
    // 绘制箭头
    double angle = std::atan2(m_targetPoint.y() - m_sourcePoint.y(),
                             m_targetPoint.x() - m_sourcePoint.x());
    
    QPointF arrowP1 = m_targetPoint - QPointF(sin(angle + M_PI / 3) * 10,
                                             cos(angle + M_PI / 3) * 10);
    QPointF arrowP2 = m_targetPoint - QPointF(sin(angle + M_PI - M_PI / 3) * 10,
                                             cos(angle + M_PI - M_PI / 3) * 10);
    
    painter->setBrush(Qt::black);
    painter->drawPolygon(QPolygonF() << m_targetPoint << arrowP1 << arrowP2);
}

void ConnectionLine::updatePosition()
{
    if (!m_sourceNode || !m_targetNode) {
        return;
    }
    
    QRectF sourceRect = m_sourceNode->boundingRect();
    QRectF targetRect = m_targetNode->boundingRect();
    
    m_sourcePoint = m_sourceNode->mapToScene(sourceRect.center());
    m_targetPoint = m_targetNode->mapToScene(targetRect.center());
    
    prepareGeometryChange();
}

// EventActionEditor 实现
EventActionEditor::EventActionEditor(QWidget *parent)
    : QWidget(parent)
    , m_model(new EventActionModel(this))
    , m_undoStack(new QUndoStack(this))
    , m_debugMode(false)
{
    setupUI();
    setupToolbar();
    setupContextMenu();
    createDefaultNodes();
}

EventActionEditor::~EventActionEditor()
{
    clearScene();
}

void EventActionEditor::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // 创建工具栏
    QToolBar *toolbar = new QToolBar(this);
    mainLayout->addWidget(toolbar);
    
    // 创建图形视图
    m_scene = new QGraphicsScene(this);
    m_graphicsView = new QGraphicsView(m_scene, this);
    m_graphicsView->setRenderHint(QPainter::Antialiasing);
    m_graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
    m_graphicsView->setContextMenuPolicy(Qt::CustomContextMenu);
    
    mainLayout->addWidget(m_graphicsView);
    
    // 连接信号
    connect(m_graphicsView, &QGraphicsView::customContextMenuRequested,
            this, [this](const QPoint &pos) {
                QPoint globalPos = m_graphicsView->mapToGlobal(pos);
                m_contextMenu->exec(globalPos);
            });
}

void EventActionEditor::setupToolbar()
{
    QToolBar *toolbar = findChild<QToolBar*>();
    if (!toolbar) return;
    
    // 添加节点按钮
    QToolButton *addEventBtn = new QToolButton(toolbar);
    addEventBtn->setText("添加事件");
    addEventBtn->setToolTip("添加事件节点");
    connect(addEventBtn, &QToolButton::clicked, this, &EventActionEditor::onAddEventNode);
    toolbar->addWidget(addEventBtn);
    
    QToolButton *addActionBtn = new QToolButton(toolbar);
    addActionBtn->setText("添加动作");
    addActionBtn->setToolTip("添加动作节点");
    connect(addActionBtn, &QToolButton::clicked, this, &EventActionEditor::onAddActionNode);
    toolbar->addWidget(addActionBtn);
    
    QToolButton *addConditionBtn = new QToolButton(toolbar);
    addConditionBtn->setText("添加条件");
    addConditionBtn->setToolTip("添加条件节点");
    connect(addConditionBtn, &QToolButton::clicked, this, &EventActionEditor::onAddConditionNode);
    toolbar->addWidget(addConditionBtn);
    
    toolbar->addSeparator();
    
    // 连接操作按钮
    QToolButton *connectBtn = new QToolButton(toolbar);
    connectBtn->setText("连接");
    connectBtn->setToolTip("连接选中的节点");
    connect(connectBtn, &QToolButton::clicked, this, &EventActionEditor::onConnectNodes);
    toolbar->addWidget(connectBtn);
    
    QToolButton *disconnectBtn = new QToolButton(toolbar);
    disconnectBtn->setText("断开");
    disconnectBtn->setToolTip("断开选中的连接");
    connect(disconnectBtn, &QToolButton::clicked, this, &EventActionEditor::onDisconnectNodes);
    toolbar->addWidget(disconnectBtn);
    
    toolbar->addSeparator();
    
    // 缩放按钮
    QToolButton *zoomInBtn = new QToolButton(toolbar);
    zoomInBtn->setText("放大");
    connect(zoomInBtn, &QToolButton::clicked, this, &EventActionEditor::onZoomIn);
    toolbar->addWidget(zoomInBtn);
    
    QToolButton *zoomOutBtn = new QToolButton(toolbar);
    zoomOutBtn->setText("缩小");
    connect(zoomOutBtn, &QToolButton::clicked, this, &EventActionEditor::onZoomOut);
    toolbar->addWidget(zoomOutBtn);
    
    QToolButton *fitBtn = new QToolButton(toolbar);
    fitBtn->setText("适应视图");
    connect(fitBtn, &QToolButton::clicked, this, &EventActionEditor::onFitToView);
    toolbar->addWidget(fitBtn);
    
    toolbar->addSeparator();
    
    // 验证和测试按钮
    QToolButton *validateBtn = new QToolButton(toolbar);
    validateBtn->setText("验证规则");
    connect(validateBtn, &QToolButton::clicked, this, &EventActionEditor::onValidateRules);
    toolbar->addWidget(validateBtn);
    
    QToolButton *testBtn = new QToolButton(toolbar);
    testBtn->setText("测试规则");
    connect(testBtn, &QToolButton::clicked, this, &EventActionEditor::onTestRules);
    toolbar->addWidget(testBtn);
}

void EventActionEditor::setupContextMenu()
{
    m_contextMenu = new QMenu(this);
    
    m_addEventAction = m_contextMenu->addAction("添加事件节点", this, &EventActionEditor::onAddEventNode);
    m_addActionAction = m_contextMenu->addAction("添加动作节点", this, &EventActionEditor::onAddActionNode);
    m_addConditionAction = m_contextMenu->addAction("添加条件节点", this, &EventActionEditor::onAddConditionNode);
    m_contextMenu->addSeparator();
    m_deleteAction = m_contextMenu->addAction("删除选中节点", this, &EventActionEditor::onDeleteSelectedNodes);
    m_contextMenu->addSeparator();
    m_connectAction = m_contextMenu->addAction("连接节点", this, &EventActionEditor::onConnectNodes);
    m_disconnectAction = m_contextMenu->addAction("断开连接", this, &EventActionEditor::onDisconnectNodes);
    m_contextMenu->addSeparator();
    m_propertiesAction = m_contextMenu->addAction("属性", this, &EventActionEditor::onNodeProperties);
}

void EventActionEditor::createDefaultNodes()
{
    // 创建开始和结束节点
    FlowNode *startNode = createNode(NodeType::StartNode, "开始", "流程开始", QPointF(-200, 0));
    FlowNode *endNode = createNode(NodeType::EndNode, "结束", "流程结束", QPointF(200, 0));
    
    // 创建示例事件和动作节点
    FlowNode *eventNode = createNode(NodeType::EventNode, "按钮点击", "用户点击按钮", QPointF(-100, 0));
    FlowNode *actionNode = createNode(NodeType::ActionNode, "显示消息", "显示提示信息", QPointF(100, 0));
    
    // 创建连接
    createConnection(startNode, eventNode);
    createConnection(eventNode, actionNode);
    createConnection(actionNode, endNode);
}

void EventActionEditor::setModel(EventActionModel *model)
{
    if (m_model) {
        disconnect(m_model, &EventActionModel::ruleAdded, this, &EventActionEditor::onRuleAdded);
        disconnect(m_model, &EventActionModel::ruleRemoved, this, &EventActionEditor::onRuleRemoved);
        disconnect(m_model, &EventActionModel::ruleModified, this, &EventActionEditor::onRuleModified);
    }
    
    m_model = model;
    
    if (m_model) {
        connect(m_model, &EventActionModel::ruleAdded, this, &EventActionEditor::onRuleAdded);
        connect(m_model, &EventActionModel::ruleRemoved, this, &EventActionEditor::onRuleRemoved);
        connect(m_model, &EventActionModel::ruleModified, this, &EventActionEditor::onRuleModified);
        
        updateScene();
    }
}

bool EventActionEditor::loadFromFile(const QString &filePath)
{
    if (!m_model) {
        return false;
    }
    
    if (m_model->loadFromFile(filePath)) {
        updateScene();
        return true;
    }
    
    return false;
}

bool EventActionEditor::saveToFile(const QString &filePath)
{
    if (!m_model) {
        return false;
    }
    
    return m_model->saveToFile(filePath);
}

void EventActionEditor::applyTemplate(const QString &templateName)
{
    if (!m_model) {
        return;
    }
    
    InteractionTemplate interactionTemplate = m_model->getTemplate(templateName);
    if (interactionTemplate.name().isEmpty()) {
        QMessageBox::warning(this, "模板应用", "未找到指定的模板: " + templateName);
        return;
    }
    
    // 清空当前场景
    clearScene();
    
    // 应用模板规则
    for (const auto &rule : interactionTemplate.rules()) {
        m_model->addRule(rule);
    }
    
    updateScene();
}

void EventActionEditor::startDebugMode()
{
    m_debugMode = true;
    m_graphicsView->setDragMode(QGraphicsView::NoDrag);
    // 可以添加调试相关的视觉反馈
}

void EventActionEditor::stopDebugMode()
{
    m_debugMode = false;
    m_graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
}

void EventActionEditor::executeRule(const QString &ruleId)
{
    if (!m_model) {
        return;
    }
    
    EventActionRule rule = m_model->getRule(ruleId);
    if (rule.id().isEmpty()) {
        return;
    }
    
    // 在调试模式下高亮显示执行的规则
    if (m_debugMode && m_ruleNodes.contains(ruleId)) {
        FlowNode *node = m_ruleNodes[ruleId];
        node->setSelected(true);
        // 可以添加动画效果
    }
}

void EventActionEditor::onRuleAdded(const QString &ruleId)
{
    if (!m_model) {
        return;
    }
    
    EventActionRule rule = m_model->getRule(ruleId);
    
    // 创建对应的流程图节点
    FlowNode *eventNode = createNode(NodeType::EventNode, 
                                    "事件: " + rule.event().sourceWidgetId(),
                                    getEventTypeString(rule.event().type()),
                                    QPointF(0, m_ruleNodes.size() * 100));
    
    // 为每个动作创建节点
    for (int i = 0; i < rule.actions().size(); ++i) {
        const ActionDefinition &action = rule.actions()[i];
        FlowNode *actionNode = createNode(NodeType::ActionNode,
                                         "动作: " + action.targetWidgetId(),
                                         getActionTypeString(action.type()),
                                         QPointF(150, m_ruleNodes.size() * 100 + i * 80));
        
        createConnection(eventNode, actionNode);
    }
    
    m_ruleNodes[ruleId] = eventNode;
}

void EventActionEditor::onRuleRemoved(const QString &ruleId)
{
    if (m_ruleNodes.contains(ruleId)) {
        FlowNode *node = m_ruleNodes[ruleId];
        m_scene->removeItem(node);
        delete node;
        m_ruleNodes.remove(ruleId);
    }
}

void EventActionEditor::onRuleModified(const QString &ruleId)
{
    onRuleRemoved(ruleId);
    onRuleAdded(ruleId);
}

void EventActionEditor::onAddEventNode()
{
    bool ok;
    QString title = QInputDialog::getText(this, "添加事件节点", "请输入事件标题:", QLineEdit::Normal, "新事件", &ok);
    if (ok && !title.isEmpty()) {
        QString description = QInputDialog::getText(this, "事件描述", "请输入事件描述:", QLineEdit::Normal, "", &ok);
        if (ok) {
            createNode(NodeType::EventNode, title, description, QPointF(0, 0));
        }
    }
}

void EventActionEditor::onAddActionNode()
{
    bool ok;
    QString title = QInputDialog::getText(this, "添加动作节点", "请输入动作标题:", QLineEdit::Normal, "新动作", &ok);
    if (ok && !title.isEmpty()) {
        QString description = QInputDialog::getText(this, "动作描述", "请输入动作描述:", QLineEdit::Normal, "", &ok);
        if (ok) {
            createNode(NodeType::ActionNode, title, description, QPointF(0, 0));
        }
    }
}

void EventActionEditor::onAddConditionNode()
{
    bool ok;
    QString title = QInputDialog::getText(this, "添加条件节点", "请输入条件标题:", QLineEdit::Normal, "新条件", &ok);
    if (ok && !title.isEmpty()) {
        QString description = QInputDialog::getText(this, "条件描述", "请输入条件描述:", QLineEdit::Normal, "", &ok);
        if (ok) {
            createNode(NodeType::ConditionNode, title, description, QPointF(0, 0));
        }
    }
}

void EventActionEditor::onDeleteSelectedNodes()
{
    QList<QGraphicsItem*> selectedItems = m_scene->selectedItems();
    for (QGraphicsItem *item : selectedItems) {
        if (FlowNode *node = dynamic_cast<FlowNode*>(item)) {
            m_scene->removeItem(node);
            delete node;
        }
    }
}

void EventActionEditor::onConnectNodes()
{
    QList<QGraphicsItem*> selectedItems = m_scene->selectedItems();
    if (selectedItems.size() == 2) {
        FlowNode *source = dynamic_cast<FlowNode*>(selectedItems[0]);
        FlowNode *target = dynamic_cast<FlowNode*>(selectedItems[1]);
        
        if (source && target) {
            createConnection(source, target);
        }
    }
}

void EventActionEditor::onDisconnectNodes()
{
    // 实现断开连接逻辑
}

void EventActionEditor::onNodeProperties()
{
    // 实现节点属性编辑
}

void EventActionEditor::onZoomIn()
{
    m_graphicsView->scale(1.2, 1.2);
}

void EventActionEditor::onZoomOut()
{
    m_graphicsView->scale(0.8, 0.8);
}

void EventActionEditor::onFitToView()
{
    m_graphicsView->fitInView(m_scene->itemsBoundingRect(), Qt::KeepAspectRatio);
}

void EventActionEditor::onValidateRules()
{
    if (!m_model) {
        return;
    }
    
    QStringList errors;
    for (const auto &rule : m_model->getAllRules()) {
        if (!m_model->validateRule(rule)) {
            errors.append(m_model->getValidationErrors());
        }
    }
    
    if (errors.isEmpty()) {
        QMessageBox::information(this, "验证结果", "所有规则验证通过！");
    } else {
        QMessageBox::warning(this, "验证结果", "发现以下错误:\n" + errors.join("\n"));
    }
}

void EventActionEditor::onTestRules()
{
    startDebugMode();
    QMessageBox::information(this, "测试模式", "已进入测试模式，可以执行规则进行测试。");
}

void EventActionEditor::updateScene()
{
    clearScene();
    
    if (!m_model) {
        return;
    }
    
    // 为每个规则创建节点
    for (const auto &rule : m_model->getAllRules()) {
        onRuleAdded(rule.id());
    }
}

void EventActionEditor::clearScene()
{
    m_scene->clear();
    m_ruleNodes.clear();
    m_connections.clear();
}

FlowNode* EventActionEditor::createNode(NodeType type, const QString &title, const QString &description, const QPointF &pos)
{
    FlowNode *node = new FlowNode(type, title, description);
    node->setPos(pos);
    m_scene->addItem(node);
    return node;
}

void EventActionEditor::createConnection(FlowNode *source, FlowNode *target)
{
    if (!source || !target) {
        return;
    }
    
    source->addConnection(target);
    
    ConnectionLine *connection = new ConnectionLine(source, target);
    m_scene->addItem(connection);
    m_connections.append(connection);
}

void EventActionEditor::removeConnection(FlowNode *source, FlowNode *target)
{
    if (!source || !target) {
        return;
    }
    
    source->removeConnection(target);
    
    // 移除对应的连接线
    for (int i = m_connections.size() - 1; i >= 0; --i) {
        ConnectionLine *connection = m_connections[i];
        if (connection->sourceNode() == source && connection->targetNode() == target) {
            m_scene->removeItem(connection);
            m_connections.removeAt(i);
            delete connection;
        }
    }
}

QString EventActionEditor::getEventTypeString(EventType type)
{
    switch (type) {
    case EventType::ButtonClick: return "按钮点击";
    case EventType::TextChanged: return "文本改变";
    case EventType::ValueChanged: return "值改变";
    case EventType::SelectionChanged: return "选择改变";
    case EventType::FocusIn: return "获得焦点";
    case EventType::FocusOut: return "失去焦点";
    case EventType::MouseEnter: return "鼠标进入";
    case EventType::MouseLeave: return "鼠标离开";
    case EventType::CustomEvent: return "自定义事件";
    default: return "未知事件";
    }
}

QString EventActionEditor::getActionTypeString(ActionType type)
{
    switch (type) {
    case ActionType::ShowWidget: return "显示控件";
    case ActionType::HideWidget: return "隐藏控件";
    case ActionType::EnableWidget: return "启用控件";
    case ActionType::DisableWidget: return "禁用控件";
    case ActionType::SetText: return "设置文本";
    case ActionType::SetValue: return "设置值";
    case ActionType::ExecuteScript: return "执行脚本";
    case ActionType::Navigate: return "导航";
    case ActionType::ValidateForm: return "验证表单";
    case ActionType::SubmitForm: return "提交表单";
    case ActionType::CustomAction: return "自定义动作";
    default: return "未知动作";
    }
}