#ifndef PREVIEWWINDOW_H
#define PREVIEWWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QMap>
#include <QMouseEvent>

// Forward declaration
class LayoutItem;

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

    // Set layout items to preview
    void setLayoutItems(const QList<LayoutItem *> &items);

protected:
    // Mouse double click event handler
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    // Handle QLabel double click editing
    void handleLabelDoubleClick(QWidget *label, LayoutItem *item);
    // Handle QTabWidget double click editing
    void handleTabWidgetDoubleClick(QWidget *tabWidget, const QPoint &pos, LayoutItem *item);
    
    QWidget* createWidgetFromType(const QString &widgetType, QWidget *parent);

private:
    Ui::PreviewWindow *ui;
    QMap<LayoutItem *, QWidget *> m_widgetMap;
    QMap<QWidget *, LayoutItem *> m_widgetToItemMap; // Reverse mapping for quick lookup
};

#endif // PREVIEWWINDOW_H