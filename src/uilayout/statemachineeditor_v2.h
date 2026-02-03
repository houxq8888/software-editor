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
#include <QListWidget>
#include "statemachine.h"
#include "product.h"
#include "statemachineruntime.h"
#include "uiflowstatemachine.h"
#include "logicsequencestatemachine.h"
#include "statemachineintegrationmanager.h"
#include "wizard.h"
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPixmap>
#include <QPainter>
#include <QUiLoader>

// UI文件预览列表项
class UIFilePreviewItem : public QWidget
{
    Q_OBJECT

public:
    explicit UIFilePreviewItem(const ProductUIFile &uiFile, QWidget *parent = nullptr);
    
    QString filePath() const { return m_uiFile.filePath; }
    QString fileName() const { return m_uiFile.name; }
    
    void updatePreviewInfo();
    void setMainInterface(bool isMain);
    bool isMainInterface() const { return m_uiFile.isMain; }
    
    // 重写sizeHint方法以提供合适的大小
    QSize sizeHint() const override;

signals:
    void setAsMainInterfaceRequested(const QString &filePath);
    
private slots:
    void onSetMainButtonClicked();
    
private:
    ProductUIFile m_uiFile;
    QLabel *m_iconLabel;
    QLabel *m_nameLabel;
    QLabel *m_typeLabel;
    QLabel *m_controlsLabel;
    QLabel *m_previewLabel;
    QLabel *m_mainInterfaceLabel;
    QPushButton *m_setMainButton;
    
    void createPreviewIcon();
    void parseUIFileForPreview();
    
    int m_controlCount;
    QString m_uiType;
    QString m_mainWidgetClass;
};

// UI文件运行时预览窗口
class UIRuntimePreviewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UIRuntimePreviewWidget(QWidget *parent = nullptr);
    ~UIRuntimePreviewWidget();
    
    void loadUIFile(const QString &filePath);
    void clearPreview();
    bool isPreviewLoaded() const { return m_previewLoaded; }
    
private:
    QWidget *m_previewWidget;
    QLabel *m_placeholderLabel;
    QVBoxLayout *m_mainLayout;
    bool m_previewLoaded;
    
    void createPlaceholder();
    void createPreviewLayout();
};

// 向导UI串联预览窗口
class WizardPreviewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WizardPreviewWidget(QWidget *parent = nullptr);
    ~WizardPreviewWidget();
    
    void loadWizard(Wizard *wizard);
    void clearPreview();
    bool isPreviewLoaded() const { return m_previewLoaded; }
    
    // 向导页面导航
    void nextPage();
    void previousPage();
    void goToPage(int pageIndex);
    void showPage(int pageIndex);
    
    // 获取当前页面信息
    int currentPageIndex() const { return m_currentPageIndex; }
    int totalPages() const { return m_totalPages; }
    QString currentPageTitle() const;
    
    // 获取当前向导
    Wizard* currentWizard() const { return m_currentWizard; }
    
public slots:
    void onWizardPageChanged(int pageIndex);
    void onWizardCompleted();
    void onWizardCancelled();

signals:
    void pageChanged(int pageIndex, const QString &pageTitle);
    void wizardCompleted();
    void wizardCancelled();
    void nextPageRequested();
    void previousPageRequested();
    void closePreviewRequested();
    
private:
    Wizard *m_currentWizard;
    QWidget *m_previewWidget;
    QLabel *m_placeholderLabel;
    QLabel *m_pageTitleLabel;
    QLabel *m_pageDescriptionLabel;
    QLabel *m_navigationLabel;
    QPushButton *m_prevButton;
    QPushButton *m_nextButton;
    QPushButton *m_finishButton;
    QPushButton *m_cancelButton;
    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_navigationLayout;
    bool m_previewLoaded;
    int m_currentPageIndex;
    int m_totalPages;
    
    void createPreviewLayout();
    void createPlaceholder();
    void createNavigationControls();
    void updateNavigationControls();
    void loadCurrentPage();
    void clearCurrentPage();
};

// 状态机模式枚举
enum class StateMachineMode {
    UIFlowMode,           // UI流模式：编辑UI界面交互流程
    LogicSequenceMode     // 逻辑时序模式：编辑业务逻辑运行流程
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
    
    // 向导文件自动加载
    void loadWizardsFromProductConfig();
    void updateWizardFilePathLabel(QLabel *label);
    
    StateMachineMode currentMode() const;

    void printUiInterfaceComboCount();
    
public slots:
    void createNewStateMachine();
    void editStateProperties();
    void editTransitionProperties();
    void setInitialState();
    void validateStateMachine();
    void showRuntimePreview();
    void switchToUIFlowMode();
    void switchToLogicSequenceMode();
    
