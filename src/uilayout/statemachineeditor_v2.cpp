#include "statemachineeditor_v2.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QInputDialog>
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
#include <QListWidget>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QListWidgetItem>
#include <QXmlStreamReader>
#include <QScrollArea>
#include <QFrame>
#include <QGroupBox>

// 产品相关头文件
#include "../product/product.h"
#include "../product/productconfigmanager.h"

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
    
    // 获取状态编辑器
    StateMachineEditorV2 *editor = qobject_cast<StateMachineEditorV2*>(scene()->parent());
    
    if (editor && editor->currentMode() == StateMachineMode::UIFlowMode) {
        // UI流模式下只显示向导相关菜单项
        menu.addAction("运行向导");
    } else {
        // 其他模式下显示完整菜单
        menu.addAction("编辑状态");
        menu.addAction("删除状态");
        menu.addSeparator();
        menu.addAction("添加转换");
    }
    
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
    , m_step1Button(nullptr)
    , m_step2Button(nullptr)
    , m_modeDescription(nullptr)
    , m_runtime(nullptr)
    , m_runtimeView(nullptr)
    , m_runtimeDock(nullptr)
    , m_uiRuntimePreview(nullptr)
    , m_productUiFiles()  // 显式初始化QList
    , m_product(nullptr)
    , m_propertiesTab(nullptr)
    , m_uiFlowProperties(nullptr)
    , m_logicProperties(nullptr)
    , m_currentMode(StateMachineMode::UIFlowMode)
    , m_wizardManager(nullptr)
    , m_currentWizardName("")
    , m_currentWizardFilePath("")
    , m_isWizardModified(false)
    , m_uiFilesListWidget(nullptr)
    , m_uiFileDetailsTextEdit(nullptr)
    , m_uiFilesLabel(nullptr)
    , m_uiFilesDataPendingUpdate(false)
    , m_definedEventsListWidget(nullptr)
{
    qDebug() << "[DEBUG] StateMachineEditorV2 constructor started";
    
    // 创建集成管理器
    m_integrationManager = new StateMachineIntegrationManager(this);
    
    // 创建UI
    createModeSelector();
    createToolbar();
    createPropertiesPanel();
    createRuntimePreview();
    
    // 创建UI运行时预览窗口
    m_uiRuntimePreview = new UIRuntimePreviewWidget(this);
    
    qDebug()<<"createStateMachineEditor()";
    // 设置布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    qDebug()<<"mainLayout";
    mainLayout->addWidget(m_modeGroup);
    mainLayout->addWidget(m_toolbar);
    
    QHBoxLayout *contentLayout = new QHBoxLayout();
    qDebug()<<"contentLayout";
    
    // 左侧：状态机视图 (3/5宽度)
    contentLayout->addWidget(m_view, 3);
    qDebug()<<"m_view";
    
    // 中间：属性面板 (1/5宽度)
    contentLayout->addWidget(m_propertiesPanel, 1);
    qDebug()<<"m_propertiesPanel";
    
    // 右侧：UI运行时预览 (1/5宽度)
    QGroupBox *previewGroup = new QGroupBox("UI运行时预览", this);
    QVBoxLayout *previewLayout = new QVBoxLayout(previewGroup);
    previewLayout->addWidget(m_uiRuntimePreview);
    contentLayout->addWidget(previewGroup, 1);
    qDebug()<<"UI运行时预览";
    
    mainLayout->addLayout(contentLayout);
    qDebug()<<"mainLayout";
    
    // 初始模式设置 - 立即应用UI流模式的显示设置
    switchMode(StateMachineMode::UIFlowMode);
    
    // 确保向导编辑界面在初始化时正确显示，不依赖updatePropertiesPanelAvailability()
    updateWizardEditorInterface();
    
    qDebug() << "[DEBUG] StateMachineEditorV2 constructor completed";
    qDebug() << "[DEBUG] m_uiFilesListWidget pointer:" << m_uiFilesListWidget;
    qDebug() << "[DEBUG] m_productUiFiles size:" << m_productUiFiles.size();
    qDebug() << "[DEBUG] m_uiFilesDataPendingUpdate:" << m_uiFilesDataPendingUpdate;
}

// UIRuntimePreviewWidget 实现
UIRuntimePreviewWidget::UIRuntimePreviewWidget(QWidget *parent)
    : QWidget(parent)
    , m_previewWidget(nullptr)
    , m_placeholderLabel(nullptr)
    , m_mainLayout(nullptr)
    , m_previewLoaded(false)
{
    createPreviewLayout();
    createPlaceholder();
}

UIRuntimePreviewWidget::~UIRuntimePreviewWidget()
{
    clearPreview();
}

void UIRuntimePreviewWidget::createPreviewLayout()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(10, 10, 10, 10);
    m_mainLayout->setSpacing(10);
}

void UIRuntimePreviewWidget::createPlaceholder()
{
    if (m_placeholderLabel) {
        m_mainLayout->removeWidget(m_placeholderLabel);
        delete m_placeholderLabel;
    }
    
    m_placeholderLabel = new QLabel(this);
    m_placeholderLabel->setAlignment(Qt::AlignCenter);
    m_placeholderLabel->setText("请选择UI文件查看运行时预览");
    m_placeholderLabel->setStyleSheet("QLabel { color: #666; font-size: 14px; padding: 50px; }");
    
    m_mainLayout->addWidget(m_placeholderLabel);
}

void UIRuntimePreviewWidget::loadUIFile(const QString &filePath)
{
    clearPreview();
    
    if (filePath.isEmpty() || !QFile::exists(filePath)) {
        qDebug() << "UI文件不存在或路径为空:" << filePath;
        createPlaceholder();
        return;
    }
    
    qDebug() << "加载UI文件进行预览:" << filePath;
    
    try {
        // 使用QUiLoader动态加载UI文件
        QUiLoader loader;
        QFile uiFile(filePath);
        
        if (!uiFile.open(QFile::ReadOnly)) {
            qDebug() << "无法打开UI文件:" << filePath;
            createPlaceholder();
            return;
        }
        
        // 加载UI文件并创建控件
        QWidget *previewWidget = loader.load(&uiFile, this);
        uiFile.close();
        
        if (!previewWidget) {
            qDebug() << "UI文件加载失败:" << filePath;
            createPlaceholder();
            return;
        }
        
        // 设置预览窗口样式
        previewWidget->setStyleSheet("QWidget { background-color: white; border: 2px solid #4CAF50; border-radius: 8px; }");
        
        // 设置预览窗口大小策略
        previewWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        
        // 创建容器窗口来包装预览控件
        QWidget *containerWidget = new QWidget(this);
        containerWidget->setStyleSheet("QWidget { background-color: #f5f5f5; border: 1px solid #ddd; border-radius: 8px; }");
        
        QVBoxLayout *containerLayout = new QVBoxLayout(containerWidget);
        containerLayout->setContentsMargins(10, 10, 10, 10);
        containerLayout->setSpacing(5);
        
        // 添加标题栏
        QLabel *titleLabel = new QLabel("UI运行时预览: " + QFileInfo(filePath).fileName(), containerWidget);
        titleLabel->setStyleSheet("QLabel { color: #333; font-weight: bold; font-size: 12px; padding: 5px; background-color: #e8f5e8; border-radius: 4px; }");
        titleLabel->setAlignment(Qt::AlignCenter);
        containerLayout->addWidget(titleLabel);
        
        // 添加预览控件
        containerLayout->addWidget(previewWidget, 1);
        
        // 添加状态栏
        QLabel *statusLabel = new QLabel("✓ UI文件加载成功", containerWidget);
        statusLabel->setStyleSheet("QLabel { color: #4CAF50; font-size: 10px; padding: 3px; }");
        statusLabel->setAlignment(Qt::AlignRight);
        containerLayout->addWidget(statusLabel);
        
        m_previewWidget = containerWidget;
        m_mainLayout->addWidget(m_previewWidget, 1);
        m_previewLoaded = true;
        
        // 移除占位符
        if (m_placeholderLabel) {
            m_mainLayout->removeWidget(m_placeholderLabel);
            m_placeholderLabel->hide();
        }
        
        qDebug() << "UI文件预览加载成功:" << filePath << "控件类型:" << previewWidget->metaObject()->className();
        
    } catch (const std::exception &e) {
        qDebug() << "UI文件预览加载失败:" << e.what();
        createPlaceholder();
    }
}

void UIRuntimePreviewWidget::clearPreview()
{
    if (m_previewWidget) {
        m_mainLayout->removeWidget(m_previewWidget);
        delete m_previewWidget;
        m_previewWidget = nullptr;
    }
    
    m_previewLoaded = false;
    
    // 如果没有预览窗口，显示占位符
    if (!m_previewWidget && m_placeholderLabel) {
        m_mainLayout->addWidget(m_placeholderLabel);
        m_placeholderLabel->show();
    }
}

// WizardPreviewWidget 实现
WizardPreviewWidget::WizardPreviewWidget(QWidget *parent)
    : QWidget(parent)
    , m_currentWizard(nullptr)
    , m_previewWidget(nullptr)
    , m_placeholderLabel(nullptr)
    , m_pageTitleLabel(nullptr)
    , m_pageDescriptionLabel(nullptr)
    , m_navigationLabel(nullptr)
    , m_prevButton(nullptr)
    , m_nextButton(nullptr)
    , m_finishButton(nullptr)
    , m_cancelButton(nullptr)
    , m_mainLayout(nullptr)
    , m_navigationLayout(nullptr)
    , m_previewLoaded(false)
    , m_currentPageIndex(-1)
    , m_totalPages(0)
{
    createPreviewLayout();
    createPlaceholder();
    createNavigationControls();
}

WizardPreviewWidget::~WizardPreviewWidget()
{
    clearPreview();
}

void WizardPreviewWidget::createPreviewLayout()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(15, 15, 15, 15);
    m_mainLayout->setSpacing(15);
}

void WizardPreviewWidget::createPlaceholder()
{
    if (m_placeholderLabel) {
        m_mainLayout->removeWidget(m_placeholderLabel);
        delete m_placeholderLabel;
    }
    
    m_placeholderLabel = new QLabel(this);
    m_placeholderLabel->setAlignment(Qt::AlignCenter);
    m_placeholderLabel->setText("请加载向导以查看UI串联预览");
    m_placeholderLabel->setStyleSheet("QLabel { color: #666; font-size: 16px; padding: 80px; background-color: #f8f8f8; border: 2px dashed #ccc; border-radius: 8px; }");
    
    m_mainLayout->addWidget(m_placeholderLabel, 1);
}

void WizardPreviewWidget::createNavigationControls()
{
    // 创建页面标题和描述标签
    m_pageTitleLabel = new QLabel(this);
    m_pageTitleLabel->setAlignment(Qt::AlignCenter);
    m_pageTitleLabel->setStyleSheet("QLabel { color: #333; font-weight: bold; font-size: 18px; padding: 10px; background-color: #e8f5e8; border-radius: 6px; }");
    m_pageTitleLabel->hide();
    
    m_pageDescriptionLabel = new QLabel(this);
    m_pageDescriptionLabel->setAlignment(Qt::AlignCenter);
    m_pageDescriptionLabel->setStyleSheet("QLabel { color: #666; font-size: 14px; padding: 5px; }");
    m_pageDescriptionLabel->hide();
    
    // 创建导航标签
    m_navigationLabel = new QLabel(this);
    m_navigationLabel->setAlignment(Qt::AlignCenter);
    m_navigationLabel->setStyleSheet("QLabel { color: #999; font-size: 12px; padding: 5px; }");
    m_navigationLabel->hide();
    
    // 创建导航按钮
    m_prevButton = new QPushButton("上一步", this);
    m_prevButton->setStyleSheet("QPushButton { background-color: #6c757d; color: white; padding: 8px 16px; border-radius: 4px; }");
    m_prevButton->hide();
    
    m_nextButton = new QPushButton("下一步", this);
    m_nextButton->setStyleSheet("QPushButton { background-color: #007bff; color: white; padding: 8px 16px; border-radius: 4px; }");
    m_nextButton->hide();
    
    m_finishButton = new QPushButton("完成", this);
    m_finishButton->setStyleSheet("QPushButton { background-color: #28a745; color: white; padding: 8px 16px; border-radius: 4px; }");
    m_finishButton->hide();
    
    m_cancelButton = new QPushButton("取消", this);
    m_cancelButton->setStyleSheet("QPushButton { background-color: #dc3545; color: white; padding: 8px 16px; border-radius: 4px; }");
    m_cancelButton->hide();
    
    // 创建导航布局
    m_navigationLayout = new QHBoxLayout();
    m_navigationLayout->setSpacing(10);
    m_navigationLayout->addStretch();
    m_navigationLayout->addWidget(m_prevButton);
    m_navigationLayout->addWidget(m_nextButton);
    m_navigationLayout->addWidget(m_finishButton);
    m_navigationLayout->addWidget(m_cancelButton);
    m_navigationLayout->addStretch();
    
    // 连接按钮信号
    connect(m_prevButton, &QPushButton::clicked, this, &WizardPreviewWidget::previousPage);
    connect(m_nextButton, &QPushButton::clicked, this, &WizardPreviewWidget::nextPage);
    connect(m_finishButton, &QPushButton::clicked, this, &WizardPreviewWidget::onWizardCompleted);
    connect(m_cancelButton, &QPushButton::clicked, this, &WizardPreviewWidget::onWizardCancelled);
}

void WizardPreviewWidget::loadWizard(Wizard *wizard)
{
    clearPreview();
    
    if (!wizard) {
        qDebug() << "向导为空，无法加载预览";
        createPlaceholder();
        return;
    }
    
    m_currentWizard = wizard;
    m_totalPages = wizard->allPages().size();
    
    if (m_totalPages == 0) {
        qDebug() << "向导没有页面，无法预览";
        createPlaceholder();
        return;
    }
    
    // 显示导航控件
    m_pageTitleLabel->show();
    m_pageDescriptionLabel->show();
    m_navigationLabel->show();
    
    // 加载第一页
    goToPage(0);
    
    qDebug() << "向导预览加载成功，总页数:" << m_totalPages;
}

