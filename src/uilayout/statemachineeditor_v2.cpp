#include "statemachineeditor_v2.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDebug>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QRandomGenerator>

// UIFlowStateNode 实现
UIFlowStateNode::UIFlowStateNode(const UIFlowStateMachine::UIFlowState &state, QGraphicsItem *parent)
    : QObject(), QGraphicsRectItem(parent)
    , m_state(state)
    , m_nameText(nullptr)
    , m_descriptionText(nullptr)
    , m_uiInterfaceIndicator(nullptr)
    , m_uiIcon(nullptr)
    , m_shadowEffect(nullptr)
    , m_selectionAnimation(nullptr)
    , m_isHovered(false)
    , m_isDragging(false)
    , m_isHighlighted(false)
{
    setRect(-50, -30, 100, 60);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    
    createVisualEffects();
    updateAppearance();
}

UIFlowStateMachine::UIFlowState UIFlowStateNode::state() const
{
    return m_state;
}

void UIFlowStateNode::setState(const UIFlowStateMachine::UIFlowState &state)
{
    m_state = state;
    updateAppearance();
}

void UIFlowStateNode::updateAppearance()
{
    if (!m_nameText) {
        m_nameText = new QGraphicsTextItem(this);
        m_nameText->setFont(QFont("Arial", 10, QFont::Bold));
        m_nameText->setDefaultTextColor(Qt::black);
        m_nameText->setTextWidth(80);
        m_nameText->setPos(-40, -25);
    }
    
    if (!m_descriptionText) {
        m_descriptionText = new QGraphicsTextItem(this);
        m_descriptionText->setFont(QFont("Arial", 8));
        m_descriptionText->setDefaultTextColor(Qt::darkGray);
        m_descriptionText->setTextWidth(80);
        m_descriptionText->setPos(-40, -5);
    }
    
    m_nameText->setPlainText(m_state.name);
    m_descriptionText->setPlainText(m_state.description);
    
    updateUiInterfaceIndicator();
    updateNodeStyle();
}

void UIFlowStateNode::animateSelection()
{
    if (!m_selectionAnimation) {
        m_selectionAnimation = new QPropertyAnimation(this, "scale");
        m_selectionAnimation->setDuration(200);
        m_selectionAnimation->setEasingCurve(QEasingCurve::OutCubic);
    }
    
    m_selectionAnimation->setStartValue(1.0);
    m_selectionAnimation->setEndValue(1.1);
    m_selectionAnimation->start();
}

void UIFlowStateNode::setHighlighted(bool highlighted)
{
    m_isHighlighted = highlighted;
    updateNodeStyle();
}

QPointF UIFlowStateNode::getConnectionPoint(const QPointF &targetPoint) const
{
    QPointF center = rect().center();
    QPointF direction = targetPoint - center;
    
    // 计算连接点
    qreal angle = atan2(direction.y(), direction.x());
    qreal radiusX = rect().width() / 2;
    qreal radiusY = rect().height() / 2;
    
    return center + QPointF(radiusX * cos(angle), radiusY * sin(angle));
}

void UIFlowStateNode::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_isDragging = true;
        m_dragStartPos = pos();
        
        UIFlowStateMachineScene *scene = qobject_cast<UIFlowStateMachineScene*>(this->scene());
        if (scene) {
            emit scene->stateSelected(m_state);
        }
    }
    
    QGraphicsRectItem::mousePressEvent(event);
}

void UIFlowStateNode::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    // 双击编辑状态属性
    QGraphicsRectItem::mouseDoubleClickEvent(event);
}

void UIFlowStateNode::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_isDragging) {
        QGraphicsRectItem::mouseMoveEvent(event);
    }
}

void UIFlowStateNode::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_isDragging = false;
    }
    
    QGraphicsRectItem::mouseReleaseEvent(event);
}

void UIFlowStateNode::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    m_isHovered = true;
    updateNodeStyle();
    QGraphicsRectItem::hoverEnterEvent(event);
}

void UIFlowStateNode::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    m_isHovered = false;
    updateNodeStyle();
    QGraphicsRectItem::hoverLeaveEvent(event);
}

void UIFlowStateNode::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QMenu menu;
    menu.addAction("编辑状态");
    menu.addAction("删除状态");
    menu.addSeparator();
    menu.addAction("添加转换");
    
    QAction *selectedAction = menu.exec(event->screenPos());
    if (selectedAction) {
        // 处理菜单操作
    }
}

void UIFlowStateNode::updateUiInterfaceIndicator()
{
    if (!m_uiInterfaceIndicator) {
        m_uiInterfaceIndicator = new QGraphicsTextItem(this);
        m_uiInterfaceIndicator->setFont(QFont("Arial", 8));
        m_uiInterfaceIndicator->setDefaultTextColor(Qt::green);
        m_uiInterfaceIndicator->setPos(5, 5);
    }
    
    if (!m_state.uiInterfaceId.isEmpty()) {
        m_uiInterfaceIndicator->setPlainText("UI");
        m_uiInterfaceIndicator->setToolTip("关联UI界面: " + m_state.uiInterfaceId);
    } else {
        m_uiInterfaceIndicator->setPlainText("");
    }
}

void UIFlowStateNode::createVisualEffects()
{
    m_shadowEffect = new QGraphicsDropShadowEffect(this);
    m_shadowEffect->setBlurRadius(10);
    m_shadowEffect->setColor(QColor(0, 0, 0, 80));
    m_shadowEffect->setOffset(2, 2);
    setGraphicsEffect(m_shadowEffect);
}

void UIFlowStateNode::updateNodeStyle()
{
    QBrush brush;
    QPen pen;
    
    if (m_isHighlighted) {
        brush = QBrush(QColor(255, 255, 0, 100));
        pen = QPen(Qt::red, 2);
    } else if (m_isHovered) {
        brush = QBrush(QColor(200, 230, 255, 150));
        pen = QPen(Qt::blue, 2);
    } else if (m_state.isInitialState) {
        brush = QBrush(QColor(200, 255, 200, 150));
        pen = QPen(Qt::darkGreen, 2);
    } else if (m_state.isFinalState) {
        brush = QBrush(QColor(255, 200, 200, 150));
        pen = QPen(Qt::darkRed, 2);
    } else {
        brush = QBrush(QColor(240, 240, 240, 200));
        pen = QPen(Qt::black, 1);
    }
    
    setBrush(brush);
    setPen(pen);
}

// StateMachineEditorV2 实现
StateMachineEditorV2::StateMachineEditorV2(QWidget *parent)
    : QWidget(parent)
    , m_stateMachineManager(nullptr)
    , m_integrationManager(nullptr)
    , m_view(nullptr)
    , m_uiFlowScene(nullptr)
    , m_logicScene(nullptr)
    , m_propertiesPanel(nullptr)
    , m_toolbar(nullptr)
    , m_undoStack(nullptr)
    , m_modeGroup(nullptr)
    , m_uiFlowModeRadio(nullptr)
    , m_logicSequenceModeRadio(nullptr)
    , m_integratedModeRadio(nullptr)
    , m_modeDescription(nullptr)
    , m_runtime(nullptr)
    , m_runtimeView(nullptr)
    , m_runtimeDock(nullptr)
    , m_product(nullptr)
    , m_propertiesTab(nullptr)
    , m_uiFlowProperties(nullptr)
    , m_logicProperties(nullptr)
    , m_currentMode(StateMachineMode::UIFlowOnly)
{
    // 创建集成管理器
    m_integrationManager = new StateMachineIntegrationManager(this);
    
    // 创建UI
    createModeSelector();
    createToolbar();
    createPropertiesPanel();
    createRuntimePreview();
    
    // 设置布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_modeGroup);
    mainLayout->addWidget(m_toolbar);
    
    QHBoxLayout *contentLayout = new QHBoxLayout();
    contentLayout->addWidget(m_view, 3);
    contentLayout->addWidget(m_propertiesPanel, 1);
    
    mainLayout->addLayout(contentLayout);
    
    // 初始模式设置
    switchToUIFlowMode();
}

