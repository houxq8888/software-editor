#include "uilayoutwindow.h"
#include "ui_uilayoutwindow.h"
#include <QInputDialog>
#include <QLineEdit>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QDrag>
#include <QMimeData>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QPluginLoader>
#include <QDir>
#include <QDebug>
#include <QListWidgetItem>

// 实现CustomListWidget的构造函数
CustomListWidget::CustomListWidget(QWidget *parent)
    : QListWidget(parent)
{
}

// 实现CustomListWidget的startDrag方法
void CustomListWidget::startDrag(Qt::DropActions supportedActions)
{
    QListWidgetItem *item = currentItem();
    if (!item) {
        qDebug() << "CustomListWidget::startDrag: 当前选中项为空";
        return;
    }

    QString widgetType = item->text();
    qDebug() << "CustomListWidget::startDrag: 开始拖动控件类型:" << widgetType;

    QMimeData *mimeData = new QMimeData;
    mimeData->setText(widgetType);
    qDebug() << "CustomListWidget::startDrag: 设置mimeData文本:" << mimeData->text();
    qDebug() << "CustomListWidget::startDrag: mimeData hasText:" << mimeData->hasText();

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    qDebug() << "CustomListWidget::startDrag: 创建QDrag对象";

    // 创建拖动时的预览
    QPixmap pixmap(80, 60);
    pixmap.fill(QColor(100, 200, 300, 150));  // 设置半透明背景
    QPainter painter(&pixmap);
    painter.setPen(Qt::black);
    painter.drawText(pixmap.rect(), Qt::AlignCenter, widgetType);  // 绘制控件类型
    painter.end();

    drag->setPixmap(pixmap);
    drag->setHotSpot(pixmap.rect().center());
    qDebug() << "CustomListWidget::startDrag: 设置拖动预览和热点";

    Qt::DropAction result = drag->exec(Qt::CopyAction | Qt::MoveAction);
    qDebug() << "CustomListWidget::startDrag: 拖动执行结果:" << result;
}

LayoutItem::LayoutItem(const QString &widgetType, const QString &text, QGraphicsItem *parent)
    : QGraphicsObject(parent), m_widgetType(widgetType), m_text(text), m_isDragging(false), m_isResizing(false), m_width(100), m_height(80)
{
    setFlag(ItemIsMovable, true);
    setFlag(ItemIsSelectable, true);
    setFlag(ItemSendsGeometryChanges, true);
    setAcceptHoverEvents(true);
}

QRectF LayoutItem::boundingRect() const
{
    return QRectF(0, 0, m_width, m_height);
}

void LayoutItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{    Q_UNUSED(widget)

    // 绘制控件外观
    QRectF rect = boundingRect();
    painter->setPen(QPen(Qt::black, 2));
    
    // 根据不同控件类型绘制不同的外观
    if (m_widgetType == "QPushButton") {
        painter->setBrush(QBrush(Qt::lightGray));
        painter->drawRect(rect.adjusted(0, 0, -2, -2));
        // 绘制按钮文字
        painter->setPen(QPen(Qt::black));
        painter->drawText(rect, Qt::AlignCenter, "Button");
    } else if (m_widgetType == "QLineEdit") {
        painter->setBrush(QBrush(Qt::white));
        painter->drawRect(rect);
        // 绘制输入框光标
        QRectF cursorRect(rect.left() + 5, rect.top() + 5, 2, rect.height() - 10);
        painter->fillRect(cursorRect, Qt::black);
    } else if (m_widgetType == "QLabel") {
        painter->setBrush(QBrush(Qt::white));
        painter->drawRect(rect);
        // 绘制标签文字
        painter->setPen(QPen(Qt::black));
        painter->drawText(rect, Qt::AlignCenter, "Label");
    } else if (m_widgetType == "QCheckBox") {
        painter->setBrush(QBrush(Qt::white));
        painter->drawRect(rect.adjusted(0, 0, -rect.width() + 20, 0));
        // 绘制复选框勾选
        painter->setPen(QPen(Qt::black, 2));
        painter->drawLine(rect.left() + 3, rect.center().y(), rect.left() + 8, rect.center().y() + 5);
        painter->drawLine(rect.left() + 8, rect.center().y() + 5, rect.left() + 17, rect.center().y() - 4);
        // 绘制复选框文字
        painter->setPen(QPen(Qt::black));
        painter->drawText(rect.adjusted(25, 0, 0, 0), Qt::AlignVCenter, "CheckBox");
    } else if (m_widgetType == "QRadioButton") {
        painter->setBrush(QBrush(Qt::white));
        painter->drawEllipse(rect.left(), rect.top() + rect.height() / 2 - 8, 16, 16);
        // 绘制单选按钮选中点
        painter->setBrush(QBrush(Qt::black));
        painter->drawEllipse(rect.left() + 4, rect.top() + rect.height() / 2 - 4, 8, 8);
        // 绘制单选按钮文字
        painter->setPen(QPen(Qt::black));
        painter->drawText(rect.adjusted(25, 0, 0, 0), Qt::AlignVCenter, "RadioButton");
    } else if (m_widgetType == "QTextEdit") {
        painter->setBrush(QBrush(Qt::white));
        painter->drawRect(rect);
        // 绘制文本编辑器内容
        painter->setPen(QPen(Qt::black));
        painter->drawText(rect.adjusted(5, 5, -5, -5), Qt::AlignLeft | Qt::AlignTop, "Text Edit\nMulti-line");
    } else if (m_widgetType == "QComboBox") {
        painter->setBrush(QBrush(Qt::white));
        painter->drawRect(rect);
        // 绘制下拉箭头
        QPointF points[3] = {
            rect.topRight() - QPointF(20, 10),
            rect.topRight() - QPointF(10, 20),
            rect.topRight() - QPointF(20, 30)
        };
        painter->setBrush(QBrush(Qt::black));
        painter->drawPolygon(points, 3);
        // 绘制当前选项
        painter->setPen(QPen(Qt::black));
        painter->drawText(rect.adjusted(5, 0, -25, 0), Qt::AlignVCenter, "Option 1");
    } else if (m_widgetType == "QSpinBox") {
        painter->setBrush(QBrush(Qt::white));
        painter->drawRect(rect);
        // 绘制上下箭头
        QRectF upArrowRect(rect.topRight() - QPointF(20, 0), QSizeF(20, rect.height() / 2));
        QRectF downArrowRect(rect.bottomRight() - QPointF(20, 0), QSizeF(20, rect.height() / 2));
        painter->setBrush(QBrush(Qt::lightGray));
        painter->drawRect(upArrowRect);
        painter->drawRect(downArrowRect);
        // 绘制箭头符号
        painter->setPen(QPen(Qt::black));
        painter->drawLine(rect.right() - 15, rect.top() + rect.height() / 4, rect.right() - 10, rect.top() + rect.height() / 4);
        painter->drawLine(rect.right() - 10, rect.top() + rect.height() / 4, rect.right() - 10, rect.top() + rect.height() / 2 - 5);
        painter->drawLine(rect.right() - 15, rect.bottom() - rect.height() / 4, rect.right() - 10, rect.bottom() - rect.height() / 4);
        painter->drawLine(rect.right() - 10, rect.bottom() - rect.height() / 2 + 5, rect.right() - 10, rect.bottom() - rect.height() / 4);
        // 绘制数值
        painter->drawText(rect.adjusted(5, 0, -25, 0), Qt::AlignVCenter, "123");
    } else if (m_widgetType == "QSlider") {
        painter->setBrush(QBrush(Qt::lightGray));
        painter->drawRect(rect.adjusted(0, rect.height() / 2 - 2, 0, -rect.height() / 2 + 2));
        // 绘制滑块
        painter->setBrush(QBrush(Qt::darkGray));
        painter->drawRect(QRectF(rect.width() / 2 - 5, rect.top() + 2, 10, rect.height() - 4));
    } else if (m_widgetType == "QDial") {
        painter->setBrush(QBrush(Qt::lightGray));
        painter->drawEllipse(rect.center(), rect.width() / 2, rect.height() / 2);
        // 绘制指针
        painter->setPen(QPen(Qt::black, 2));
        painter->drawLine(rect.center(), QPointF(rect.center().x() + rect.width() / 3 * cos(M_PI / 4), rect.center().y() + rect.height() / 3 * sin(M_PI / 4)));
    } else if (m_widgetType == "QProgressBar") {
        painter->setBrush(QBrush(Qt::lightGray));
        painter->drawRect(rect);
        // 绘制进度条填充
        painter->setBrush(QBrush(Qt::blue));
        painter->drawRect(rect.adjusted(0, 0, -rect.width() * 0.6, 0));
        // 绘制进度文字
        painter->setPen(QPen(Qt::white));
        painter->drawText(rect, Qt::AlignCenter, "60%");
    } else if (m_widgetType == "QCalendarWidget") {
        painter->setBrush(QBrush(Qt::white));
        painter->drawRect(rect);
        // 绘制日历标题
        painter->setPen(QPen(Qt::black, 1));
        painter->drawText(rect.adjusted(5, 5, -5, -5), Qt::AlignTop | Qt::AlignHCenter, "January 2023");
        // 绘制星期标题
        QStringList days = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
        for (int i = 0; i < 7; ++i) {
            painter->drawText(QRectF(rect.left() + 5 + i * (rect.width() - 10) / 7, rect.top() + 25, (rect.width() - 10) / 7, 20), Qt::AlignCenter, days[i]);
        }
        // 绘制日期
        for (int i = 0; i < 31; ++i) {
            int x = rect.left() + 5 + (i % 7) * (rect.width() - 10) / 7;
            int y = rect.top() + 50 + (i / 7) * 20;
            painter->drawText(QRectF(x, y, (rect.width() - 10) / 7, 20), Qt::AlignCenter, QString::number(i + 1));
        }
    } else if (m_widgetType == "QGroupBox") {
        painter->setBrush(QBrush(Qt::white));
        painter->drawRect(rect);
        // 绘制分组框标题
        painter->setPen(QPen(Qt::black));
        painter->drawText(QRectF(rect.left() + 10, rect.top() - 10, rect.width() - 20, 20), Qt::AlignCenter, "Group Box");
        // 绘制分组框内容
        painter->drawRect(rect.adjusted(20, 30, -20, -20));
    } else if (m_widgetType == "QTabWidget") {
        painter->setBrush(QBrush(Qt::lightGray));
        painter->drawRect(rect);
        // 绘制标签页
        painter->setBrush(QBrush(Qt::white));
        painter->drawRect(rect.adjusted(5, 5, -rect.width() / 2, 30));
        painter->drawRect(rect.adjusted(rect.width() / 2, 5, -5, 30));
        // 绘制标签文字
        painter->setPen(QPen(Qt::black));
        painter->drawText(rect.adjusted(15, 5, -rect.width() / 2, 30), Qt::AlignCenter, "Tab 1");
        painter->drawText(rect.adjusted(rect.width() / 2 + 10, 5, -10, 30), Qt::AlignCenter, "Tab 2");
        // 绘制标签页内容
        painter->drawRect(rect.adjusted(5, 35, -5, -5));
    } else if (m_widgetType == "QListWidget") {
        painter->setBrush(QBrush(Qt::white));
        painter->drawRect(rect);
        // 绘制列表项
        painter->setPen(QPen(Qt::black));
        for (int i = 0; i < 5; ++i) {
            painter->drawText(rect.adjusted(5, 5 + i * 20, -5, -5), Qt::AlignLeft | Qt::AlignTop, QString("Item %1").arg(i + 1));
        }
    } else if (m_widgetType == "QTreeWidget") {
        painter->setBrush(QBrush(Qt::white));
        painter->drawRect(rect);
        // 绘制树形结构
        painter->setPen(QPen(Qt::black));
        painter->drawText(rect.adjusted(5, 5, -5, -5), Qt::AlignLeft | Qt::AlignTop, "Parent Item");
        painter->drawText(rect.adjusted(25, 25, -5, -5), Qt::AlignLeft | Qt::AlignTop, "- Child Item 1");
        painter->drawText(rect.adjusted(25, 45, -5, -5), Qt::AlignLeft | Qt::AlignTop, "- Child Item 2");
    } else if (m_widgetType == "QTableWidget") {
        painter->setBrush(QBrush(Qt::white));
        painter->drawRect(rect);
        // 绘制表格标题
        painter->setPen(QPen(Qt::black, 1));
        painter->drawLine(rect.left(), rect.top() + 25, rect.right(), rect.top() + 25);
        painter->drawText(rect.adjusted(5, 5, -5, -5), Qt::AlignTop | Qt::AlignLeft, "Column 1");
        painter->drawText(rect.adjusted(rect.width() / 2, 5, -5, -5), Qt::AlignTop | Qt::AlignLeft, "Column 2");
        // 绘制表格内容
        for (int i = 0; i < 3; ++i) {
            painter->drawLine(rect.left(), rect.top() + 50 + i * 25, rect.right(), rect.top() + 50 + i * 25);
            painter->drawText(rect.adjusted(5, 30 + i * 25, -5, -5), Qt::AlignLeft | Qt::AlignTop, QString("Row %1, Col 1").arg(i + 1));
            painter->drawText(rect.adjusted(rect.width() / 2, 30 + i * 25, -5, -5), Qt::AlignLeft | Qt::AlignTop, QString("Row %1, Col 2").arg(i + 1));
        }
    } else if (m_widgetType == "QScrollBar") {
        painter->setBrush(QBrush(Qt::lightGray));
        if (rect.width() > rect.height()) {
            // 水平滚动条
            painter->drawRect(rect);
            painter->setBrush(QBrush(Qt::darkGray));
            painter->drawRect(rect.adjusted(rect.width() / 4, 2, -rect.width() * 3 / 4, -2));
        } else {
            // 垂直滚动条
            painter->drawRect(rect);
            painter->setBrush(QBrush(Qt::darkGray));
            painter->drawRect(rect.adjusted(2, rect.height() / 4, -2, -rect.height() * 3 / 4));
        }
    } else if (m_widgetType == "QDoubleSpinBox") {
        painter->setBrush(QBrush(Qt::white));
        painter->drawRect(rect);
        // 绘制上下箭头
        QRectF upArrowRect(rect.topRight() - QPointF(20, 0), QSizeF(20, rect.height() / 2));
        QRectF downArrowRect(rect.bottomRight() - QPointF(20, 0), QSizeF(20, rect.height() / 2));
        painter->setBrush(QBrush(Qt::lightGray));
        painter->drawRect(upArrowRect);
        painter->drawRect(downArrowRect);
        // 绘制箭头符号
        painter->setPen(QPen(Qt::black));
        painter->drawLine(rect.right() - 15, rect.top() + rect.height() / 4, rect.right() - 10, rect.top() + rect.height() / 4);
        painter->drawLine(rect.right() - 10, rect.top() + rect.height() / 4, rect.right() - 10, rect.top() + rect.height() / 2 - 5);
        painter->drawLine(rect.right() - 15, rect.bottom() - rect.height() / 4, rect.right() - 10, rect.bottom() - rect.height() / 4);
        painter->drawLine(rect.right() - 10, rect.bottom() - rect.height() / 2 + 5, rect.right() - 10, rect.bottom() - rect.height() / 4);
        // 绘制数值
        painter->drawText(rect.adjusted(5, 0, -25, 0), Qt::AlignVCenter, "123.45");
    } else if (m_widgetType == "QDateEdit" || m_widgetType == "QTimeEdit" || m_widgetType == "QDateTimeEdit") {
        painter->setBrush(QBrush(Qt::white));
        painter->drawRect(rect);
        // 绘制编辑框
        painter->setPen(QPen(Qt::black));
        QString format = m_widgetType == "QDateEdit" ? "2023-01-01" : (m_widgetType == "QTimeEdit" ? "12:34:56" : "2023-01-01 12:34:56");
        painter->drawText(rect.adjusted(5, 0, -25, 0), Qt::AlignVCenter, format);
        // 绘制下拉箭头
        QPointF points[3] = {
            rect.topRight() - QPointF(20, 10),
            rect.topRight() - QPointF(10, 20),
            rect.topRight() - QPointF(20, 30)
        };
        painter->setBrush(QBrush(Qt::black));
        painter->drawPolygon(points, 3);
    } else if (m_widgetType == "QFontComboBox") {
        painter->setBrush(QBrush(Qt::white));
        painter->drawRect(rect);
        // 绘制字体名称
        painter->setPen(QPen(Qt::black));
        painter->drawText(rect.adjusted(5, 0, -25, 0), Qt::AlignVCenter, "Arial");
        // 绘制下拉箭头
        QPointF points[3] = {
            rect.topRight() - QPointF(20, 10),
            rect.topRight() - QPointF(10, 20),
            rect.topRight() - QPointF(20, 30)
        };
        painter->setBrush(QBrush(Qt::black));
        painter->drawPolygon(points, 3);
    } else if (m_widgetType == "QDockWidget") {
        painter->setBrush(QBrush(Qt::white));
        painter->drawRect(rect);
        // 绘制标题栏
        painter->setBrush(QBrush(Qt::lightGray));
        painter->drawRect(rect.adjusted(0, 0, 0, 25));
        painter->setPen(QPen(Qt::black));
        painter->drawText(rect.adjusted(5, 5, -5, -5), Qt::AlignTop | Qt::AlignLeft, "Dock Widget");
        // 绘制关闭按钮
        painter->drawRect(rect.adjusted(rect.width() - 25, 5, -5, -rect.height() + 5));
        painter->drawLine(rect.right() - 20, 10, rect.right() - 10, 20);
        painter->drawLine(rect.right() - 10, 10, rect.right() - 20, 20);
    } else if (m_widgetType == "QSplitter") {
        painter->setBrush(QBrush(Qt::white));
        painter->drawRect(rect);
        // 绘制分隔条
        if (rect.width() > rect.height()) {
            // 水平分隔器
            painter->setBrush(QBrush(Qt::gray));
            painter->drawRect(rect.adjusted(rect.width() / 2 - 5, 0, -rect.width() / 2 + 5, 0));
        } else {
            // 垂直分隔器
            painter->setBrush(QBrush(Qt::gray));
            painter->drawRect(rect.adjusted(0, rect.height() / 2 - 5, 0, -rect.height() / 2 + 5));
        }
    } else if (m_widgetType == "QStackedWidget") {
        painter->setBrush(QBrush(Qt::white));
        painter->drawRect(rect);
        // 绘制页面内容
        painter->setPen(QPen(Qt::black));
        painter->drawText(rect.adjusted(5, 5, -5, -5), Qt::AlignTop | Qt::AlignLeft, "Stacked Widget Page 1");
        // 绘制页面指示器
        painter->setBrush(QBrush(Qt::gray));
        painter->drawEllipse(rect.right() - 50, rect.bottom() - 20, 10, 10);
        painter->setBrush(QBrush(Qt::black));
        painter->drawEllipse(rect.right() - 35, rect.bottom() - 20, 10, 10);
        painter->setBrush(QBrush(Qt::gray));
        painter->drawEllipse(rect.right() - 20, rect.bottom() - 20, 10, 10);
    } else if (m_widgetType == "QScrollArea") {
        painter->setBrush(QBrush(Qt::white));
        painter->drawRect(rect);
        // 绘制滚动内容
        painter->drawRect(rect.adjusted(10, 10, -40, -40));
        painter->drawText(rect.adjusted(15, 15, -5, -5), Qt::AlignTop | Qt::AlignLeft, "Scrollable Content");
        // 绘制滚动条
        painter->setBrush(QBrush(Qt::lightGray));
        painter->drawRect(rect.adjusted(rect.width() - 30, 0, 0, -30));
        painter->drawRect(rect.adjusted(0, rect.height() - 30, -30, 0));
        painter->setBrush(QBrush(Qt::darkGray));
        painter->drawRect(rect.adjusted(rect.width() - 25, 5, -5, -rect.height() + 5));
        painter->drawRect(rect.adjusted(5, rect.height() - 25, -rect.width() + 5, -5));
    } else if (m_widgetType == "QMdiArea") {
        painter->setBrush(QBrush(Qt::lightGray));
        painter->drawRect(rect);
        // 绘制子窗口
        painter->setBrush(QBrush(Qt::white));
        painter->drawRect(rect.adjusted(20, 20, -50, -50));
        painter->drawRect(rect.adjusted(50, 50, -20, -20));
        // 绘制窗口标题
        painter->setBrush(QBrush(Qt::blue));
        painter->drawRect(rect.adjusted(20, 20, -50, -rect.height() + 45));
        painter->drawRect(rect.adjusted(50, 50, -20, -rect.height() + 75));
        painter->setPen(QPen(Qt::white));
        painter->drawText(rect.adjusted(25, 25, -55, -rect.height() + 45), Qt::AlignVCenter, "Mdi Window 1");
        painter->drawText(rect.adjusted(55, 55, -25, -rect.height() + 75), Qt::AlignVCenter, "Mdi Window 2");
    } else if (m_widgetType == "QFrame") {
        painter->setBrush(QBrush(Qt::lightGray));
        painter->drawRect(rect);
        // 绘制边框
        painter->setPen(QPen(Qt::black, 2));
        painter->drawRect(rect.adjusted(5, 5, -5, -5));
    } else if (m_widgetType == "QLCDNumber") {
        painter->setBrush(QBrush(Qt::black));
        painter->drawRect(rect);
        // 绘制LCD数字
        painter->setPen(QPen(Qt::green));
        painter->drawText(rect, Qt::AlignCenter, "88:88:88");
    } else if (m_widgetType == "QToolButton") {
        painter->setBrush(QBrush(Qt::lightGray));
        painter->drawRect(rect);
        // 绘制按钮文字
        painter->setPen(QPen(Qt::black));
        painter->drawText(rect, Qt::AlignCenter, "Tool");
    } else if (m_widgetType == "QCommandLinkButton") {
        painter->setBrush(QBrush(Qt::lightGray));
        painter->drawRect(rect);
        // 绘制按钮箭头
        painter->setPen(QPen(Qt::black, 2));
        painter->drawLine(rect.left() + 10, rect.center().y(), rect.left() + 20, rect.center().y());
        painter->drawLine(rect.left() + 15, rect.center().y() - 5, rect.left() + 20, rect.center().y());
        painter->drawLine(rect.left() + 15, rect.center().y() + 5, rect.left() + 20, rect.center().y());
        // 绘制按钮文字
        painter->drawText(rect.adjusted(30, 0, 0, 0), Qt::AlignVCenter, "Command Link");
    } else {
        // 默认绘制
        painter->setBrush(QBrush(Qt::lightGray));
        painter->drawRect(rect);
        painter->setPen(QPen(Qt::black));
        painter->drawText(rect, Qt::AlignCenter, m_widgetType);
    }

    // 如果选中，绘制选中边框和缩放手柄
    if (option->state & QStyle::State_Selected) {
        // 保存当前画家状态
        painter->save();
        
        // 绘制蓝色虚线边框
        painter->setPen(QPen(Qt::blue, 2, Qt::DashLine));
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(rect.adjusted(1, 1, -1, -1));
        
        // 绘制八个方向的缩放手柄
        qreal handleSize = 8;
        QPointF handles[] = {
            rect.topLeft(),
            rect.topRight(),
            rect.bottomLeft(),
            rect.bottomRight(),
            rect.topLeft() + QPointF(rect.width() / 2, 0),
            rect.topLeft() + QPointF(0, rect.height() / 2),
            rect.bottomLeft() + QPointF(rect.width() / 2, 0),
            rect.topRight() + QPointF(0, rect.height() / 2)
        };
        
        painter->setPen(QPen(Qt::blue, 1));
        painter->setBrush(QBrush(Qt::white));
        for (const QPointF &handle : handles) {
            painter->drawRect(QRectF(handle - QPointF(handleSize / 2, handleSize / 2), QSizeF(handleSize, handleSize)));
        }
        
        // 恢复画家状态
        painter->restore();
    }
}

