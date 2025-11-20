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
    : m_widgetType(widgetType), m_text(text), m_pos(0, 0), m_size(200, 50), m_tabIndex(-1) {}
QString LayoutItem::widgetType() const { return m_widgetType; }

QString LayoutItem::text() const { return m_text; }

void LayoutItem::setText(const QString &newText) { m_text = newText; }

QPoint LayoutItem::pos() const { return m_pos; }

void LayoutItem::setPos(const QPoint &pos) { m_pos = pos; }

QSize LayoutItem::size() const { return m_size; }

void LayoutItem::setSize(const QSize &size) { m_size = size; }

// 设置控件所属的Tab页索引
void LayoutItem::setTabIndex(int index) {
    m_tabIndex = index;
}

// 获取控件所属的Tab页索引
int LayoutItem::tabIndex() const {
    return m_tabIndex;
}

QWidget *LayoutItem::createWidget(QWidget *parent) const {
    QWidget *widget = nullptr;
    if (m_widgetType == "QPushButton") {
        widget = new QPushButton(m_text, parent);
    } else if (m_widgetType == "QLabel") {
        widget = new QLabel(m_text, parent);
    } else if (m_widgetType == "QLineEdit") {
        widget = new QLineEdit(m_text, parent);
        widget->setStyleSheet("background-color: white;");
    } else if (m_widgetType == "QCheckBox") {
        widget = new QCheckBox(m_text, parent);
    } else if (m_widgetType == "QRadioButton") {
        widget = new QRadioButton(m_text, parent);
    } else if (m_widgetType == "QTextEdit") {
        QTextEdit *textEdit = new QTextEdit(parent);
        textEdit->setText(m_text);
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
        widget = new QGroupBox(m_text, parent);
    } else if (m_widgetType == "QTabWidget") {
        QTabWidget *tabWidget = new QTabWidget(parent);
        // 添加默认标签页
        tabWidget->addTab(new QWidget(), "Tab 1");
        tabWidget->addTab(new QWidget(), "Tab 2");
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
        widget = new QWidget(parent);
        widget->setStyleSheet("background-color: lightgray; border: 1px solid gray;");
    }
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
    ui->splitter->setStretchFactor(0, 1); // 左侧控件库可拉伸
    ui->splitter->setStretchFactor(1, 1); // 右侧编辑区可拉伸
    ui->splitter->setHandleWidth(8); // 设置分隔条宽度
    ui->splitter->setChildrenCollapsible(false); // 禁止子控件折叠

    // 初始化编辑区域
    setupEditArea();
    qDebug() << "UILayoutWindow: 编辑区域初始化完成";

    // 连接EditAreaWidget的信号
    connect(m_editAreaWidget, &EditAreaWidget::handleDragged, this, &UILayoutWindow::onHandleDragged);
    connect(m_editAreaWidget, &EditAreaWidget::handleReleased, this, &UILayoutWindow::onHandleReleased);
    connect(m_editAreaWidget, &EditAreaWidget::handlesShouldHide, this, [=]() {
        // 隐藏控制点
        m_editAreaWidget->setHandles(QList<QRect>());
    });

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

// 更新控制点位置
void UILayoutWindow::updateHandles(QWidget *widget) {
    if (!widget || !m_editAreaWidget) {
        return;
    }
    
    // 检查控件是否在当前显示的Tab页中
    QTabWidget *tabWidget = nullptr;
    QWidget *tabPage = nullptr;
    
    // 递归查找父控件中的QTabWidget
    QWidget *tempParent = widget;
    while (tempParent) {
        tabWidget = qobject_cast<QTabWidget*>(tempParent);
        if (tabWidget) {
            // 查找widget是否在QTabWidget的某个Tab页中
            QWidget *widgetPage = widget;
            while (widgetPage) {
                if (tabWidget->indexOf(widgetPage) != -1) {
                    tabPage = widgetPage;
                    break;
                }
                widgetPage = widgetPage->parentWidget();
            }
            break;
        }
        tempParent = tempParent->parentWidget();
    }
    
    if (tabWidget && tabPage) {
        // 获取当前Tab页
        int currentIndex = tabWidget->currentIndex();
        // 获取控件所在的Tab页
        int widgetTabIndex = tabWidget->indexOf(tabPage);
        // 如果不在当前Tab页，不显示控制点
        if (widgetTabIndex != -1 && widgetTabIndex != currentIndex) {
            m_editAreaWidget->setHandles(QList<QRect>());
            return;
        }
    }
    
    // 获取控件在编辑区中的本地位置
    QPoint widgetLocalPos = widget->mapTo(m_editAreaWidget, QPoint(0, 0));  // 将控件坐标映射到编辑区坐标系
    QRect widgetRect(widgetLocalPos, widget->size());
    
    // 计算所有控制点
    QList<QRect> handles;
    
    // 顶部左
    handles.append(QRect(widgetRect.topLeft() - QPoint(HANDLE_SIZE/2, HANDLE_SIZE/2), QSize(HANDLE_SIZE, HANDLE_SIZE)));
    // 顶部中
    handles.append(QRect(QPoint(widgetRect.left() + widgetRect.width()/2 - HANDLE_SIZE/2, widgetRect.top() - HANDLE_SIZE/2), QSize(HANDLE_SIZE, HANDLE_SIZE)));
    // 顶部右
    handles.append(QRect(QPoint(widgetRect.right() - HANDLE_SIZE/2, widgetRect.top() - HANDLE_SIZE/2), QSize(HANDLE_SIZE, HANDLE_SIZE)));
    // 中间左
    handles.append(QRect(QPoint(widgetRect.left() - HANDLE_SIZE/2, widgetRect.top() + widgetRect.height()/2 - HANDLE_SIZE/2), QSize(HANDLE_SIZE, HANDLE_SIZE)));
    // 中间右
    handles.append(QRect(QPoint(widgetRect.right() - HANDLE_SIZE/2, widgetRect.top() + widgetRect.height()/2 - HANDLE_SIZE/2), QSize(HANDLE_SIZE, HANDLE_SIZE)));
    // 底部左
    handles.append(QRect(QPoint(widgetRect.left() - HANDLE_SIZE/2, widgetRect.bottom() - HANDLE_SIZE/2), QSize(HANDLE_SIZE, HANDLE_SIZE)));
    // 底部中
    handles.append(QRect(QPoint(widgetRect.left() + widgetRect.width()/2 - HANDLE_SIZE/2, widgetRect.bottom() - HANDLE_SIZE/2), QSize(HANDLE_SIZE, HANDLE_SIZE)));
    // 底部右
    handles.append(QRect(widgetRect.bottomRight() - QPoint(HANDLE_SIZE/2, HANDLE_SIZE/2), QSize(HANDLE_SIZE, HANDLE_SIZE)));
    
    // 将控制点传递给EditAreaWidget
    m_editAreaWidget->setHandles(handles);
}

// 绘制控制点 - 已转移到EditAreaWidget
// 重写paintEvent来更新EditAreaWidget的绘制
void UILayoutWindow::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);  // 确保不遗漏基类实现
    // 只有当选中的控件可见时才更新控制点
    if (m_selectedWidget && m_selectedWidget->isVisible() && m_editAreaWidget) {
        // 检查控件是否在当前显示的Tab页中
        QTabWidget *tabWidget = nullptr;
        QWidget *tabPage = nullptr;
        
        // 递归查找父控件中的QTabWidget
        QWidget *tempParent = m_selectedWidget;
        while (tempParent) {
            tabWidget = qobject_cast<QTabWidget*>(tempParent);
            if (tabWidget) {
                // 查找widget是否在QTabWidget的某个Tab页中
                QWidget *widgetPage = m_selectedWidget;
                while (widgetPage) {
                    if (tabWidget->indexOf(widgetPage) != -1) {
                        tabPage = widgetPage;
                        break;
                    }
                    widgetPage = widgetPage->parentWidget();
                }
                break;
            }
            tempParent = tempParent->parentWidget();
        }
        
        if (tabWidget && tabPage) {
            // 获取当前Tab页
            int currentIndex = tabWidget->currentIndex();
            // 获取控件所在的Tab页
            int widgetTabIndex = tabWidget->indexOf(tabPage);
            // 如果不在当前Tab页，不显示控制点
            if (widgetTabIndex != -1 && widgetTabIndex != currentIndex) {
                return;
            }
        }
        // 通知EditAreaWidget更新绘制
        m_editAreaWidget->update();
    }
}