void StateMachineEditorV2::setStateMachineManager(StateMachineManager *manager)
{
    m_stateMachineManager = manager;
}

void StateMachineEditorV2::setProductUiFiles(const QList<ProductUIFile> &uiFiles)
{
    m_productUiFiles = uiFiles;
    
    // 更新UI界面下拉框
    if (m_uiInterfaceCombo) {
        m_uiInterfaceCombo->clear();
        for (const auto &uiFile : uiFiles) {
            // 使用文件路径作为标识符，因为ProductUIFile没有id成员
            m_uiInterfaceCombo->addItem(uiFile.name, uiFile.filePath);
        }
    }
}

void StateMachineEditorV2::setProduct(Product *product)
{
    m_product = product;
}

void StateMachineEditorV2::loadStateMachine(const QString &filePath)
{
    if (filePath.isEmpty()) return;
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "错误", "无法打开状态机文件: " + filePath);
        return;
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    
    if (doc.isNull()) {
        QMessageBox::warning(this, "错误", "状态机文件格式错误: " + filePath);
        return;
    }
    
    // 根据文件内容判断模式并加载
    QJsonObject json = doc.object();
    
    if (json.contains("integrationMode")) {
        // 集成模式文件
        m_integrationManager->fromJson(json);
        switchToIntegratedMode();
    } else if (json.contains("logicStates")) {
        // 逻辑时序状态机文件
        if (!m_integrationManager->logicSequenceStateMachine()) {
            m_integrationManager->setLogicSequenceStateMachine(new LogicSequenceStateMachine("逻辑时序状态机", this));
        }
        m_integrationManager->logicSequenceStateMachine()->fromJson(json);
        switchToLogicSequenceMode();
    } else {
        // UI流状态机文件
        if (!m_integrationManager->uiFlowStateMachine()) {
            m_integrationManager->setUiFlowStateMachine(new UIFlowStateMachine("UI流状态机", this));
        }
        m_integrationManager->uiFlowStateMachine()->fromJson(json);
        switchToUIFlowMode();
    }
    
    QMessageBox::information(this, "成功", "状态机文件已加载: " + filePath);
}

void StateMachineEditorV2::saveStateMachine(const QString &filePath)
{
    if (filePath.isEmpty()) return;
    
    QJsonObject json;
    
    switch (m_currentMode) {
    case StateMachineMode::UIFlowOnly:
        if (m_integrationManager->uiFlowStateMachine()) {
            json = m_integrationManager->uiFlowStateMachine()->toJson();
        }
        break;
    case StateMachineMode::LogicSequenceOnly:
        if (m_integrationManager->logicSequenceStateMachine()) {
            json = m_integrationManager->logicSequenceStateMachine()->toJson();
        }
        break;
    case StateMachineMode::Integrated:
        json = m_integrationManager->toJson();
        break;
    }
    
    if (json.isEmpty()) {
        QMessageBox::warning(this, "错误", "没有可保存的状态机数据");
        return;
    }
    
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, "错误", "无法保存状态机文件: " + filePath);
        return;
    }
    
    QJsonDocument doc(json);
    file.write(doc.toJson());
    file.close();
    
    QMessageBox::information(this, "成功", "状态机文件已保存: " + filePath);
}

StateMachineMode StateMachineEditorV2::currentMode() const
{
    return m_currentMode;
}

void StateMachineEditorV2::createNewStateMachine()
{
    switch (m_currentMode) {
    case StateMachineMode::UIFlowOnly:
        if (!m_integrationManager->uiFlowStateMachine()) {
            m_integrationManager->setUiFlowStateMachine(new UIFlowStateMachine("UI流状态机", this));
        }
        // 通过创建新的状态机来"清除"现有内容
        m_integrationManager->setUiFlowStateMachine(new UIFlowStateMachine("UI流状态机", this));
        break;
    case StateMachineMode::LogicSequenceOnly:
        if (!m_integrationManager->logicSequenceStateMachine()) {
            m_integrationManager->setLogicSequenceStateMachine(new LogicSequenceStateMachine("逻辑时序状态机", this));
        }
        // 通过创建新的状态机来"清除"现有内容
        m_integrationManager->setLogicSequenceStateMachine(new LogicSequenceStateMachine("逻辑时序状态机", this));
        break;
    case StateMachineMode::Integrated:
        if (!m_integrationManager->uiFlowStateMachine()) {
            m_integrationManager->setUiFlowStateMachine(new UIFlowStateMachine("UI流状态机", this));
        }
        if (!m_integrationManager->logicSequenceStateMachine()) {
            m_integrationManager->setLogicSequenceStateMachine(new LogicSequenceStateMachine("逻辑时序状态机", this));
        }
        // 通过创建新的状态机来"清除"现有内容
        m_integrationManager->setUiFlowStateMachine(new UIFlowStateMachine("UI流状态机", this));
        m_integrationManager->setLogicSequenceStateMachine(new LogicSequenceStateMachine("逻辑时序状态机", this));
        m_integrationManager->mappings().clear();
        break;
    }
    
    if (m_uiFlowScene) m_uiFlowScene->refreshScene();
    if (m_logicScene) m_logicScene->refreshScene();
    
    QMessageBox::information(this, "成功", "已创建新的状态机");
}

void StateMachineEditorV2::editStateProperties()
{
    // 根据当前模式编辑状态属性
    switch (m_currentMode) {
    case StateMachineMode::UIFlowOnly:
        if (!m_currentUIFlowState.stateId.isEmpty()) {
            // 打开UI流状态属性编辑器
        }
        break;
    case StateMachineMode::LogicSequenceOnly:
        if (!m_currentLogicState.stateId.isEmpty()) {
            // 打开逻辑状态属性编辑器
        }
        break;
    case StateMachineMode::Integrated:
        // 集成模式下需要同时编辑两个状态
        break;
    }
}

void StateMachineEditorV2::editTransitionProperties()
{
    // 根据当前模式编辑转换属性
    switch (m_currentMode) {
    case StateMachineMode::UIFlowOnly:
        if (!m_currentUIFlowTransition.transitionId.isEmpty()) {
            // 打开UI流转换属性编辑器
        }
        break;
    case StateMachineMode::LogicSequenceOnly:
        if (!m_currentLogicTransition.transitionId.isEmpty()) {
            // 打开逻辑转换属性编辑器
        }
        break;
    case StateMachineMode::Integrated:
        // 集成模式下需要同时编辑两个转换
        break;
    }
}

void StateMachineEditorV2::setInitialState()
{
    // 根据当前模式设置初始状态
    switch (m_currentMode) {
    case StateMachineMode::UIFlowOnly:
        if (!m_currentUIFlowState.stateId.isEmpty()) {
            m_integrationManager->uiFlowStateMachine()->setInitialState(m_currentUIFlowState.stateId);
            if (m_uiFlowScene) m_uiFlowScene->refreshScene();
        }
        break;
    case StateMachineMode::LogicSequenceOnly:
        if (!m_currentLogicState.stateId.isEmpty()) {
            m_integrationManager->logicSequenceStateMachine()->setInitialState(m_currentLogicState.stateId);
            if (m_logicScene) m_logicScene->refreshScene();
        }
        break;
    case StateMachineMode::Integrated:
        // 集成模式下需要同时设置两个状态机
        break;
    }
}