void LayoutItem::setSize(qreal width, qreal height)
{
    m_width = qMax(width, 20.0);
    m_height = qMax(height, 20.0);
    update();
}

void LayoutItem::setText(const QString &text)
{
    m_text = text;
    update();
}

void LayoutItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QRectF resizeHandle(boundingRect().bottomRight() - QPointF(10, 10), QSizeF(10, 10));
    if (resizeHandle.contains(event->pos())) {
        m_isResizing = true;
        event->accept();
    } else {
        m_lastMousePos = event->pos();
        m_isDragging = true;
        QGraphicsItem::mousePressEvent(event);
    }
}

void LayoutItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_isResizing) {
        QPointF newPos = event->pos();
        qreal newWidth = qMax(newPos.x(), 20.0);
        qreal newHeight = qMax(newPos.y(), 20.0);
        setSize(newWidth, newHeight);
        if (scene()) {
            scene()->update(); // 更新整个场景，解决拖影问题
        }
        event->accept();
    } else if (m_isDragging) {
        QPointF delta = event->pos() - m_lastMousePos;
        setPos(pos() + delta);
        if (scene()) {
            scene()->update(); // 更新整个场景，解决拖影问题
        }
        event->accept();
    } else {
        QGraphicsItem::mouseMoveEvent(event);
    }
}

void LayoutItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    m_isDragging = false;
    m_isResizing = false;
    QGraphicsItem::mouseReleaseEvent(event);
}

void LayoutItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    QRectF resizeHandle(boundingRect().bottomRight() - QPointF(10, 10), QSizeF(10, 10));
    if (resizeHandle.contains(event->pos())) {
        setCursor(Qt::SizeFDiagCursor);
    } else {
        setCursor(Qt::ArrowCursor);
    }
    QGraphicsItem::hoverMoveEvent(event);
}

void LayoutItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    // 只有QLabel和QLineEdit可以编辑文本
    if (m_widgetType == "QLabel" || m_widgetType == "QLineEdit") {
        // 发射自定义信号，通知主窗口需要编辑文本
        emit textDoubleClicked();
    }
    QGraphicsItem::mouseDoubleClickEvent(event);
}

UILayoutWindow::UILayoutWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::UILayoutWindow), m_scene(nullptr), m_previewWindow(nullptr)
{
    ui->setupUi(this);

    // 初始化图形场景
    m_scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(m_scene);
    qDebug() << "UILayoutWindow: 场景和视图初始化完成";
    qDebug() << "UILayoutWindow: 场景地址:" << m_scene;
    qDebug() << "UILayoutWindow: 视图地址:" << ui->graphicsView;
    qDebug() << "UILayoutWindow: 视图的场景:" << ui->graphicsView->scene();

    // 允许拖放
    setAcceptDrops(true);
    ui->graphicsView->setAcceptDrops(true);
    ui->graphicsView->viewport()->setAcceptDrops(true);  // 确保视口也接受拖放
    qDebug() << "UILayoutWindow: 拖放设置完成";

    // 设置背景
    ui->graphicsView->setBackgroundBrush(QBrush(QColor(240, 240, 240)));

    // 初始化控件库
    initWidgetLibrary();
    qDebug() << "UILayoutWindow: 控件库初始化完成，控件数量:" << ui->widgetListWidget->count();

    // 加载插件
    loadPlugins();

    // 设置QListWidget的拖放属性
    ui->widgetListWidget->setDragEnabled(true);
    // 为UILayoutWindow安装事件过滤器
    ui->graphicsView->installEventFilter(this);
    ui->graphicsView->viewport()->installEventFilter(this);
    qDebug() << "事件过滤器安装完成，目标:" << ui->graphicsView << "和" << ui->graphicsView->viewport() << "，过滤器对象:" << this;
}