void WizardPreviewWidget::clearPreview()
{
    clearCurrentPage();
    
    m_currentWizard = nullptr;
    m_currentPageIndex = -1;
    m_totalPages = 0;
    m_previewLoaded = false;
    
    // 隐藏导航控件
    m_pageTitleLabel->hide();
    m_pageDescriptionLabel->hide();
    m_navigationLabel->hide();
    m_prevButton->hide();
    m_nextButton->hide();
    m_finishButton->hide();
    m_cancelButton->hide();
    
    // 显示占位符
    if (!m_previewWidget && m_placeholderLabel) {
        m_mainLayout->addWidget(m_placeholderLabel, 1);
        m_placeholderLabel->show();
    }
}

void WizardPreviewWidget::nextPage()
{
    if (m_currentWizard && m_currentPageIndex < m_totalPages - 1) {
        goToPage(m_currentPageIndex + 1);
    }
}

void WizardPreviewWidget::previousPage()
{
    if (m_currentWizard && m_currentPageIndex > 0) {
        goToPage(m_currentPageIndex - 1);
    }
}

void WizardPreviewWidget::goToPage(int pageIndex)
{
    if (!m_currentWizard || pageIndex < 0 || pageIndex >= m_totalPages) {
        return;
    }
    
    clearCurrentPage();
    
    m_currentPageIndex = pageIndex;
    
    // 加载当前页面内容
    loadCurrentPage();
    
    // 更新导航控件
    updateNavigationControls();
    
    emit pageChanged(pageIndex, currentPageTitle());
}

void WizardPreviewWidget::showPage(int pageIndex)
{
    goToPage(pageIndex);
}

QString WizardPreviewWidget::currentPageTitle() const
{
    if (m_currentWizard && m_currentPageIndex >= 0) {
        QList<WizardPage*> pages = m_currentWizard->allPages();
        if (m_currentPageIndex < pages.size()) {
            WizardPage *page = pages[m_currentPageIndex];
            if (page) {
                return page->title;
            }
        }
    }
    return "未知页面";
}

void WizardPreviewWidget::updateNavigationControls()
{
    if (!m_currentWizard) return;
    
    // 更新页面标题和描述
    QList<WizardPage*> pages = m_currentWizard->allPages();
    if (m_currentPageIndex < pages.size()) {
        WizardPage *currentPage = pages[m_currentPageIndex];
        if (currentPage) {
            m_pageTitleLabel->setText(currentPage->title);
            m_pageDescriptionLabel->setText(currentPage->description);
            
            // 添加到主布局（如果尚未添加）
            if (m_mainLayout->indexOf(m_pageTitleLabel) == -1) {
                m_mainLayout->insertWidget(0, m_pageTitleLabel);
                m_mainLayout->insertWidget(1, m_pageDescriptionLabel);
            }
        }
    }
    
    // 更新导航标签
    m_navigationLabel->setText(QString("第 %1 页 / 共 %2 页").arg(m_currentPageIndex + 1).arg(m_totalPages));
    
    // 更新按钮状态
    m_prevButton->setEnabled(m_currentPageIndex > 0);
    m_nextButton->setEnabled(m_currentPageIndex < m_totalPages - 1);
    m_finishButton->setEnabled(m_currentPageIndex == m_totalPages - 1);
    
    // 显示导航控件
    m_prevButton->show();
    m_nextButton->show();
    m_finishButton->show();
    m_cancelButton->show();
    
    // 添加导航布局（如果尚未添加）
    if (m_mainLayout->indexOf(m_navigationLayout) == -1) {
        m_mainLayout->addLayout(m_navigationLayout);
    }
}

void WizardPreviewWidget::loadCurrentPage()
{
    if (!m_currentWizard || m_currentPageIndex < 0) return;
    
    QList<WizardPage*> pages = m_currentWizard->allPages();
    if (m_currentPageIndex >= pages.size()) return;
    
    WizardPage *currentPage = pages[m_currentPageIndex];
    if (!currentPage) return;
    
    // 创建预览容器
    QWidget *containerWidget = new QWidget(this);
    containerWidget->setStyleSheet("QWidget { background-color: white; border: 2px solid #4CAF50; border-radius: 8px; }");
    
    QVBoxLayout *containerLayout = new QVBoxLayout(containerWidget);
    containerLayout->setContentsMargins(20, 20, 20, 20);
    containerLayout->setSpacing(10);
    
    // 添加页面内容预览
    QLabel *contentLabel = new QLabel("页面内容预览: " + currentPage->title, containerWidget);
    contentLabel->setStyleSheet("QLabel { color: #333; font-size: 16px; font-weight: bold; }");
    contentLabel->setAlignment(Qt::AlignCenter);
    containerLayout->addWidget(contentLabel);
    
    // 添加页面描述
    QLabel *descriptionLabel = new QLabel(currentPage->description, containerWidget);
    descriptionLabel->setStyleSheet("QLabel { color: #666; font-size: 14px; padding: 10px; }");
    descriptionLabel->setWordWrap(true);
    descriptionLabel->setAlignment(Qt::AlignCenter);
    containerLayout->addWidget(descriptionLabel);
    
    // 添加页面类型指示
    QString pageType = "向导页面";
    QLabel *typeLabel = new QLabel("类型: " + pageType, containerWidget);
    typeLabel->setStyleSheet("QLabel { color: #999; font-size: 12px; padding: 5px; }");
    typeLabel->setAlignment(Qt::AlignCenter);
    containerLayout->addWidget(typeLabel);
    
    m_previewWidget = containerWidget;
    m_mainLayout->insertWidget(2, m_previewWidget, 1);
    m_previewLoaded = true;
    
    // 移除占位符
    if (m_placeholderLabel) {
        m_mainLayout->removeWidget(m_placeholderLabel);
        m_placeholderLabel->hide();
    }
}

void WizardPreviewWidget::clearCurrentPage()
{
    if (m_previewWidget) {
        m_mainLayout->removeWidget(m_previewWidget);
        delete m_previewWidget;
        m_previewWidget = nullptr;
    }
    
    m_previewLoaded = false;
}

void WizardPreviewWidget::onWizardPageChanged(int pageIndex)
{
    goToPage(pageIndex);
}

void WizardPreviewWidget::onWizardCompleted()
{
    QMessageBox::information(this, "向导完成", "向导已成功完成！");
    clearPreview();
    emit wizardCompleted();
}

void WizardPreviewWidget::onWizardCancelled()
{
    QMessageBox::information(this, "向导取消", "向导已被取消。");
    clearPreview();
    emit wizardCancelled();
}

void StateMachineEditorV2::setStateMachineManager(StateMachineManager *manager)
{
    m_stateMachineManager = manager;
    
    // 设置向导管理器
    if (manager) {
        qDebug() << "State machine manager is not empty, getting wizard manager...";
        m_wizardManager = manager->wizardManager();
        
        // 如果向导管理器不存在，自动创建一个
        if (!m_wizardManager) {
            qDebug() << "Wizard manager is empty, creating a new one...";
            m_wizardManager = new WizardManager(this);
            manager->setWizardManager(m_wizardManager);
            qDebug() << "New wizard manager created and set successfully";
        } else {
            qDebug() << "Wizard manager successfully set, number of wizards:" << m_wizardManager->wizards().size();
        }
    } else {
        qDebug() << "Error: State machine manager is empty!";
    }
}

void StateMachineEditorV2::setProductUiFiles(const QList<ProductUIFile> &uiFiles)
{
    qDebug() << "setProductUiFiles() called with" << uiFiles.size() << "UI files";
    
    // 使用深拷贝确保数据安全，避免引用问题
    m_productUiFiles.clear();
    for (const auto &uiFile : uiFiles) {
        m_productUiFiles.append(uiFile);
    }
    
    qDebug() << "m_productUiFiles size after assignment:" << m_productUiFiles.size();
    
    // 更新UI文件列表显示 - 只有在控件已创建时才调用
    qDebug() << "Calling updateUIFilesList() from setProductUiFiles";
    qDebug() << "m_uiFilesListWidget pointer:" << m_uiFilesListWidget;
    
    if (m_uiFilesListWidget) {
        updateUIFilesList();
    } else {
        qDebug() << "UI files list widget not yet created, UI files data saved for later initialization";
        // 设置标志，等待控件创建后自动更新
        m_uiFilesDataPendingUpdate = true;
    }
}

void StateMachineEditorV2::setProduct(Product *product)
{
    m_product = product;
    
    // 设置产品后立即加载向导文件
    if (m_product) {
        loadWizardsFromProductConfig();
    }
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
    
    QJsonObject json = doc.object();
    
    // 检查是否为状态机格式
    if (json.contains("uiFlowStateMachine") && 
        json.contains("logicSequenceStateMachine") &&
        json.contains("integrationManager")) {
        
        // 加载UI流状态机
        QJsonObject uiFlowObj = json["uiFlowStateMachine"].toObject();
        if (!m_integrationManager->uiFlowStateMachine()) {
            m_integrationManager->setUiFlowStateMachine(new UIFlowStateMachine("UI流状态机", this));
        }
        if (!m_integrationManager->uiFlowStateMachine()->fromJson(uiFlowObj)) {
            QMessageBox::warning(this, "错误", "加载UI流状态机失败");
            return;
        }
        
        // 加载逻辑时序状态机
        QJsonObject logicObj = json["logicSequenceStateMachine"].toObject();
        if (!m_integrationManager->logicSequenceStateMachine()) {
            m_integrationManager->setLogicSequenceStateMachine(new LogicSequenceStateMachine("逻辑时序状态机", this));
        }
        if (!m_integrationManager->logicSequenceStateMachine()->fromJson(logicObj)) {
            QMessageBox::warning(this, "错误", "加载逻辑时序状态机失败");
            return;
        }
        
        // 加载集成管理器
        QJsonObject integrationObj = json["integrationManager"].toObject();
        if (!m_integrationManager->fromJson(integrationObj)) {
            QMessageBox::warning(this, "错误", "加载集成管理器失败");
            return;
        }
        
        // 设置向导状态
        m_currentWizardName = QFileInfo(filePath).baseName();
        m_currentWizardFilePath = filePath;
        m_isWizardModified = false;
        updateWindowTitle();
        
        // 状态机文件加载完成后，加载向导文件
        if (m_product) {
            loadWizardsFromProductConfig();
        }
        
        QMessageBox::information(this, "成功", "状态机文件已加载: " + filePath);
    } else {
        QMessageBox::warning(this, "错误", "状态机文件不是正确格式，只支持状态机格式: " + filePath);
        return;
    }
}

void StateMachineEditorV2::saveStateMachine(const QString &filePath)
{
    if (filePath.isEmpty()) return;
    
    QJsonObject json;
    
    switch (m_currentMode) {
    case StateMachineMode::UIFlowMode:
        if (m_integrationManager->uiFlowStateMachine()) {
            json = m_integrationManager->uiFlowStateMachine()->toJson();
        }
        break;
    case StateMachineMode::LogicSequenceMode:
        if (m_integrationManager->logicSequenceStateMachine()) {
            json = m_integrationManager->logicSequenceStateMachine()->toJson();
        }
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
    QString message;
    
    switch (m_currentMode) {
    case StateMachineMode::UIFlowMode: {
        // 在UI流模式下，直接创建向导
        createNewWizard();
        return;
    }
    case StateMachineMode::LogicSequenceMode:
        if (!m_integrationManager->logicSequenceStateMachine()) {
            m_integrationManager->setLogicSequenceStateMachine(new LogicSequenceStateMachine("逻辑时序状态机", this));
        }
        m_integrationManager->setLogicSequenceStateMachine(new LogicSequenceStateMachine("逻辑时序状态机", this));
        message = "已成功创建新的状态机";
        break;
    }
    
    if (m_uiFlowScene) m_uiFlowScene->refreshScene();
    if (m_logicScene) m_logicScene->refreshScene();
    
    QMessageBox::information(this, "成功", message);
}

void StateMachineEditorV2::switchWizardFile()
{
    qDebug() << "switchWizardFile() called";
    
    // 检查是否需要保存当前向导
    if (m_isWizardModified && !m_currentWizardName.isEmpty()) {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "保存向导", 
            "向导 \"" + m_currentWizardName + "\" 已修改但未保存，是否先保存？",
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        
        if (reply == QMessageBox::Cancel) {
            return; // 用户取消操作
        } else if (reply == QMessageBox::Save) {
            // 保存当前向导
            saveCurrentWizard();
        }
    }
    
    // 打开文件对话框选择新的向导文件
    QString filePath = QFileDialog::getOpenFileName(this, "选择向导文件", 
        QDir::currentPath(), "向导文件 (*.json)");
    
    if (!filePath.isEmpty()) {
        qDebug() << "User selected wizard file:" << filePath;
        
        // 更新state_machine.json中的wizardJsonPath
    if (m_product && m_product->hasStateMachine()) {
        // 这里需要实现更新state_machine.json的逻辑
        // 由于Product类没有直接提供stateMachine()方法，需要从文件系统读取和更新
        QString configRootPath = m_product->configPackageRootPath();
        if (!configRootPath.isEmpty()) {
            QString stateMachinePath = configRootPath + "/state_machine.json";
            QFile file(stateMachinePath);
            if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
                QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
                QJsonObject config = doc.object();
                config["wizardJsonPath"] = filePath;
                
                file.seek(0);
                file.write(QJsonDocument(config).toJson());
                file.resize(file.pos());
                
                // 同时更新缓存信息
                m_cachedWizardJsonPath = filePath;
                
                qDebug() << "Updated wizardJsonPath in state_machine.json to:" << filePath;
            }
        }
    }
        
        // 重新加载向导
        loadWizardsFromProductConfig();
        
        QMessageBox::information(this, "切换向导文件", "向导文件已成功切换到: " + filePath);
    }
}

