#include "uilayoutwindow.h"
#include "ui_uilayoutwindow.h"
#include <QInputDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QPainter>
#include <QRadioButton>
#include <QTableWidget>
#include <QGroupBox>
#include <QCalendarWidget>
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

LayoutItem::LayoutItem(const QString &widgetType, const QString &text)
    : m_widgetType(widgetType), m_text(text) {}
QString LayoutItem::widgetType() const { return m_widgetType; }

QString LayoutItem::text() const { return m_text; }

void LayoutItem::setText(const QString &newText) { m_text = newText; }

QWidget *LayoutItem::createWidget(QWidget *parent) const {
    QWidget *widget = nullptr;
    if (m_widgetType == "QPushButton") {
        widget = new QPushButton(m_text, parent);
    } else if (m_widgetType == "QLabel") {
        widget = new QLabel(m_text, parent);
    } else if (m_widgetType == "QLineEdit") {
        widget = new QLineEdit(m_text, parent);
    } else if (m_widgetType == "QCheckBox") {
        widget = new QCheckBox("Check Box", parent);
    } else if (m_widgetType == "QRadioButton") {
        widget = new QRadioButton("Radio Button", parent);
    } else if (m_widgetType == "QTextEdit") {
        QTextEdit *textEdit = new QTextEdit(parent);
        textEdit->setText("Text Edit");
        widget = textEdit;
    } else if (m_widgetType == "QComboBox") {
        QComboBox *comboBox = new QComboBox(parent);
        comboBox->addItem("Option 1");
        comboBox->addItem("Option 2");
        comboBox->addItem("Option 3");
        widget = comboBox;
    } else if (m_widgetType == "QSpinBox") {
        widget = new QSpinBox(parent);
    } else if (m_widgetType == "QSlider") {
        widget = new QSlider(Qt::Horizontal, parent);
    } else if (m_widgetType == "QProgressBar") {
        QProgressBar *progressBar = new QProgressBar(parent);
        progressBar->setValue(50);
        widget = progressBar;
    } else if (m_widgetType == "QCalendarWidget") {
        widget = new QCalendarWidget(parent);
    } else if (m_widgetType == "QGroupBox") {
        widget = new QGroupBox("Group Box", parent);
    } else if (m_widgetType == "QTabWidget") {
        QTabWidget *tabWidget = new QTabWidget(parent);
        widget = tabWidget;
    } else if (m_widgetType == "QListWidget") {
        QListWidget *listWidget = new QListWidget(parent);
        listWidget->addItem("Item 1");
        listWidget->addItem("Item 2");
        listWidget->addItem("Item 3");
        widget = listWidget;
    } else if (m_widgetType == "QTableWidget") {
        widget = new QTableWidget(3, 3, parent);
    } else {
        // 默认返回一个QWidget
        QWidget *widget = new QWidget(parent);
        widget->setStyleSheet("background-color: lightgray; border: 1px solid gray;");
        return widget;
    }
    // 其他控件类型的创建...
    return widget;
}


UILayoutWindow::UILayoutWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::UILayoutWindow), m_previewWindow(nullptr), m_editAreaWidget(nullptr)
{
    ui->setupUi(this);

    // 初始化splitter
    QList<int> sizes;
    sizes << 200 << 800; // 设置初始大小比例
    ui->splitter->setSizes(sizes);
    ui->splitter->setStretchFactor(0, 0); // 左侧控件库不可拉伸
    ui->splitter->setStretchFactor(1, 1); // 右侧编辑区可拉伸
    ui->splitter->setHandleWidth(8); // 设置分隔条宽度
    ui->splitter->setChildrenCollapsible(false); // 禁止子控件折叠

    // 初始化编辑区域
    setupEditArea();
    qDebug() << "UILayoutWindow: 编辑区域初始化完成";

    // 允许拖放
    setAcceptDrops(true);
    m_editAreaWidget->setAcceptDrops(true);
    qDebug() << "UILayoutWindow: 拖放设置完成";

    // 初始化控件库
    initWidgetLibrary();
    qDebug() << "UILayoutWindow: 控件库初始化完成，控件数量:" << ui->widgetListWidget->count();

    // 加载插件
    loadPlugins();
}

UILayoutWindow::~UILayoutWindow()
{
    delete ui;
    delete m_editAreaWidget;
}

