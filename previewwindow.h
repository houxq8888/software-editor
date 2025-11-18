#ifndef PREVIEWWINDOW_H
#define PREVIEWWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QMap>

// 前置声明
class LayoutItem;

QT_BEGIN_NAMESPACE

QT_BEGIN_NAMESPACE
namespace Ui {
class PreviewWindow;
}
QT_END_NAMESPACE

class PreviewWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit PreviewWindow(QWidget *parent = nullptr);
    ~PreviewWindow();

    // 设置要预览的布局项
    void setLayoutItems(const QList<LayoutItem *> &items);

private:
    Ui::PreviewWindow *ui;
    QWidget *m_previewWidget;
    QMap<LayoutItem *, QWidget *> m_widgetMap;
};

#endif // PREVIEWWINDOW_H
