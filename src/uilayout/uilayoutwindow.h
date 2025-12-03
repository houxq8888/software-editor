#ifndef UILAYOUTWINDOW_H
#define UILAYOUTWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QComboBox>
#include <QFontComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QSlider>
#include <QScrollBar>
#include <QKeySequenceEdit>
#include <QProgressBar>
#include <QSpacerItem>
#include <QGraphicsView>
#include <QMdiArea>
#include <QStackedWidget>
#include <QToolBox>
#include <QScrollArea>
#include <QDialogButtonBox>
#include <QColumnView>
#include <QUndoView>
#include <QTimeEdit>
#include <QDateEdit>
#include <QTextBrowser>
#include <QLCDNumber>
#include <QDateTimeEdit>
#include <QDial>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QToolButton>
#include <QCommandLinkButton>
#include <QGroupBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QTabWidget>
#include <QFormLayout>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNodeList>
#include <QOpenGLWidget>
#include <QFrame>
#include <QMessageBox>
#include <QInputDialog>
#include <QStringList>
#include <algorithm>
#include "editareawidget.h"
#include "previewwindow.h"
#include "customtreewidget.h"
#include "product.h"
#include "eventactioneditor.h"
#include "uiinterfacemanager.h"
#include "layoutitem.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class UILayoutWindow;
}
QT_END_NAMESPACE

// UILayoutWindow 类，用于管理布局编辑界面
class ProductConfigManager;

class UILayoutWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit UILayoutWindow(QWidget *parent = nullptr, bool isNewProduct = false, const QString &productFilePath = QString(), ProductConfigManager *configManager = nullptr);
    ~UILayoutWindow() override;

    // 获取当前布局项
    QList<LayoutItem*> getLayoutItems() const;
    
    // 从指定文件路径加载布局
    bool loadLayout(const QString &filePath);
    
    // 获取当前UI文件路径
    QString getCurrentLayoutPath() const;

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    // 用于跟踪控件移动和调整大小的状态
    QWidget *m_currentWidget = nullptr;
    QWidget *m_selectedWidget = nullptr;  // 当前选中的控件
    Qt::CursorShape m_currentCursorShape = Qt::ArrowCursor;
    QPoint m_mousePressPos;
    QPoint m_widgetPos;
    bool m_resizing = false;
    bool m_moving = false;
    // 调整大小的方向
    bool m_resizeLeft = false;
    bool m_resizeRight = false;
    bool m_resizeTop = false;
    bool m_resizeBottom = false;
    // 调整大小的边界阈值
    const int RESIZE_MARGIN = 8;
    // 控制点大小
    const int HANDLE_SIZE = 8;
    // 控制点位置
    QRect m_topLeftHandle;
    // 更新控制点位置
    void updateHandles(QWidget *widget);

private slots:
    void on_actionSave_Layout_triggered();
    void on_actionSave_As_Layout_triggered();
    void on_actionLoad_Layout_triggered();
    void on_actionUndo_triggered();
    void on_actionRedo_triggered();
    void on_widgetListWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void on_actionNew_Layout_triggered();
    void on_actionPreview_triggered();
    void handleDoubleClick(const QPoint &pos);
    void onHandleDragged(int handleIndex, const QPoint &delta);
    void onHandleReleased();
    void onEditAreaDoubleClicked(const QPoint &pos);
    void onWidgetDropped(const QString &widgetType, const QPoint &pos);
    void onBindFeatureButtonClicked();
    void onUnbindFeatureButtonClicked();
    void onSyncFeaturesButtonClicked();
    void onFeaturesTreeWidgetItemDoubleClicked(QTreeWidgetItem *item, int column);
    // 产品上下文感知相关方法
    void updateStatusBarWithProductContext();
    void suggestLayoutForFeatures();
    void highlightRelevantWidgets(const QString &featureName);
    void showFeatureContextMenu(const QPoint &pos);
    
