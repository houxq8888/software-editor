#include "customtreewidget.h"

CustomTreeWidget::CustomTreeWidget(QWidget *parent)
    : QTreeWidget(parent)
{
}

void CustomTreeWidget::search(const QString &text)
{
    // 遍历所有根节点
    for (int i = 0; i < topLevelItemCount(); ++i) {
        QTreeWidgetItem *rootItem = topLevelItem(i);
        filterItems(rootItem, text);
    }
}

void CustomTreeWidget::filterItems(QTreeWidgetItem *item, const QString &text)
{
    if (!item) return;

    // 检查当前项是否匹配搜索文本
    bool match = item->text(0).contains(text, Qt::CaseInsensitive);

    // 遍历所有子项
    for (int i = 0; i < item->childCount(); ++i) {
        QTreeWidgetItem *childItem = item->child(i);
        filterItems(childItem, text);

        // 如果子项匹配或有匹配的子项，则当前项也匹配
        if (childItem->isHidden() == false) {
            match = true;
        }
    }

    // 显示或隐藏当前项
    if (item->parent() == nullptr) {
        // 根节点（分类）仅当有匹配子项时显示
        item->setHidden(!match);
    } else {
        // 子节点根据是否匹配显示
        item->setHidden(!match);
    }
}

void CustomTreeWidget::startDrag(Qt::DropActions supportedActions)
{
    QTreeWidgetItem *item = currentItem();
    if (!item) {
        qDebug() << "CustomTreeWidget::startDrag: 当前选中项为空";
        return;
    }

    // 只有子项可以拖动
    if (item->childCount() > 0) {
        qDebug() << "CustomTreeWidget::startDrag: 父项不可拖动";
        return;
    }

    QString widgetType = item->text(0);
    qDebug() << "CustomTreeWidget::startDrag: 开始拖动控件类型:" << widgetType;

    QMimeData *mimeData = new QMimeData;
    mimeData->setText(widgetType);
    qDebug() << "CustomTreeWidget::startDrag: 设置mimeData文本:" << mimeData->text();
    qDebug() << "CustomTreeWidget::startDrag: mimeData hasText:" << mimeData->hasText();

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    qDebug() << "CustomTreeWidget::startDrag: 创建QDrag对象";

    // 创建拖动时的预览
    QPixmap pixmap(80, 60);
    pixmap.fill(QColor(100, 200, 300, 150));  // 设置半透明背景
    QPainter painter(&pixmap);
    painter.setPen(Qt::black);
    painter.drawText(pixmap.rect(), Qt::AlignCenter, widgetType);  // 绘制控件类型
    painter.end();

    drag->setPixmap(pixmap);
    drag->setHotSpot(pixmap.rect().center());
    qDebug() << "CustomTreeWidget::startDrag: 设置拖动预览和热点";

    Qt::DropAction result = drag->exec(Qt::CopyAction | Qt::MoveAction);
    qDebug() << "CustomTreeWidget::startDrag: 拖动执行结果:" << result;
}