void StateMachineEditorV2::createNewWizard()
{
    qDebug() << "createNewWizard() called";
    qDebug() << "m_wizardManager:" << m_wizardManager;
    
    // 防止重复调用
    static bool isCreating = false;
    if (isCreating) {
        qDebug() << "createNewWizard() already in progress, ignoring duplicate call";
        return;
    }
    
    isCreating = true;
    
    // 检查是否需要保存当前向导
    if (m_isWizardModified && !m_currentWizardName.isEmpty()) {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "保存向导", 
            "向导 \"" + m_currentWizardName + "\" 已修改但未保存，是否先保存？",
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        
        if (reply == QMessageBox::Cancel) {
            isCreating = false;
            return; // 用户取消操作
        } else if (reply == QMessageBox::Save) {
            // 保存当前向导
            saveCurrentWizard();
        }
    }
    
    // 创建新向导
    if (m_wizardManager) {
        QString name = QInputDialog::getText(this, "创建新向导", "请输入向导名称:");
        if (!name.isEmpty()) {
            QString description = QInputDialog::getText(this, "创建新向导", "请输入向导描述:");
            Wizard* wizard = m_wizardManager->createWizard(name, description);
            
            // 创建并显示向导界面
            if (wizard) {
                // 设置当前向导状态
                m_currentWizardName = name;
                m_currentWizardFilePath = ""; // 新创建，未保存
                m_isWizardModified = true;
                
                qDebug() << "Wizard created successfully, calling updateWindowTitle()";
                
                // 更新标题栏显示
                updateWindowTitle();
                
                QMessageBox::information(this, "创建向导", "向导已成功创建!");
                
                // 切换到向导模式
                switchMode(StateMachineMode::UIFlowMode);
                
                // 启动向导
                if (wizard->start()) {
                    qDebug() << "Wizard started";
                } else {
                    qWarning() << "向导启动失败";
                }
            }
        }
    } else {
        qDebug() << "m_wizardManager is nullptr, cannot create wizard";
        QMessageBox::warning(this, "创建向导", "向导管理器未初始化，无法创建向导。");
    }
    
    isCreating = false;
}

void StateMachineEditorV2::editWizard()
{
    // 编辑向导
    if (m_wizardManager) {
        if (m_currentWizardName.isEmpty()) {
            QMessageBox::warning(this, "编辑向导", "没有可编辑的向导，请先创建向导。");
            return;
        }
        
        // 这里可以添加编辑向导的UI界面
        // 暂时使用简单的输入对话框
        QString newName = QInputDialog::getText(this, "编辑向导名称", "请输入新的向导名称:", QLineEdit::Normal, m_currentWizardName);
        if (!newName.isEmpty() && newName != m_currentWizardName) {
            m_currentWizardName = newName;
            m_isWizardModified = true;
            updateWindowTitle();
            
            QMessageBox::information(this, "编辑向导", "向导名称已更新为: " + newName);
        }
    }
}

void StateMachineEditorV2::deleteWizard()
{
    // 删除向导
    if (m_wizardManager) {
        // 这里可以添加删除向导的UI界面
        QMessageBox::information(this, "删除向导", "删除向导功能将在后续版本中实现。");
    }
}

void StateMachineEditorV2::runWizard()
{
    // 运行向导并显示UI串联预览
    if (m_wizardManager) {
        Wizard* wizard = nullptr;
        
        // 优先使用当前向导管理器中的向导
        wizard = m_wizardManager->currentWizard();
        
        // 如果没有当前向导，但已经通过文件加载了向导名称，则尝试查找对应的向导
        if (!wizard && !m_currentWizardName.isEmpty()) {
            wizard = m_wizardManager->findWizard(m_currentWizardName);
            
            // 如果找到了向导，将其设置为当前向导
            if (wizard) {
                m_wizardManager->setCurrentWizard(wizard);
                qDebug() << "Found and set current wizard:" << m_currentWizardName;
            }
        }
        
        if (wizard) {
            // 切换到向导模式
            switchToUIFlowMode();
            
            // 创建UI串联预览窗口
            if (!m_wizardPreviewWidget) {
                m_wizardPreviewWidget = new WizardPreviewWidget(this);
                
                // 连接信号槽
                connect(m_wizardPreviewWidget, &WizardPreviewWidget::nextPageRequested, 
                        this, &StateMachineEditorV2::onWizardNextPage);
                connect(m_wizardPreviewWidget, &WizardPreviewWidget::previousPageRequested, 
                        this, &StateMachineEditorV2::onWizardPreviousPage);
                connect(m_wizardPreviewWidget, &WizardPreviewWidget::closePreviewRequested, 
                        this, &StateMachineEditorV2::onWizardPreviewClosed);
            }
            
            // 加载向导到预览窗口
            m_wizardPreviewWidget->loadWizard(wizard);
            
            // 显示预览窗口
            m_wizardPreviewWidget->show();
            m_wizardPreviewWidget->raise();
            m_wizardPreviewWidget->activateWindow();
            
            qDebug() << "Wizard preview started for wizard:" << wizard->name();
        } else {
            QMessageBox::warning(this, "运行向导", "没有可运行的向导，请先创建向导。");
        }
    }
}

void StateMachineEditorV2::editStateProperties()
{
    // 根据当前模式编辑状态属性
    switch (m_currentMode) {
    case StateMachineMode::UIFlowMode:
        if (!m_currentUIFlowState.stateId.isEmpty()) {
            // 打开UI流状态属性编辑器
        }
        break;
    case StateMachineMode::LogicSequenceMode:
        if (!m_currentLogicState.stateId.isEmpty()) {
            // 打开逻辑状态属性编辑器
        }
        break;
    }
}

void StateMachineEditorV2::editTransitionProperties()
{
    // 根据当前模式编辑转换属性
    switch (m_currentMode) {
    case StateMachineMode::UIFlowMode:
        if (!m_currentUIFlowTransition.transitionId.isEmpty()) {
            // 打开UI流转换属性编辑器
        }
        break;
    case StateMachineMode::LogicSequenceMode:
        if (!m_currentLogicTransition.transitionId.isEmpty()) {
            // 打开逻辑转换属性编辑器
        }
        break;
    }
}

void StateMachineEditorV2::setInitialState()
{
    // 根据当前模式设置初始状态
    switch (m_currentMode) {
    case StateMachineMode::UIFlowMode:
        if (!m_currentUIFlowState.stateId.isEmpty()) {
            m_integrationManager->uiFlowStateMachine()->setInitialState(m_currentUIFlowState.stateId);
            if (m_uiFlowScene) m_uiFlowScene->refreshScene();
        }
        break;
    case StateMachineMode::LogicSequenceMode:
        if (!m_currentLogicState.stateId.isEmpty()) {
            m_integrationManager->logicSequenceStateMachine()->setInitialState(m_currentLogicState.stateId);
            if (m_logicScene) m_logicScene->refreshScene();
        }
        break;
    }
}

