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
    : QGraphicsItem(parent), m_widgetType(widgetType), m_isDragging(false)
{
    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);
    setFlag(ItemSendsGeometryChanges);
}

QRectF LayoutItem::boundingRect() const
{
    return QRectF(0, 0, 100, 80);
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

    // 如果选中，绘制选中边框
    if (option->state & QStyle::State_Selected) {
        painter->setPen(QPen(Qt::blue, 2, Qt::DashLine));
        painter->drawRect(rect.adjusted(1, 1, -1, -1));
    }
}

void LayoutItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    m_lastMousePos = event->pos();
    m_isDragging = true;
    QGraphicsItem::mousePressEvent(event);
}

void LayoutItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_isDragging) {
        QPointF delta = event->pos() - m_lastMousePos;
        setPos(pos() + delta);
    }
    QGraphicsItem::mouseMoveEvent(event);
}

void LayoutItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    m_isDragging = false;
    QGraphicsItem::mouseReleaseEvent(event);
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
}

UILayoutWindow::~UILayoutWindow()
{
    delete ui;
    delete m_scene;
}

void UILayoutWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasText()) {
        event->acceptProposedAction();
    }
}

void UILayoutWindow::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasText()) {
        event->acceptProposedAction();
    }
}

void UILayoutWindow::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasText()) {
        QString widgetType = event->mimeData()->text();

        // 将窗口坐标转换为场景坐标
        QPointF scenePos = ui->graphicsView->mapToScene(event->pos());

        // 创建新的布局项
        LayoutItem *item = new LayoutItem(widgetType);
        item->setPos(scenePos - item->boundingRect().center());
        m_scene->addItem(item);

        event->acceptProposedAction();
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

        LayoutItem *item = new LayoutItem(type);
        item->setPos(x, y);
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
    // 添加内置控件类型
    QStringList widgetTypes = {
        "QLabel",
        "QPushButton",
        "QLineEdit",
        "QCheckBox",
        "QRadioButton",
        "QComboBox",
        "QSlider",
        "QSpinBox",
        "QTextEdit",
        "QGroupBox"
    };

    foreach (const QString &type, widgetTypes) {
        ui->widgetListWidget->addItem(type);
    }
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