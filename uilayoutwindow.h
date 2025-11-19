#ifndef UILAYOUTWINDOW_H
#define UILAYOUTWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QListWidget>
#include <QListWidgetItem>
#include <QDrag>
#include <cmath> // 引入cmath以支持M_PI
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

// 自定义图形项类，用于表示可编辑的控件
class LayoutItem : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit LayoutItem(const QString &widgetType, const QString &text = "", QGraphicsItem *parent = nullptr);
    ~LayoutItem() override;

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    QString widgetType() const { return m_widgetType; }
    qreal width() const { return m_width; }
    qreal height() const { return m_height; }
    QString text() const { return m_text; }
    void setSize(qreal width, qreal height);
    void setText(const QString &text);

    // 文本编辑相关方法
    void startEditing();
    void finishEditing();

signals:
    void textDoubleClicked();
    void textChanged(const QString &newText);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    QString m_widgetType;
    QString m_text;
    QPointF m_lastMousePos;
    bool m_isDragging;
    bool m_isResizing;
    bool m_isEditing;
    qreal m_width;
    qreal m_height;
    QGraphicsTextItem *m_textItem;
};

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
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void on_actionSave_Layout_triggered();
    void on_actionLoad_Layout_triggered();
    void on_actionUndo_triggered();
    void on_actionRedo_triggered();
    void on_widgetListWidget_itemDoubleClicked(QListWidgetItem *item);
    void on_actionNew_Layout_triggered();
    void onActionPreviewTriggered();

private:
    Ui::UILayoutWindow *ui;
    QGraphicsScene *m_scene;
    LayoutItem *m_currentItem; // 当前选中的布局项
    bool m_isDragging;
    QPointF m_dragStartPos;
    bool m_isResizing; // 是否正在调整大小
    Qt::CursorShape m_resizeCursor; // 调整大小的光标形状
    PreviewWindow *m_previewWindow; // 预览窗口

    // 初始化自定义控件列表
    void initWidgetLibrary();
    // 加载插件
    void loadPlugins();
    void setupDragDrop();
    void drawGrid(); // 绘制网格线
    void setupScene(); // 初始化场景
    void onLayoutItemDoubleClicked(); // 处理布局项双击事件
};

#endif // UILAYOUTWINDOW_H