void StateMachineEditorV2::validateStateMachine()
{
    bool valid = false;
    QString message;
    
    switch (m_currentMode) {
    case StateMachineMode::UIFlowMode:
        if (m_integrationManager->uiFlowStateMachine()) {
            valid = m_integrationManager->uiFlowStateMachine()->validate();
            message = valid ? "UI流状态机验证通过" : "UI流状态机验证失败";
        }
        break;
    case StateMachineMode::LogicSequenceMode:
        if (m_integrationManager->logicSequenceStateMachine()) {
            valid = m_integrationManager->logicSequenceStateMachine()->validate();
            message = valid ? "逻辑时序状态机验证通过" : "逻辑时序状态机验证失败";
        }
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
    switchMode(StateMachineMode::UIFlowMode);
}

void StateMachineEditorV2::switchToLogicSequenceMode()
{
    switchMode(StateMachineMode::LogicSequenceMode);
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

void StateMachineEditorV2::onUiInterfaceChanged()
{
    if (!m_integrationManager->uiFlowStateMachine()) return;
    
    // 从列表控件获取当前选择的UI文件
    QList<QListWidgetItem*> selectedItems = m_uiFilesListWidget->selectedItems();
    if (selectedItems.isEmpty()) {
        qDebug() << "No UI file is currently selected, UI interface selection will not take effect";
        return;
    }
    
    QString uiInterfaceId = selectedItems.first()->data(Qt::UserRole).toString();
    
    if (m_currentUIFlowState.stateId.isEmpty()) {
        qDebug() << "No UI flow state is currently selected, UI interface selection will not take effect";
        return;
    }
    
    // 更新当前状态的UI界面ID
    m_currentUIFlowState.uiInterfaceId = uiInterfaceId;
    m_integrationManager->uiFlowStateMachine()->updateState(m_currentUIFlowState);
    
    if (m_uiFlowScene) {
        m_uiFlowScene->refreshScene();
        qDebug() << "UI flow state" << m_currentUIFlowState.name << "UI interface updated to:" 
                 << selectedItems.first()->text() << "(" << uiInterfaceId << ")";
    }
}

void StateMachineEditorV2::onModeChanged()
{
    // 更新步骤按钮状态
    if (m_currentMode == StateMachineMode::UIFlowMode) {
        m_step1Button->setChecked(true);
        m_step2Button->setChecked(false);
        m_step2Button->setEnabled(true); // 完成步骤1后启用步骤2
    } else if (m_currentMode == StateMachineMode::LogicSequenceMode) {
        m_step1Button->setChecked(false);
        m_step2Button->setChecked(true);
    }
}

void StateMachineEditorV2::createModeSelector()
{
    m_modeGroup = new QGroupBox("状态机编辑步骤", this);
    QVBoxLayout *modeLayout = new QVBoxLayout(m_modeGroup);
    
    // 创建步骤指示器
    QHBoxLayout *stepLayout = new QHBoxLayout();
    
    // 步骤1：UI流模式
    m_step1Button = new QPushButton("步骤1：UI流模式", m_modeGroup);
    m_step1Button->setCheckable(true);
    m_step1Button->setChecked(true);
    m_step1Button->setStyleSheet("QPushButton:checked { background-color: #4CAF50; color: white; }");
    
    // 步骤箭头
    QLabel *arrowLabel1 = new QLabel("→", m_modeGroup);
    arrowLabel1->setAlignment(Qt::AlignCenter);
    
    // 步骤2：逻辑时序模式
    m_step2Button = new QPushButton("步骤2：逻辑时序模式", m_modeGroup);
    m_step2Button->setCheckable(true);
    m_step2Button->setEnabled(false); // 初始禁用，需要先完成步骤1
    m_step2Button->setStyleSheet("QPushButton:checked { background-color: #2196F3; color: white; }");
    
    stepLayout->addWidget(m_step1Button);
    stepLayout->addWidget(arrowLabel1);
    stepLayout->addWidget(m_step2Button);
    
    modeLayout->addLayout(stepLayout);
    
    // 步骤描述标签
    m_modeDescription = new QLabel("步骤1：UI流模式 - 串联多个UI界面，实现界面交互流程", m_modeGroup);
    m_modeDescription->setWordWrap(true);
    modeLayout->addWidget(m_modeDescription);
    
    // 连接信号
    connect(m_step1Button, &QPushButton::clicked, this, [this]() {
        if (m_currentMode != StateMachineMode::UIFlowMode) {
            switchMode(StateMachineMode::UIFlowMode);
        }
    });
    
    connect(m_step2Button, &QPushButton::clicked, this, [this]() {
        if (m_currentMode != StateMachineMode::LogicSequenceMode) {
            switchMode(StateMachineMode::LogicSequenceMode);
        }
    });
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
    
    // 向导操作
    QAction *newWizardAction = new QAction("新建向导", this);
    QAction *editWizardAction = new QAction("编辑向导", this);
    QAction *deleteWizardAction = new QAction("删除向导", this);
    QAction *runWizardAction = new QAction("运行向导", this);
    
    // 添加所有操作到工具栏
    m_toolbar->addAction(newAction);
    m_toolbar->addAction(openAction);
    m_toolbar->addAction(saveAction);
    
    // 保存操作指针以便后续控制可见性
    m_fileActions = {newAction, openAction, saveAction};
    m_editActions = {addStateAction, addTransitionAction, deleteAction};
    m_toolActions = {validateAction, previewAction};
    m_wizardActions = {newWizardAction, editWizardAction, deleteWizardAction, runWizardAction};
    
    // 连接信号
    connect(newAction, &QAction::triggered, this, &StateMachineEditorV2::createNewStateMachine);
    connect(openAction, &QAction::triggered, this, [this]() {
        QString title = "打开状态机文件";
        QString filter = "状态机文件 (*.json)";
        
        // 根据当前模式设置不同的标题和过滤器
        if (m_currentMode == StateMachineMode::UIFlowMode) {
            title = "打开向导文件";
            filter = "向导文件 (*.json)";
        }
        
        QString filePath = QFileDialog::getOpenFileName(this, title, "", filter);
        if (!filePath.isEmpty()) loadStateMachine(filePath);
    });
    connect(saveAction, &QAction::triggered, this, [this]() {
        QString title = "保存状态机文件";
        QString filter = "状态机文件 (*.json)";
        
        // 根据当前模式设置不同的保存逻辑
        if (m_currentMode == StateMachineMode::UIFlowMode) {
            // UI流模式下保存向导
            saveCurrentWizard();
            return;
        } else {
            // 逻辑时序模式下保存状态机
            title = "保存状态机文件";
            filter = "状态机文件 (*.json)";
        }
        
        QString filePath = QFileDialog::getSaveFileName(this, title, "", filter);
        if (!filePath.isEmpty()) saveStateMachine(filePath);
    });
    connect(validateAction, &QAction::triggered, this, &StateMachineEditorV2::validateStateMachine);
    connect(previewAction, &QAction::triggered, this, &StateMachineEditorV2::showRuntimePreview);
    
    // 连接向导信号
    connect(newWizardAction, &QAction::triggered, this, &StateMachineEditorV2::createNewWizard);
    connect(editWizardAction, &QAction::triggered, this, &StateMachineEditorV2::editWizard);
    connect(deleteWizardAction, &QAction::triggered, this, &StateMachineEditorV2::deleteWizard);
    connect(runWizardAction, &QAction::triggered, this, &StateMachineEditorV2::runWizard);
}

void StateMachineEditorV2::createPropertiesPanel()
{
    m_propertiesPanel = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(m_propertiesPanel);
    
    // 创建属性选项卡
    m_propertiesTab = new QTabWidget(m_propertiesPanel);
    
    // UI流属性页 - 直接创建向导编辑界面，不创建初始的表单布局
    m_uiFlowProperties = new QWidget();
    
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
    
    m_propertiesTab->addTab(m_uiFlowProperties, "向导编辑");
    m_propertiesTab->addTab(m_logicProperties, "逻辑属性");
    
    layout->addWidget(m_propertiesTab);
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
void StateMachineEditorV2::printUiInterfaceComboCount() {
    qDebug() << "[DEBUG] printUiInterfaceComboCount() called";
    qDebug() << "[DEBUG] m_uiFilesListWidget pointer:" << m_uiFilesListWidget;
    
    if (m_uiFilesListWidget) {
        qDebug() << "[DEBUG] current uiFiles List count:" << m_uiFilesListWidget->count();
        qDebug() << "[DEBUG] m_productUiFiles size:" << m_productUiFiles.size();
    } else {
        qDebug() << "[DEBUG] UI files list widget is null, cannot get count";
        qDebug() << "[DEBUG] m_productUiFiles size:" << m_productUiFiles.size();
        qDebug() << "[DEBUG] m_uiFilesDataPendingUpdate flag:" << m_uiFilesDataPendingUpdate;
    }
}
void StateMachineEditorV2::updatePropertiesPanel()
{
    // 根据当前模式更新属性面板
    switch (m_currentMode) {
    case StateMachineMode::UIFlowMode:
        m_propertiesTab->setCurrentWidget(m_uiFlowProperties);
        
        if (!m_currentUIFlowState.stateId.isEmpty()) {
            m_uiFlowStateNameEdit->setText(m_currentUIFlowState.name);
            m_uiFlowStateDescriptionEdit->setPlainText(m_currentUIFlowState.description);
            m_initialStateCheck->setChecked(m_currentUIFlowState.isInitialState);
            m_finalStateCheck->setChecked(m_currentUIFlowState.isFinalState);
            
            // UI界面选择已转移到m_uiFilesListWidget，无需设置下拉框
        }
        break;
    case StateMachineMode::LogicSequenceMode:
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
    }
}

void StateMachineEditorV2::setupRuntime()
{
    // 设置运行时预览
    if (m_runtime && m_integrationManager) {
        // 根据当前模式设置运行时状态机
        // TODO: 需要适配不同类型的状态机到运行时引擎
        switch (m_currentMode) {
        case StateMachineMode::UIFlowMode:
            // UIFlowStateMachine 不兼容 StateMachineRuntime，需要适配器
            // m_runtime->setStateMachine(m_integrationManager->uiFlowStateMachine());
            break;
        case StateMachineMode::LogicSequenceMode:
            // 逻辑时序状态机可能需要特殊的运行时处理
            break;
        }
    }
}

void StateMachineEditorV2::switchMode(StateMachineMode newMode)
{
    if (m_currentMode == newMode) return;
    
    m_currentMode = newMode;
    
    switch (newMode) {
    case StateMachineMode::UIFlowMode:
        setupUIFlowMode();
        break;
    case StateMachineMode::LogicSequenceMode:
        setupLogicSequenceMode();
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
        m_view->setCurrentMode(StateMachineMode::UIFlowMode);
    }
    
    // 设置集成管理器为UI流模式
    m_integrationManager->setIntegrationMode(StateMachineIntegrationManager::IntegrationMode::UIFlowMode);
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
        m_view->setCurrentMode(StateMachineMode::LogicSequenceMode);
    }
    
    // 设置集成管理器为逻辑时序模式
    m_integrationManager->setIntegrationMode(StateMachineIntegrationManager::IntegrationMode::LogicSequenceMode);
}



void StateMachineEditorV2::updateModeUI()
{
    // 更新模式描述
    switch (m_currentMode) {
    case StateMachineMode::UIFlowMode:
        m_modeDescription->setText("步骤1：UI流模式 - 串联多个UI界面，实现界面交互流程");
        break;
    case StateMachineMode::LogicSequenceMode:
        m_modeDescription->setText("步骤2：逻辑时序模式 - 定义软件业务逻辑运行流程");
        break;
    }
    
    // 更新工具栏和属性面板的可用性
    updateToolbarAvailability();
    updatePropertiesPanelAvailability();
}

void StateMachineEditorV2::updateToolbarAvailability()
{
    // 根据当前模式更新工具栏按钮的可用性
    bool isUIFlowMode = (m_currentMode == StateMachineMode::UIFlowMode);
    
    // 在UI流模式下只显示向导相关操作
    for (QAction *action : m_fileActions) {
        action->setVisible(!isUIFlowMode);
    }
    for (QAction *action : m_editActions) {
        action->setVisible(!isUIFlowMode);
    }
    for (QAction *action : m_toolActions) {
        action->setVisible(!isUIFlowMode);
    }
    for (QAction *action : m_wizardActions) {
        action->setVisible(true);
    }
}

void StateMachineEditorV2::updatePropertiesPanelAvailability()
{
    // 根据当前模式更新属性面板的可用性
    switch (m_currentMode) {
    case StateMachineMode::UIFlowMode:
        // UI流模式下显示向导编辑界面
        m_propertiesTab->show();
        m_propertiesTab->setTabEnabled(0, true);  // UI流属性页
        m_propertiesTab->setTabEnabled(1, false); // 逻辑属性页
        m_propertiesTab->setCurrentIndex(0);      // 切换到UI流属性页
        
        // 更新UI流属性页的内容为向导编辑界面
        updateWizardEditorInterface();
        break;
    case StateMachineMode::LogicSequenceMode:
        // 逻辑时序模式下显示逻辑属性页，隐藏UI流属性页
        m_propertiesTab->show();
        m_propertiesTab->setTabEnabled(0, false); // UI流属性页
        m_propertiesTab->setTabEnabled(1, true);  // 逻辑属性页
        m_propertiesTab->setCurrentIndex(1);      // 切换到逻辑属性页
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
    , m_currentMode(StateMachineMode::UIFlowMode)
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
    case StateMachineMode::UIFlowMode:
        if (m_uiFlowScene) {
            setScene(m_uiFlowScene);
        }
        break;
    case StateMachineMode::LogicSequenceMode:
        if (m_logicScene) {
            setScene(m_logicScene);
        }
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

void StateMachineEditorV2::updateWizardEditorInterface()
{
    // 在UI流模式下更新属性面板为向导编辑界面
    if (m_currentMode != StateMachineMode::UIFlowMode) {
        return;
    }
    
    qDebug() << "Starting to update wizard editing interface...";
    
    // 清空UI流属性面板的内容 - 使用Qt自动清理机制
    if (m_uiFlowProperties) {
        // 删除现有布局及其所有子组件
        QLayout *existingLayout = m_uiFlowProperties->layout();
        if (existingLayout) {
            qDebug() << "Clearing existing layout...";
            QLayoutItem *item;
            while ((item = existingLayout->takeAt(0)) != nullptr) {
                if (item->widget()) {
                    item->widget()->deleteLater();
                }
                delete item;
            }
            delete existingLayout;
        }
    }
    
    qDebug() << "Creating new wizard editing interface layout...";
    
    // 创建滚动区域和滚动内容
    QScrollArea *scrollArea = new QScrollArea(m_uiFlowProperties);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setFrameShape(QFrame::NoFrame);
    
    // 创建滚动内容容器
    QWidget *scrollContent = new QWidget();
    scrollContent->setMinimumWidth(400); // 设置最小宽度以确保内容正常显示
    
    // 创建向导编辑界面
    QVBoxLayout *wizardLayout = new QVBoxLayout(scrollContent);
    wizardLayout->setSpacing(15);  // 增加整体间距
    wizardLayout->setContentsMargins(15, 15, 15, 15);  // 增加边距
    
    // 1. 当前向导文件区域 - 添加分组框（压缩布局）
    QGroupBox *fileGroup = new QGroupBox("当前向导文件", m_uiFlowProperties);
    fileGroup->setStyleSheet("QGroupBox { font-weight: bold; font-size: 10pt; margin-top: 5px; }");
    
    QVBoxLayout *fileLayout = new QVBoxLayout(fileGroup);
    fileLayout->setSpacing(4);
    
    QLabel *wizardFilePathLabel = new QLabel("", m_uiFlowProperties);
    wizardFilePathLabel->setObjectName("wizardFilePathLabel");
    wizardFilePathLabel->setWordWrap(true);
    wizardFilePathLabel->setStyleSheet("border: 1px solid #ddd; padding: 4px; background-color: #f8f9fa; border-radius: 3px; font-size: 9pt;");
    
    // 显示当前向导文件路径
    updateWizardFilePathLabel(wizardFilePathLabel);
    
    fileLayout->addWidget(wizardFilePathLabel);
    
    // 2. 向导操作按钮区域 - 添加分组框（压缩布局）
    QGroupBox *actionGroup = new QGroupBox("向导操作", m_uiFlowProperties);
    actionGroup->setStyleSheet("QGroupBox { font-weight: bold; font-size: 10pt; margin-top: 5px; }");
    
    QHBoxLayout *buttonLayout = new QHBoxLayout(actionGroup);
    buttonLayout->setSpacing(6);
    
    QPushButton *runWizardButton = new QPushButton("运行向导", m_uiFlowProperties);
    runWizardButton->setMinimumHeight(28);
    runWizardButton->setStyleSheet("QPushButton { font-size: 9pt; padding: 4px 12px; }");
    
    buttonLayout->addWidget(runWizardButton);
    buttonLayout->addStretch();
    
    // 3. 向导属性编辑区域 - 添加分组框（压缩布局）
    QGroupBox *propertyGroup = new QGroupBox("向导属性", m_uiFlowProperties);
    propertyGroup->setStyleSheet("QGroupBox { font-weight: bold; font-size: 10pt; margin-top: 5px; }");
    
    QFormLayout *propertyLayout = new QFormLayout(propertyGroup);
    propertyLayout->setSpacing(6);
    propertyLayout->setLabelAlignment(Qt::AlignRight);
    
    QLineEdit *wizardNameEdit = new QLineEdit(m_uiFlowProperties);
    wizardNameEdit->setMinimumHeight(25);
    wizardNameEdit->setStyleSheet("QLineEdit { font-size: 9pt; padding: 3px; }");
    
    QTextEdit *wizardDescriptionEdit = new QTextEdit(m_uiFlowProperties);
    wizardDescriptionEdit->setMinimumHeight(60);
    wizardDescriptionEdit->setMaximumHeight(80);
    wizardDescriptionEdit->setStyleSheet("QTextEdit { font-size: 9pt; padding: 3px; }");
    
    // 如果当前有向导，填充属性
    if (!m_currentWizardName.isEmpty()) {
        wizardNameEdit->setText(m_currentWizardName);
        
        // 如果有向导管理器，获取描述
        if (m_wizardManager && !m_wizardManager->wizards().isEmpty()) {
            Wizard* wizard = m_wizardManager->wizards().first();
            if (wizard) {
                wizardDescriptionEdit->setText(wizard->description());
            }
        }
    }
    
    propertyLayout->addRow("名称:", wizardNameEdit);
    propertyLayout->addRow("描述:", wizardDescriptionEdit);
    
    // 布局设置 - 使用垂直布局替代水平布局，解决控件遮挡问题
    QHBoxLayout *combinedLayout = new QHBoxLayout();
    combinedLayout->setSpacing(12);
    combinedLayout->setContentsMargins(0, 0, 0, 0);
    
    // 移除固定宽度设置，让控件自适应宽度
    fileGroup->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    actionGroup->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    propertyGroup->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    combinedLayout->addWidget(fileGroup);
    combinedLayout->addWidget(actionGroup);
    combinedLayout->addWidget(propertyGroup);
    combinedLayout->addStretch();
    
    wizardLayout->addLayout(combinedLayout);
    wizardLayout->addStretch();
    
    // 连接信号槽
    connect(runWizardButton, &QPushButton::clicked, this, &StateMachineEditorV2::runWizard);
    
    // 向导属性编辑
    connect(wizardNameEdit, &QLineEdit::textChanged, this, 
            [=](const QString &text) {
                // 更新当前向导名称
                m_currentWizardName = text;
                m_isWizardModified = true;
                updateWindowTitle();
            });
    
    connect(wizardDescriptionEdit, &QTextEdit::textChanged, this, 
            [=]() {
                // 标记向导已修改
                m_isWizardModified = true;
                updateWindowTitle();
            });
    
    // 设置滚动内容
    scrollArea->setWidget(scrollContent);
    
    // 添加UI文件列表显示区域（必须在设置滚动内容之后调用）
    createUIFilesDisplay();
    
    // 创建主布局并添加滚动区域
    QVBoxLayout *mainLayout = new QVBoxLayout(m_uiFlowProperties);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(scrollArea);
    
    m_propertiesTab->setTabText(0, "向导编辑");
}

void StateMachineEditorV2::saveCurrentWizard()
{
    if (m_currentWizardName.isEmpty()) {
        QMessageBox::warning(this, "保存向导", "没有可保存的向导");
        return;
    }
    
    // 检查是否有产品配置目录
    if (!m_product) {
        QMessageBox::warning(this, "保存向导", "没有加载产品配置，无法保存向导文件");
        return;
    }
    
    QString filePath;
    
    // 优先使用state_machine.json中指定的wizardJsonPath路径
    QString wizardJsonPath = m_product->getWizardJsonPath();
    if (!wizardJsonPath.isEmpty() && m_currentWizardFilePath.isEmpty()) {
        qDebug() << "Using wizardJsonPath from state_machine.json:" << wizardJsonPath;
        
        // 将相对路径转换为绝对路径
        if (QDir::isRelativePath(wizardJsonPath)) {
            QString configPackageRootPath = m_product->configPackageRootPath();
            if (!configPackageRootPath.isEmpty()) {
                filePath = QDir(configPackageRootPath).absoluteFilePath(wizardJsonPath);
                qDebug() << "Converted to absolute path:" << filePath;
            } else {
                QMessageBox::warning(this, "保存向导", "产品配置包根路径未设置，无法转换相对路径");
                return;
            }
        } else {
            filePath = wizardJsonPath;
        }
        
        // 确保文件扩展名
        if (!filePath.endsWith(".json")) {
            filePath += ".json";
        }
        
        // 创建目录（如果不存在）
        QFileInfo fileInfo(filePath);
        QDir dir = fileInfo.absoluteDir();
        if (!dir.exists()) {
            dir.mkpath(".");
            qDebug() << "Created directory:" << dir.absolutePath();
        }
        
        m_currentWizardFilePath = filePath;
        qDebug() << "Set current wizard file path to:" << m_currentWizardFilePath;
    }
    else if (m_currentWizardFilePath.isEmpty()) {
        // 新向导，保存到产品配置包根目录下的state_machines/wizards子目录
        QString productConfigRootDir = m_product->configPackageRootPath();
        
        if (productConfigRootDir.isEmpty()) {
            QMessageBox::warning(this, "保存向导", "产品配置包根路径未设置，无法保存向导文件");
            return;
        }
        
        QString stateMachinesDir = productConfigRootDir + "/state_machines";
        QString wizardsDir = stateMachinesDir + "/wizards";
        
        // 创建state_machines和wizards目录（如果不存在）
        QDir stateMachinesDirObj(stateMachinesDir);
        if (!stateMachinesDirObj.exists()) {
            stateMachinesDirObj.mkpath(".");
        }
        
        QDir wizardsDirObj(wizardsDir);
        if (!wizardsDirObj.exists()) {
            wizardsDirObj.mkpath(".");
        }
        
        // 生成默认文件名
        QString defaultFileName = m_currentWizardName + ".json";
        QString defaultFilePath = wizardsDir + "/" + defaultFileName;
        
        // 如果文件已存在，询问是否覆盖
        if (QFile::exists(defaultFilePath)) {
            QMessageBox::StandardButton reply = QMessageBox::question(this, 
                "保存向导", 
                "向导文件 \"" + defaultFileName + "\" 已存在，是否覆盖？",
                QMessageBox::Yes | QMessageBox::No);
            
            if (reply == QMessageBox::No) {
                // 用户选择不覆盖，让用户选择其他路径
                filePath = QFileDialog::getSaveFileName(this, 
                    "保存向导文件", 
                    wizardsDir, 
                    "向导文件 (*.json)");
                if (filePath.isEmpty()) {
                    return; // 用户取消
                }
            } else {
                filePath = defaultFilePath;
            }
        } else {
            filePath = defaultFilePath;
        }
        
        // 确保文件扩展名
        if (!filePath.endsWith(".json")) {
            filePath += ".json";
        }
        
        m_currentWizardFilePath = filePath;
    } else {
        filePath = m_currentWizardFilePath;
    }
    
    // 保存向导数据到文件
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, "保存向导", "无法保存向导文件: " + filePath);
        return;
    }
    
    // 保存向导的实际数据
    QJsonObject wizardData;
    wizardData["name"] = m_currentWizardName;
    wizardData["type"] = "wizard";
    wizardData["created"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    // 保存控件事件定义 - 先检查JSON文件中是否已存在对应事件
    QJsonArray controlEventsArray;
    
    // 如果文件已存在，先读取现有的控件事件
    QJsonArray existingControlEventsArray;
    if (QFile::exists(m_currentWizardFilePath)) {
        QFile existingFile(m_currentWizardFilePath);
        if (existingFile.open(QIODevice::ReadOnly)) {
            QJsonDocument existingDoc = QJsonDocument::fromJson(existingFile.readAll());
            existingFile.close();
            
            if (!existingDoc.isNull() && existingDoc.isObject()) {
                QJsonObject existingJson = existingDoc.object();
                if (existingJson.contains("controlEvents")) {
                    existingControlEventsArray = existingJson["controlEvents"].toArray();
                }
            }
        }
    }
    
    // 创建事件标识符到事件对象的映射，用于快速查找
    QMap<QString, QJsonObject> existingEventsMap;
    for (const QJsonValue &eventValue : existingControlEventsArray) {
        QJsonObject eventObj = eventValue.toObject();
        QString eventKey = QString("%1|%2|%3")
            .arg(eventObj["eventName"].toString())
            .arg(eventObj["controlName"].toString())
            .arg(eventObj["eventType"].toString());
        existingEventsMap[eventKey] = eventObj;
    }
    
    // 处理当前定义的控件事件
    for (const QString &eventData : m_definedControlEvents) {
        QStringList parts = eventData.split("|");
        if (parts.size() >= 3) {
            QString eventKey = QString("%1|%2|%3").arg(parts[0]).arg(parts[1]).arg(parts[2]);
            
            QJsonObject eventObj;
            
            // 如果事件已存在，使用现有的事件对象作为基础，但强制更新路径信息
            if (existingEventsMap.contains(eventKey)) {
                eventObj = existingEventsMap[eventKey];
                // 强制更新事件的基础信息，确保使用最新的数据
                eventObj["eventName"] = parts[0];
                eventObj["controlName"] = parts[1];
                eventObj["eventType"] = parts[2];
            } else {
                // 新事件，创建基础对象
                eventObj["eventName"] = parts[0];
                eventObj["controlName"] = parts[1];
                eventObj["eventType"] = parts[2];
            }
            
            // 更新UI文件信息（如果存在）
            if (parts.size() >= 4) {
                QString uiFilePath = parts[3];
                // 将UI文件路径转换为相对于产品配置目录的相对路径
                if (m_product && !uiFilePath.isEmpty()) {
                    QString configPackageRootPath = m_product->configPackageRootPath();
                    if (!configPackageRootPath.isEmpty()) {
                        QDir configDir(configPackageRootPath);
                        QString relativePath = configDir.relativeFilePath(uiFilePath);
                        eventObj["uiFilePath"] = relativePath;
                    } else {
                        // 如果没有配置包根路径，保持原路径
                        eventObj["uiFilePath"] = uiFilePath;
                    }
                } else {
                    eventObj["uiFilePath"] = uiFilePath;
                }
            }
            
            // 更新目标UI页面信息（如果存在）
            if (parts.size() >= 5) {
                QString targetUIPath = parts[4];
                // 将目标UI文件路径转换为相对于产品配置目录的相对路径
                if (m_product && !targetUIPath.isEmpty()) {
                    QString configPackageRootPath = m_product->configPackageRootPath();
                    if (!configPackageRootPath.isEmpty()) {
                        QDir configDir(configPackageRootPath);
                        QString relativePath = configDir.relativeFilePath(targetUIPath);
                        eventObj["targetUIPath"] = relativePath;
                    } else {
                        // 如果没有配置包根路径，保持原路径
                        eventObj["targetUIPath"] = targetUIPath;
                    }
                } else {
                    eventObj["targetUIPath"] = targetUIPath;
                }
            }
            
            controlEventsArray.append(eventObj);
        }
    }
    wizardData["controlEvents"] = controlEventsArray;
    
    QJsonDocument doc(wizardData);
    file.write(doc.toJson());
    file.close();
    
    m_isWizardModified = false;
    updateWindowTitle();
    
    QMessageBox::information(this, "保存向导", "向导 \"" + m_currentWizardName + "\" 已成功保存到产品配置目录");
}

void StateMachineEditorV2::updateWindowTitle()
{
    QString title = "软件编辑器";
    
    // 优先显示当前向导名称
    if (!m_currentWizardName.isEmpty()) {
        title += " - " + m_currentWizardName;
        if (m_isWizardModified) {
            title += " *"; // 表示未保存
        }
    }
    // 如果没有当前向导，但产品配置中有wizards，显示第一个wizard文件名称
    else if (m_product && m_product->hasStateMachineConfig()) {
        QString wizardJsonPath = m_cachedWizardJsonPath;
        
        // 如果缓存为空，则回退到从产品配置中读取（兼容旧代码）
        if (wizardJsonPath.isEmpty()) {
            wizardJsonPath = m_product->getWizardJsonPath();
        }
        
        if (!wizardJsonPath.isEmpty()) {
            // 将相对路径转换为绝对路径以正确获取文件名
            if (QDir::isRelativePath(wizardJsonPath)) {
                QString configPackageRootPath = m_product->configPackageRootPath();
                if (!configPackageRootPath.isEmpty()) {
                    wizardJsonPath = QDir(configPackageRootPath).absoluteFilePath(wizardJsonPath);
                }
            }
            
            QFileInfo fileInfo(wizardJsonPath);
            QString wizardFileName = fileInfo.fileName();
            if (!wizardFileName.isEmpty()) {
                title += " - " + wizardFileName;
            }
        }
    }
    
    qDebug() << "updateWindowTitle() called, setting title to:" << title;
    qDebug() << "m_currentWizardName:" << m_currentWizardName;
    qDebug() << "m_isWizardModified:" << m_isWizardModified;
    qDebug() << "parentWidget():" << parentWidget();
    
    setWindowTitle(title);
    
    // 更新状态栏提示（如果有状态栏）
    if (parentWidget()) {
        parentWidget()->setWindowTitle(title);
    }
}

void StateMachineEditorV2::updateWizardFilePathLabel(QLabel *label)
{
    if (!label) {
        return;
    }
    
    // 显示当前向导文件路径
    if (!m_currentWizardFilePath.isEmpty()) {
        label->setText(m_currentWizardFilePath);
    } else {
        label->setText("未指定向导文件");
    }
}

void StateMachineEditorV2::loadWizardsFromProductConfig()
{
    if (!m_product) {
        qDebug() << "loadWizardsFromProductConfig: No product configuration, cannot load wizard files";
        return;
    }
    
    // 优先使用缓存的向导文件路径（避免重复读取state_machine.json文件）
    QString wizardJsonPath = m_cachedWizardJsonPath;
    
    // 如果缓存为空，则回退到从产品配置中读取（兼容旧代码）
    if (wizardJsonPath.isEmpty()) {
        wizardJsonPath = m_product->getWizardJsonPath();
        qDebug() << "Using fallback method to get wizard path from product config";
    }
    
    if (wizardJsonPath.isEmpty()) {
        qDebug() << "loadWizardsFromProductConfig: No wizard file path specified in state_machine.json";
        return;
    }
    
    // 将相对路径转换为绝对路径
    if (QDir::isRelativePath(wizardJsonPath)) {
        QString configPackageRootPath = m_product->configPackageRootPath();
        if (!configPackageRootPath.isEmpty()) {
            wizardJsonPath = QDir(configPackageRootPath).absoluteFilePath(wizardJsonPath);
            qDebug() << "Converting to absolute path:" << wizardJsonPath;
        }
    }
    
    qDebug() << "Loading specified wizard file:" << wizardJsonPath;
    
    // 检查向导文件是否存在
    QFileInfo fileInfo(wizardJsonPath);
    if (!fileInfo.exists()) {
        qDebug() << "Specified wizard file does not exist:" << wizardJsonPath;
        return;
    }
    
    // 加载指定的向导文件
    QFile file(wizardJsonPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "无法打开向导文件:" << wizardJsonPath;
        return;
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    
    if (doc.isNull()) {
        qWarning() << "向导文件格式错误:" << wizardJsonPath;
        return;
    }
    
    QJsonObject json = doc.object();
    
    // 检查是否为向导文件
    if (json.contains("type") && json["type"].toString() == "wizard") {
        QString wizardName = json["name"].toString();
        
        if (!wizardName.isEmpty()) {
            qDebug() << "Loading wizard:" << wizardName << "from" << wizardJsonPath;
            
            // 创建向导对象
            if (m_wizardManager) {
                QString description = json.contains("description") ? json["description"].toString() : "";
                Wizard* wizard = m_wizardManager->createWizard(wizardName, description);
                
                if (wizard) {
                    // 设置向导文件路径
                    m_currentWizardName = wizardName;
                    m_currentWizardFilePath = wizardJsonPath;
                    m_isWizardModified = false;
                    
                    // 加载控件事件定义
                    m_definedControlEvents.clear();
                    if (json.contains("controlEvents")) {
                        QJsonArray controlEventsArray = json["controlEvents"].toArray();
                        for (const QJsonValue &eventValue : controlEventsArray) {
                            QJsonObject eventObj = eventValue.toObject();
                            QString eventName = eventObj["eventName"].toString();
                            QString controlName = eventObj["controlName"].toString();
                            QString eventType = eventObj["eventType"].toString();
                            QString uiFilePath = eventObj["uiFilePath"].toString(); // 读取UI文件路径
                            
                            if (!eventName.isEmpty() && !controlName.isEmpty() && !eventType.isEmpty()) {
                                QString eventData;
                                QString targetUIPath = eventObj["targetUIPath"].toString(); // 读取目标UI文件路径
                                
                                if (!uiFilePath.isEmpty()) {
                                    // 将相对路径转换为绝对路径
                                    QString absoluteUiFilePath = uiFilePath;
                                    if (m_product && !uiFilePath.isEmpty()) {
                                        QString configPackageRootPath = m_product->configPackageRootPath();
                                        if (!configPackageRootPath.isEmpty() && QDir::isRelativePath(uiFilePath)) {
                                            QDir configDir(configPackageRootPath);
                                            absoluteUiFilePath = configDir.absoluteFilePath(uiFilePath);
                                        }
                                    }
                                    
                                    if (!targetUIPath.isEmpty()) {
                                        // 将目标UI文件路径转换为绝对路径
                                        QString absoluteTargetUIPath = targetUIPath;
                                        if (m_product && !targetUIPath.isEmpty()) {
                                            QString configPackageRootPath = m_product->configPackageRootPath();
                                            if (!configPackageRootPath.isEmpty() && QDir::isRelativePath(targetUIPath)) {
                                                QDir configDir(configPackageRootPath);
                                                absoluteTargetUIPath = configDir.absoluteFilePath(targetUIPath);
                                            }
                                        }
                                        // 包含UI文件路径和目标UI路径的新格式
                                        eventData = QString("%1|%2|%3|%4|%5").arg(eventName).arg(controlName).arg(eventType).arg(absoluteUiFilePath).arg(absoluteTargetUIPath);
                                    } else {
                                        // 包含UI文件路径但没有目标UI路径的格式
                                        eventData = QString("%1|%2|%3|%4").arg(eventName).arg(controlName).arg(eventType).arg(absoluteUiFilePath);
                                    }
                                } else {
                                    // 兼容旧格式（没有UI文件路径）
                                    eventData = QString("%1|%2|%3").arg(eventName).arg(controlName).arg(eventType);
                                }
                                m_definedControlEvents.append(eventData);
                                qDebug() << "Loaded control event:" << eventData;
                            }
                        }
                        qDebug() << "Loaded" << m_definedControlEvents.size() << "control events";
                    }
                    
                    // 将新创建的向导设置为当前向导
                    m_wizardManager->setCurrentWizard(wizard);
                    
                    qDebug() << "Wizard loaded successfully:" << wizardName;
                }
            }
        }
    } else {
        qWarning() << "文件不是有效的向导文件:" << wizardJsonPath;
    }
    
    // 更新窗口标题
    updateWindowTitle();
    
    // 更新向导文件路径标签
    if (m_uiFlowProperties) {
        // 查找向导文件路径标签（在整个对象树中查找）
        QLabel *wizardFilePathLabel = m_uiFlowProperties->findChild<QLabel*>(QString("wizardFilePathLabel"));
        if (wizardFilePathLabel) {
            updateWizardFilePathLabel(wizardFilePathLabel);
        } else {
            qDebug() << "wizardFilePathLabel not found in m_uiFlowProperties";
        }
        
        // 更新已定义事件列表显示（初始化时保持列表为空）
        updateDefinedEventsList("");
    }
    
    qDebug() << "Wizard file loading completed";
}

void StateMachineEditorV2::createUIFilesDisplay()
{
    // 创建UI文件列表显示区域
    QLabel *uiFilesLabel = new QLabel("产品UI文件列表:", m_uiFlowProperties);
    uiFilesLabel->setStyleSheet("font-weight: bold; font-size: 12pt; margin-top: 20px;");
    
    // 创建水平布局来容纳UI文件列表和详情
    QHBoxLayout *uiFilesLayout = new QHBoxLayout();
    uiFilesLayout->setSpacing(10);
    uiFilesLayout->setContentsMargins(0, 0, 0, 0);
    
    // 创建UI文件列表控件
    m_uiFilesListWidget = new QListWidget(m_uiFlowProperties);
    m_uiFilesListWidget->setMinimumWidth(200);
    m_uiFilesListWidget->setMaximumWidth(300);
    m_uiFilesListWidget->setStyleSheet("border: 1px solid #ccc; background-color: #f9f9f9;");
    
    // 创建UI文件详情显示控件
    m_uiFileDetailsTextEdit = new QTextEdit(m_uiFlowProperties);
    m_uiFileDetailsTextEdit->setMinimumHeight(200);
    m_uiFileDetailsTextEdit->setReadOnly(true);
    m_uiFileDetailsTextEdit->setStyleSheet("border: 1px solid #ccc; background-color: #f9f9f9; font-family: 'Courier New'; font-size: 10pt;");
    
    // 添加到布局
    uiFilesLayout->addWidget(m_uiFilesListWidget);
    uiFilesLayout->addWidget(m_uiFileDetailsTextEdit);
    
    // 创建控件事件定义区域
    QLabel *controlEventsLabel = new QLabel("控件事件定义:", m_uiFlowProperties);
    controlEventsLabel->setStyleSheet("font-weight: bold; font-size: 12pt; margin-top: 20px;");
    
    QGroupBox *controlEventsGroup = new QGroupBox("控件事件关联", m_uiFlowProperties);
    controlEventsGroup->setStyleSheet("QGroupBox { font-weight: normal; font-size: 10pt; margin-top: 10px; }");
    
    QVBoxLayout *controlEventsLayout = new QVBoxLayout(controlEventsGroup);
    controlEventsLayout->setSpacing(15);
    controlEventsLayout->setContentsMargins(15, 20, 15, 15);
    
    // 第一行：控件选择和事件类型选择（水平布局）
    QHBoxLayout *firstRowLayout = new QHBoxLayout();
    firstRowLayout->setSpacing(20);
    
    // 控件选择区域
    QVBoxLayout *controlLayout = new QVBoxLayout();
    controlLayout->setSpacing(5);
    QLabel *controlLabel = new QLabel("选择控件:", controlEventsGroup);
    QComboBox *controlComboBox = new QComboBox(controlEventsGroup);
    controlComboBox->setMinimumHeight(35);
    controlComboBox->setStyleSheet("QComboBox { font-size: 10pt; padding: 8px; }");
    controlComboBox->addItem("请先选择UI文件查看可用控件");
    controlLayout->addWidget(controlLabel);
    controlLayout->addWidget(controlComboBox);
    
    // 事件类型选择区域
    QVBoxLayout *eventTypeLayout = new QVBoxLayout();
    eventTypeLayout->setSpacing(5);
    QLabel *eventLabel = new QLabel("事件类型:", controlEventsGroup);
    QComboBox *eventComboBox = new QComboBox(controlEventsGroup);
    eventComboBox->setMinimumHeight(35);
    eventComboBox->setStyleSheet("QComboBox { font-size: 10pt; padding: 8px; }");
    eventComboBox->addItem("请先选择控件");
    eventTypeLayout->addWidget(eventLabel);
    eventTypeLayout->addWidget(eventComboBox);
    
    firstRowLayout->addLayout(controlLayout);
    firstRowLayout->addLayout(eventTypeLayout);
    firstRowLayout->addStretch(); // 添加拉伸因子
    
    // 第二行：事件名称输入和添加按钮（水平布局）
    QHBoxLayout *secondRowLayout = new QHBoxLayout();
    secondRowLayout->setSpacing(15);
    
    // 事件名称输入区域
    QVBoxLayout *eventNameLayout = new QVBoxLayout();
    eventNameLayout->setSpacing(5);
    QLabel *eventNameLabel = new QLabel("事件名称:", controlEventsGroup);
    QLineEdit *eventNameEdit = new QLineEdit(controlEventsGroup);
    eventNameEdit->setMinimumHeight(35);
    eventNameEdit->setStyleSheet("QLineEdit { font-size: 10pt; padding: 8px; }");
    eventNameEdit->setPlaceholderText("输入事件名称（如：button1_clicked）");
    eventNameLayout->addWidget(eventNameLabel);
    eventNameLayout->addWidget(eventNameEdit);
    
    // 目标UI页面选择区域
    QVBoxLayout *targetUILayout = new QVBoxLayout();
    targetUILayout->setSpacing(5);
    QLabel *targetUILabel = new QLabel("目标UI页面:", controlEventsGroup);
    QComboBox *targetUIComboBox = new QComboBox(controlEventsGroup);
    targetUIComboBox->setMinimumHeight(35);
    targetUIComboBox->setStyleSheet("QComboBox { font-size: 10pt; padding: 8px; }");
    targetUIComboBox->addItem("无跳转（保持当前页面）");
    targetUIComboBox->addItem("请先选择UI文件查看可用页面");
    targetUILayout->addWidget(targetUILabel);
    targetUILayout->addWidget(targetUIComboBox);
    
    // 添加事件按钮
    QPushButton *addEventButton = new QPushButton("添加事件定义", controlEventsGroup);
    addEventButton->setMinimumHeight(35);
    addEventButton->setMinimumWidth(120);
    addEventButton->setStyleSheet("QPushButton { font-size: 10pt; padding: 8px 16px; background-color: #4CAF50; color: white; }");
    
    secondRowLayout->addLayout(eventNameLayout);
    secondRowLayout->addLayout(targetUILayout);
    secondRowLayout->addWidget(addEventButton);
    secondRowLayout->addStretch(); // 添加拉伸因子
    
    // 已定义事件列表
    QLabel *definedEventsLabel = new QLabel("已定义的事件:", controlEventsGroup);
    definedEventsLabel->setStyleSheet("font-weight: bold; margin-top: 15px;");
    m_definedEventsListWidget = new QListWidget(controlEventsGroup);
    m_definedEventsListWidget->setMinimumHeight(150);
    m_definedEventsListWidget->setStyleSheet("border: 1px solid #ccc; background-color: #f9f9f9;");
    
    // 添加到控件事件布局
    controlEventsLayout->addLayout(firstRowLayout);
    controlEventsLayout->addLayout(secondRowLayout);
    controlEventsLayout->addWidget(definedEventsLabel);
    controlEventsLayout->addWidget(m_definedEventsListWidget);
    
    // 添加到向导布局（滚动内容中）
    QScrollArea *scrollArea = m_uiFlowProperties->findChild<QScrollArea*>();
    if (scrollArea && scrollArea->widget()) {
        qDebug()<<"scrollArea widget layout:"<<scrollArea->widget()->layout();
        QVBoxLayout *wizardLayout = qobject_cast<QVBoxLayout*>(scrollArea->widget()->layout());
        if (wizardLayout) {
            qDebug()<<"wizardLayout count:"<<wizardLayout->count();
            wizardLayout->insertWidget(wizardLayout->count() - 1, uiFilesLabel); // 在stretch之前添加
            wizardLayout->insertLayout(wizardLayout->count() - 1, uiFilesLayout);
            wizardLayout->insertWidget(wizardLayout->count() - 1, controlEventsLabel);
            wizardLayout->insertWidget(wizardLayout->count() - 1, controlEventsGroup);
        } else {
            qDebug()<<"scrollArea widget layout is not QVBoxLayout";
        }
    } else {
        qDebug()<<"scrollArea widget layout is null";
    }
    
    // 连接信号槽：当选择UI文件时显示详情并更新UI界面选择
    connect(m_uiFilesListWidget, &QListWidget::itemSelectionChanged, this, 
            [=]() {
                QList<QListWidgetItem*> selectedItems = m_uiFilesListWidget->selectedItems();
                if (!selectedItems.isEmpty()) {
                    QString filePath = selectedItems.first()->data(Qt::UserRole).toString();
                    displayUIFileDetails(filePath);
                    
                    // 更新控件选择下拉框
                    updateControlComboBox(controlComboBox);
                    
                    // 更新目标UI页面选择器
                    updateTargetUIComboBox(targetUIComboBox);
                    
                    // 更新UI界面选择状态
                    onUiInterfaceChanged();
                    
                    // 更新已定义事件列表，仅显示当前选中UI文件的事件
                    updateDefinedEventsList(filePath);
                    
                    // 在右侧预览窗口中显示UI文件的运行时效果
                    if (m_uiRuntimePreview) {
                        m_uiRuntimePreview->loadUIFile(filePath);
                    }
                } else {
                    // 如果没有选中任何UI文件，清空事件列表（什么都不显示）
                    updateDefinedEventsList("");
                    
                    // 清空右侧预览窗口
                    if (m_uiRuntimePreview) {
                        m_uiRuntimePreview->clearPreview();
                    }
                }
            });
    
    // 连接信号槽：当选择控件时更新事件类型
    connect(controlComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            [=](int index) {
                if (index > 0) {
                    QString controlName = controlComboBox->currentText();
                    updateEventComboBox(eventComboBox, controlName);
                } else {
                    eventComboBox->clear();
                    eventComboBox->addItem("请先选择控件");
                }
            });
    
    // 连接信号槽：添加事件定义
    connect(addEventButton, &QPushButton::clicked, this, 
            [=]() {
                addControlEventDefinition(controlComboBox, eventComboBox, eventNameEdit, targetUIComboBox);
            });
    
    // 连接信号槽：当点击已定义事件列表中的事件时，自动填充控件、事件名称、事件类型和跳转页面信息
    connect(m_definedEventsListWidget, &QListWidget::itemClicked, this,
            [=](QListWidgetItem *item) {
                if (!item) return;
                
                // 获取点击的事件在列表中的索引
                int index = m_definedEventsListWidget->row(item);
                if (index < 0 || index >= m_definedControlEvents.size()) return;
                
                // 获取事件数据
                QString eventData = m_definedControlEvents[index];
                QStringList parts = eventData.split('|');
                if (parts.size() < 5) return;
                
                QString eventName = parts[0];
                QString controlName = parts[1];
                QString eventType = parts[2];
                QString targetUIPath = parts[4];
                
                // 自动填充控件选择
                int controlIndex = controlComboBox->findText(controlName);
                if (controlIndex >= 0) {
                    controlComboBox->setCurrentIndex(controlIndex);
                    // 更新事件类型下拉框
                    updateEventComboBox(eventComboBox, controlName);
                }
                
                // 自动填充事件类型
                int eventIndex = eventComboBox->findText(eventType);
                if (eventIndex >= 0) {
                    eventComboBox->setCurrentIndex(eventIndex);
                }
                
                // 自动填充事件名称
                eventNameEdit->setText(eventName);
                
                // 自动填充目标UI页面
                if (targetUIPath.isEmpty()) {
                    targetUIComboBox->setCurrentIndex(0); // 无跳转
                } else {
                    // 查找目标UI页面的索引
                    int targetIndex = -1;
                    for (int i = 0; i < targetUIComboBox->count(); i++) {
                        if (targetUIComboBox->itemData(i).toString() == targetUIPath) {
                            targetIndex = i;
                            break;
                        }
                    }
                    if (targetIndex >= 0) {
                        targetUIComboBox->setCurrentIndex(targetIndex);
                    } else {
                        targetUIComboBox->setCurrentIndex(0); // 无跳转
                    }
                }
                
                qDebug() << "已定义事件点击：" << eventName << "-" << controlName << "(" << eventType << ")" << "目标UI:" << targetUIPath;
            });
    
    // 检查是否有待更新的UI文件数据
    qDebug() << "createUIFilesDisplay() completed, m_productUiFiles size:" << m_productUiFiles.size();
    qDebug() << "UI files list widget created, checking pending update flag:" << m_uiFilesDataPendingUpdate;
    
    // 修复时序问题：无论数据是否先于控件设置，只要控件已创建且有数据，就立即更新列表
    if (m_uiFilesListWidget) {
        qDebug() << "UI files list widget is valid, checking data conditions";
        if (!m_productUiFiles.isEmpty()) {
            qDebug() << "UI files list widget created and data available, updating list now";
            qDebug() << "Data contains" << m_productUiFiles.size() << "UI files";
            updateUIFilesList();
            m_uiFilesDataPendingUpdate = false;
            qDebug() << "UI files list updated after widget creation, current count:" << m_uiFilesListWidget->count();
        } else if (m_uiFilesDataPendingUpdate) {
            qDebug() << "Pending UI files data found, updating list widget now";
            updateUIFilesList();
            m_uiFilesDataPendingUpdate = false;
            qDebug() << "UI files list updated after widget creation, current count:" << m_uiFilesListWidget->count();
        } else {
            qDebug() << "UI files list widget created but no data available yet, current count:" << m_uiFilesListWidget->count();
            qDebug() << "m_productUiFiles is empty and m_uiFilesDataPendingUpdate is false";
        }
    } else {
        qDebug() << "UI files list widget not ready, current count: 0";
    }
}

void StateMachineEditorV2::updateUIFilesList()
{
    qDebug() << "updateUIFilesList() called, m_uiFilesListWidget:" << m_uiFilesListWidget;
    
    if (!m_uiFilesListWidget) {
        qWarning() << "m_uiFilesListWidget is null, cannot update UI file list";
        return;
    }
    
    qDebug() << "m_productUiFiles size:" << m_productUiFiles.size();
    
    // 优化清空时机：只有当列表中有内容且需要更新时才清空
    if (m_uiFilesListWidget->count() > 0) {
        m_uiFilesListWidget->clear();
        qDebug() << "Cleared UI files list widget, previous count was:" << m_uiFilesListWidget->count();
    } else {
        qDebug() << "UI files list widget is already empty, no need to clear";
    }

    // 设置列表控件属性以支持自定义项
    m_uiFilesListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    m_uiFilesListWidget->setResizeMode(QListView::Adjust);
    m_uiFilesListWidget->setViewMode(QListView::ListMode);
    m_uiFilesListWidget->setSpacing(2);
    
    // 添加产品配置中的UI文件
    int addedCount = 0;
    for (const auto &uiFile : m_productUiFiles) {
        qDebug() << "Adding UI file to list:" << uiFile.name << "path:" << uiFile.filePath;
        
        // 创建自定义预览项
        UIFilePreviewItem *previewItem = new UIFilePreviewItem(uiFile);
        
        // 创建列表项并设置自定义widget
        QListWidgetItem *item = new QListWidgetItem();
        item->setSizeHint(previewItem->sizeHint());
        item->setData(Qt::UserRole, uiFile.filePath);
        item->setToolTip(uiFile.filePath);
        
        m_uiFilesListWidget->addItem(item);
        m_uiFilesListWidget->setItemWidget(item, previewItem);
        
        addedCount++;
        qDebug() << "Preview item added, current count:" << m_uiFilesListWidget->count();
    }
    
    // 添加调试输出，显示最终列表计数
    qDebug() << "Final UI files list count:" << m_uiFilesListWidget->count();
    qDebug() << "Total UI files added:" << addedCount;
    
    qDebug() << "UI file list updated, total" << m_productUiFiles.size() << "files, added:" << addedCount;
    qDebug() << "List widget item count:" << m_uiFilesListWidget->count();
}

void StateMachineEditorV2::displayUIFileDetails(const QString &filePath)
{
    if (!m_uiFileDetailsTextEdit) {
        return;
    }
    
    if (filePath.isEmpty()) {
        m_uiFileDetailsTextEdit->setText("请选择一个UI文件查看详情");
        return;
    }
    
    // 解析UI文件并显示控件信息
    parseUIFile(filePath);
}

void StateMachineEditorV2::parseUIFile(const QString &filePath)
{
    if (filePath.isEmpty()) {
        m_uiFileDetailsTextEdit->setText("文件路径为空");
        return;
    }
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        m_uiFileDetailsTextEdit->setText("无法打开文件: " + filePath);
        return;
    }
    
    QString content = file.readAll();
    file.close();
    
    // 解析UI文件内容，提取控件信息
    QString details = "文件: " + QFileInfo(filePath).fileName() + "\n";
    details += "路径: " + filePath + "\n\n";
    details += "控件列表 (支持事件定义):\n";
    details += QString("=").repeated(50) + "\n";
    
    // 使用XML解析来提取详细的控件信息
    QXmlStreamReader xml(content);
    int controlCount = 0;
    QMap<QString, QStringList> controlEvents; // 存储控件支持的事件类型
    
    // 定义常见Qt控件及其支持的事件
    QMap<QString, QStringList> standardControlEvents = {
        {"QPushButton", {"clicked()", "pressed()", "released()"}},
        {"QCheckBox", {"stateChanged(int)", "toggled(bool)"}},
        {"QRadioButton", {"toggled(bool)"}},
        {"QLineEdit", {"textChanged(const QString&)", "editingFinished()", "returnPressed()"}},
        {"QComboBox", {"currentIndexChanged(int)", "currentTextChanged(const QString&)"}},
        {"QSlider", {"valueChanged(int)", "sliderMoved(int)"}},
        {"QSpinBox", {"valueChanged(int)", "valueChanged(const QString&)"}},
        {"QDoubleSpinBox", {"valueChanged(double)"}},
        {"QTabWidget", {"currentChanged(int)"}},
        {"QListWidget", {"currentItemChanged(QListWidgetItem*,QListWidgetItem*)", "itemClicked(QListWidgetItem*)"}},
        {"QTreeWidget", {"itemClicked(QTreeWidgetItem*,int)", "itemDoubleClicked(QTreeWidgetItem*,int)"}},
        {"QTableWidget", {"cellClicked(int,int)", "cellDoubleClicked(int,int)"}}
    };
    
    while (!xml.atEnd() && !xml.hasError()) {
        QXmlStreamReader::TokenType token = xml.readNext();
        
        if (token == QXmlStreamReader::StartElement) {
            QString elementName = xml.name().toString();
            
            // 检查是否是widget元素
            if (elementName == "widget") {
                // 获取控件类名
                QString widgetClass = xml.attributes().value("class").toString();
                
                // 检查是否是Qt控件类（排除容器控件）
                if (widgetClass.startsWith("Q") && widgetClass.length() > 1 && 
                    widgetClass != "QWidget" && widgetClass != "QMainWindow" && 
                    widgetClass != "QDialog" && widgetClass != "QFrame" && 
                    widgetClass != "QGroupBox" && widgetClass != "QTabWidget" &&
                    widgetClass != "QScrollArea" && widgetClass != "QSplitter" &&
                    widgetClass != "QStackedWidget" && widgetClass != "QDockWidget") {
                    controlCount++;
                    
                    // 获取控件名称
                    QString controlName = xml.attributes().value("name").toString();
                    if (controlName.isEmpty()) {
                        controlName = "未命名控件" + QString::number(controlCount);
                    }
                    
                    // 读取widget元素内的属性
                    QString controlText;
                    QString geometry;
                    
                    // 读取widget内的所有property元素
                    while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "widget") && !xml.atEnd()) {
                        xml.readNext();
                        
                        if (xml.tokenType() == QXmlStreamReader::StartElement && xml.name() == "property") {
                            QString propertyName = xml.attributes().value("name").toString();
                            
                            if (propertyName == "text") {
                                xml.readNext(); // 读取<string>元素
                                if (xml.tokenType() == QXmlStreamReader::StartElement && xml.name() == "string") {
                                    xml.readNext(); // 读取文本内容
                                    if (xml.tokenType() == QXmlStreamReader::Characters) {
                                        controlText = xml.text().toString();
                                    }
                                }
                            } else if (propertyName == "geometry") {
                                xml.readNext(); // 读取<rect>元素
                                if (xml.tokenType() == QXmlStreamReader::StartElement && xml.name() == "rect") {
                                    // 读取几何信息
                                    QString x, y, width, height;
                                    while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "rect") && !xml.atEnd()) {
                                        xml.readNext();
                                        if (xml.tokenType() == QXmlStreamReader::StartElement) {
                                            if (xml.name() == "x") {
                                                xml.readNext();
                                                if (xml.tokenType() == QXmlStreamReader::Characters) x = xml.text().toString();
                                            } else if (xml.name() == "y") {
                                                xml.readNext();
                                                if (xml.tokenType() == QXmlStreamReader::Characters) y = xml.text().toString();
                                            } else if (xml.name() == "width") {
                                                xml.readNext();
                                                if (xml.tokenType() == QXmlStreamReader::Characters) width = xml.text().toString();
                                            } else if (xml.name() == "height") {
                                                xml.readNext();
                                                if (xml.tokenType() == QXmlStreamReader::Characters) height = xml.text().toString();
                                            }
                                        }
                                    }
                                    if (!x.isEmpty() && !y.isEmpty() && !width.isEmpty() && !height.isEmpty()) {
                                        geometry = QString("x:%1, y:%2, w:%3, h:%4").arg(x).arg(y).arg(width).arg(height);
                                    }
                                }
                            }
                        }
                    }
                    
                    details += QString("%1. %2 (%3)\n").arg(controlCount).arg(controlName).arg(widgetClass);
                    
                    // 显示控件基本信息
                    if (!controlText.isEmpty()) {
                        details += "   文本: " + controlText + "\n";
                    }
                    
                    // 显示几何信息
                    if (!geometry.isEmpty()) {
                        details += "   几何: " + geometry + "\n";
                    }
                    
                    // 显示控件支持的事件类型
                    if (standardControlEvents.contains(widgetClass)) {
                        details += "   支持的事件类型:\n";
                        QStringList events = standardControlEvents[widgetClass];
                        for (const QString &event : events) {
                            details += "     - " + event + "\n";
                        }
                        controlEvents[controlName] = events;
                    } else {
                        details += "   支持的事件类型: 通用事件\n";
                        controlEvents[controlName] = {"clicked()", "pressed()", "released()"};
                    }
                    
                    details += "\n";
                }
            }
        }
    }
    
    if (xml.hasError()) {
        details += "解析错误: " + xml.errorString() + "\n";
    }
    
    if (controlCount == 0) {
        details += "未找到控件信息\n";
    } else {
        details += QString("=").repeated(50) + "\n";
        details += QString("总计: %1 个控件\n").arg(controlCount);
        details += "\n提示: 您可以为上述控件定义向导事件，关联到状态机转换\n";
    }
    
    m_uiFileDetailsTextEdit->setText(details);
    
    // 保存控件事件信息，供后续事件定义使用
    m_currentUIFileControls = controlEvents;
    m_currentUIFilePath = filePath;
}