private slots:
    void onActionSuggestLayoutTriggered();
    // 事件-动作编辑器相关
    void onActionEventActionEditorTriggered();
    // 更新属性编辑器
    void updatePropertiesEditor(QWidget *widget);
    // 属性项编辑完成
    void onPropertyItemChanged(QTreeWidgetItem *item, int column);
    // 多界面管理相关槽函数
    void onInterfaceComboBoxChanged(int index);
    void onAddInterfaceAction();
    void onDeleteInterfaceAction();
    void onRenameInterfaceAction();
    void onCopyInterfaceAction();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::UILayoutWindow *ui;
    EditAreaWidget *m_editAreaWidget; // 编辑区的 QWidget，替代 QGraphicsView
    QPoint m_lastMousePos; // 记录鼠标位置
    QList<LayoutItem*> m_layoutItems; // 存储所有布局项
    QMap<QWidget*, LayoutItem*> m_widgetItemMap; // 控件到LayoutItem的映射
    PreviewWindow *m_previewWindow; // 预览窗口
    UIInterfaceManager *m_interfaceManager; // 多界面管理器
    
    // 多界面管理相关方法
    void setupInterfaceManagementUI();
    void updateInterfaceList();
    void switchToInterface(UIInterface *interface);
    void saveCurrentInterfaceState();
    void loadInterfaceState(UIInterface *interface);
    // Tab页标题编辑相关
    QTabWidget *m_editingTabWidget;
    QLineEdit *m_tabTitleEdit;
    QString m_currentLayoutPath; // 当前布局文件路径
    QString m_productFilePath; // 关联的产品配置文件路径
    ProductConfigManager *m_configManager; // 产品配置管理器
    QTabWidget *rightTabWidget;
    QTreeWidget *featuresTreeWidget;
    QPushButton *bindFeatureButton;
    QPushButton *unbindFeatureButton;
    QPushButton *syncFeaturesButton;

    // 初始化自定义控件列表
    void initWidgetLibrary();
    // 加载插件
    void loadPlugins();
    void setupDragDrop(); // 设置拖放
    void setupEditArea(); // 初始化编辑区域
    void onLayoutItemDoubleClicked(); // 处理布局项双击事件
    void onActionPreviewTriggered(); // 预览布局
    void addWidgetToEditArea(LayoutItem *item, QWidget *targetWidget = nullptr); // 添加控件到编辑区，并指定父窗口将控件添加到编辑区
    
    // 撤销/重做功能相关
    void saveLayoutState(); // 保存当前布局状态
    
    // 产品配置集成功能
    void updateProductContext(); // 更新产品上下文
    void syncWithProductFeatures(); // 与产品功能特性同步
    void applyLayoutTemplate(const QString &templateName); // 应用布局模板
    
    // 控件ID管理
    QString generateWidgetId(const QString &widgetType) const;
    void updateWidgetBindings(); // 更新控件绑定
    bool restoreLayoutState(QList<QByteArray> &stack); // 恢复布局状态
    
    // 智能布局建议相关方法
    void suggestSimpleColumnLayout(); // 建议简单单列布局
    void suggestTwoColumnLayout(); // 建议两列布局
    void suggestTabbedLayout(); // 建议Tab页布局
    QWidget* createWidgetForFeature(const ProductFeature &feature, const QString &widgetType); // 为功能特性创建控件
    
    // 智能模板管理相关方法
    void saveCurrentLayoutAsTemplate(const QString &templateName); // 保存当前布局为模板
    void showTemplateSelectionDialog(); // 显示模板选择对话框
    void applySmartTemplate(); // 应用智能模板
    
    // 智能模板管理辅助方法
    QString selectBestTemplateForFeatures(const QList<ProductFeature> &features);
    double calculateTemplateMatchScore(const QString &templateName, int featureCount, int inputCount, int displayCount, int actionCount);
    
    // 辅助方法：根据功能特性建议控件类型
    QString suggestWidgetTypeForFeature(const ProductFeature &feature);
    
    // 辅助方法：根据控件ID查找控件
    QWidget* findWidgetById(const QString &widgetId);
    
    // 更新功能特性树控件
    void updateFeaturesTreeWidget();
    
    // 清空布局
    void clearLayout();
    // 清空编辑区
    void clearEditArea();
    
    // 操作历史记录
    QList<QByteArray> m_undoStack;
    QList<QByteArray> m_redoStack;
    int m_maxUndoSteps = 50; // 最大撤销步数
    
    // 事件-动作编辑器相关
    EventActionEditor *m_eventActionEditor; // 事件-动作编辑器窗口
};

#endif // UILAYOUTWINDOW_H