    // 向导操作
    void createNewWizard();
    void editWizard();
    void deleteWizard();
    void runWizard();
    void saveCurrentWizard();
    void switchWizardFile();
    void updateWindowTitle();

private slots:
    void onStateSelected(const UIFlowStateMachine::UIFlowState &state);
    void onTransitionSelected(const UIFlowStateMachine::UIFlowTransition &transition);
    void onLogicStateSelected(const LogicSequenceStateMachine::LogicState &state);
    void onLogicTransitionSelected(const LogicSequenceStateMachine::LogicTransition &transition);
    void onStateMachineChanged();
    void onUiInterfaceChanged();
    void onModeChanged();
    
    // 向导预览相关
    void onWizardNextPage();
    void onWizardPreviousPage();
    void onWizardPreviewClosed();
    
    // 属性标签页切换
    void onPropertiesTabChanged(int index);

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

    void updateModeUI();
    void updateToolbarAvailability();
    void updatePropertiesPanelAvailability();
    void updateWizardEditorInterface();
    
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
    QPushButton *m_step1Button;
    QPushButton *m_step2Button;
    QLabel *m_modeDescription;
    
    // 运行时预览
    StateMachineRuntime *m_runtime;
    StateMachineRuntimeView *m_runtimeView;
    QDockWidget *m_runtimeDock;
    
    // UI文件运行时预览
    UIRuntimePreviewWidget *m_uiRuntimePreview;
    
    // 向导预览窗口
    WizardPreviewWidget *m_wizardPreviewWidget;
    
    // 产品UI文件信息
    QList<ProductUIFile> m_productUiFiles;
    Product *m_product;
    
    // 缓存的状态机配置信息（避免重复读取文件）
    QString m_cachedWizardJsonPath; // 缓存的向导文件路径
    QString m_cachedLogicJsonPath;  // 缓存的逻辑文件路径
    
    // 工具栏操作指针
    QList<QAction*> m_fileActions;
    QList<QAction*> m_editActions;
    QList<QAction*> m_toolActions;
    QList<QAction*> m_wizardActions;
    
    // UI元素
    QTabWidget *m_propertiesTab;
    QWidget *m_uiFlowProperties;
    QWidget *m_logicProperties;
    
    // UI流属性面板
    QLineEdit *m_uiFlowStateNameEdit;
    QTextEdit *m_uiFlowStateDescriptionEdit;
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
    
    // 向导管理器
    WizardManager *m_wizardManager;
    
    // 当前向导状态跟踪
    QString m_currentWizardName;          // 当前打开的向导名称
    QString m_currentWizardFilePath;      // 当前向导文件路径（如果已保存）
    bool m_isWizardModified;              // 向导是否已修改但未保存

    // UI文件列表显示相关
    QListWidget *m_uiFilesListWidget;    // UI文件列表控件
    QTextEdit *m_uiFileDetailsTextEdit;  // UI文件详情显示控件
    QLabel *m_uiFilesLabel;              // UI文件列表标题
    bool m_uiFilesDataPendingUpdate;     // UI文件数据待更新标志
    
    // 控件事件定义相关
    QString m_currentUIFilePath;         // 当前选中的UI文件路径
    QMap<QString, QStringList> m_currentUIFileControls; // 当前UI文件的控件信息（控件名 -> 支持的事件列表）
    QMap<QString, QStringList> m_controlEventMap; // 控件类型到支持的事件映射
    QList<QString> m_definedControlEvents; // 已定义的控件事件列表（事件名称 -> 控件名 -> 事件类型）
    QListWidget *m_definedEventsListWidget; // 已定义事件列表控件指针
    
    void createUIFilesDisplay();         // 创建UI文件显示界面
    void updateUIFilesList();            // 更新UI文件列表
    void parseUIFile(const QString &filePath); // 解析UI文件获取控件信息
    void displayUIFileDetails(const QString &filePath); // 显示UI文件详情
    
    // 控件事件定义相关方法
    void updateControlComboBox(QComboBox *controlComboBox); // 更新控件选择下拉框
    void updateEventComboBox(QComboBox *eventComboBox, const QString &controlName); // 更新事件类型下拉框
    void updateTargetUIComboBox(QComboBox *targetUIComboBox); // 更新目标UI页面选择下拉框
    void addControlEventDefinition(QComboBox *controlComboBox, QComboBox *eventComboBox, 
                                   QLineEdit *eventNameEdit, QComboBox *targetUIComboBox); // 添加控件事件定义
    void updateDefinedEventsList(const QString &uiFilePath = QString()); // 更新已定义事件列表显示，支持按UI文件路径过滤
    
    // 主界面设置相关方法
    void onSetAsMainInterfaceRequested(const QString &filePath); // 处理设置主界面请求
    void updateMainInterfaceStatus(); // 更新所有UI文件的主界面状态显示
    
    // QT代码生成方法
    QString generateQtCodeForWizard(Wizard *wizard, const QList<UIFilePreviewItem*> &uiItems);

};

#endif // STATEMACHINEEDITOR_V2_H