void StateMachineEditorV2::updateControlComboBox(QComboBox *controlComboBox)
{
    if (!controlComboBox) {
        return;
    }
    
    controlComboBox->clear();
    
    if (m_currentUIFileControls.isEmpty()) {
        controlComboBox->addItem("请先选择UI文件查看可用控件");
        return;
    }
    
    controlComboBox->addItem("请选择控件");
    
    // 添加当前UI文件中的所有控件
    for (const QString &controlName : m_currentUIFileControls.keys()) {
        controlComboBox->addItem(controlName);
    }
    
    qDebug() << "controls choice update，total: " << m_currentUIFileControls.size();
}

void StateMachineEditorV2::updateEventComboBox(QComboBox *eventComboBox, const QString &controlName)
{
    if (!eventComboBox || controlName.isEmpty()) {
        return;
    }
    
    eventComboBox->clear();
    
    if (!m_currentUIFileControls.contains(controlName)) {
        eventComboBox->addItem("该控件无可用事件");
        return;
    }
    
    eventComboBox->addItem("请选择事件类型");
    
    // 添加该控件支持的事件类型
    QStringList events = m_currentUIFileControls[controlName];
    for (const QString &event : events) {
        eventComboBox->addItem(event);
    }
    
    qDebug() << "event choice update，control: " << controlName << "total: " << events.size();
}

