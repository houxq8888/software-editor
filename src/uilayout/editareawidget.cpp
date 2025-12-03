#include "editareawidget.h"
#include <QMimeData>

EditAreaWidget::EditAreaWidget(QWidget *parent)
    : QWidget{parent}
{
    setMouseTracking(true); // 启用鼠标跟踪
    setAcceptDrops(true); // 启用拖放功能
}

void EditAreaWidget::setHandles(const QList<QRect> &handles) {
    m_handles = handles;
    update(); // 触发重绘
}

void EditAreaWidget::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event); // 调用父类的paintEvent
    
    QPainter painter(this);
    
    // 绘制点阵网格背景
    painter.setPen(QPen(Qt::lightGray, 0)); // 使用细线绘制
    painter.setBrush(QBrush(Qt::lightGray));
    
    int gridSize = 20; // 网格间距
    int dotSize = 2;    // 点的大小
    
    // 绘制水平方向的点
    for (int x = 0; x < width(); x += gridSize) {
        for (int y = 0; y < height(); y += gridSize) {
            painter.drawRect(x - dotSize / 2, y - dotSize / 2, dotSize, dotSize);
        }
    }
    
    // 绘制所有控制点
    painter.setPen(QPen(Qt::blue, 1));
    painter.setBrush(QBrush(Qt::blue));
    foreach (const QRect &handle, m_handles) {
        painter.drawRect(handle);
    }
}

void EditAreaWidget::mousePressEvent(QMouseEvent *event) {
    // 检查鼠标是否点击在某个控制点上
    for (int i = 0; i < m_handles.size(); ++i) {
        if (m_handles[i].contains(event->pos())) {
            m_draggingHandleIndex = i;
            m_mousePressPos = event->pos();
            return;
        }
    }
    // 点击空白处，让所有子控件失去焦点
    if (focusWidget()) {
        focusWidget()->clearFocus();
    }
    // 点击空白处，隐藏所有控制点
    emit handlesShouldHide();
    QWidget::mousePressEvent(event);
}

void EditAreaWidget::mouseMoveEvent(QMouseEvent *event)
{
    // 如果事件为空，直接返回
    if (!event) {
        return;
    }

    if (m_draggingHandleIndex != -1) {
        // 计算鼠标移动的偏移量
        QPoint delta = event->pos() - m_mousePressPos;
        emit handleDragged(m_draggingHandleIndex, delta);
        m_mousePressPos = event->pos(); // 更新鼠标按下位置
        return;
    }

    // 根据鼠标位置设置光标形状
    Qt::CursorShape cursorShape = Qt::ArrowCursor;
    for (int i = 0; i < m_handles.size(); ++i) {
        if (m_handles[i].contains(event->pos())) {
            switch (i) {
            case 0: // 左上角
                cursorShape = Qt::SizeFDiagCursor;
                break;
            case 1: // 上中
                cursorShape = Qt::SizeVerCursor;
                break;
            case 2: // 右上角
                cursorShape = Qt::SizeBDiagCursor;
                break;
            case 3: // 左中
                cursorShape = Qt::SizeHorCursor;
                break;
            case 4: // 右中
                cursorShape = Qt::SizeHorCursor;
                break;
            case 5: // 左下角
                cursorShape = Qt::SizeBDiagCursor;
                break;
            case 6: // 下中
                cursorShape = Qt::SizeVerCursor;
                break;
            case 7: // 右下角
                cursorShape = Qt::SizeFDiagCursor;
                break;
            default:
                cursorShape = Qt::ArrowCursor;
                break;
            }
            break;
        }
    }
    setCursor(cursorShape);

    QWidget::mouseMoveEvent(event);
}

void EditAreaWidget::enterEvent(QEnterEvent *event)
{
    // 鼠标进入时检查光标位置
    QWidget::enterEvent(event);
    mouseMoveEvent(nullptr); // 触发一次鼠标移动事件来更新光标
}

void EditAreaWidget::leaveEvent(QEvent *event)
{
    // 鼠标离开时重置光标
    setCursor(Qt::ArrowCursor);
    QWidget::leaveEvent(event);
}

void EditAreaWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_draggingHandleIndex != -1) {
        emit handleReleased();
        m_draggingHandleIndex = -1;
        return;
    }
    QWidget::mouseReleaseEvent(event);
}

void EditAreaWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    // 将双击事件传递给父窗口处理
    QWidget::mouseDoubleClickEvent(event);
    emit doubleClicked(event->pos());
}

void EditAreaWidget::dragEnterEvent(QDragEnterEvent *event)
{
    // 检查是否有文本数据
    if (event->mimeData()->hasText()) {
        event->acceptProposedAction();
    }
}

void EditAreaWidget::dragMoveEvent(QDragMoveEvent *event)
{
    // 检查是否有文本数据
    if (event->mimeData()->hasText()) {
        event->acceptProposedAction();
    }
}

void EditAreaWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
    QWidget::dragLeaveEvent(event);
}

void EditAreaWidget::dropEvent(QDropEvent *event)
{
    // 获取拖放的文本数据
    QString widgetType = event->mimeData()->text();
    // 获取拖放的位置
    QPoint pos = event->pos();
    // 发送控件拖放信号
    emit widgetDropped(widgetType, pos);
    // 接受拖放动作
    event->acceptProposedAction();
}