void StateMachineEditorV2::validateStateMachine()
{
    bool valid = false;
    QString message;
    
    switch (m_currentMode) {
    case StateMachineMode::UIFlowOnly:
        if (m_integrationManager->uiFlowStateMachine()) {
            valid = m_integrationManager->uiFlowStateMachine()->validate();
            message = valid ? "UI流状态机验证通过" : "UI流状态机验证失败";
        }
        break;
    case StateMachineMode::LogicSequenceOnly:
        if (m_integrationManager->logicSequenceStateMachine()) {
            valid = m_integrationManager->logicSequenceStateMachine()->validate();
            message = valid ? "逻辑时序状态机验证通过" : "逻辑时序状态机验证失败";
        }
        break;
    case StateMachineMode::Integrated:
        valid = m_integrationManager->synchronizeStates();
        message = valid ? "集成状态机验证通过" : "集成状态机验证失败";
        break;
    }
    
    QMessageBox::information(this, valid ? "验证通过" : "验证失败", message);
}

void StateMachineEditorV2::showRuntimePreview()
{
    if (!m_runtimeDock) return;
    
    m_runtimeDock->show();
    m_runtimeDock->raise();
}

void StateMachineEditorV2::switchToUIFlowMode()
{
    switchMode(StateMachineMode::UIFlowOnly);
}

void StateMachineEditorV2::switchToLogicSequenceMode()
{
    switchMode(StateMachineMode::LogicSequenceOnly);
}

void StateMachineEditorV2::switchToIntegratedMode()
{
    switchMode(StateMachineMode::Integrated);
}

void StateMachineEditorV2::onStateSelected(const UIFlowStateMachine::UIFlowState &state)
{
    m_currentUIFlowState = state;
    updatePropertiesPanel();
}

void StateMachineEditorV2::onTransitionSelected(const UIFlowStateMachine::UIFlowTransition &transition)
{
    m_currentUIFlowTransition = transition;
    updatePropertiesPanel();
}

void StateMachineEditorV2::onLogicStateSelected(const LogicSequenceStateMachine::LogicState &state)
{
    m_currentLogicState = state;
    updatePropertiesPanel();
}

void StateMachineEditorV2::onLogicTransitionSelected(const LogicSequenceStateMachine::LogicTransition &transition)
{
    m_currentLogicTransition = transition;
    updatePropertiesPanel();
}

void StateMachineEditorV2::onStateMachineChanged()
{
    // 状态机变化时更新显示
    if (m_uiFlowScene) m_uiFlowScene->refreshScene();
    if (m_logicScene) m_logicScene->refreshScene();
}

void StateMachineEditorV2::onUiInterfaceChanged(int index)
{
    if (index < 0 || !m_integrationManager->uiFlowStateMachine()) return;
    
    QString uiInterfaceId = m_uiInterfaceCombo->currentData().toString();
    
    if (m_currentUIFlowState.stateId.isEmpty()) {
        qDebug() << "当前没有选中任何UI流状态，UI界面选择不会生效";
        return;
    }
    
    // 更新当前状态的UI界面ID
    m_currentUIFlowState.uiInterfaceId = uiInterfaceId;
    m_integrationManager->uiFlowStateMachine()->updateState(m_currentUIFlowState);
    
    if (m_uiFlowScene) {
        m_uiFlowScene->refreshScene();
        qDebug() << "UI流状态" << m_currentUIFlowState.name << "的UI界面已更新为:" 
                 << m_uiInterfaceCombo->currentText() << "(" << uiInterfaceId << ")";
    }
}

void StateMachineEditorV2::onModeChanged()
{
    if (m_uiFlowModeRadio->isChecked()) {
        switchToUIFlowMode();
    } else if (m_logicSequenceModeRadio->isChecked()) {
        switchToLogicSequenceMode();
    } else if (m_integratedModeRadio->isChecked()) {
        switchToIntegratedMode();
    }
}

void StateMachineEditorV2::createModeSelector()
{
    m_modeGroup = new QGroupBox("状态机模式", this);
    QVBoxLayout *modeLayout = new QVBoxLayout(m_modeGroup);
    
    // 创建模式选择按钮
    m_uiFlowModeRadio = new QRadioButton("UI流模式", m_modeGroup);
    m_logicSequenceModeRadio = new QRadioButton("逻辑时序模式", m_modeGroup);
    m_integratedModeRadio = new QRadioButton("集成模式", m_modeGroup);
    
    m_uiFlowModeRadio->setChecked(true);
    
    modeLayout->addWidget(m_uiFlowModeRadio);
    modeLayout->addWidget(m_logicSequenceModeRadio);
    modeLayout->addWidget(m_integratedModeRadio);
    
    // 模式描述标签
    m_modeDescription = new QLabel("专注于UI界面的串联和跳转", m_modeGroup);
    m_modeDescription->setWordWrap(true);
    modeLayout->addWidget(m_modeDescription);
    
    // 连接信号
    connect(m_uiFlowModeRadio, &QRadioButton::toggled, this, &StateMachineEditorV2::onModeChanged);
    connect(m_logicSequenceModeRadio, &QRadioButton::toggled, this, &StateMachineEditorV2::onModeChanged);
    connect(m_integratedModeRadio, &QRadioButton::toggled, this, &StateMachineEditorV2::onModeChanged);
}

void StateMachineEditorV2::createToolbar()
{
    m_toolbar = new QToolBar(this);
    
    // 文件操作
    QAction *newAction = new QAction("新建", this);
    QAction *openAction = new QAction("打开", this);
    QAction *saveAction = new QAction("保存", this);
    
    // 编辑操作
    QAction *addStateAction = new QAction("添加状态", this);
    QAction *addTransitionAction = new QAction("添加转换", this);
    QAction *deleteAction = new QAction("删除", this);
    
    // 工具操作
    QAction *validateAction = new QAction("验证", this);
    QAction *previewAction = new QAction("预览", this);
    
    m_toolbar->addAction(newAction);
    m_toolbar->addAction(openAction);
    m_toolbar->addAction(saveAction);
    m_toolbar->addSeparator();
    m_toolbar->addAction(addStateAction);
    m_toolbar->addAction(addTransitionAction);
    m_toolbar->addAction(deleteAction);
    m_toolbar->addSeparator();
    m_toolbar->addAction(validateAction);
    m_toolbar->addAction(previewAction);
    
    // 连接信号
    connect(newAction, &QAction::triggered, this, &StateMachineEditorV2::createNewStateMachine);
    connect(openAction, &QAction::triggered, this, [this]() {
        QString filePath = QFileDialog::getOpenFileName(this, "打开状态机文件", "", "状态机文件 (*.json)");
        if (!filePath.isEmpty()) loadStateMachine(filePath);
    });
    connect(saveAction, &QAction::triggered, this, [this]() {
        QString filePath = QFileDialog::getSaveFileName(this, "保存状态机文件", "", "状态机文件 (*.json)");
        if (!filePath.isEmpty()) saveStateMachine(filePath);
    });
    connect(validateAction, &QAction::triggered, this, &StateMachineEditorV2::validateStateMachine);
    connect(previewAction, &QAction::triggered, this, &StateMachineEditorV2::showRuntimePreview);
}

