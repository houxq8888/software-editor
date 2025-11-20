#ifndef CUSTOMTREEWIDGET_H
#define CUSTOMTREEWIDGET_H

#include <QTreeWidget>
#include <QDrag>
#include <QMimeData>
#include <QPainter>
#include <QDebug>

class CustomTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    explicit CustomTreeWidget(QWidget *parent = nullptr);

public slots:
    void search(const QString &text);

protected:
    void startDrag(Qt::DropActions supportedActions) override;

private:
    void filterItems(QTreeWidgetItem *item, const QString &text);
};

#endif // CUSTOMTREEWIDGET_H