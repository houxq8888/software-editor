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
{
    Q_UNUSED(widget)

    // 绘制控件外观
    QRectF rect = boundingRect();
    painter->setPen(QPen(Qt::black, 2));
    painter->setBrush(QBrush(Qt::lightGray));
    painter->drawRect(rect);

    // 绘制控件类型
    painter->setPen(QPen(Qt::black));
    painter->drawText(rect, Qt::AlignCenter, m_widgetType);

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

    // 允许拖放
    setAcceptDrops(true);
    ui->graphicsView->setAcceptDrops(true);

    // 初始化控件库
    initWidgetLibrary();

    // 加载插件
    loadPlugins();

    // 设置QListWidget的拖放属性
    ui->widgetListWidget->setDragEnabled(true);

    // 设置QGraphicsView的拖放属性
    ui->graphicsView->setAcceptDrops(true);

    // 连接列表项点击事件
    connect(ui->widgetListWidget, &QListWidget::itemPressed, this, &UILayoutWindow::onItemPressed);
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
    QPoint pos = event->position().toPoint();
    if (ui->graphicsView->geometry().contains(pos)) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void UILayoutWindow::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasText()) {
        // 计算相对于graphicsView的局部位置
        QPoint localPos = event->position().toPoint() - ui->graphicsView->geometry().topLeft();
        // 转换为场景坐标
        QPointF scenePos = ui->graphicsView->mapToScene(localPos);
        
        QString widgetType = event->mimeData()->text();
        LayoutItem *item = new LayoutItem(widgetType);
        // 设置位置为场景坐标中心
        item->setPos(scenePos - item->boundingRect().center());
        m_scene->addItem(item);
        
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

void UILayoutWindow::onItemPressed(QListWidgetItem *item)
{
    if (!item) {
        return;
    }

    QString widgetType = item->text();
    QMimeData *mimeData = new QMimeData;
    mimeData->setText(widgetType);

    QDrag *drag = new QDrag(ui->widgetListWidget);
    drag->setMimeData(mimeData);

    // 创建拖动时的预览
    QPixmap pixmap(80, 60);
    pixmap.fill(Qt::lightGray);
    QPainter painter(&pixmap);
    painter.setPen(Qt::black);
    painter.drawRect(0, 0, 79, 59);
    painter.drawText(pixmap.rect(), Qt::AlignCenter, widgetType);
    drag->setPixmap(pixmap);
    drag->setHotSpot(pixmap.rect().center());

    drag->exec(Qt::CopyAction | Qt::MoveAction);
}

void UILayoutWindow::loadPlugins()
{
    // 加载插件目录中的插件
    QDir pluginsDir(QCoreApplication::applicationDirPath());
#ifdef Q_OS_WIN
    pluginsDir.cd("plugins");
#else
    pluginsDir.cd("../lib/software-editor/plugins");
#endif

    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = pluginLoader.instance();
        if (plugin) {
            // TODO: 实现插件接口
            QString pluginName = plugin->property("name").toString();
            if (!pluginName.isEmpty()) {
                ui->widgetListWidget->addItem(pluginName);
            }
        }
    }
}