void StateMachineEditorV2::createPropertiesPanel()
{
    m_propertiesPanel = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(m_propertiesPanel);
    
    // 创建属性选项卡
    m_propertiesTab = new QTabWidget(m_propertiesPanel);
    
    // UI流属性页
    m_uiFlowProperties = new QWidget();
    QFormLayout *uiFlowLayout = new QFormLayout(m_uiFlowProperties);
    
    m_uiFlowStateNameEdit = new QLineEdit();
    m_uiFlowStateDescriptionEdit = new QTextEdit();
    m_uiInterfaceCombo = new QComboBox();
    m_initialStateCheck = new QCheckBox();
    m_finalStateCheck = new QCheckBox();
    m_uiFlowTransitionEventEdit = new QLineEdit();
    m_uiFlowTransitionEventTypeCombo = new QComboBox();
    
    uiFlowLayout->addRow("状态名称:", m_uiFlowStateNameEdit);
    uiFlowLayout->addRow("状态描述:", m_uiFlowStateDescriptionEdit);
    uiFlowLayout->addRow("UI界面:", m_uiInterfaceCombo);
    uiFlowLayout->addRow("初始状态:", m_initialStateCheck);
    uiFlowLayout->addRow("最终状态:", m_finalStateCheck);
    uiFlowLayout->addRow("转换事件:", m_uiFlowTransitionEventEdit);
    
    // 逻辑时序属性页
    m_logicProperties = new QWidget();
    QFormLayout *logicLayout = new QFormLayout(m_logicProperties);
    
    m_logicStateNameEdit = new QLineEdit();
    m_logicStateDescriptionEdit = new QTextEdit();
    m_logicTypeCombo = new QComboBox();
    m_logicStateDataEdit = new QLineEdit();
    m_logicTransitionEventEdit = new QLineEdit();
    m_logicTransitionEventTypeCombo = new QComboBox();
    m_logicTransitionActionEdit = new QTextEdit();
    
    logicLayout->addRow("状态名称:", m_logicStateNameEdit);
    logicLayout->addRow("状态描述:", m_logicStateDescriptionEdit);
    logicLayout->addRow("逻辑类型:", m_logicTypeCombo);
    logicLayout->addRow("状态数据:", m_logicStateDataEdit);
    logicLayout->addRow("转换事件:", m_logicTransitionEventEdit);
    logicLayout->addRow("转换动作:", m_logicTransitionActionEdit);
    
    m_propertiesTab->addTab(m_uiFlowProperties, "UI流属性");
    m_propertiesTab->addTab(m_logicProperties, "逻辑属性");
    
    layout->addWidget(m_propertiesTab);
    
    // 连接信号
    connect(m_uiInterfaceCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &StateMachineEditorV2::onUiInterfaceChanged);
}

void StateMachineEditorV2::createRuntimePreview()
{
    // 创建运行时预览窗口
    m_runtime = new StateMachineRuntime(this);
    m_runtimeView = new StateMachineRuntimeView(this);
    m_runtimeView->setRuntime(m_runtime);
    
    m_runtimeDock = new QDockWidget("运行时预览", this);
    m_runtimeDock->setWidget(m_runtimeView);
    m_runtimeDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    m_runtimeDock->hide();
}

void StateMachineEditorV2::updatePropertiesPanel()
{
    // 根据当前模式更新属性面板
    switch (m_currentMode) {
    case StateMachineMode::UIFlowOnly:
        m_propertiesTab->setCurrentWidget(m_uiFlowProperties);
        
        if (!m_currentUIFlowState.stateId.isEmpty()) {
            m_uiFlowStateNameEdit->setText(m_currentUIFlowState.name);
            m_uiFlowStateDescriptionEdit->setPlainText(m_currentUIFlowState.description);
            m_initialStateCheck->setChecked(m_currentUIFlowState.isInitialState);
            m_finalStateCheck->setChecked(m_currentUIFlowState.isFinalState);
            
            // 设置UI界面选择
            int index = m_uiInterfaceCombo->findData(m_currentUIFlowState.uiInterfaceId);
            if (index >= 0) m_uiInterfaceCombo->setCurrentIndex(index);
        }
        break;
    case StateMachineMode::LogicSequenceOnly:
        m_propertiesTab->setCurrentWidget(m_logicProperties);
        
        if (!m_currentLogicState.stateId.isEmpty()) {
            m_logicStateNameEdit->setText(m_currentLogicState.name);
            m_logicStateDescriptionEdit->setPlainText(m_currentLogicState.description);
            // TODO: 实现逻辑状态数据显示
            // m_logicStateDataEdit->setText(m_currentLogicState.data.toString());
            
            // 设置逻辑类型选择
            // TODO: 实现逻辑类型选择
        }
        break;
    case StateMachineMode::Integrated:
        // 集成模式下显示两个属性页
        break;
    }
}

void StateMachineEditorV2::setupRuntime()
{
    // 设置运行时预览
    if (m_runtime && m_integrationManager) {
        // 根据当前模式设置运行时状态机
        // TODO: 需要适配不同类型的状态机到运行时引擎
        switch (m_currentMode) {
        case StateMachineMode::UIFlowOnly:
            // UIFlowStateMachine 不兼容 StateMachineRuntime，需要适配器
            // m_runtime->setStateMachine(m_integrationManager->uiFlowStateMachine());
            break;
        case StateMachineMode::LogicSequenceOnly:
            // 逻辑时序状态机可能需要特殊的运行时处理
            break;
        case StateMachineMode::Integrated:
            // 集成模式下使用集成管理器
            break;
        }
    }
}

void StateMachineEditorV2::switchMode(StateMachineMode newMode)
{
    if (m_currentMode == newMode) return;
    
    m_currentMode = newMode;
    
    switch (newMode) {
    case StateMachineMode::UIFlowOnly:
        setupUIFlowMode();
        break;
    case StateMachineMode::LogicSequenceOnly:
        setupLogicSequenceMode();
        break;
    case StateMachineMode::Integrated:
        setupIntegratedMode();
        break;
    }
    
    updateModeUI();
    emit onStateMachineChanged();
}

void StateMachineEditorV2::setupUIFlowMode()
{
    if (!m_uiFlowScene) {
        m_uiFlowScene = new UIFlowStateMachineScene(this);
    }
    
    if (m_integrationManager->uiFlowStateMachine()) {
        m_uiFlowScene->setStateMachine(m_integrationManager->uiFlowStateMachine());
    }
    
    if (m_view) {
        m_view->setUIFlowScene(m_uiFlowScene);
        m_view->setCurrentMode(StateMachineMode::UIFlowOnly);
    }
    
    // 设置集成管理器为UI流模式
    m_integrationManager->setIntegrationMode(StateMachineIntegrationManager::IntegrationMode::UIFlowOnly);
}

void StateMachineEditorV2::setupLogicSequenceMode()
{
    if (!m_logicScene) {
        m_logicScene = new LogicStateMachineScene(this);
    }
    
    if (m_integrationManager->logicSequenceStateMachine()) {
        m_logicScene->setStateMachine(m_integrationManager->logicSequenceStateMachine());
    }
    
    if (m_view) {
        m_view->setLogicScene(m_logicScene);
        m_view->setCurrentMode(StateMachineMode::LogicSequenceOnly);
    }
    
    // 设置集成管理器为逻辑时序模式
    m_integrationManager->setIntegrationMode(StateMachineIntegrationManager::IntegrationMode::LogicSequenceOnly);
}