void UILayoutWindow::dragMoveEvent(QDragMoveEvent *event)
{
    // 检查拖放位置是否在编辑区widget中，并且mimeData包含文本
    QPoint localPos = event->position().toPoint();
    QPoint globalPos = mapToGlobal(localPos);
    QPoint editAreaLocalPos = m_editAreaWidget->mapFromGlobal(globalPos);
    
    if (event->mimeData()->hasText() && m_editAreaWidget->rect().contains(editAreaLocalPos)) {
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

        // 获取鼠标在编辑区中的局部位置
        QPoint localPos = event->position().toPoint();
        QPoint globalPos = mapToGlobal(localPos);
        QPoint editAreaLocalPos = m_editAreaWidget->mapFromGlobal(globalPos);

        // 根据控件类型创建对应的 LayoutItem
        LayoutItem *layoutItem = new LayoutItem(widgetType, widgetType);
        layoutItem->setPos(editAreaLocalPos);
        layoutItem->setSize(QSize(200, 50));

        // 创建相应的控件
        // 检查鼠标位置是否在某个 Tab 页上
        QWidget *targetWidget = m_editAreaWidget; // 默认添加到编辑区
        QPoint editAreaGlobalPos = m_editAreaWidget->mapToGlobal(editAreaLocalPos);
        for (auto it = m_widgetItemMap.begin(); it != m_widgetItemMap.end(); ++it) {
            QWidget *w = it.key();
            if (QTabWidget *tabWidget = qobject_cast<QTabWidget*>(w)) {
                // 检查鼠标是否在 TabWidget 上
                if (tabWidget->geometry().contains(m_editAreaWidget->mapFromGlobal(editAreaGlobalPos))) {
                    // 获取当前选中的 Tab 页
                    QWidget *currentTabPage = tabWidget->currentWidget();
                    if (currentTabPage) {
                        // 计算鼠标在当前 Tab 页中的位置
                        QPoint tabPageLocalPos = currentTabPage->mapFromGlobal(editAreaGlobalPos);
                        // 将控件添加到当前 Tab 页
                        targetWidget = currentTabPage;
                        // 更新控件位置到 Tab 页坐标
                        editAreaLocalPos = m_editAreaWidget->mapFromGlobal(currentTabPage->mapToGlobal(tabPageLocalPos));
                        // 设置控件所属的 Tab 页索引
                        int tabIndex = tabWidget->currentIndex();
                        layoutItem->setTabIndex(tabIndex);
                        qDebug() << "控件拖放到 Tab 页" << tabIndex + 1;
                    }
                }
            }
        }
        QWidget *widget = layoutItem->createWidget(targetWidget); // 将控件添加到目标 Tab 页中

        // 设置控件位置和大小
        widget->move(layoutItem->pos());
        widget->resize(layoutItem->size());
        widget->show();

        // 为可编辑的控件安装事件过滤器
        widget->installEventFilter(this);
        // 将控件添加到编辑区
        addWidgetToEditArea(layoutItem, targetWidget);

        m_editAreaWidget->repaint();
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void UILayoutWindow::addWidgetToEditArea(LayoutItem *item, QWidget *targetWidget)
{
    if (!item || !targetWidget) {
        return;
    }

    // 创建相应的控件
    QWidget *widget = item->createWidget(targetWidget);
    if (!widget) {
        return;
    }

    // 设置控件位置和大小
    widget->move(item->pos());
    widget->resize(item->size());
    widget->show();

    // 为可编辑的控件安装事件过滤器
    widget->installEventFilter(this);

    // 保存 LayoutItem
    m_layoutItems.append(item);
    m_widgetItemMap[widget] = item;

    // 将最新添加的控件设为选中状态并显示控制点
    m_selectedWidget = widget;
    updateHandles(m_selectedWidget);
}




void UILayoutWindow::on_actionPreview_triggered()
{
    // 创建预览窗口
    if (!m_previewWindow) {
        m_previewWindow = new PreviewWindow(this);
    }
    
    // 设置布局项
    m_previewWindow->setLayoutItems(m_layoutItems);
    
    // 显示预览窗口
    m_previewWindow->show();
}

void UILayoutWindow::on_actionSave_Layout_triggered()
{
    QString filePath = QFileDialog::getSaveFileName(this, "保存布局", "", "JSON文件 (*.json)");
    if (filePath.isEmpty()) {
        return;
    }

    QJsonArray itemsArray;
    const QList<LayoutItem*> layoutItems = getLayoutItems();
    for (LayoutItem* item : layoutItems) {
        QJsonObject itemObj;
        itemObj["widgetType"] = item->widgetType();
        itemObj["text"] = item->text();
        itemObj["x"] = item->pos().x();
        itemObj["y"] = item->pos().y();
        itemObj["width"] = item->size().width();
        itemObj["height"] = item->size().height();
        itemObj["tabIndex"] = item->tabIndex();
        itemsArray.append(itemObj);
    }

    QJsonObject rootObj;
    rootObj["items"] = itemsArray;

    QJsonDocument doc(rootObj);
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        file.write(doc.toJson());
        file.close();
        qDebug() << "Layout saved to" << filePath;
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
    for (QWidget *widget : m_widgetItemMap.keys()) {
        delete widget;
    }
    m_layoutItems.clear();
    m_widgetItemMap.clear();

    // 加载新项
    for (const QJsonValue &value : itemsArray) {
        QJsonObject itemObj = value.toObject();
        QString widgetType = itemObj["widgetType"].toString();
        QString text = itemObj["text"].toString();
        int x = itemObj["x"].toInt();
        int y = itemObj["y"].toInt();
        int width = itemObj["width"].toInt();
        int height = itemObj["height"].toInt();
        int tabIndex = itemObj["tabIndex"].toInt();

        // 创建 LayoutItem
        LayoutItem *item = new LayoutItem(widgetType, text);
        item->setPos(QPoint(x, y));
        item->setSize(QSize(width, height));
        item->setTabIndex(tabIndex);

        // 确定控件的目标父窗口
        QWidget *targetWidget = m_editAreaWidget;
        if (tabIndex >= 0) {
            // 找到TabWidget并添加到相应的Tab页
            for (auto it = m_widgetItemMap.begin(); it != m_widgetItemMap.end(); ++it) {
                QWidget *w = it.key();
                if (QTabWidget *tabWidget = qobject_cast<QTabWidget*>(w)) {
                    // 确保Tab页存在
                    while (tabWidget->count() <= tabIndex) {
                        tabWidget->addTab(new QWidget(), QString("Tab页 %1").arg(tabWidget->count() + 1));
                    }
                    targetWidget = tabWidget->widget(tabIndex);
                    break;
                }
            }
        }

        // 将控件添加到编辑区
        addWidgetToEditArea(item, targetWidget);
    }
    qDebug() << "Layout loaded from" << filePath;
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
    // 创建编辑区的 QWidget
    m_editAreaWidget = new EditAreaWidget(this);
    m_editAreaWidget->setObjectName("editAreaWidget");
    m_editAreaWidget->setLayout(nullptr); // 移除布局，允许手动定位
    m_editAreaWidget->setStyleSheet("background-color: #f0f0f0; border: 1px solid #ccc;");

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
        "QDateTimeEdit",
        "QDateEdit",
        "QTimeEdit",
        "QCalendarWidget",
        "QCheckBox",
        "QRadioButton",
        "QGroupBox",
        "QToolButton",
        "QCommandLinkButton",
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
    // 处理编辑区空白处点击事件
    if (event->type() == QEvent::MouseButtonPress) {
        EditAreaWidget *editArea = qobject_cast<EditAreaWidget*>(obj);
        if (editArea) {
            // 点击编辑区空白处，让所有正在编辑的QLineEdit失去焦点以完成编辑
            QList<QLineEdit*> lineEdits = editArea->findChildren<QLineEdit*>();
            foreach (QLineEdit *edit, lineEdits) {
                if (edit && edit->focusPolicy() == Qt::StrongFocus && edit->isVisible()) {
                    edit->clearFocus();
                }
            }
        }
    }
    // 处理控件的双击事件
    if (event->type() == QEvent::MouseButtonDblClick) {
        qDebug() << "双击事件触发，对象类型:" << obj->metaObject()->className();
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            // 记录鼠标位置
            m_lastMousePos = mouseEvent->globalPos();
            // 处理QLabel文本编辑
            QLabel *label = qobject_cast<QLabel*>(obj);
            if (label) {
                qDebug() << "处理QLabel双击事件:" << label << "当前文本:" << label->text();
                // 创建一个临时的QLineEdit来编辑文本
                QLineEdit *edit = new QLineEdit(label);
                edit->setText(label->text());
                edit->setGeometry(label->rect());
                edit->selectAll();
                edit->setFocusPolicy(Qt::StrongFocus);
                edit->setStyleSheet("border: 2px solid blue; background-color: white;");
                edit->raise();
                edit->show();
                
                // 确保获得焦点
                edit->activateWindow();
                edit->setFocus();
                
                qDebug() << "创建编辑框:" << edit << "位置:" << edit->geometry() << "焦点:" << edit->hasFocus() << "可见:" << edit->isVisible();
                
                // 连接信号槽，在编辑完成后更新QLabel的文本
                connect(edit, &QLineEdit::editingFinished, [=]() {
                    if (label) {
                        label->setText(edit->text());
                        qDebug() << "编辑完成，更新QLabel文本为:" << edit->text();
                        // 更新对应的LayoutItem的文本属性
                        LayoutItem *item = m_widgetItemMap.value(label);
                        if (item) {
                            item->setText(edit->text());
                            qDebug() << "更新LayoutItem文本为:" << edit->text();
                        }
                        edit->deleteLater();
                    }
                });
                
                return true;
            }
            
            // 处理QPushButton文本编辑
            QPushButton *pushButton = qobject_cast<QPushButton*>(obj);
            if (pushButton) {
                // 创建一个临时的QLineEdit来编辑文本
                QLineEdit *edit = new QLineEdit(pushButton);
                edit->setText(pushButton->text());
                edit->setGeometry(pushButton->rect().adjusted(4, 4, -4, -4));
                edit->selectAll();
                edit->setFocusPolicy(Qt::StrongFocus);
                edit->setStyleSheet("border: 2px solid blue; background-color: white;");
                edit->raise();
                edit->show();
                edit->activateWindow();
                edit->setFocus();
                
                // 连接信号槽，在编辑完成后更新QPushButton的文本
                connect(edit, &QLineEdit::editingFinished, [=]() {
                    if (pushButton) {
                        pushButton->setText(edit->text());
                        // 更新对应的LayoutItem的文本属性
                        LayoutItem *item = m_widgetItemMap.value(pushButton);
                        if (item) {
                            item->setText(edit->text());
                            qDebug() << "更新LayoutItem文本为:" << edit->text();
                        }
                        edit->deleteLater();
                    }
                });
                
                return true;
            }
            
            // 处理QGroupBox标题编辑
            QGroupBox *groupBox = qobject_cast<QGroupBox*>(obj);
            if (groupBox) {
                // 创建一个临时的QLineEdit来编辑标题
                QLineEdit *edit = new QLineEdit(groupBox);
                edit->setText(groupBox->title());
                edit->setGeometry(8, 0, groupBox->width() - 16, 22);
                edit->selectAll();
                edit->setFocusPolicy(Qt::StrongFocus);
                edit->setStyleSheet("border: 2px solid blue; background-color: white;");
                edit->raise();
                edit->show();
                edit->activateWindow();
                edit->setFocus();
                
                // 连接信号槽，在编辑完成后更新QGroupBox的标题
                connect(edit, &QLineEdit::editingFinished, [=]() {
                    groupBox->setTitle(edit->text());
                    delete edit;
                });
                
                return true;
            }
            
            // 处理QRadioButton文本编辑
            QRadioButton *radioButton = qobject_cast<QRadioButton*>(obj);
            if (radioButton) {
                qDebug() << "Double clicked QRadioButton:" << radioButton;
                // 创建一个临时的QLineEdit来编辑文本
                QLineEdit *edit = new QLineEdit(radioButton);
                edit->setText(radioButton->text());
                edit->setGeometry(radioButton->rect().adjusted(4, 4, -4, -4));
                edit->selectAll();
                edit->setFocusPolicy(Qt::StrongFocus);
                edit->setStyleSheet("border: 2px solid blue; background-color: white;");
                edit->raise();
                edit->show();
                edit->activateWindow();
                edit->setFocus();
                
                // 连接信号槽，在编辑完成后更新QRadioButton的文本
                connect(edit, &QLineEdit::editingFinished, [=]() {
                    radioButton->setText(edit->text());
                    delete edit;
                });
                
                return true;
            }
            
            // 处理QCheckBox文本编辑
            QCheckBox *checkBox = qobject_cast<QCheckBox*>(obj);
            if (checkBox) {
                qDebug() << "Double clicked QCheckBox:" << checkBox;
                // 创建一个临时的QLineEdit来编辑文本
                QLineEdit *edit = new QLineEdit(checkBox);
                edit->setText(checkBox->text());
                edit->setGeometry(checkBox->rect().adjusted(4, 4, -4, -4));
                edit->selectAll();
                edit->setFocusPolicy(Qt::StrongFocus);
                edit->setStyleSheet("border: 2px solid blue; background-color: white;");
                edit->raise();
                edit->show();
                edit->activateWindow();
                edit->setFocus();
                
                // 连接信号槽，在编辑完成后更新QCheckBox的文本
                connect(edit, &QLineEdit::editingFinished, [=]() {
                    checkBox->setText(edit->text());
                    delete edit;
                });
                
                return true;
            }
            
            // 处理QToolButton文本编辑
            QToolButton *toolButton = qobject_cast<QToolButton*>(obj);
            if (toolButton) {
                // 创建一个临时的QLineEdit来编辑文本
                QLineEdit *edit = new QLineEdit(toolButton);
                edit->setText(toolButton->text());
                edit->setGeometry(toolButton->rect().adjusted(4, 4, -4, -4));
                edit->selectAll();
                edit->setFocus();
                
                // 连接信号槽，在编辑完成后更新QToolButton的文本
                connect(edit, &QLineEdit::editingFinished, [=]() {
                    toolButton->setText(edit->text());
                    delete edit;
                });
                
                return true;
            }
            
            // 处理QCommandLinkButton文本编辑
            QCommandLinkButton *commandLinkButton = qobject_cast<QCommandLinkButton*>(obj);
            if (commandLinkButton) {
                // 创建一个临时的QLineEdit来编辑文本
                QLineEdit *edit = new QLineEdit(commandLinkButton);
                edit->setText(commandLinkButton->text());
                edit->setGeometry(commandLinkButton->rect().adjusted(8, 8, -8, -8));
                edit->selectAll();
                edit->setFocus();
                
                // 连接信号槽，在编辑完成后更新QCommandLinkButton的文本
                connect(edit, &QLineEdit::editingFinished, [=]() {
                    commandLinkButton->setText(edit->text());
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
                    
                    // 连接信号槽，在编辑完成后更新QTabWidget的标签文本
                    connect(edit, &QLineEdit::editingFinished, [=]() {
                        if (tabWidget) {
                            tabWidget->setTabText(tabIndex, edit->text());
                            edit->deleteLater();
                        }
                    });
                    
                    return true;
                }
            }
        }
    }
    
    // 处理鼠标按下事件
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            QWidget *widget = qobject_cast<QWidget*>(obj);
            if (widget) {
                // 检查是否在调整大小的边界
                QRect widgetRect = widget->rect();
                QPoint mousePos = mouseEvent->pos();
                
                // 重置调整大小状态
                m_resizeLeft = false;
                m_resizeRight = false;
                m_resizeTop = false;
                m_resizeBottom = false;
                m_resizing = false;
                m_moving = false;
                
                // 检查左侧边界
                if (mousePos.x() <= RESIZE_MARGIN) {
                    m_resizeLeft = true;
                    m_resizing = true;
                } 
                // 检查右侧边界
                else if (mousePos.x() >= widgetRect.width() - RESIZE_MARGIN) {
                    m_resizeRight = true;
                    m_resizing = true;
                }
                
                // 检查顶部边界
                if (mousePos.y() <= RESIZE_MARGIN) {
                    m_resizeTop = true;
                    m_resizing = true;
                } 
                // 检查底部边界
                else if (mousePos.y() >= widgetRect.height() - RESIZE_MARGIN) {
                    m_resizeBottom = true;
                    m_resizing = true;
                }
                
                // 如果不在调整大小边界，则准备移动
                if (!m_resizing) {
                    m_moving = true;
                }
                
                // 检查该控件是否是由布局管理器管理的控件
                if (m_widgetItemMap.contains(widget)) {
                    // 选择该控件并更新控制点
                    m_selectedWidget = widget;
                    updateHandles(widget);
                    update(); // 重新绘制
                }
                
                m_currentWidget = widget;
                m_mousePressPos = mouseEvent->globalPos();
                m_widgetPos = widget->pos();
                
                return true;
            }
        }
    }
    
    // 处理鼠标移动事件
    if (event->type() == QEvent::MouseMove) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        QWidget *widget = qobject_cast<QWidget*>(obj);
        if (widget) {
            if (m_currentWidget == widget) {
                if (m_resizing) {
                    // 调整控件大小
                    QPoint delta = mouseEvent->globalPos() - m_mousePressPos;
                    QRect newRect = widget->geometry();
                    
                    if (m_resizeLeft) {
                        newRect.setLeft(newRect.left() + delta.x());
                    }
                    if (m_resizeRight) {
                        newRect.setWidth(newRect.width() + delta.x());
                    }
                    if (m_resizeTop) {
                        newRect.setTop(newRect.top() + delta.y());
                    }
                    if (m_resizeBottom) {
                        newRect.setHeight(newRect.height() + delta.y());
                    }
                    
                    // 确保控件大小不小于最小尺寸
                    if (newRect.width() < 50) {
                        if (m_resizeLeft) {
                            newRect.setLeft(newRect.right() - 50);
                        } else {
                            newRect.setWidth(50);
                        }
                    }
                    if (newRect.height() < 20) {
                        if (m_resizeTop) {
                            newRect.setTop(newRect.bottom() - 20);
                        } else {
                            newRect.setHeight(20);
                        }
                    }
                    
                    widget->setGeometry(newRect);
                    updateHandles(widget);
                    update(); // 重新绘制
                    
                    // 更新LayoutItem的大小和位置（因为调整左边界或上边界时位置会变化）
                    LayoutItem *item = m_widgetItemMap.value(widget);
                    if (item) {
                        item->setSize(widget->size());
                        item->setPos(widget->pos());
                    }
                    m_mousePressPos = mouseEvent->globalPos();
                    return true;
                } else if (m_moving) {
                    // 移动控件
                    QPoint delta = mouseEvent->globalPos() - m_mousePressPos;
                    QPoint newPos = m_widgetPos + delta;
                    widget->move(newPos);
                    updateHandles(widget);
                    update(); // 重新绘制
                    
                    // 更新LayoutItem的位置
                    LayoutItem *item = m_widgetItemMap.value(widget);
                    if (item) {
                        item->setPos(widget->pos());
                    }
                    return true;
                }
            } else {
                // 检查鼠标位置以更新光标
                QRect widgetRect = widget->rect();
                QPoint mousePos = mouseEvent->pos();
                
                Qt::CursorShape cursorShape = Qt::ArrowCursor;
                bool onEdge = false;
                
                // 检查左侧或右侧边界
                if (mousePos.x() <= RESIZE_MARGIN || mousePos.x() >= widgetRect.width() - RESIZE_MARGIN) {
                    cursorShape = Qt::SizeHorCursor;
                    onEdge = true;
                }
                
                // 检查顶部或底部边界
                if (mousePos.y() <= RESIZE_MARGIN || mousePos.y() >= widgetRect.height() - RESIZE_MARGIN) {
                    cursorShape = onEdge ? Qt::SizeFDiagCursor : Qt::SizeVerCursor;
                    onEdge = true;
                }
                
                widget->setCursor(cursorShape);
            }
        }
    }
    
    // 处理鼠标释放事件
    if (event->type() == QEvent::MouseButtonRelease) {
        if (event->type() == QEvent::MouseButtonRelease) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton) {
                m_currentWidget = nullptr;
                m_resizing = false;
                m_moving = false;
                return true;
            }
        }
    }
    
    // 处理鼠标离开事件
    if (event->type() == QEvent::Leave) {
        QWidget *widget = qobject_cast<QWidget*>(obj);
        if (widget) {
            widget->unsetCursor();
        }
    }
    // 处理控件失去焦点事件
    if (event->type() == QEvent::FocusOut) {
        QWidget *widget = qobject_cast<QWidget*>(obj);
        if (widget && widget == m_selectedWidget) {
            // 当选中的控件失去焦点时，隐藏控制点
            m_editAreaWidget->setHandles(QList<QRect>());
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
    if (!m_selectedWidget) {
        return;
    }

    // 处理QLabel双击编辑文本
    QLabel *label = qobject_cast<QLabel*>(m_selectedWidget);
    if (label) {
        bool ok;
        QString text = QInputDialog::getText(this, tr("编辑文本"), tr("文本:"), QLineEdit::Normal, label->text(), &ok);
        if (ok && !text.isEmpty()) {
            label->setText(text);
        }
        return;
    }

    // 处理QTabWidget双击编辑Tab页标题
    QTabWidget *tabWidget = qobject_cast<QTabWidget*>(m_selectedWidget);
    if (tabWidget) {
        // 转换为QTabWidget的本地坐标
        QPoint tabWidgetLocalPos = tabWidget->mapFromGlobal(m_lastMousePos);
        // 获取Tab栏的区域
        QRect tabBarRect = tabWidget->tabBar()->geometry();
        if (tabBarRect.contains(tabWidgetLocalPos)) {
            // 计算用户点击的是哪个Tab页
            int tabIndex = tabWidget->tabBar()->tabAt(tabWidgetLocalPos);
            if (tabIndex != -1) {
                bool ok;
                QString text = QInputDialog::getText(this, tr("编辑Tab标题"), tr("标题:"), QLineEdit::Normal, tabWidget->tabText(tabIndex), &ok);
                if (ok && !text.isEmpty()) {
                    tabWidget->setTabText(tabIndex, text);
                }
            }
        }
        return;
    }
}

// 处理控制点拖动
void UILayoutWindow::onHandleDragged(int handleIndex, const QPoint &delta) {
    if (!m_selectedWidget) {
        return;
    }

    // 根据控制点索引调整控件大小
    QRect newGeometry = m_selectedWidget->geometry();
    int left = newGeometry.left();
    int top = newGeometry.top();
    int width = newGeometry.width();
    int height = newGeometry.height();

    // 定义最小宽高
    const int MIN_WIDTH = 50;
    const int MIN_HEIGHT = 20;

    // 8个控制点的索引分别对应：
    // 0: 顶部左, 1: 顶部中, 2: 顶部右
    // 3: 中间左, 4: 中间右
    // 5: 底部左, 6: 底部中, 7: 底部右

    switch (handleIndex) {
        case 0: // 顶部左
            left += delta.x();
            width -= delta.x();
            top += delta.y();
            height -= delta.y();
            break;
        case 1: // 顶部中
            top += delta.y();
            height -= delta.y();
            break;
        case 2: // 顶部右
            width += delta.x();
            top += delta.y();
            height -= delta.y();
            break;
        case 3: // 中间左
            left += delta.x();
            width -= delta.x();
            break;
        case 4: // 中间右
            width += delta.x();
            break;
        case 5: // 底部左
            left += delta.x();
            width -= delta.x();
            height += delta.y();
            break;
        case 6: // 底部中
            height += delta.y();
            break;
        case 7: // 底部右
            width += delta.x();
            height += delta.y();
            break;
        default:
            return;
    }

    // 确保宽高不小于最小值
    width = qMax(width, MIN_WIDTH);
    height = qMax(height, MIN_HEIGHT);

    // 更新控件几何形状
    m_selectedWidget->setGeometry(left, top, width, height);

    // 更新布局项
    LayoutItem *item = m_widgetItemMap.value(m_selectedWidget);
    if (item) {
        item->setPos(m_selectedWidget->pos());
        item->setSize(m_selectedWidget->size());
    }

    // 更新控制点位置
    updateHandles(m_selectedWidget);
}

// 处理控制点释放
void UILayoutWindow::onHandleReleased() {
    // 可以在这里添加释放后的处理逻辑
}