void StateMachineEditorV2::updateTargetUIComboBox(QComboBox *targetUIComboBox)
{
    if (!targetUIComboBox) {
        return;
    }
    
    targetUIComboBox->clear();
    
    // 添加默认选项
    targetUIComboBox->addItem("无跳转（保持当前页面）");
    
    if (m_productUiFiles.isEmpty()) {
        targetUIComboBox->addItem("请先选择UI文件查看可用页面");
        return;
    }
    
    // 添加所有可用的UI页面
    for (const auto &uiFile : m_productUiFiles) {
        targetUIComboBox->addItem(uiFile.name, uiFile.filePath);
    }
    
    qDebug() << "target UI choice update，total: " << m_productUiFiles.size();
}

void StateMachineEditorV2::addControlEventDefinition(QComboBox *controlComboBox, 
                                                     QComboBox *eventComboBox, 
                                                     QLineEdit *eventNameEdit,
                                                     QComboBox *targetUIComboBox)
{
    if (!controlComboBox || !eventComboBox || !eventNameEdit || !targetUIComboBox || !m_definedEventsListWidget) {
        return;
    }
    
    // 检查输入有效性
    QString controlName = controlComboBox->currentText();
    QString eventType = eventComboBox->currentText();
    QString eventName = eventNameEdit->text().trimmed();
    QString targetUIName = targetUIComboBox->currentText();
    QString targetUIPath = targetUIComboBox->currentData().toString();
    
    if (controlComboBox->currentIndex() <= 0) {
        QMessageBox::warning(this, "添加事件定义", "请先选择控件");
        return;
    }
    
    if (eventComboBox->currentIndex() <= 0) {
        QMessageBox::warning(this, "添加事件定义", "请先选择事件类型");
        return;
    }
    
    if (eventName.isEmpty()) {
        QMessageBox::warning(this, "添加事件定义", "请输入事件名称");
        return;
    }
    
    // 检查事件名称是否已存在
    for (int i = 0; i < m_definedEventsListWidget->count(); i++) {
        QListWidgetItem *item = m_definedEventsListWidget->item(i);
        if (item->text().contains(eventName)) {
            QMessageBox::warning(this, "添加事件定义", "事件名称已存在，请使用其他名称");
            return;
        }
    }
    
    // 构建事件定义显示文本（包含目标UI页面信息）
    QString eventDefinition;
    if (targetUIName == "无跳转（保持当前页面）" || targetUIPath.isEmpty()) {
        eventDefinition = QString("%1 - %2 (%3)").arg(eventName).arg(controlName).arg(eventType);
    } else {
        eventDefinition = QString("%1 - %2 (%3) → %4").arg(eventName).arg(controlName).arg(eventType).arg(targetUIName);
    }
    
    QListWidgetItem *item = new QListWidgetItem(eventDefinition);
    m_definedEventsListWidget->addItem(item);
    
    // 保存事件定义到数据结构中（包含UI文件信息和目标UI页面信息）
    QString eventData = QString("%1|%2|%3|%4|%5").arg(eventName).arg(controlName).arg(eventType).arg(m_currentUIFilePath).arg(targetUIPath);
    m_definedControlEvents.append(eventData);
    
    // 清空输入框
    eventNameEdit->clear();
    
    // 显示成功消息
    QString successMessage;
    if (targetUIName == "无跳转（保持当前页面）" || targetUIPath.isEmpty()) {
        successMessage = QString("事件定义已添加:\n控件: %1\n事件类型: %2\n事件名称: %3\n目标UI页面: 无跳转")
                        .arg(controlName).arg(eventType).arg(eventName);
    } else {
        successMessage = QString("事件定义已添加:\n控件: %1\n事件类型: %2\n事件名称: %3\n目标UI页面: %4")
                        .arg(controlName).arg(eventType).arg(eventName).arg(targetUIName);
    }
    
    QMessageBox::information(this, "添加事件定义", successMessage);
    
    qDebug() << "control event add，definition: " << eventDefinition;
    
    // 标记向导已修改
    m_isWizardModified = true;
    updateWindowTitle();
}