// 实现拖放事件处理器
void UILayoutWindow::dragEnterEvent(QDragEnterEvent *event)
{
    qDebug() << "拖放进入，MIME 数据是否有效：" << event->mimeData()->hasText();
    if (event->mimeData()->hasText()) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void UILayoutWindow::dragMoveEvent(QDragMoveEvent *event)
{
    // 检查拖放位置是否在编辑区widget中，并且mimeData包含文本
    QPoint localPos = event->position().toPoint();
    // Convert to global position relative to the edit area widget
    QRect editAreaGeometry = m_editAreaWidget->geometry();
    if (localPos.x() >= editAreaGeometry.left() && localPos.x() <= editAreaGeometry.right() &&
        localPos.y() >= editAreaGeometry.top() && localPos.y() <= editAreaGeometry.bottom()) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void UILayoutWindow::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasText()) {
        // 获取控件类型
        QString widgetType = event->mimeData()->text();
        qDebug() << "dropEvent: 拖放的控件类型:" << widgetType;

        // 根据控件类型创建对应的 LayoutItem
        LayoutItem *layoutItem = new LayoutItem(widgetType,widgetType);

        // 创建相应的控件
        QWidget *widget = layoutItem->createWidget(m_editAreaWidget);  // 将控件添加到编辑区 QWidget 中

        // 如果没有设置布局，手动设置布局
        if (!m_editAreaWidget->layout()) {
            qDebug() << "No layout set for m_previewWidget. Setting QVBoxLayout.";
            QVBoxLayout *layout = new QVBoxLayout(m_editAreaWidget);
            m_editAreaWidget->setLayout(layout);
        }
        widget->show();  // 显示控件
        m_editAreaWidget->layout()->addWidget(widget);  // 将控件添加到布局中
        
        // 为可编辑的控件安装事件过滤器
        widget->installEventFilter(this);
        // 保存 LayoutItem
        m_layoutItems.append(layoutItem);

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

}

void UILayoutWindow::onActionPreviewTriggered()
{
    // 显示预览窗口
    if (!m_previewWindow) {
        m_previewWindow = new PreviewWindow(this);
    }

    m_previewWindow->setLayoutItems(m_layoutItems);
    m_previewWindow->show();
    m_previewWindow->raise();
    m_previewWindow->activateWindow();
}

void UILayoutWindow::setupEditArea()
{
    // 创建编辑区域widget
    m_editAreaWidget = new QWidget(this);
    m_editAreaWidget->setObjectName("editAreaWidget");
    m_editAreaWidget->setLayout(nullptr); // 移除布局，允许手动定位

    // 替换splitter中的QGraphicsView为我们的编辑区widget
    int index = ui->splitter->indexOf(ui->graphicsView);
    if (index != -1) {
        ui->splitter->replaceWidget(index, m_editAreaWidget);
        delete ui->graphicsView;
        ui->graphicsView = nullptr;
    }
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
}


bool UILayoutWindow::eventFilter(QObject *obj, QEvent *event)
{
    // 处理控件的双击事件
    if (event->type() == QEvent::MouseButtonDblClick) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            // 处理QLabel文本编辑
            QLabel *label = qobject_cast<QLabel*>(obj);
            if (label) {
                // 创建一个临时的QLineEdit来编辑文本
                QLineEdit *edit = new QLineEdit(label);
                edit->setText(label->text());
                edit->setGeometry(label->rect());
                edit->selectAll();
                edit->setFocus();
                
                // 连接信号槽，在编辑完成后更新QLabel的文本
                connect(edit, &QLineEdit::editingFinished, [=]() {
                    label->setText(edit->text());
                    delete edit;
                });
                
                return true;
            }
            
            // 处理QTabWidget的标签页标题编辑
            QTabWidget *tabWidget = qobject_cast<QTabWidget*>(obj);
            if (tabWidget) {
                // 获取双击位置下的标签页索引
                int tabIndex = tabWidget->tabBar()->tabAt(mouseEvent->position().toPoint());
                if (tabIndex != -1) {
                    // 创建一个临时的QLineEdit来编辑标签页标题
                    QLineEdit *edit = new QLineEdit(tabWidget);
                    edit->setText(tabWidget->tabText(tabIndex));
                    edit->setGeometry(tabWidget->tabBar()->tabRect(tabIndex));
                    edit->selectAll();
                    edit->setFocus();
                    
                    // 连接信号槽，在编辑完成后更新标签页标题
                    connect(edit, &QLineEdit::editingFinished, [=]() {
                        tabWidget->setTabText(tabIndex, edit->text());
                        delete edit;
                    });
                    
                    return true;
                }
            }
        }
    }
    
    // 默认处理事件
    return QObject::eventFilter(obj, event);
}

void UILayoutWindow::loadPlugins()
{
    // 插件加载逻辑
    // 这里可以添加插件加载代码来扩展支持的控件类型
    // 插件应该实现WidgetInterface接口，用于创建新的控件
}

void UILayoutWindow::onLayoutItemDoubleClicked()
{
}