UILayoutWindow::~UILayoutWindow()
{
    delete ui;
    delete m_scene;
}

// 实现拖放事件处理器
void UILayoutWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasText()) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void UILayoutWindow::dragMoveEvent(QDragMoveEvent *event)
{
    // 检查拖放位置是否在graphicsView中，并且mimeData包含文本
    QPoint viewPos = ui->graphicsView->mapFromGlobal(event->position().toPoint());
    if (event->mimeData()->hasText() && ui->graphicsView->rect().contains(viewPos)) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void UILayoutWindow::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasText()) {
        // 计算鼠标位置在graphicsView中的相对位置
        QPoint viewPos = ui->graphicsView->mapFromGlobal(event->position().toPoint());
        QPointF scenePos = ui->graphicsView->mapToScene(viewPos);
        
        // 创建一个新的LayoutItem并添加到场景
        QString widgetType = event->mimeData()->text();
        LayoutItem *layoutItem = new LayoutItem(widgetType);
        // 设置位置为鼠标位置减去控件一半大小，确保鼠标在控件中心
        layoutItem->setPos(scenePos - layoutItem->boundingRect().center());
        m_scene->addItem(layoutItem);

        // 连接文本双击信号
        connect(layoutItem, &LayoutItem::textDoubleClicked, this, &UILayoutWindow::onLayoutItemDoubleClicked);

        // 调试信息
        qDebug() << "控件类型:" << widgetType;
        qDebug() << "创建控件位置:" << scenePos;
        qDebug() << "场景中项的数量:" << m_scene->items().size();

        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}