void StateMachineEditorV2::setupIntegratedMode()
{
    // 集成模式下需要同时显示两个场景
    if (!m_uiFlowScene) {
        m_uiFlowScene = new UIFlowStateMachineScene(this);
    }
    
    if (!m_logicScene) {
        m_logicScene = new LogicStateMachineScene(this);
    }
    
    if (m_integrationManager->uiFlowStateMachine()) {
        m_uiFlowScene->setStateMachine(m_integrationManager->uiFlowStateMachine());
    }
    
    if (m_integrationManager->logicSequenceStateMachine()) {
        m_logicScene->setStateMachine(m_integrationManager->logicSequenceStateMachine());
    }
    
    if (m_view) {
        m_view->setUIFlowScene(m_uiFlowScene);
        m_view->setLogicScene(m_logicScene);
        m_view->setCurrentMode(StateMachineMode::Integrated);
    }
    
    // 设置集成管理器为集成模式
    m_integrationManager->setIntegrationMode(StateMachineIntegrationManager::IntegrationMode::Integrated);
}

void StateMachineEditorV2::updateModeUI()
{
    // 更新模式描述
    switch (m_currentMode) {
    case StateMachineMode::UIFlowOnly:
        m_modeDescription->setText("专注于UI界面的串联和跳转，实现用户界面之间的导航流程");
        break;
    case StateMachineMode::LogicSequenceOnly:
        m_modeDescription->setText("专注于软件内部逻辑时序，处理业务逻辑和数据处理流程");
        break;
    case StateMachineMode::Integrated:
        m_modeDescription->setText("集成UI流和逻辑时序，实现完整的软件状态管理");
        break;
    }
    
    // 更新工具栏和属性面板的可用性
    updateToolbarAvailability();
    updatePropertiesPanelAvailability();
}

void StateMachineEditorV2::updateToolbarAvailability()
{
    // 根据当前模式更新工具栏按钮的可用性
    // TODO: 实现工具栏按钮的可用性控制
}

void StateMachineEditorV2::updatePropertiesPanelAvailability()
{
    // 根据当前模式更新属性面板的可用性
    switch (m_currentMode) {
    case StateMachineMode::UIFlowOnly:
        m_propertiesTab->setTabEnabled(0, true);  // UI流属性页
        m_propertiesTab->setTabEnabled(1, false); // 逻辑属性页
        break;
    case StateMachineMode::LogicSequenceOnly:
        m_propertiesTab->setTabEnabled(0, false); // UI流属性页
        m_propertiesTab->setTabEnabled(1, true);  // 逻辑属性页
        break;
    case StateMachineMode::Integrated:
        m_propertiesTab->setTabEnabled(0, true);  // UI流属性页
        m_propertiesTab->setTabEnabled(1, true);  // 逻辑属性页
        break;
    }
}

// UIFlowStateMachineScene 实现
UIFlowStateMachineScene::UIFlowStateMachineScene(QObject *parent)
    : QGraphicsScene(parent)
    , m_stateMachine(nullptr)
    , m_currentStateNode(nullptr)
    , m_currentTransitionEdge(nullptr)
    , m_isConnecting(false)
    , m_connectionFromNode(nullptr)
    , m_tempConnectionLine(nullptr)
{
    setSceneRect(-1000, -1000, 2000, 2000);
    
    // 设置背景
    setBackgroundBrush(QBrush(QColor(240, 240, 240)));
}

void UIFlowStateMachineScene::setStateMachine(UIFlowStateMachine *stateMachine)
{
    if (m_stateMachine == stateMachine) return;
    
    m_stateMachine = stateMachine;
    refreshScene();
}

void UIFlowStateMachineScene::refreshScene()
{
    if (!m_stateMachine) return;
    
    clear();
    m_stateNodes.clear();
    m_transitionEdges.clear();
    
    // 创建状态节点
    for (const auto &state : m_stateMachine->states()) {
        UIFlowStateNode *node = new UIFlowStateNode(state);
        addItem(node);
        m_stateNodes[state.stateId] = node;
        
        // 随机位置
        qreal x = (QRandomGenerator::global()->generate() % 800) - 400;
        qreal y = (QRandomGenerator::global()->generate() % 600) - 300;
        node->setPos(x, y);
        
        connect(node, &UIFlowStateNode::stateSelected, this, &UIFlowStateMachineScene::onStateSelected);
    }
    
    // 创建转换边
    for (const auto &transition : m_stateMachine->transitions()) {
        UIFlowStateNode *fromNode = m_stateNodes.value(transition.fromStateId);
        UIFlowStateNode *toNode = m_stateNodes.value(transition.toStateId);
        
        if (fromNode && toNode) {
            UIFlowTransitionEdge *edge = new UIFlowTransitionEdge(transition, fromNode, toNode);
            addItem(edge);
            m_transitionEdges[transition.transitionId] = edge;
            
            connect(edge, &UIFlowTransitionEdge::transitionSelected, this, &UIFlowStateMachineScene::onTransitionSelected);
        }
    }
}

void UIFlowStateMachineScene::onStateSelected(const UIFlowStateMachine::UIFlowState &state)
{
    m_currentStateNode = m_stateNodes.value(state.stateId);
    
    // 高亮显示当前状态
    for (auto *node : m_stateNodes) {
        node->setHighlighted(node == m_currentStateNode);
    }
    
    emit stateSelected(state);
}

void UIFlowStateMachineScene::onTransitionSelected(const UIFlowStateMachine::UIFlowTransition &transition)
{
    m_currentTransitionEdge = m_transitionEdges.value(transition.transitionId);
    
    // 高亮显示当前转换
    for (auto *edge : m_transitionEdges) {
        edge->setHighlighted(edge == m_currentTransitionEdge);
    }
    
    emit transitionSelected(transition);
}

void UIFlowStateMachineScene::addState(const QPointF &pos)
{
    if (!m_stateMachine) return;
    
    UIFlowStateMachine::UIFlowState newState;
    newState.stateId = QUuid::createUuid().toString();
    newState.name = QString("状态%1").arg(m_stateMachine->states().size() + 1);
    newState.description = "新状态";
    
    m_stateMachine->addState(newState);
    
    // 创建新节点
    UIFlowStateNode *node = new UIFlowStateNode(newState);
    node->setPos(pos);
    addItem(node);
    m_stateNodes[newState.stateId] = node;
    
    connect(node, &UIFlowStateNode::stateSelected, this, &UIFlowStateMachineScene::onStateSelected);
    
    emit stateMachineChanged();
}

void UIFlowStateMachineScene::removeState(const QString &stateId)
{
    if (!m_stateMachine) return;
    
    m_stateMachine->removeState(stateId);
    
    if (m_stateNodes.contains(stateId)) {
        UIFlowStateNode *node = m_stateNodes[stateId];
        removeItem(node);
        delete node;
        m_stateNodes.remove(stateId);
    }
    
    // 移除相关的转换
    QList<QString> transitionsToRemove;
    for (const auto &transition : m_stateMachine->transitions()) {
        if (transition.fromStateId == stateId || transition.toStateId == stateId) {
            transitionsToRemove.append(transition.transitionId);
        }
    }
    
    for (const auto &transitionId : transitionsToRemove) {
        removeTransition(transitionId);
    }
    
    emit stateMachineChanged();
}

void UIFlowStateMachineScene::addTransition(const QString &fromStateId, const QString &toStateId)
{
    if (!m_stateMachine) return;
    
    UIFlowStateMachine::UIFlowTransition newTransition;
    newTransition.transitionId = QUuid::createUuid().toString();
    newTransition.fromStateId = fromStateId;
    newTransition.toStateId = toStateId;
    newTransition.eventType = "UI_EVENT";
    newTransition.eventType = "点击事件";
    
    m_stateMachine->addTransition(newTransition);
    
    UIFlowStateNode *fromNode = m_stateNodes.value(fromStateId);
    UIFlowStateNode *toNode = m_stateNodes.value(toStateId);
    
    if (fromNode && toNode) {
        UIFlowTransitionEdge *edge = new UIFlowTransitionEdge(newTransition, fromNode, toNode);
        addItem(edge);
        m_transitionEdges[newTransition.transitionId] = edge;
        
        connect(edge, &UIFlowTransitionEdge::transitionSelected, this, &UIFlowStateMachineScene::onTransitionSelected);
    }
    
    emit stateMachineChanged();
}

