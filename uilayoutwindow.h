#ifndef UILAYOUTWINDOW_H
#define UILAYOUTWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QListWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QSlider>
#include <QProgressBar>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QToolButton>
#include <QCommandLinkButton>
#include <QGroupBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QTabWidget>
#include "editareawidget.h"
#include "previewwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class UILayoutWindow;
}
QT_END_NAMESPACE

// 自定义QListWidget类，用于实现拖放功能
class CustomListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit CustomListWidget(QWidget *parent = nullptr);

protected:
    void startDrag(Qt::DropActions supportedActions) override;
};

// 用于表示布局控件的结构
class LayoutItem {
public:
    explicit LayoutItem(const QString &widgetType, const QString &text = "");
    QString widgetType() const;
    QString text() const;
    void setText(const QString &newText);
    
    // 位置和大小
    QPoint pos() const; 
    void setPos(const QPoint &pos);
    QSize size() const;
    void setSize(const QSize &size);
    
    QWidget *createWidget(QWidget *parent = nullptr) const; // 创建相应的控件

private:
    QString m_widgetType;
    QString m_text;
    QPoint m_pos;
    QSize m_size;
};

// UILayoutWindow 类，用于管理布局编辑界面
class UILayoutWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit UILayoutWindow(QWidget *parent = nullptr);
    ~UILayoutWindow() override;

    // 获取当前布局项
    QList<LayoutItem*> getLayoutItems() const;

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
    void on_actionLoad_Layout_triggered();
    void on_actionUndo_triggered();
    void on_actionRedo_triggered();
    void on_widgetListWidget_itemDoubleClicked(QListWidgetItem *item);
    void on_actionNew_Layout_triggered();
    void on_actionPreview_triggered();
    void onActionPreviewTriggered();
    void onHandleDragged(int handleIndex, const QPoint &delta);
    void onHandleReleased();

private:
    Ui::UILayoutWindow *ui;
    EditAreaWidget *m_editAreaWidget; // 编辑区的 QWidget，替代 QGraphicsView
    QList<LayoutItem*> m_layoutItems; // 存储所有布局项
    QMap<QWidget*, LayoutItem*> m_widgetItemMap; // 控件到LayoutItem的映射
    PreviewWindow *m_previewWindow; // 预览窗口

    // 初始化自定义控件列表
    void initWidgetLibrary();
    // 加载插件
    void loadPlugins();
    void setupDragDrop(); // 设置拖放
    void setupEditArea(); // 初始化编辑区域
    void onLayoutItemDoubleClicked(); // 处理布局项双击事件
    void addWidgetToEditArea(LayoutItem *item); // 将控件添加到编辑区
};

#endif // UILAYOUTWINDOW_H
