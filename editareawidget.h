#ifndef EDITAREAWIDGET_H
#define EDITAREAWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QList>
#include <QRect>

class EditAreaWidget : public QWidget
{
    Q_OBJECT
public:
    explicit EditAreaWidget(QWidget *parent = nullptr);

    // 设置控制点
    void setHandles(const QList<QRect> &handles);

protected:
    void paintEvent(QPaintEvent *event) override;
signals:

private:
    QList<QRect> m_handles; // 存储控制点
    const int HANDLE_SIZE = 8; // 控制点大小
};

#endif // EDITAREAWIDGET_H