void UIFlowStateMachineScene::removeTransition(const QString &transitionId)
{
    if (!m_stateMachine) return;
    
    m_stateMachine->removeTransition(transitionId);
    
    if (m_transitionEdges.contains(transitionId)) {
        UIFlowTransitionEdge *edge = m_transitionEdges[transitionId];
        removeItem(edge);
        delete edge;
        m_transitionEdges.remove(transitionId);
    }
    
    emit stateMachineChanged();
}

void UIFlowStateMachineScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        // 左键点击处理
        QGraphicsItem *item = itemAt(event->scenePos(), QTransform());
        if (!item) {
            // 点击空白区域，清除选择
            clearSelection();
            m_currentStateNode = nullptr;
            m_currentTransitionEdge = nullptr;
        }
    }
    
    QGraphicsScene::mousePressEvent(event);
}

void UIFlowStateMachineScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    // 处理鼠标移动事件
    if (m_isConnecting && m_connectionFromNode) {
        // 正在连接状态，更新临时连接线
        if (m_tempConnectionLine) {
            QPointF fromPoint = m_connectionFromNode->getConnectionPoint(event->scenePos());
            m_tempConnectionLine->setLine(QLineF(fromPoint, event->scenePos()));
        }
    }
    
    QGraphicsScene::mouseMoveEvent(event);
}

void UIFlowStateMachineScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_isConnecting && m_connectionFromNode) {
        // 连接状态结束
        QGraphicsItem *item = itemAt(event->scenePos(), QTransform());
        if (item && item != m_connectionFromNode) {
            UIFlowStateNode *toNode = dynamic_cast<UIFlowStateNode*>(item);
            if (toNode) {
                // 创建新的转换
                addTransition(m_connectionFromNode->state().stateId, toNode->state().stateId);
            }
        }
        
        // 清理临时连接线
        if (m_tempConnectionLine) {
            removeItem(m_tempConnectionLine);
            delete m_tempConnectionLine;
            m_tempConnectionLine = nullptr;
        }
        
        m_isConnecting = false;
        m_connectionFromNode = nullptr;
    }
    
    QGraphicsScene::mouseReleaseEvent(event);
}

// LogicStateNode类的成员函数实现
LogicSequenceStateMachine::LogicState LogicStateNode::state() const
{
    return m_state;
}

// LogicTransitionEdge类的成员函数实现
LogicSequenceStateMachine::LogicTransition LogicTransitionEdge::transition() const
{
    return m_transition;
}

// LogicStateMachineScene类的连接相关函数实现
void LogicStateMachineScene::finishConnection(LogicStateNode *toNode)
{
    if (!m_connectionFromNode || !toNode) {
        cancelConnection();
        return;
    }
    
    // TODO: 实现完成连接的实际逻辑
    qDebug() << "Finish connection from" << m_connectionFromNode->state().stateId << "to" << toNode->state().stateId;
    
    // 发出连接完成信号
    emit connectionFinished(m_connectionFromNode, toNode);
    
    cleanupConnectionMode();
}

void LogicStateMachineScene::cancelConnection()
{
    // TODO: 实现取消连接的实际逻辑
    qDebug() << "Cancel connection";
    
    cleanupConnectionMode();
}

void LogicStateMachineScene::cleanupConnectionMode()
{
    // 清理连接模式
    if (m_tempConnectionLine) {
        removeItem(m_tempConnectionLine);
        delete m_tempConnectionLine;
        m_tempConnectionLine = nullptr;
    }
    
    m_connectionFromNode = nullptr;
    m_isConnecting = false;
    
    qDebug() << "Connection mode cleaned up";
}

// UIFlowTransitionEdge 实现
UIFlowTransitionEdge::UIFlowTransitionEdge(const UIFlowStateMachine::UIFlowTransition &transition, 
                                             UIFlowStateNode *fromNode, UIFlowStateNode *toNode, 
                                             QGraphicsItem *parent)
    : QObject(), QGraphicsLineItem(parent)
    , m_transition(transition)
    , m_fromNode(fromNode)
    , m_toNode(toNode)
    , m_labelText(nullptr)
    , m_arrowHead(nullptr)
    , m_isHovered(false)
    , m_isHighlighted(false)
{
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setZValue(-1); // 确保边在节点下方
    
    updatePath();
    updateLineStyle();
}

UIFlowStateMachine::UIFlowTransition UIFlowTransitionEdge::transition() const
{
    return m_transition;
}

void UIFlowTransitionEdge::setTransition(const UIFlowStateMachine::UIFlowTransition &transition)
{
    m_transition = transition;
    updateLineStyle();
}

void UIFlowTransitionEdge::setHighlighted(bool highlighted)
{
    m_isHighlighted = highlighted;
    updateLineStyle();
}

void UIFlowTransitionEdge::updatePath()
{
    if (!m_fromNode || !m_toNode) return;
    
    QPointF fromPoint = m_fromNode->getConnectionPoint(m_toNode->pos());
    QPointF toPoint = m_toNode->getConnectionPoint(m_fromNode->pos());
    
    QPainterPath path;
    path.moveTo(fromPoint);
    
    // 创建曲线路径
    qreal dx = toPoint.x() - fromPoint.x();
    qreal dy = toPoint.y() - fromPoint.y();
    
    QPointF ctrl1(fromPoint.x() + dx * 0.25, fromPoint.y());
    QPointF ctrl2(fromPoint.x() + dx * 0.75, toPoint.y());
    
    path.cubicTo(ctrl1, ctrl2, toPoint);
    
    // 对于QGraphicsLineItem，我们需要设置直线而不是路径
    setLine(QLineF(fromPoint, toPoint));
    
    // 更新箭头位置
    updateArrowHead(path);
    
    // 更新标签位置
    updateLabelPosition(path);
}

void UIFlowTransitionEdge::updateArrowHead(const QPainterPath &path)
{
    if (!m_arrowHead) {
        m_arrowHead = new QGraphicsPolygonItem(this);
    }
    
    // 计算箭头位置和方向
    qreal t = 0.9; // 箭头在路径上的位置（0-1）
    QPointF arrowPos = path.pointAtPercent(t);
    QPointF tangent = path.pointAtPercent(t + 0.01) - path.pointAtPercent(t - 0.01);
    qreal angle = atan2(tangent.y(), tangent.x());
    
    // 创建箭头形状
    QPolygonF arrow;
    arrow << QPointF(0, 0)
          << QPointF(-10, -5)
          << QPointF(-10, 5);
    
    QTransform transform;
    transform.translate(arrowPos.x(), arrowPos.y());
    transform.rotate(angle * 180 / M_PI);
    
    m_arrowHead->setPolygon(transform.map(arrow));
    
    // 设置箭头样式
    QBrush arrowBrush(Qt::black);
    QPen arrowPen(Qt::black, 1);
    m_arrowHead->setBrush(arrowBrush);
    m_arrowHead->setPen(arrowPen);
}

