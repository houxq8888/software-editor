#include "editareawidget.h"

EditAreaWidget::EditAreaWidget(QWidget *parent)
    : QWidget{parent}
{}

void EditAreaWidget::setHandles(const QList<QRect> &handles) {
    m_handles = handles;
    update(); // 触发重绘
}

void EditAreaWidget::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event); // 调用父类的paintEvent
    
    QPainter painter(this);
    painter.setPen(QPen(Qt::blue, 1));
    painter.setBrush(QBrush(Qt::blue));
    
    // 绘制所有控制点
    foreach (const QRect &handle, m_handles) {
        painter.drawRect(handle);
    }
}
