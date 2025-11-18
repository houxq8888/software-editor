#include "uilayoutwindow.h"
#include "ui_uilayoutwindow.h"
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

LayoutItem::LayoutItem(const QString &widgetType, QGraphicsItem *parent)
    : QGraphicsItem(parent), m_widgetType(widgetType), m_isDragging(false), m_isResizing(false), m_width(100), m_height(80)
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
    } else {
        // 默认绘制
        painter->setBrush(QBrush(Qt::lightGray));
        painter->drawRect(rect);
        painter->setPen(QPen(Qt::black));
        painter->drawText(rect, Qt::AlignCenter, m_widgetType);
    }

    // 如果选中，绘制选中边框和缩放手柄
    if (option->state & QStyle::State_Selected) {
        painter->setPen(QPen(Qt::blue, 2, Qt::DashLine));
        painter->drawRect(rect.adjusted(1, 1, -1, -1));
        
        // 绘制右下角缩放手柄
        QRectF resizeHandle(rect.bottomRight() - QPointF(10, 10), QSizeF(10, 10));
        painter->setPen(QPen(Qt::blue, 1));
        painter->setBrush(QBrush(Qt::blue));
        painter->drawRect(resizeHandle);
    }
}

void LayoutItem::setSize(qreal width, qreal height)
{
    m_width = qMax(width, 20.0);
    m_height = qMax(height, 20.0);
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
        event->accept();
    } else if (m_isDragging) {
        QPointF delta = event->pos() - m_lastMousePos;
        setPos(pos() + delta);
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

UILayoutWindow::UILayoutWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::UILayoutWindow), m_scene(nullptr)
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
        ui->widgetListWidget->addItem(type);
    }
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