void StateMachineEditorV2::updateDefinedEventsList(const QString &uiFilePath)
{
    if (!m_definedEventsListWidget) {
        return;
    }
    
    // 清空当前列表
    m_definedEventsListWidget->clear();
    
    // 如果没有指定UI文件路径，什么都不显示（保持列表为空）
    if (uiFilePath.isEmpty()) {
        qDebug() << "update defined events list，no ui file selected，clear list";
        return;
    }
    
    // 根据UI文件路径过滤事件
    for (const QString &eventData : m_definedControlEvents) {
        QStringList parts = eventData.split('|');
        if (parts.size() >= 5) {
            QString eventName = parts[0];
            QString controlName = parts[1];
            QString eventType = parts[2];
            QString eventUIFilePath = parts[3];
            QString targetUIPath = parts[4];
            
            // 如果事件对应的UI文件路径与当前选中的UI文件路径匹配，则显示该事件
            if (eventUIFilePath == uiFilePath) {
                // 构建事件定义显示文本（包含目标UI页面信息）
                QString eventDefinition;
                if (targetUIPath.isEmpty()) {
                    eventDefinition = QString("%1 - %2 (%3)").arg(eventName).arg(controlName).arg(eventType);
                } else {
                    // 查找目标UI页面的名称
                    QString targetUIName = "未知页面";
                    for (const auto &uiFile : m_productUiFiles) {
                        if (uiFile.filePath == targetUIPath) {
                            targetUIName = uiFile.name;
                            break;
                        }
                    }
                    eventDefinition = QString("%1 - %2 (%3) → %4").arg(eventName).arg(controlName).arg(eventType).arg(targetUIName);
                }
                
                QListWidgetItem *item = new QListWidgetItem(eventDefinition);
                m_definedEventsListWidget->addItem(item);
            }
        }
    }
    
    qDebug() << "update defined events list，ui file: " << uiFilePath 
             << "total events: " << m_definedEventsListWidget->count();
}

