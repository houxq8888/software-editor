#ifndef EDITAREAWIDGET_H
#define EDITAREAWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QList>
#include <QRect>
#include <QMouseEvent>

class EditAreaWidget : public QWidget
{
    Q_OBJECT
public:
    explicit EditAreaWidget(QWidget *parent = nullptr);

    // 设置控制点
    void setHandles(const QList<QRect> &handles);

    // 获取当前拖动的控制点索引
    int getDraggingHandleIndex() const { return m_draggingHandleIndex; }

    // 设置当前拖动的控制点索引
    void setDraggingHandleIndex(int index) { m_draggingHandleIndex = index; }

    // 清除拖动状态
    void clearDraggingHandle() { m_draggingHandleIndex = -1; }

signals:
    // 控制点拖动信号
    void handleDragged(int handleIndex, const QPoint &delta);

    // 鼠标释放信号
    void handleReleased();

    // 控制点应隐藏信号
    void handlesShouldHide();
    
    // 双击事件信号
    void doubleClicked(const QPoint &pos);
    
    // 控件选中信号
    void widgetSelected(QWidget *widget);

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    QList<QRect> m_handles; // 存储控制点
    const int HANDLE_SIZE = 8; // 控制点大小
    int m_draggingHandleIndex = -1; // 当前拖动的控制点索引
    QPoint m_mousePressPos; // 鼠标按下时的位置
};

#endif // EDITAREAWIDGET_H