void UILayoutWindow::on_actionSave_Layout_triggered()
{
    QString filePath = QFileDialog::getSaveFileName(this, "保存布局", "", "JSON文件 (*.json)");
    if (filePath.isEmpty()) {
        return;
    }

    QJsonArray itemsArray;
    foreach (QGraphicsItem *item, m_scene->items()) {
        LayoutItem *layoutItem = dynamic_cast<LayoutItem*>(item);
        if (layoutItem) {
            QJsonObject itemObj;
            itemObj["type"] = layoutItem->widgetType();
            itemObj["x"] = layoutItem->pos().x();
            itemObj["y"] = layoutItem->pos().y();
            itemObj["width"] = layoutItem->width();
            itemObj["height"] = layoutItem->height();
            itemsArray.append(itemObj);
        }
    }

    QJsonObject rootObj;
    rootObj["items"] = itemsArray;

    QJsonDocument doc(rootObj);
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        file.write(doc.toJson());
        file.close();
    }
}

void UILayoutWindow::on_actionLoad_Layout_triggered()
{
    QString filePath = QFileDialog::getOpenFileName(this, "加载布局", "", "JSON文件 (*.json)");
    if (filePath.isEmpty()) {
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    QJsonObject rootObj = doc.object();
    QJsonArray itemsArray = rootObj["items"].toArray();

    // 清除现有项
    m_scene->clear();

    foreach (const QJsonValue &value, itemsArray) {
        QJsonObject itemObj = value.toObject();
        QString type = itemObj["type"].toString();
        qreal x = itemObj["x"].toDouble();
        qreal y = itemObj["y"].toDouble();
        qreal width = itemObj.value("width").toDouble(100);
        qreal height = itemObj.value("height").toDouble(80);

        LayoutItem *item = new LayoutItem(type);
        item->setPos(x, y);
        item->setSize(width, height);
        m_scene->addItem(item);
    }
}

void UILayoutWindow::on_actionUndo_triggered()
{
    // TODO: 实现撤销功能
}

void UILayoutWindow::on_actionRedo_triggered()
{
    // TODO: 实现重做功能
}

void UILayoutWindow::on_actionNew_Layout_triggered()
{
    // 清除当前场景
    m_scene->clear();
    // 绘制网格
    int sceneWidth = 2000;
    int sceneHeight = 2000;
    int gridSize = 20;
    QPen gridPen(Qt::lightGray, 0.5, Qt::DotLine);
    for (int x = 0; x < sceneWidth; x += gridSize) {
        m_scene->addLine(x, 0, x, sceneHeight, gridPen);
    }
    for (int y = 0; y < sceneHeight; y += gridSize) {
        m_scene->addLine(0, y, sceneWidth, y, gridPen);
    }
}

void UILayoutWindow::onActionPreviewTriggered()
{
    // 获取所有布局项
    QList<LayoutItem*> items;
    foreach (QGraphicsItem *item, m_scene->items()) {
        LayoutItem *layoutItem = dynamic_cast<LayoutItem*>(item);
        if (layoutItem) {
            items.append(layoutItem);
        }
    }

    // 显示预览窗口
    if (!m_previewWindow) {
        m_previewWindow = new PreviewWindow(this);
    }

    m_previewWindow->setLayoutItems(items);
    m_previewWindow->show();
    m_previewWindow->raise();
    m_previewWindow->activateWindow();
}

void UILayoutWindow::initWidgetLibrary()
{
    // 添加内置控件类型 - 基本控件
    QStringList widgetTypes = {
        "QLabel",
        "QPushButton",
        "QLineEdit",
        "QTextEdit",
        "QPlainTextEdit",
        "QSpinBox",
        "QDoubleSpinBox",
        "QSlider",
        "QDial",
        "QProgressBar",
        "QScrollBar",
        "QSpinBox",
        "QDoubleSpinBox",
        "QDateTimeEdit",
        "QDateEdit",
        "QTimeEdit",
        "QCalendarWidget",
        "QCheckBox",
        "QRadioButton",
        "QGroupBox",
        "QToolButton",
        "QCommandLinkButton",
        "QCheckBox",
        "QRadioButton",
        "QComboBox",
        "QFontComboBox",
        "QListWidget",
        "QTreeWidget",
        "QTableWidget",
        "QTabWidget",
        "QDockWidget",
        "QSplitter",
        "QStackedWidget",
        "QScrollArea",
        "QMdiArea",
        "QFrame",
        "QLCDNumber"
    };

    foreach (const QString &type, widgetTypes) {
        QListWidgetItem *item = new QListWidgetItem(type, ui->widgetListWidget);
        item->setData(Qt::UserRole, type);
    }

    // 连接预览布局动作的信号和槽
    connect(ui->actionPreview, &QAction::triggered, this, &UILayoutWindow::onActionPreviewTriggered);

    // 初始化场景
    setupScene();
}

void UILayoutWindow::setupScene()
{
    // 创建场景
    m_scene = new QGraphicsScene(this);
    m_scene->setSceneRect(0, 0, 2000, 2000);
    
    // 设置背景颜色
    m_scene->setBackgroundBrush(QColor(240, 240, 240));
    
    // 绘制网格
    drawGrid();
    
    // 将场景设置到视图
    ui->graphicsView->setScene(m_scene);
    
    // 设置视图的缩放
    ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
    // ui->graphicsView->setViewportMargins(10, 10, 10, 10);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
}

void UILayoutWindow::drawGrid()
{
    // 实现网格绘制逻辑
    QPen gridPen(Qt::lightGray, 1, Qt::DotLine);
    
    // 绘制垂直线
    for (int x = 0; x <= 2000; x += 20) {
        m_scene->addLine(x, 0, x, 2000, gridPen);
    }
    
    // 绘制水平线
    for (int y = 0; y <= 2000; y += 20) {
        m_scene->addLine(0, y, 2000, y, gridPen);
    }
}

void UILayoutWindow::on_widgetListWidget_itemDoubleClicked(QListWidgetItem *item)
{
    // 当用户双击控件库中的控件时，直接在场景中央添加该控件
    if (!item) {
        return;
    }

    QString widgetType = item->text();
    LayoutItem *layoutItem = new LayoutItem(widgetType);
    
    // 将控件添加到场景中央
    QPointF centerPos = m_scene->sceneRect().center();
    layoutItem->setPos(centerPos - layoutItem->boundingRect().center());
    m_scene->addItem(layoutItem);

    // 连接文本双击信号
    connect(layoutItem, &LayoutItem::textDoubleClicked, this, &UILayoutWindow::onLayoutItemDoubleClicked);
}

// 注释掉onItemPressed函数，使用CustomListWidget的startDrag方法
// void UILayoutWindow::onItemPressed(QListWidgetItem *item)
// {
//     if (!item) {
//         qDebug() << "onItemPressed: item is nullptr";
//         return;
//     }

//     QString widgetType = item->text();
//     qDebug() << "onItemPressed: 开始拖动控件:" << widgetType;
//     
//     QMimeData *mimeData = new QMimeData;
//     mimeData->setText(widgetType);
//     qDebug() << "onItemPressed: mimeData text:" << mimeData->text();

//     QDrag *drag = new QDrag(this); // 使用UILayoutWindow作为父对象
//     drag->setMimeData(mimeData);

//     // 创建拖动时的预览
//     QPixmap pixmap(80, 60);
//     pixmap.fill(QColor(100, 200, 300, 150));  // 设置半透明背景
//     QPainter painter(&pixmap);
//     painter.setPen(Qt::black);
//     painter.drawText(pixmap.rect(), Qt::AlignCenter, widgetType);  // 绘制控件类型
//     painter.end();
//     
//     drag->setPixmap(pixmap);
//     drag->setHotSpot(pixmap.rect().center());

//     qDebug() << "onItemPressed: 执行drag->exec";
//     // 只使用CopyAction，并且不设置默认值
//     Qt::DropAction result = drag->exec(Qt::CopyAction);
//     qDebug() << "onItemPressed: 拖动结果:" << result;
// }

bool UILayoutWindow::eventFilter(QObject *obj, QEvent *event)
{
    qDebug() << "eventFilter: 进入事件过滤器，对象:" << obj << "，事件类型:" << event->type();
    bool result = QObject::eventFilter(obj, event); // 默认处理
    if (obj == ui->graphicsView || obj == ui->graphicsView->viewport()) {
        qDebug() << "eventFilter: 捕获事件类型:" << event->type();
        if (event->type() == QEvent::DragEnter) {
            QDragEnterEvent *dragEnterEvent = static_cast<QDragEnterEvent*>(event);
            qDebug() << "eventFilter: DragEnter事件，mimeData hasText:" << dragEnterEvent->mimeData()->hasText();
            dragEnterEvent->acceptProposedAction();
            result = true;
        } else if (event->type() == QEvent::DragMove) {
            QDragMoveEvent *dragMoveEvent = static_cast<QDragMoveEvent*>(event);
            qDebug() << "eventFilter: DragMove事件，mimeData hasText:" << dragMoveEvent->mimeData()->hasText();
            qDebug() << "eventFilter: DragMove事件，位置:" << dragMoveEvent->position();
            dragMoveEvent->acceptProposedAction();
            result = true;
        } else if (event->type() == QEvent::Drop) {
            QDropEvent *dropEvent = static_cast<QDropEvent*>(event);
            qDebug() << "eventFilter: Drop事件，mimeData hasText:" << dropEvent->mimeData()->hasText();
            // 计算鼠标位置在graphicsView中的相对位置
            QPoint viewPos;
            if (obj == ui->graphicsView->viewport()) {
                viewPos = dropEvent->position().toPoint();
            } else {
                viewPos = ui->graphicsView->mapFromGlobal(dropEvent->position().toPoint());
            }
            QPointF scenePos = ui->graphicsView->mapToScene(viewPos);
            
            // 创建一个新的LayoutItem并添加到场景
            QString widgetType = dropEvent->mimeData()->text();
            qDebug() << "eventFilter: 控件类型:" << widgetType;
            LayoutItem *layoutItem = new LayoutItem(widgetType);
            if (layoutItem) {
                qDebug() << "eventFilter: LayoutItem创建成功，地址:" << layoutItem;
                qDebug() << "eventFilter: LayoutItem边界矩形:" << layoutItem->boundingRect();
                // 设置位置为鼠标位置减去控件一半大小，确保鼠标在控件中心
                QPointF itemPos = scenePos - layoutItem->boundingRect().center();
                layoutItem->setPos(itemPos);
                qDebug() << "eventFilter: 设置控件位置:" << itemPos;
                m_scene->addItem(layoutItem);
                qDebug() << "eventFilter: 控件添加到场景成功";
                qDebug() << "eventFilter: 场景中项的数量:" << m_scene->items().size();
                // 强制更新场景
                m_scene->update();
                qDebug() << "eventFilter: 场景已更新";

                // 连接文本双击信号
                connect(layoutItem, &LayoutItem::textDoubleClicked, this, &UILayoutWindow::onLayoutItemDoubleClicked);
            } else {
                qDebug() << "eventFilter: LayoutItem创建失败";
            }

            dropEvent->acceptProposedAction();
            result = true;
        }
    }
    return result;
}

void UILayoutWindow::loadPlugins()
{
    // 插件加载逻辑
    // 这里可以添加插件加载代码来扩展支持的控件类型
    // 插件应该实现WidgetInterface接口，用于创建新的控件
}

void UILayoutWindow::onLayoutItemDoubleClicked()
{
    LayoutItem *item = qobject_cast<LayoutItem*>(sender());
    if (!item) {
        return;
    }
    
    // 只有QLabel和QLineEdit可以编辑文本
    if (item->widgetType() != "QLabel" && item->widgetType() != "QLineEdit") {
        return;
    }
    
    // 创建文本输入对话框
    bool ok;
    QString text = QInputDialog::getText(this, tr("编辑文本"), tr("请输入文本:"), QLineEdit::Normal, item->text(), &ok);
    
    if (ok && !text.isEmpty()) {
        // 更新布局项的文本
        item->setText(text);
    }
}