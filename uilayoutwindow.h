#ifndef UILAYOUTWINDOW_H
#define UILAYOUTWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QListWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui {
class UILayoutWindow;
}
QT_END_NAMESPACE

// 自定义图形项类，用于表示可编辑的控件
class LayoutItem : public QGraphicsItem
{
public:
    explicit LayoutItem(const QString &widgetType, QGraphicsItem *parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    QString widgetType() const { return m_widgetType; }
    qreal width() const { return m_width; }
    qreal height() const { return m_height; }
    void setSize(qreal width, qreal height);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;

private:
    QString m_widgetType;
    QPointF m_lastMousePos;
    bool m_isDragging;
    bool m_isResizing;
    qreal m_width;
    qreal m_height;
};

class UILayoutWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit UILayoutWindow(QWidget *parent = nullptr);
    ~UILayoutWindow() override;

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private slots:
    void on_actionSave_Layout_triggered();
    void on_actionLoad_Layout_triggered();
    void on_actionUndo_triggered();
    void on_actionRedo_triggered();

private slots:
    void onItemPressed(QListWidgetItem *item);

private:
    Ui::UILayoutWindow *ui;
    QGraphicsScene *m_scene;

    // 初始化自定义控件列表
    void initWidgetLibrary();
    // 加载插件
    void loadPlugins();
};

#endif // UILAYOUTWINDOW_H