// UIFilePreviewItem 实现
UIFilePreviewItem::UIFilePreviewItem(const ProductUIFile &uiFile, QWidget *parent)
    : QWidget(parent)
    , m_uiFile(uiFile)
    , m_controlCount(0)
    , m_uiType("unknown")
    , m_mainWidgetClass("QWidget")
{
    // 创建布局
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(6, 4, 6, 4);  // 减少边距
    mainLayout->setSpacing(6);                    // 减少间距
    
    // 创建图标标签
    m_iconLabel = new QLabel(this);
    m_iconLabel->setFixedSize(28, 28);           // 减小图标尺寸
    m_iconLabel->setStyleSheet("border: 1px solid #ccc; background-color: #f0f0f0; border-radius: 3px;");
    
    // 创建信息布局
    QVBoxLayout *infoLayout = new QVBoxLayout();
    infoLayout->setSpacing(2);                   // 减少间距
    infoLayout->setContentsMargins(0, 0, 0, 0);  // 移除内部边距
    
    // 文件名标签 - 优化文本显示
    m_nameLabel = new QLabel(uiFile.name, this);
    m_nameLabel->setStyleSheet("font-weight: bold; font-size: 11px; color: #333; margin: 0; padding: 0;");
    m_nameLabel->setWordWrap(false);             // 禁止换行
    m_nameLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_nameLabel->setMinimumHeight(14);           // 设置最小高度
    
    // 类型标签
    m_typeLabel = new QLabel("类型: 加载中...", this);
    m_typeLabel->setStyleSheet("font-size: 9px; color: #666; margin: 0; padding: 0;");
    m_typeLabel->setWordWrap(false);
    m_typeLabel->setMinimumHeight(12);
    
    // 控件数量标签
    m_controlsLabel = new QLabel("控件: 解析中...", this);
    m_controlsLabel->setStyleSheet("font-size: 9px; color: #666; margin: 0; padding: 0;");
    m_controlsLabel->setWordWrap(false);
    m_controlsLabel->setMinimumHeight(12);
    
    // 预览标签
    m_previewLabel = new QLabel("预览: 生成中...", this);
    m_previewLabel->setStyleSheet("font-size: 8px; color: #999; margin: 0; padding: 0;");
    m_previewLabel->setWordWrap(false);
    m_previewLabel->setMinimumHeight(10);
    
    infoLayout->addWidget(m_nameLabel);
    infoLayout->addWidget(m_typeLabel);
    infoLayout->addWidget(m_controlsLabel);
    infoLayout->addWidget(m_previewLabel);
    
    mainLayout->addWidget(m_iconLabel);
    mainLayout->addLayout(infoLayout);
    
    // 设置样式 - 移除固定高度，使用sizeHint
    setStyleSheet("UIFilePreviewItem { border: 1px solid #ddd; border-radius: 4px; background-color: #fafafa; margin: 1px; }"
                  "UIFilePreviewItem:hover { border-color: #aaa; background-color: #f0f0f0; }");
    
    // 设置合适的最小高度
    setMinimumHeight(48);
    
    // 异步更新预览信息
    QTimer::singleShot(0, this, &UIFilePreviewItem::updatePreviewInfo);
}

void UIFilePreviewItem::updatePreviewInfo()
{
    // 解析UI文件获取预览信息
    parseUIFileForPreview();
    
    // 创建预览图标
    createPreviewIcon();
    
    // 更新显示信息
    m_typeLabel->setText(QString("类型: %1").arg(m_uiType));
    m_controlsLabel->setText(QString("控件: %1 个").arg(m_controlCount));
    m_previewLabel->setText(QString("预览: %1").arg(m_mainWidgetClass));
}

void UIFilePreviewItem::parseUIFileForPreview()
{
    if (m_uiFile.filePath.isEmpty()) {
        return;
    }
    
    QFile file(m_uiFile.filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        m_uiType = "文件无法访问";
        m_controlCount = 0;
        return;
    }
    
    QString content = file.readAll();
    file.close();
    
    // 解析UI文件内容
    QXmlStreamReader xml(content);
    m_controlCount = 0;
    
    // 定义UI类型映射
    QMap<QString, QString> uiTypeMap = {
        {"QMainWindow", "主窗口"},
        {"QDialog", "对话框"},
        {"QWidget", "控件"},
        {"QWizard", "向导"}
    };
    
    while (!xml.atEnd() && !xml.hasError()) {
        QXmlStreamReader::TokenType token = xml.readNext();
        
        if (token == QXmlStreamReader::StartElement) {
            QString elementName = xml.name().toString();
            
            if (elementName == "widget") {
                QString widgetClass = xml.attributes().value("class").toString();
                
                // 如果是顶层widget，确定UI类型
                if (m_mainWidgetClass.isEmpty() || m_mainWidgetClass == "QWidget") {
                    m_mainWidgetClass = widgetClass;
                    
                    if (uiTypeMap.contains(widgetClass)) {
                        m_uiType = uiTypeMap[widgetClass];
                    } else if (widgetClass.contains("Dialog")) {
                        m_uiType = "对话框";
                    } else if (widgetClass.contains("Wizard")) {
                        m_uiType = "向导";
                    } else if (widgetClass.contains("MainWindow")) {
                        m_uiType = "主窗口";
                    } else {
                        m_uiType = "自定义窗口";
                    }
                }
                
                // 统计控件数量（排除容器控件）
                if (widgetClass.startsWith("Q") && widgetClass.length() > 1 && 
                    widgetClass != "QWidget" && widgetClass != "QMainWindow" && 
                    widgetClass != "QDialog" && widgetClass != "QFrame" && 
                    widgetClass != "QGroupBox" && widgetClass != "QTabWidget" &&
                    widgetClass != "QScrollArea" && widgetClass != "QSplitter" &&
                    widgetClass != "QStackedWidget" && widgetClass != "QDockWidget") {
                    m_controlCount++;
                }
            }
        }
    }
    
    if (xml.hasError()) {
        m_uiType = "解析错误";
    }
}

// 重写sizeHint方法以提供合适的大小
QSize UIFilePreviewItem::sizeHint() const
{
    // 计算合适的大小：图标宽度 + 文本区域宽度 + 边距
    int width = 28 + 6 + 200 + 6 + 6; // 图标 + 间距 + 文本区域 + 边距
    int height = 48; // 固定高度
    
    return QSize(width, height);
}

void UIFilePreviewItem::createPreviewIcon()
{
    QPixmap pixmap(28, 28);  // 更新为28x28尺寸
    pixmap.fill(Qt::transparent);
    
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 根据UI类型设置不同的图标颜色
    QColor bgColor;
    if (m_uiType == "主窗口") {
        bgColor = QColor(74, 144, 226); // 蓝色
    } else if (m_uiType == "对话框") {
        bgColor = QColor(46, 204, 113); // 绿色
    } else if (m_uiType == "向导") {
        bgColor = QColor(155, 89, 182); // 紫色
    } else {
        bgColor = QColor(241, 196, 15); // 黄色
    }
    
    // 绘制背景
    painter.setBrush(bgColor);
    painter.setPen(QPen(Qt::darkGray, 1));
    painter.drawRoundedRect(2, 2, 24, 24, 3, 3);  // 更新尺寸
    
    // 绘制控件数量指示器
    if (m_controlCount > 0) {
        painter.setBrush(Qt::white);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(16, 2, 8, 8);  // 更新位置和尺寸
        
        painter.setPen(QPen(Qt::black, 1));
        painter.setFont(QFont("Arial", 5));  // 减小字体
        painter.drawText(QRect(16, 2, 8, 8), Qt::AlignCenter, QString::number(qMin(m_controlCount, 9)));
    }
    
    // 绘制窗口图标
    painter.setPen(QPen(Qt::white, 2));
    painter.drawRect(6, 6, 12, 9);  // 更新尺寸
    
    // 绘制标题栏
    painter.setPen(QPen(Qt::white, 1));
    painter.drawLine(6, 7, 18, 7);  // 更新位置
    
    m_iconLabel->setPixmap(pixmap);
}

void StateMachineEditorV2::onWizardNextPage()
{
    if (m_wizardPreviewWidget && m_wizardManager && m_wizardManager->currentWizard()) {
        Wizard* wizard = m_wizardManager->currentWizard();
        
        // 获取向导的当前页面索引
        int currentIndex = m_wizardPreviewWidget->currentPageIndex();
        int totalPages = wizard->allPages().size();
        
        // 如果还有下一页，切换到下一页
        if (currentIndex < totalPages - 1) {
            m_wizardPreviewWidget->showPage(currentIndex + 1);
            qDebug() << "Wizard preview navigated to page" << currentIndex + 1;
        } else {
            // 已经是最后一页，显示完成消息
            QMessageBox::information(this, "向导完成", "向导流程已完成！");
        }
    }
}

void StateMachineEditorV2::onWizardPreviousPage()
{
    if (m_wizardPreviewWidget && m_wizardManager && m_wizardManager->currentWizard()) {
        Wizard* wizard = m_wizardManager->currentWizard();
        
        // 获取向导的当前页面索引
        int currentIndex = m_wizardPreviewWidget->currentPageIndex();
        
        // 如果还有上一页，切换到上一页
        if (currentIndex > 0) {
            m_wizardPreviewWidget->showPage(currentIndex - 1);
            qDebug() << "Wizard preview navigated to page" << currentIndex - 1;
        } else {
            // 已经是第一页
            QMessageBox::information(this, "向导首页", "当前已是向导的第一页。");
        }
    }
}

void StateMachineEditorV2::onWizardPreviewClosed()
{
    if (m_wizardPreviewWidget) {
        m_wizardPreviewWidget->hide();
        qDebug() << "Wizard preview closed";
    }
}