void UIFlowTransitionEdge::updateLabelPosition(const QPainterPath &path)
{
    if (!m_labelText) {
        m_labelText = new QGraphicsTextItem(this);
        m_labelText->setFont(QFont("Arial", 8));
        m_labelText->setDefaultTextColor(Qt::darkBlue);
        m_labelText->setTextWidth(80);
    }
    
    QPointF labelPos = path.pointAtPercent(0.5);
    m_labelText->setPos(labelPos.x() - 40, labelPos.y() - 10);
    m_labelText->setPlainText(m_transition.eventType);
}

void UIFlowTransitionEdge::updateLineStyle()
{
    QPen pen;
    
    if (m_isHighlighted) {
        pen = QPen(Qt::red, 3);
    } else if (m_isHovered) {
        pen = QPen(Qt::blue, 2);
    } else {
        pen = QPen(Qt::black, 1);
    }
    
    setPen(pen);
    
    // 更新标签文本
    if (m_labelText) {
        m_labelText->setPlainText(m_transition.eventType);
    }
}

void UIFlowTransitionEdge::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emit transitionSelected(m_transition);
    }
    
    QGraphicsLineItem::mousePressEvent(event);
}

void UIFlowTransitionEdge::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        // 双击转换边，可以编辑转换属性
        emit transitionSelected(m_transition);
        
        // TODO: 打开转换属性编辑对话框
        // QMessageBox::information(nullptr, "编辑转换", 
        //     QString("编辑转换: %1 -> %2\n事件类型: %3")
        //     .arg(m_transition.fromStateId)
        //     .arg(m_transition.toStateId)
        //     .arg(m_transition.eventType));
    }
    
    QGraphicsLineItem::mouseDoubleClickEvent(event);
}

void UIFlowTransitionEdge::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    m_isHovered = true;
    updateLineStyle();
    QGraphicsLineItem::hoverEnterEvent(event);
}

void UIFlowTransitionEdge::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    m_isHovered = false;
    updateLineStyle();
    QGraphicsLineItem::hoverLeaveEvent(event);
}

void UIFlowTransitionEdge::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QMenu menu;
    menu.addAction("编辑转换");
    menu.addAction("删除转换");
    
    QAction *selectedAction = menu.exec(event->screenPos());
    if (selectedAction) {
        // 处理菜单操作
    }
}

// StateMachineViewV2 实现
StateMachineViewV2::StateMachineViewV2(QWidget *parent)
    : QGraphicsView(parent)
    , m_uiFlowScene(nullptr)
    , m_logicScene(nullptr)
    , m_currentMode(StateMachineMode::UIFlowOnly)
    , m_isPanning(false)
    , m_panStartX(0)
    , m_panStartY(0)
{
    setRenderHint(QPainter::Antialiasing);
    setDragMode(QGraphicsView::RubberBandDrag);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    
    // 设置缩放控制
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
}

void StateMachineViewV2::setUIFlowScene(UIFlowStateMachineScene *scene)
{
    m_uiFlowScene = scene;
    updateCurrentScene();
}

void StateMachineViewV2::setLogicScene(LogicStateMachineScene *scene)
{
    m_logicScene = scene;
    updateCurrentScene();
}

void StateMachineViewV2::setCurrentMode(StateMachineMode mode)
{
    if (m_currentMode == mode) return;
    
    m_currentMode = mode;
    updateCurrentScene();
}

void StateMachineViewV2::updateCurrentScene()
{
    switch (m_currentMode) {
    case StateMachineMode::UIFlowOnly:
        if (m_uiFlowScene) {
            setScene(m_uiFlowScene);
        }
        break;
    case StateMachineMode::LogicSequenceOnly:
        if (m_logicScene) {
            setScene(m_logicScene);
        }
        break;
    case StateMachineMode::Integrated:
        // 集成模式下需要特殊处理
        // TODO: 实现集成模式下的场景显示
        break;
    }
    
    centerOn(0, 0);
    fitInView(sceneRect(), Qt::KeepAspectRatio);
}

void StateMachineViewV2::wheelEvent(QWheelEvent *event)
{
    // 缩放控制
    if (event->modifiers() & Qt::ControlModifier) {
        qreal scaleFactor = 1.15;
        if (event->angleDelta().y() < 0) {
            scaleFactor = 1.0 / scaleFactor;
        }
        
        scale(scaleFactor, scaleFactor);
        event->accept();
    } else {
        QGraphicsView::wheelEvent(event);
    }
}

void StateMachineViewV2::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton) {
        // 中键拖动
        m_isPanning = true;
        m_panStartX = event->position().x();
        m_panStartY = event->position().y();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
        return;
    }
    
    QGraphicsView::mousePressEvent(event);
}

void StateMachineViewV2::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isPanning) {
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - (event->position().x() - m_panStartX));
        verticalScrollBar()->setValue(verticalScrollBar()->value() - (event->position().y() - m_panStartY));
        m_panStartX = event->position().x();
        m_panStartY = event->position().y();
        event->accept();
        return;
    }
    
    QGraphicsView::mouseMoveEvent(event);
}

void StateMachineViewV2::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton) {
        m_isPanning = false;
        setCursor(Qt::ArrowCursor);
        event->accept();
        return;
    }
    
    QGraphicsView::mouseReleaseEvent(event);
}

void StateMachineViewV2::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Plus:
        scale(1.2, 1.2);
        break;
    case Qt::Key_Minus:
        scale(1.0 / 1.2, 1.0 / 1.2);
        break;
    case Qt::Key_0:
        // 重置缩放
        resetTransform();
        fitInView(sceneRect(), Qt::KeepAspectRatio);
        break;
    case Qt::Key_Delete:
        // 删除选中的项目
        deleteSelectedItems();
        break;
    default:
        QGraphicsView::keyPressEvent(event);
    }
}

void StateMachineViewV2::deleteSelectedItems()
{
    if (!scene()) return;
    
    QList<QGraphicsItem*> selectedItems = scene()->selectedItems();
    for (QGraphicsItem *item : selectedItems) {
        // 根据项目类型进行删除
        if (UIFlowStateNode *node = dynamic_cast<UIFlowStateNode*>(item)) {
            // 删除状态节点
            if (m_uiFlowScene) {
                m_uiFlowScene->removeState(node->state().stateId);
            }
        } else if (UIFlowTransitionEdge *edge = dynamic_cast<UIFlowTransitionEdge*>(item)) {
            // 删除转换边
            if (m_uiFlowScene) {
                m_uiFlowScene->removeTransition(edge->transition().transitionId);
            }
        }
    }
}

void StateMachineViewV2::fitToView()
{
    if (scene()) {
        fitInView(scene()->itemsBoundingRect(), Qt::KeepAspectRatio);
    }
}

void StateMachineViewV2::zoomIn()
{
    scale(1.2, 1.2);
}

void StateMachineViewV2::zoomOut()
{
    scale(1.0 / 1.2, 1.0 / 1.2);
}

void StateMachineViewV2::resetZoom()
{
    resetTransform();
    fitInView(sceneRect(), Qt::KeepAspectRatio);
}

// 其他相关类的实现（简化版本）
LogicStateMachineScene::LogicStateMachineScene(QObject *parent)
    : QGraphicsScene(parent)
    , m_stateMachine(nullptr)
{
    setSceneRect(-1000, -1000, 2000, 2000);
    setBackgroundBrush(QBrush(QColor(250, 250, 250)));
}

void LogicStateMachineScene::setStateMachine(LogicSequenceStateMachine *stateMachine)
{
    m_stateMachine = stateMachine;
    refreshScene();
}

void LogicStateMachineScene::refreshScene()
{
    // 实现逻辑时序状态机的场景刷新
    // TODO: 实现逻辑时序状态机的可视化
}

// 包含必要的头文件
#include "uiflowstatemachine.h"
#include "logicsequencestatemachine.h"
#include "statemachineintegrationmanager.h"
#include "statemachineruntime.h"

// LogicStateNode类的鼠标事件实现
void LogicStateNode::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    // 处理鼠标按下事件
    if (event->button() == Qt::LeftButton) {
        // 选中当前节点
        setSelected(true);
        // TODO: 发出状态选中信号（需要先在头文件中声明）
        // emit selected();
    }
    
    QGraphicsItem::mousePressEvent(event);
}

void LogicStateNode::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    // 处理鼠标双击事件
    if (event->button() == Qt::LeftButton) {
        // 准备编辑状态属性
        // TODO: 实现状态属性编辑对话框
        qDebug() << "LogicStateNode double-clicked, ready for property editing";
    }
    
    QGraphicsItem::mouseDoubleClickEvent(event);
}

void LogicStateNode::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    // 处理鼠标移动事件
    if (event->buttons() & Qt::LeftButton) {
        // 拖动节点
        setPos(mapToParent(event->pos() - event->buttonDownPos(Qt::LeftButton)));
        // 更新连接的边（TODO: 实现连接边更新逻辑）
        // updateConnectedEdges();
    }
    
    QGraphicsItem::mouseMoveEvent(event);
}

void LogicStateNode::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    // 处理鼠标释放事件
    if (event->button() == Qt::LeftButton) {
        // 完成拖动操作
        // 可以在这里添加位置验证逻辑
    }
    
    QGraphicsItem::mouseReleaseEvent(event);
}

void LogicStateNode::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    // 处理鼠标悬停进入事件
    setZValue(1); // 提升Z值以显示在顶部
    update(); // 重绘以显示悬停效果
    
    QGraphicsItem::hoverEnterEvent(event);
}

void LogicStateNode::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    // 处理鼠标悬停离开事件
    setZValue(0); // 恢复Z值
    update(); // 重绘以移除悬停效果
    
    QGraphicsItem::hoverLeaveEvent(event);
}

void LogicStateNode::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    // 处理右键菜单事件
    QMenu menu;
    
    QAction *editAction = menu.addAction("编辑属性");
    QAction *deleteAction = menu.addAction("删除状态");
    
    QAction *selectedAction = menu.exec(event->screenPos());
    
    if (selectedAction == editAction) {
        // 编辑状态属性
        // TODO: 实现属性编辑
        qDebug() << "Edit properties requested for LogicStateNode";
    } else if (selectedAction == deleteAction) {
        // 删除状态
        // TODO: 实现删除状态逻辑
        qDebug() << "Delete requested for LogicStateNode";
    }
    
    QGraphicsItem::contextMenuEvent(event);
}

// LogicTransitionEdge类的鼠标事件实现
void LogicTransitionEdge::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    // 处理鼠标按下事件
    if (event->button() == Qt::LeftButton) {
        // 选中当前转换线
        setSelected(true);
        // TODO: 发出转换选中信号（需要先在头文件中声明）
        // emit selected();
    }
    
    QGraphicsItem::mousePressEvent(event);
}

void LogicTransitionEdge::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    // 处理鼠标双击事件
    if (event->button() == Qt::LeftButton) {
        // 准备编辑转换属性
        // TODO: 实现转换属性编辑对话框
        qDebug() << "LogicTransitionEdge double-clicked, ready for property editing";
    }
    
    QGraphicsItem::mouseDoubleClickEvent(event);
}

void LogicTransitionEdge::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    // 处理鼠标悬停进入事件
    setZValue(1); // 提升Z值以显示在顶部
    update(); // 重绘以显示悬停效果
    
    QGraphicsItem::hoverEnterEvent(event);
}

void LogicTransitionEdge::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    // 处理鼠标悬停离开事件
    setZValue(0); // 恢复Z值
    update(); // 重绘以移除悬停效果
    
    QGraphicsItem::hoverLeaveEvent(event);
}

void LogicTransitionEdge::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    // 处理右键菜单事件
    QMenu menu;
    
    QAction *editAction = menu.addAction("编辑属性");
    QAction *deleteAction = menu.addAction("删除转换");
    
    QAction *selectedAction = menu.exec(event->screenPos());
    
    if (selectedAction == editAction) {
        // 编辑转换属性
        // TODO: 实现属性编辑
        qDebug() << "Edit properties requested for LogicTransitionEdge";
    } else if (selectedAction == deleteAction) {
        // 删除转换
        // TODO: 实现删除转换逻辑
        qDebug() << "Delete requested for LogicTransitionEdge";
    }
    
    QGraphicsItem::contextMenuEvent(event);
}

// LogicStateMachineScene类的鼠标事件和槽函数实现
void LogicStateMachineScene::addState(const QPointF &position)
{
    // 添加新状态
    if (!m_stateMachine) return;
    
    // TODO: 实现添加状态的逻辑
    qDebug() << "Add state requested at position:" << position;
}

void LogicStateMachineScene::addTransition(const QString &fromStateId, const QString &toStateId)
{
    // 添加新转换
    if (!m_stateMachine) return;
    
    // TODO: 实现添加转换的逻辑
    qDebug() << "Add transition requested from" << fromStateId << "to" << toStateId;
}

void LogicStateMachineScene::removeState(const QString &stateId)
{
    // 移除状态
    if (!m_stateMachine) return;
    
    // TODO: 实现移除状态的逻辑
    qDebug() << "Remove state requested for state ID:" << stateId;
}

void LogicStateMachineScene::removeTransition(const QString &transitionId)
{
    // 移除转换
    if (!m_stateMachine) return;
    
    // TODO: 实现移除转换的逻辑
    qDebug() << "Remove transition requested for transition ID:" << transitionId;
}

void LogicStateMachineScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    // 处理鼠标按下事件
    if (event->button() == Qt::LeftButton) {
        // 清除当前选择
        clearSelection();
        
        // 检查是否点击了状态节点或转换线
        QGraphicsItem *item = itemAt(event->scenePos(), QTransform());
        if (item) {
            if (LogicStateNode *node = dynamic_cast<LogicStateNode*>(item)) {
                // 选中状态节点
                node->setSelected(true);
                emit stateSelected(node->state());
            } else if (LogicTransitionEdge *edge = dynamic_cast<LogicTransitionEdge*>(item)) {
                // 选中转换线
                edge->setSelected(true);
                emit transitionSelected(edge->transition());
            }
        }
    }
    
    QGraphicsScene::mousePressEvent(event);
}

void LogicStateMachineScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    // 处理鼠标移动事件
    if (m_isConnecting && m_tempConnectionLine) {
        // 更新临时连接线
        QPointF endPos = event->scenePos();
        QLineF line = m_tempConnectionLine->line();
        line.setP2(endPos);
        m_tempConnectionLine->setLine(line);
    }
    
    QGraphicsScene::mouseMoveEvent(event);
}

void LogicStateMachineScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    // 处理鼠标释放事件
    if (event->button() == Qt::LeftButton && m_isConnecting) {
        // 完成连接操作
        QGraphicsItem *item = itemAt(event->scenePos(), QTransform());
        if (item && dynamic_cast<LogicStateNode*>(item)) {
            LogicStateNode *toNode = dynamic_cast<LogicStateNode*>(item);
            if (toNode != m_connectionFromNode) {
                finishConnection(toNode);
            } else {
                cancelConnection();
            }
        } else {
            cancelConnection();
        }
    }
    
    QGraphicsScene::mouseReleaseEvent(event);
}