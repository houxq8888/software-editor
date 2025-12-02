#include "uilayoutwindow.h"
#include "ui_uilayoutwindow.h"
#include "customtreewidget.h"
#include "productconfigmanager.h"
#include <QDockWidget>
#include <QInputDialog>
#include <QRegularExpression>
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
#include <QMessageBox>
#include <QCloseEvent>
#include <QScreen>
#include <QGuiApplication>



LayoutItem::LayoutItem(const QString &widgetType, const QString &displayName)
    : m_widgetType(widgetType), m_displayName(displayName), m_text(displayName), m_pos(0, 0), m_size(200, 50), m_zIndex(0), m_tabIndex(-1), m_dockArea(Qt::LeftDockWidgetArea), m_floating(false) {}

QString LayoutItem::widgetType() const { return m_widgetType; }

QString LayoutItem::displayName() const { return m_displayName; }

QString LayoutItem::text() const { return m_text; }

void LayoutItem::setText(const QString &text) { m_text = text; }

QPoint LayoutItem::pos() const { return m_pos; }

void LayoutItem::setPos(const QPoint &pos) { m_pos = pos; }

QSize LayoutItem::size() const { return m_size; }

void LayoutItem::setSize(const QSize &size) { m_size = size; }

int LayoutItem::zIndex() const { return m_zIndex; }

void LayoutItem::setZIndex(int zIndex) { m_zIndex = zIndex; }

// 设置控件所属的Tab页索引
void LayoutItem::setTabIndex(int index) {
    m_tabIndex = index;
}

// 获取控件所属的Tab页索引
int LayoutItem::tabIndex() const {
    return m_tabIndex;
}

// DockWidget相关方法实现
Qt::DockWidgetArea LayoutItem::dockArea() const {
    return m_dockArea;
}

void LayoutItem::setDockArea(Qt::DockWidgetArea area) {
    m_dockArea = area;
}

bool LayoutItem::isFloating() const {
    return m_floating;
}

void LayoutItem::setFloating(bool floating) {
    m_floating = floating;
}

QObject *LayoutItem::createWidget(QWidget *parent) const {
    QWidget *widget = nullptr;
    if (m_widgetType == "QPushButton") {
        widget = new QPushButton(m_text, parent);
    } else if (m_widgetType == "QToolButton") {
        QToolButton *toolButton = new QToolButton(parent);
        toolButton->setText(m_text);
        widget = toolButton;
    } else if (m_widgetType == "QRadioButton") {
        widget = new QRadioButton(m_text, parent);
    } else if (m_widgetType == "QCheckBox") {
        widget = new QCheckBox(m_text, parent);
    } else if (m_widgetType == "QCommandLinkButton") {
        widget = new QCommandLinkButton(m_text, parent);
    } else if (m_widgetType == "QDialogButtonBox") {
        QDialogButtonBox *dialogButtonBox = new QDialogButtonBox(parent);
        dialogButtonBox->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        widget = dialogButtonBox;
    } else if (m_widgetType == "QLabel") {
        widget = new QLabel(m_text, parent);
    } else if (m_widgetType == "QTextBrowser") {
        QTextBrowser *textBrowser = new QTextBrowser(parent);
        textBrowser->setText(m_text);
        widget = textBrowser;
    } else if (m_widgetType == "QGraphicsView") {
        QGraphicsView *graphicsView = new QGraphicsView(parent);
        graphicsView->setScene(new QGraphicsScene());
        widget = graphicsView;
    } else if (m_widgetType == "QCalendarWidget") {
        widget = new QCalendarWidget(parent);
    } else if (m_widgetType == "QLCDNumber") {
        widget = new QLCDNumber(parent);
    } else if (m_widgetType == "QProgressBar") {
        QProgressBar *progressBar = new QProgressBar(parent);
        progressBar->setValue(50);
        widget = progressBar;
    } else if (m_widgetType == "QFrame") {
        QFrame *frame = new QFrame(parent);
        if (m_displayName == "Vertical Line") {
            frame->setFrameShape(QFrame::VLine);
        } else {
            frame->setFrameShape(QFrame::HLine);
        }
        widget = frame;
    // } else if (m_widgetType == "QOpenGLWidget") {
    //     widget = new QOpenGLWidget(parent);
    // QQuickWidget 需要 Qt Quick 模块支持
    // } else if (m_widgetType == "QQuickWidget") {
    //     widget = new QQuickWidget(parent);
    } else if (m_widgetType == "QComboBox") {
        QComboBox *comboBox = new QComboBox(parent);
        comboBox->addItem("Option 1");
        comboBox->addItem("Option 2");
        comboBox->addItem("Option 3");
        widget = comboBox;
    } else if (m_widgetType == "QFontComboBox") {
        widget = new QFontComboBox(parent);
    } else if (m_widgetType == "QLineEdit") {
        widget = new QLineEdit(m_text, parent);
        widget->setStyleSheet("background-color: white;");
    } else if (m_widgetType == "QTextEdit") {
        QTextEdit *textEdit = new QTextEdit(parent);
        textEdit->setText(m_text);
        widget = textEdit;
    } else if (m_widgetType == "QPlainTextEdit") {
        QPlainTextEdit *plainTextEdit = new QPlainTextEdit(parent);
        plainTextEdit->setPlainText(m_text);
        widget = plainTextEdit;
    } else if (m_widgetType == "QSpinBox") {
        widget = new QSpinBox(parent);
    } else if (m_widgetType == "QDoubleSpinBox") {
        widget = new QDoubleSpinBox(parent);
    } else if (m_widgetType == "QTimeEdit") {
        widget = new QTimeEdit(parent);
    } else if (m_widgetType == "QDateEdit") {
        widget = new QDateEdit(parent);
    } else if (m_widgetType == "QDateTimeEdit") {
        widget = new QDateTimeEdit(parent);
    } else if (m_widgetType == "QDial") {
        widget = new QDial(parent);
    } else if (m_widgetType == "QScrollBar") {
        if (m_displayName == "Vertical Scroll Bar") {
            widget = new QScrollBar(Qt::Vertical, parent);
        } else {
            widget = new QScrollBar(Qt::Horizontal, parent);
        }
    } else if (m_widgetType == "QSlider") {
        if (m_displayName == "Vertial Slider"){
            widget = new QSlider(Qt::Vertical, parent);
        } else {
            widget = new QSlider(Qt::Horizontal, parent);
        }
    } else if (m_widgetType == "QKeySequenceEdit") {
        widget = new QKeySequenceEdit(parent);
    } else if (m_widgetType == "QGroupBox") {
        widget = new QGroupBox(m_text, parent);
    } else if (m_widgetType == "QVBoxLayout") {
        return new QVBoxLayout(parent);
    } else if (m_widgetType == "QHBoxLayout") {
        return new QHBoxLayout(parent);
    } else if (m_widgetType == "QGridLayout") {
        return new QGridLayout(parent);
    } else if (m_widgetType == "QFormLayout") {
        return new QFormLayout(parent);
    } else if (m_widgetType == "QListView") {
        widget = new QListView(parent);
    } else if (m_widgetType == "QTreeView") {
        widget = new QTreeView(parent);
    } else if (m_widgetType == "QTableView") {
        widget = new QTableView(parent);
    } else if (m_widgetType == "QColumnView") {
        widget = new QColumnView(parent);
    } else if (m_widgetType == "QUndoView") {
        widget = new QUndoView(parent);
    } else if (m_widgetType == "QListWidget") {
        QListWidget *listWidget = new QListWidget(parent);
        listWidget->addItem("Item 1");
        listWidget->addItem("Item 2");
        listWidget->addItem("Item 3");
        widget = listWidget;
    } else if (m_widgetType == "QTreeWidget") {
        widget = new QTreeWidget(parent);
    } else if (m_widgetType == "QTableWidget") {
        widget = new QTableWidget(3, 3, parent);
    } else if (m_widgetType == "QGroupBox"){
        widget = new QGroupBox(m_text, parent);
    } else if (m_widgetType == "QScrollArea") {
        widget = new QScrollArea(parent);
    } else if (m_widgetType == "QToolBox") {
        QToolBox *toolBox = new QToolBox(parent);
        toolBox->addItem(new QWidget(), "Toolbox Item 1");
        toolBox->addItem(new QWidget(), "Toolbox Item 2");
        widget = toolBox;
    } else if (m_widgetType == "QTabWidget") {
        QTabWidget *tabWidget = new QTabWidget(parent);
        // 添加默认标签页
        tabWidget->addTab(new QWidget(), "Tab 1");
        tabWidget->addTab(new QWidget(), "Tab 2");
        widget = tabWidget;
    } else if (m_widgetType == "QStackedWidget") {
        QStackedWidget *stackedWidget = new QStackedWidget(parent);
        stackedWidget->addWidget(new QWidget());
        stackedWidget->addWidget(new QWidget());
        widget = stackedWidget;
    } else if (m_widgetType == "QFrame") {
        widget = new QFrame(parent);
    } else if (m_widgetType == "QWidget"){
        widget = new QWidget(parent);
    } else if (m_widgetType == "QMdiArea") {
        widget = new QMdiArea(parent);
    } else if (m_widgetType == "QDockWidget") {
        QDockWidget *dockWidget = new QDockWidget(m_text, parent);
        dockWidget->setWidget(new QWidget());
        widget = dockWidget;
    } else {
        // 默认返回一个QWidget
        widget = new QWidget(parent);
        widget->setStyleSheet("background-color: lightgray; border: 1px solid gray;");
    }
    return widget;
}


UILayoutWindow::UILayoutWindow(QWidget *parent, bool isNewProduct, const QString &productFilePath, ProductConfigManager *configManager)
    : QMainWindow(parent), ui(new Ui::UILayoutWindow), m_previewWindow(nullptr), m_editAreaWidget(nullptr), m_editingTabWidget(nullptr), m_tabTitleEdit(nullptr), m_currentLayoutPath(""), m_productFilePath(productFilePath), m_configManager(configManager)
{
    ui->setupUi(this);
    
    // 初始化多界面管理器
    m_interfaceManager = new UIInterfaceManager(this);
    
    // 限制窗口大小不超过屏幕分辨率
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->availableGeometry();
    int maxWidth = screenGeometry.width() - 100; // 留出边距
    int maxHeight = screenGeometry.height() - 100;
    
    // 设置窗口最大尺寸
    setMaximumSize(maxWidth, maxHeight);
    
    // 如果当前尺寸超过屏幕，则调整到合适大小
    if (width() > maxWidth || height() > maxHeight) {
        resize(qMin(width(), maxWidth), qMin(height(), maxHeight));
    }
    
    // 如果是新建产品，设置窗口标题为新建布局
    if (isNewProduct) {
        setWindowTitle("UI布局编辑器 - 新建布局");
    }

    // 初始化splitter
    QList<int> sizes;
    sizes << 200 << 700 << 300; // 设置初始大小比例（左侧控件库、编辑区、属性编辑器）
    ui->splitter->setSizes(sizes);
    ui->splitter->setStretchFactor(0, 1); // 左侧控件库可拉伸
    ui->splitter->setStretchFactor(1, 1); // 中间编辑区可拉伸
    ui->splitter->setStretchFactor(2, 1); // 右侧属性编辑器可拉伸
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
    // 连接双击事件用于编辑控件文本
    connect(m_editAreaWidget, &EditAreaWidget::doubleClicked, this, &UILayoutWindow::onEditAreaDoubleClicked);
    // 连接控件拖放事件
    connect(m_editAreaWidget, &EditAreaWidget::widgetDropped, this, &UILayoutWindow::onWidgetDropped);
    
    // 初始化属性编辑器
    ui->propertiesTreeWidget->setColumnCount(2);
    ui->propertiesTreeWidget->setHeaderLabels(QStringList() << "属性名" << "属性值");
    ui->propertiesTreeWidget->setAlternatingRowColors(true);
    // 设置为可编辑
    ui->propertiesTreeWidget->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);
    // 连接属性编辑信号
    connect(ui->propertiesTreeWidget, &QTreeWidget::itemChanged, this, &UILayoutWindow::onPropertyItemChanged);

    // 允许拖放
    setAcceptDrops(true);
    m_editAreaWidget->setAcceptDrops(true);
    qDebug() << "UILayoutWindow: 拖放设置完成";

    // 初始化控件库
    initWidgetLibrary();
    qDebug() << "UILayoutWindow: 控件库初始化完成";

    // 连接搜索框信号
    connect(ui->searchLineEdit, &QLineEdit::textChanged, ui->widgetListWidget, &CustomTreeWidget::search);

    // 连接功能特性相关信号槽
    connect(ui->bindFeatureButton, &QPushButton::clicked, this, &UILayoutWindow::onBindFeatureButtonClicked);
    connect(ui->unbindFeatureButton, &QPushButton::clicked, this, &UILayoutWindow::onUnbindFeatureButtonClicked);
    connect(ui->syncFeaturesButton, &QPushButton::clicked, this, &UILayoutWindow::onSyncFeaturesButtonClicked);
    connect(ui->featuresTreeWidget, &QTreeWidget::itemDoubleClicked, this, &UILayoutWindow::onFeaturesTreeWidgetItemDoubleClicked);

    // 加载插件
    loadPlugins();

    // UI文件中定义的动作已由moc自动连接，无需手动连接
    
    // 手动连接事件-动作编辑器按钮信号
    connect(ui->actionEventActionEditor, &QAction::triggered, this, &UILayoutWindow::onActionEventActionEditorTriggered);
    
    // 设置多界面管理UI
    setupInterfaceManagementUI();
}

UILayoutWindow::~UILayoutWindow()
{
    delete ui;
    delete m_editAreaWidget;
}

void UILayoutWindow::updatePropertiesEditor(QWidget *widget)
{
    // 清空属性树
    ui->propertiesTreeWidget->clear();
    
    if (!widget) {
        return;
    }
    
    // 获取控件的元对象
    const QMetaObject *metaObject = widget->metaObject();
    
    // 添加QObject属性
    QTreeWidgetItem *objectItem = new QTreeWidgetItem(ui->propertiesTreeWidget, QStringList() << "QObject" << "");
    objectItem->setExpanded(true);
    
    // objectName属性
    QTreeWidgetItem *objectNameItem = new QTreeWidgetItem(objectItem, QStringList() << "objectName" << widget->objectName());
    objectNameItem->setData(0, Qt::UserRole, QVariant::fromValue(widget));
    objectNameItem->setData(0, Qt::UserRole + 1, QVariant("objectName"));
    
    // 添加QWidget属性
    QTreeWidgetItem *widgetItem = new QTreeWidgetItem(ui->propertiesTreeWidget, QStringList() << "QWidget" << "");
    widgetItem->setExpanded(true);
    
    // enabled属性
    QTreeWidgetItem *enabledItem = new QTreeWidgetItem(widgetItem, QStringList() << "enabled" << (widget->isEnabled() ? "true" : "false"));
    enabledItem->setData(0, Qt::UserRole, QVariant::fromValue(widget));
    enabledItem->setData(0, Qt::UserRole + 1, QVariant("enabled"));
    
    // geometry属性
    QRect geometry = widget->geometry();
    QString geometryStr = QString("x:%1, y:%2, width:%3, height:%4").arg(geometry.x()).arg(geometry.y()).arg(geometry.width()).arg(geometry.height());
    QTreeWidgetItem *geometryItem = new QTreeWidgetItem(widgetItem, QStringList() << "geometry" << geometryStr);
    geometryItem->setData(0, Qt::UserRole, QVariant::fromValue(widget));
    geometryItem->setData(0, Qt::UserRole + 1, QVariant("geometry"));
    
    // minimumSize属性
    QSize minSize = widget->minimumSize();
    QString minSizeStr = QString("%1x%2").arg(minSize.width()).arg(minSize.height());
    QTreeWidgetItem *minSizeItem = new QTreeWidgetItem(widgetItem, QStringList() << "minimumSize" << minSizeStr);
    minSizeItem->setData(0, Qt::UserRole, QVariant::fromValue(widget));
    minSizeItem->setData(0, Qt::UserRole + 1, QVariant("minimumSize"));
    
    // maximumSize属性
    QSize maxSize = widget->maximumSize();
    QString maxSizeStr = QString("%1x%2").arg(maxSize.width()).arg(maxSize.height());
    QTreeWidgetItem *maxSizeItem = new QTreeWidgetItem(widgetItem, QStringList() << "maximumSize" << maxSizeStr);
    maxSizeItem->setData(0, Qt::UserRole, QVariant::fromValue(widget));
    maxSizeItem->setData(0, Qt::UserRole + 1, QVariant("maximumSize"));
    
    // sizePolicy属性
    QSizePolicy sizePolicy = widget->sizePolicy();
    QString sizePolicyStr = QString("%1, %2").arg(sizePolicy.horizontalPolicy()).arg(sizePolicy.verticalPolicy());
    QTreeWidgetItem *sizePolicyItem = new QTreeWidgetItem(widgetItem, QStringList() << "sizePolicy" << sizePolicyStr);
    sizePolicyItem->setData(0, Qt::UserRole, QVariant::fromValue(widget));
    sizePolicyItem->setData(0, Qt::UserRole + 1, QVariant("sizePolicy"));
    
    // 字体属性
    QFont font = widget->font();
    QString fontStr = QString("%1, %2pt").arg(font.family()).arg(font.pointSize());
    QTreeWidgetItem *fontItem = new QTreeWidgetItem(widgetItem, QStringList() << "font" << fontStr);
    fontItem->setData(0, Qt::UserRole, QVariant::fromValue(widget));
    fontItem->setData(0, Qt::UserRole + 1, QVariant("font"));
    
    // 根据控件类型添加特定属性
    if (qobject_cast<QAbstractSlider*>(widget)) {
        QTreeWidgetItem *sliderItem = new QTreeWidgetItem(ui->propertiesTreeWidget, QStringList() << "QAbstractSlider" << "");
        sliderItem->setExpanded(true);
        QAbstractSlider *slider = qobject_cast<QAbstractSlider*>(widget);
        // 添加slider特有的属性
        QTreeWidgetItem *minValueItem = new QTreeWidgetItem(sliderItem, QStringList() << "minimum" << QString::number(slider->minimum()));
        minValueItem->setData(0, Qt::UserRole, QVariant::fromValue(widget));
        minValueItem->setData(0, Qt::UserRole + 1, QVariant("minimum"));
        
        QTreeWidgetItem *maxValueItem = new QTreeWidgetItem(sliderItem, QStringList() << "maximum" << QString::number(slider->maximum()));
        maxValueItem->setData(0, Qt::UserRole, QVariant::fromValue(widget));
        maxValueItem->setData(0, Qt::UserRole + 1, QVariant("maximum"));
        
        QTreeWidgetItem *valueItem = new QTreeWidgetItem(sliderItem, QStringList() << "value" << QString::number(slider->value()));
        valueItem->setData(0, Qt::UserRole, QVariant::fromValue(widget));
        valueItem->setData(0, Qt::UserRole + 1, QVariant("value"));
    }
    
    if (qobject_cast<QDial*>(widget)) {
        QTreeWidgetItem *dialItem = new QTreeWidgetItem(ui->propertiesTreeWidget, QStringList() << "QDial" << "");
        dialItem->setExpanded(true);
        QDial *dial = qobject_cast<QDial*>(widget);
        // 添加dial特有的属性
        QTreeWidgetItem *dialMinItem = new QTreeWidgetItem(dialItem, QStringList() << "minimum" << QString::number(dial->minimum()));
        dialMinItem->setData(0, Qt::UserRole, QVariant::fromValue(widget));
        dialMinItem->setData(0, Qt::UserRole + 1, QVariant("minimum"));
        
        QTreeWidgetItem *dialMaxItem = new QTreeWidgetItem(dialItem, QStringList() << "maximum" << QString::number(dial->maximum()));
        dialMaxItem->setData(0, Qt::UserRole, QVariant::fromValue(widget));
        dialMaxItem->setData(0, Qt::UserRole + 1, QVariant("maximum"));
        
        QTreeWidgetItem *dialValueItem = new QTreeWidgetItem(dialItem, QStringList() << "value" << QString::number(dial->value()));
        dialValueItem->setData(0, Qt::UserRole, QVariant::fromValue(widget));
        dialValueItem->setData(0, Qt::UserRole + 1, QVariant("value"));
    }
    
    ui->propertiesTreeWidget->resizeColumnToContents(0);
}

// 更新综合修改状态


void UILayoutWindow::onPropertyItemChanged(QTreeWidgetItem *item, int column)
{
    if (column != 1) {
        return; // 只处理值列的变化
    }
    
    QWidget *widget = item->data(0, Qt::UserRole).value<QWidget*>();
    QString propertyName = item->data(0, Qt::UserRole + 1).toString();
    QString propertyValue = item->text(1);
    
    if (!widget || propertyName.isEmpty()) {
        return;
    }
    
    // 根据属性名更新控件属性
    if (propertyName == "objectName") {
        widget->setObjectName(propertyValue);
    } else if (propertyName == "enabled") {
        widget->setEnabled(propertyValue.toLower() == "true");
    } else if (propertyName == "minimumSize") {
        QStringList sizeParts = propertyValue.split("x");
        if (sizeParts.size() == 2) {
            int width = sizeParts[0].toInt();
            int height = sizeParts[1].toInt();
            widget->setMinimumSize(width, height);
        }
    } else if (propertyName == "maximumSize") {
        QStringList sizeParts = propertyValue.split("x");
        if (sizeParts.size() == 2) {
            int width = sizeParts[0].toInt();
            int height = sizeParts[1].toInt();
            widget->setMaximumSize(width, height);
        }
    } else if (propertyName == "geometry") {
        // 解析geometry字符串: "x:100, y:200, width:300, height:400"
        QRegularExpression rx("x:(\\d+), y:(\\d+), width:(\\d+), height:(\\d+)");
        QRegularExpressionMatch match = rx.match(propertyValue);
        if (match.hasMatch()) {
            int x = match.captured(1).toInt();
            int y = match.captured(2).toInt();
            int width = match.captured(3).toInt();
            int height = match.captured(4).toInt();
            widget->setGeometry(x, y, width, height);
            // 更新布局项
            LayoutItem *item = m_widgetItemMap.value(widget);
            if (item) {
                item->setPos(QPoint(x, y));
                item->setSize(QSize(width, height));
            }
            // 更新控制点
            updateHandles(widget);
        }
    } else if (propertyName == "minimum") {
        if (QAbstractSlider *slider = qobject_cast<QAbstractSlider*>(widget)) {
            slider->setMinimum(propertyValue.toInt());
        } else if (QDial *dial = qobject_cast<QDial*>(widget)) {
            dial->setMinimum(propertyValue.toInt());
        }
    } else if (propertyName == "maximum") {
        if (QAbstractSlider *slider = qobject_cast<QAbstractSlider*>(widget)) {
            slider->setMaximum(propertyValue.toInt());
        } else if (QDial *dial = qobject_cast<QDial*>(widget)) {
            dial->setMaximum(propertyValue.toInt());
        }
    } else if (propertyName == "value") {
        if (QAbstractSlider *slider = qobject_cast<QAbstractSlider*>(widget)) {
            slider->setValue(propertyValue.toInt());
        } else if (QDial *dial = qobject_cast<QDial*>(widget)) {
            dial->setValue(propertyValue.toInt());
        }
    }
    
    // 更新布局状态
    qDebug()<<"onPropertyItemChanged: propertyName:"<<propertyName<<",propertyValue:"<<propertyValue;
    saveLayoutState();
}

// 实现键盘事件处理
void UILayoutWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Delete && m_selectedWidget != nullptr) {
        // 删除选中的控件
        qDebug()<<"delete selected widget:"<<m_selectedWidget->objectName();
        saveLayoutState();
        
        // 找到对应的LayoutItem
        LayoutItem *item = m_widgetItemMap.value(m_selectedWidget);
        if (item != nullptr) {
            // 从列表中移除
            m_layoutItems.removeOne(item);
            m_widgetItemMap.remove(m_selectedWidget);
            
            // 删除控件和LayoutItem
            delete m_selectedWidget;
            delete item;
            
            // 清除选中状态
            m_selectedWidget = nullptr;
            m_editAreaWidget->setHandles(QList<QRect>());
            m_editAreaWidget->repaint();
            

        }
    } else {
        QMainWindow::keyPressEvent(event);
    }
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

        // 保存当前状态
        qDebug()<<"dropEvent: 鼠标在编辑区中的局部位置:" << localPos << ", 全局位置:" << globalPos << ", 编辑区局部位置:" << editAreaLocalPos;
        saveLayoutState();
        
        // 根据控件类型创建对应的 LayoutItem
        // 需要将 "Label" 转换为 "QLabel" 等带Q前缀的格式
        QString qtWidgetType = widgetType;
        // Buttons包含Push Button,Tool Button,Radio Button,Check Box,Command Link Button,Dialog Button Box.
        if (widgetType == "Push Button") qtWidgetType = "QPushButton";
        else if (widgetType == "Tool Button") qtWidgetType = "QToolButton";
        else if (widgetType == "Radio Button") qtWidgetType = "QRadioButton";
        else if (widgetType == "Check Box") qtWidgetType = "QCheckBox";
        else if (widgetType == "Command Link Button") qtWidgetType = "QCommandLinkButton";
        else if (widgetType == "Dialog Button Box") qtWidgetType = "QDialogButtonBox";
        // Display Widgets 
        else if (widgetType == "Label") qtWidgetType = "QLabel";
        else if (widgetType == "Text Browser") qtWidgetType = "QTextBrowser";
        else if (widgetType == "Graphics View") qtWidgetType = "QGraphicsView";
        else if (widgetType == "Calendar Widget") qtWidgetType = "QCalendarWidget";
        else if (widgetType == "LCD Number") qtWidgetType = "QLCDNumber";
        else if (widgetType == "Progress Bar") qtWidgetType = "QProgressBar";
        else if (widgetType == "Horizontal Line") qtWidgetType = "QFrame";
        else if (widgetType == "Vertical Line") qtWidgetType = "QFrame";
        else if (widgetType == "OpenGL Widget") qtWidgetType = "QOpenGLWidget";
        else if (widgetType == "QQuickWidget") qtWidgetType = "QQuickWidget";
        // Input Widgets
        else if (widgetType == "Combo Box") qtWidgetType = "QComboBox";
        else if (widgetType == "Font Combo Box") qtWidgetType = "QFontComboBox";
        else if (widgetType == "Line Edit") qtWidgetType = "QLineEdit";
        else if (widgetType == "Text Edit") qtWidgetType = "QTextEdit";
        else if (widgetType == "Plain Text Edit") qtWidgetType = "QPlainTextEdit";
        else if (widgetType == "Spin Box") qtWidgetType = "QSpinBox";
        else if (widgetType == "Double Spin Box") qtWidgetType = "QDoubleSpinBox";
        else if (widgetType == "Time Edit") qtWidgetType = "QTimeEdit";
        else if (widgetType == "Date Edit") qtWidgetType = "QDateEdit";
        else if (widgetType == "Date/Time Edit") qtWidgetType = "QDateTimeEdit";
        else if (widgetType == "Dial") qtWidgetType = "QDial";
        else if (widgetType == "Horizontal Scroll Bar") qtWidgetType = "QScrollBar";
        else if (widgetType == "Vertical Scroll Bar") qtWidgetType = "QScrollBar";
        else if (widgetType == "Horizontal Slider") qtWidgetType = "QSlider";
        else if (widgetType == "Vertical Slider") qtWidgetType = "QSlider";
        else if (widgetType == "Key Sequence Edit") qtWidgetType = "QKeySequenceEdit";
        // Layouts
        else if (widgetType == "Vertical Layout") qtWidgetType = "QVBoxLayout";
        else if (widgetType == "Horizontal Layout") qtWidgetType = "QHBoxLayout";
        else if (widgetType == "Grid Layout") qtWidgetType = "QGridLayout";
        else if (widgetType == "Form Layout") qtWidgetType = "QFormLayout";
        // Spacers
        else if (widgetType == "Horizontal Spacer") qtWidgetType = "QSpacerItem";
        else if (widgetType == "Vertical Spacer") qtWidgetType = "QSpacerItem";
        // Item Views(Model-Based)
        else if (widgetType == "List View") qtWidgetType = "QListView";
        else if (widgetType == "Tree View") qtWidgetType = "QTreeView";
        else if (widgetType == "Table View") qtWidgetType = "QTableView";
        else if (widgetType == "Column View") qtWidgetType = "QColumnView";
        else if (widgetType == "Undo View") qtWidgetType = "QUndoView";
        // Item Widgets(Item-Based)
        else if (widgetType == "List Widget") qtWidgetType = "QListWidget";
        else if (widgetType == "Tree Widget") qtWidgetType = "QTreeWidget";
        else if (widgetType == "Table Widget") qtWidgetType = "QTableWidget";
        // Containers
        else if (widgetType == "Group Box") qtWidgetType = "QGroupBox";
        else if (widgetType == "Scroll Area") qtWidgetType = "QScrollArea";
        else if (widgetType == "Tool Box") qtWidgetType = "QToolBox";
        else if (widgetType == "Tab Widget") qtWidgetType = "QTabWidget";
        else if (widgetType == "Stacked Widget") qtWidgetType = "QStackedWidget";
        else if (widgetType == "Frame") qtWidgetType = "QFrame";
        else if (widgetType == "Widget") qtWidgetType = "QWidget";
        else if (widgetType == "MDI Area") qtWidgetType = "QMdiArea";
        else if (widgetType == "Dock Widget") qtWidgetType = "QDockWidget";
        else if (widgetType == "QAxWidget") qtWidgetType = "QAxWidget";



        LayoutItem *layoutItem = new LayoutItem(qtWidgetType, widgetType);
        layoutItem->setPos(editAreaLocalPos);
        layoutItem->setSize(QSize(200, 50));

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
        // 特殊处理QDockWidget，需要添加到主窗口
        if (qtWidgetType == "QDockWidget") {
            // 创建QDockWidget
            QDockWidget *dockWidget = new QDockWidget(widgetType, this);
            dockWidget->setWidget(new QWidget());
            
            // 添加到主窗口
            addDockWidget(Qt::LeftDockWidgetArea, dockWidget);
            
            // 保存位置信息到LayoutItem
            layoutItem->setDockArea(Qt::LeftDockWidgetArea);
            layoutItem->setFloating(false);
            
            // 为QDockWidget安装事件过滤器
            dockWidget->installEventFilter(this);
            
            // 连接QDockWidget的位置变化信号
            connect(dockWidget, &QDockWidget::dockLocationChanged, this, [=](Qt::DockWidgetArea area) {
                // 更新对应的LayoutItem
                LayoutItem *item = m_widgetItemMap.value(dockWidget);
                if (item) {
                    item->setDockArea(area);
                    item->setFloating(dockWidget->isFloating());

                }
            });
            
            // 连接QDockWidget的浮动状态变化信号
            connect(dockWidget, &QDockWidget::topLevelChanged, this, [=](bool floating) {
                // 更新对应的LayoutItem
                LayoutItem *item = m_widgetItemMap.value(dockWidget);
                if (item) {
                    item->setFloating(floating);
                    // 如果是浮动状态，获取停靠区域
                    if (!floating) {
                        Qt::DockWidgetArea area = Qt::NoDockWidgetArea;
                        QMainWindow *mainWindow = qobject_cast<QMainWindow*>(dockWidget->parentWidget());
                        if (mainWindow) {
                            area = mainWindow->dockWidgetArea(dockWidget);
                            item->setDockArea(area);
                        }
                    }

                }
            });
            
            // 保存LayoutItem
            m_layoutItems.append(layoutItem);
            m_widgetItemMap[dockWidget] = layoutItem;
            
            // 将最新添加的控件设为选中状态
            m_selectedWidget = dockWidget;
    updatePropertiesEditor(m_selectedWidget);
            updateHandles(m_selectedWidget);
        } else {
            // 其他控件添加到编辑区
            addWidgetToEditArea(layoutItem, targetWidget);
        }

        m_editAreaWidget->repaint();
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void UILayoutWindow::onWidgetDropped(const QString &widgetType, const QPoint &pos)
{
    qDebug() << "onWidgetDropped: 拖放的控件类型:" << widgetType << "位置:" << pos;

    // 保存当前状态
    saveLayoutState();
    
    // 根据控件类型创建对应的 LayoutItem
    // 需要将 "Label" 转换为 "QLabel" 等带Q前缀的格式
    QString qtWidgetType = widgetType;
    // Buttons包含Push Button,Tool Button,Radio Button,Check Box,Command Link Button,Dialog Button Box.
    if (widgetType == "Push Button") qtWidgetType = "QPushButton";
    else if (widgetType == "Tool Button") qtWidgetType = "QToolButton";
    else if (widgetType == "Radio Button") qtWidgetType = "QRadioButton";
    else if (widgetType == "Check Box") qtWidgetType = "QCheckBox";
    else if (widgetType == "Command Link Button") qtWidgetType = "QCommandLinkButton";
    else if (widgetType == "Dialog Button Box") qtWidgetType = "QDialogButtonBox";
    // Display Widgets 
    else if (widgetType == "Label") qtWidgetType = "QLabel";
    else if (widgetType == "Text Browser") qtWidgetType = "QTextBrowser";
    else if (widgetType == "Graphics View") qtWidgetType = "QGraphicsView";
    else if (widgetType == "Calendar Widget") qtWidgetType = "QCalendarWidget";
    else if (widgetType == "LCD Number") qtWidgetType = "QLCDNumber";
    else if (widgetType == "Progress Bar") qtWidgetType = "QProgressBar";
    else if (widgetType == "Horizontal Line") qtWidgetType = "QFrame";
    else if (widgetType == "Vertical Line") qtWidgetType = "QFrame";
    else if (widgetType == "OpenGL Widget") qtWidgetType = "QOpenGLWidget";
    else if (widgetType == "QQuickWidget") qtWidgetType = "QQuickWidget";
    // Input Widgets
    else if (widgetType == "Combo Box") qtWidgetType = "QComboBox";
    else if (widgetType == "Font Combo Box") qtWidgetType = "QFontComboBox";
    else if (widgetType == "Line Edit") qtWidgetType = "QLineEdit";
    else if (widgetType == "Text Edit") qtWidgetType = "QTextEdit";
    else if (widgetType == "Plain Text Edit") qtWidgetType = "QPlainTextEdit";
    else if (widgetType == "Spin Box") qtWidgetType = "QSpinBox";
    else if (widgetType == "Double Spin Box") qtWidgetType = "QDoubleSpinBox";
    else if (widgetType == "Time Edit") qtWidgetType = "QTimeEdit";
    else if (widgetType == "Date Edit") qtWidgetType = "QDateEdit";
    else if (widgetType == "Date/Time Edit") qtWidgetType = "QDateTimeEdit";
    else if (widgetType == "Dial") qtWidgetType = "QDial";
    else if (widgetType == "Horizontal Scroll Bar") qtWidgetType = "QScrollBar";
    else if (widgetType == "Vertical Scroll Bar") qtWidgetType = "QScrollBar";
    else if (widgetType == "Horizontal Slider") qtWidgetType = "QSlider";
    else if (widgetType == "Vertical Slider") qtWidgetType = "QSlider";
    else if (widgetType == "Key Sequence Edit") qtWidgetType = "QKeySequenceEdit";
    // Layouts
    else if (widgetType == "Vertical Layout") qtWidgetType = "QVBoxLayout";
    else if (widgetType == "Horizontal Layout") qtWidgetType = "QHBoxLayout";
    else if (widgetType == "Grid Layout") qtWidgetType = "QGridLayout";
    else if (widgetType == "Form Layout") qtWidgetType = "QFormLayout";
    // Spacers
    else if (widgetType == "Horizontal Spacer") qtWidgetType = "QSpacerItem";
    else if (widgetType == "Vertical Spacer") qtWidgetType = "QSpacerItem";
    // Item Views(Model-Based)
    else if (widgetType == "List View") qtWidgetType = "QListView";
    else if (widgetType == "Tree View") qtWidgetType = "QTreeView";
    else if (widgetType == "Table View") qtWidgetType = "QTableView";
    else if (widgetType == "Column View") qtWidgetType = "QColumnView";
    else if (widgetType == "Undo View") qtWidgetType = "QUndoView";
    // Item Widgets(Item-Based)
    else if (widgetType == "List Widget") qtWidgetType = "QListWidget";
    else if (widgetType == "Tree Widget") qtWidgetType = "QTreeWidget";
    else if (widgetType == "Table Widget") qtWidgetType = "QTableWidget";
    // Containers
    else if (widgetType == "Group Box") qtWidgetType = "QGroupBox";
    else if (widgetType == "Scroll Area") qtWidgetType = "QScrollArea";
    else if (widgetType == "Tool Box") qtWidgetType = "QToolBox";
    else if (widgetType == "Tab Widget") qtWidgetType = "QTabWidget";
    else if (widgetType == "Stacked Widget") qtWidgetType = "QStackedWidget";
    else if (widgetType == "Frame") qtWidgetType = "QFrame";
    else if (widgetType == "Widget") qtWidgetType = "QWidget";
    else if (widgetType == "MDI Area") qtWidgetType = "QMdiArea";
    else if (widgetType == "Dock Widget") qtWidgetType = "QDockWidget";
    else if (widgetType == "QAxWidget") qtWidgetType = "QAxWidget";

    LayoutItem *layoutItem = new LayoutItem(qtWidgetType, widgetType);
    layoutItem->setPos(pos);
    layoutItem->setSize(QSize(200, 50));

    // 检查鼠标位置是否在某个 Tab 页上
    QWidget *targetWidget = m_editAreaWidget; // 默认添加到编辑区
    QPoint editAreaGlobalPos = m_editAreaWidget->mapToGlobal(pos);
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
                    QPoint newPos = m_editAreaWidget->mapFromGlobal(currentTabPage->mapToGlobal(tabPageLocalPos));
                    layoutItem->setPos(newPos);
                    // 设置控件所属的 Tab 页索引
                    int tabIndex = tabWidget->currentIndex();
                    layoutItem->setTabIndex(tabIndex);
                    qDebug() << "控件拖放到 Tab 页" << tabIndex + 1;
                }
            }
        }
    }
    // 特殊处理QDockWidget，需要添加到主窗口
    if (qtWidgetType == "QDockWidget") {
        // 创建QDockWidget
        QDockWidget *dockWidget = new QDockWidget(widgetType, this);
        dockWidget->setWidget(new QWidget());
        
        // 添加到主窗口
        addDockWidget(Qt::LeftDockWidgetArea, dockWidget);
        
        // 保存位置信息到LayoutItem
        layoutItem->setDockArea(Qt::LeftDockWidgetArea);
        layoutItem->setFloating(false);
        
        // 为QDockWidget安装事件过滤器
        dockWidget->installEventFilter(this);
        
        // 连接QDockWidget的位置变化信号
        connect(dockWidget, &QDockWidget::dockLocationChanged, this, [=](Qt::DockWidgetArea area) {
            // 更新对应的LayoutItem
            LayoutItem *item = m_widgetItemMap.value(dockWidget);
            if (item) {
                item->setDockArea(area);
                item->setFloating(dockWidget->isFloating());

            }
        });
        
        // 连接QDockWidget的浮动状态变化信号
        connect(dockWidget, &QDockWidget::topLevelChanged, this, [=](bool floating) {
            // 更新对应的LayoutItem
            LayoutItem *item = m_widgetItemMap.value(dockWidget);
            if (item) {
                item->setFloating(floating);
                // 如果是浮动状态，获取停靠区域
                if (!floating) {
                    Qt::DockWidgetArea area = Qt::NoDockWidgetArea;
                    QMainWindow *mainWindow = qobject_cast<QMainWindow*>(dockWidget->parentWidget());
                    if (mainWindow) {
                        area = mainWindow->dockWidgetArea(dockWidget);
                        item->setDockArea(area);
                    }
                }

            }
        });
        
        // 保存LayoutItem
        m_layoutItems.append(layoutItem);
        m_widgetItemMap[dockWidget] = layoutItem;
        
        // 将最新添加的控件设为选中状态
        m_selectedWidget = dockWidget;
        updatePropertiesEditor(m_selectedWidget);
        updateHandles(m_selectedWidget);
    } else {
        // 其他控件添加到编辑区
        addWidgetToEditArea(layoutItem, targetWidget);
    }

    m_editAreaWidget->repaint();
}

void UILayoutWindow::addWidgetToEditArea(LayoutItem *item, QWidget *targetWidget)
{
    if (!item) {
        return;
    }

    // 如果没有指定父窗口，则使用编辑区作为父窗口
    QWidget *parentWidget = targetWidget;
    if (!parentWidget) {
        parentWidget = m_editAreaWidget;
    }

    // 跳过QDockWidget，已经在dropEvent中特殊处理
    if (item->widgetType() == "QDockWidget") {
        return;
    }
    
    // 创建相应的控件
    QObject *createdObj = item->createWidget(parentWidget);
    if (!createdObj) {
        return;
    }

    // 检查是否是Widget
    QWidget *widget = qobject_cast<QWidget*>(createdObj);
    if (widget) {
        // 设置控件位置和大小
        widget->move(item->pos());
        widget->resize(item->size());
        widget->show();
        qDebug() << "Widget created and shown:" << widget->metaObject()->className() << "at" << widget->pos() << "size" << widget->size();
    } else {
        // 是布局类，不需要设置位置和大小
        qDebug() << "Layout created:" << createdObj->metaObject()->className();
    }

    // 直接使用上面的widget变量，不需要重新声明
    if (widget) {
        // 为可编辑的控件安装事件过滤器
        widget->installEventFilter(this);

        // 保存 LayoutItem
        m_layoutItems.append(item);
        m_widgetItemMap[widget] = item;

        // 将最新添加的控件设为选中状态并显示控制点
        m_selectedWidget = widget;
    updatePropertiesEditor(m_selectedWidget);
        updateHandles(m_selectedWidget);
    } else {
        // 处理布局类
        QLayout *layout = qobject_cast<QLayout*>(createdObj);
        if (layout) {
            // 保存 LayoutItem
            m_layoutItems.append(item);
            // 布局类不需要事件过滤器和控制点
        }
    }
}




void UILayoutWindow::on_actionPreview_triggered()
{
    // 创建预览窗口
    if (!m_previewWindow) {
        m_previewWindow = new PreviewWindow(this);
    }
    
    // 设置布局项
    m_previewWindow->setLayoutItems(m_layoutItems);
    
    // 显示预览窗口并确保它在最前端且可交互
    m_previewWindow->show();
    m_previewWindow->raise();
    m_previewWindow->activateWindow();
}

QList<LayoutItem*> UILayoutWindow::getLayoutItems() const
{
    return m_layoutItems;
}

bool UILayoutWindow::loadLayout(const QString &filePath)
{
    if (filePath.isEmpty()) {
        return false;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    // 尝试解析为多界面JSON格式
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data, &parseError);
    
    if (parseError.error == QJsonParseError::NoError) {
        // 是多界面JSON格式
        if (!m_interfaceManager) return false;
        return m_interfaceManager->fromJson(jsonDoc.object());
    }

    // 否则按原有DOM格式加载单个界面
    QDomDocument doc;
    if (!doc.setContent(data)) {
        return false;
    }

    // 注意：加载布局时不应该保存状态到撤销栈，因为这会错误标记为已修改
    // 我们直接清除撤销栈和重做栈，并设置修改状态为false
    qDebug()<<"loadLayout: filePath:"<<filePath;
    
    // 清空撤销栈和重做栈
    m_undoStack.clear();
    m_redoStack.clear();

    // 清除现有项
    for (QWidget *widget : m_widgetItemMap.keys()) {
        delete widget;
    }
    m_layoutItems.clear();
    m_widgetItemMap.clear();
    
    // 如果没有控件，重置选中的控件并隐藏控制点
    if (m_layoutItems.isEmpty()) {
        m_selectedWidget = nullptr;
        m_editAreaWidget->setHandles(QList<QRect>());
    }

    // 加载新项
    QDomElement root = doc.documentElement();
    QDomElement mainWindow = root.firstChildElement("widget");
    if (mainWindow.isNull()) {
        qDebug() << "Main window widget not found";
        return false;
    }

    // 找到centralWidget
    QDomNodeList mainWindowChildren = mainWindow.childNodes();
    QDomElement centralWidget;
    for (int i = 0; i < mainWindowChildren.size(); ++i) {
        QDomNode childNode = mainWindowChildren.at(i);
        if (childNode.isElement()) {
            QDomElement childElem = childNode.toElement();
            if (childElem.tagName() == "widget" && childElem.attribute("name") == "centralWidget") {
                centralWidget = childElem;
                break;
            }
        }
    }

    if (centralWidget.isNull()) {
        qDebug() << "Central widget not found";
        return false;
    }

    // 递归加载控件和布局
    loadWidgetRecursive(centralWidget, m_editAreaWidget);

    qDebug() << "Layout loading completed. Total widgets loaded:" << m_widgetItemMap.size();
    
    // 设置当前布局路径并更新窗口标题
    m_currentLayoutPath = filePath;
    setWindowTitle(QString("UI布局编辑器 - %1").arg(QFileInfo(m_currentLayoutPath).fileName()));
    
    // 临时更新ProductConfigManager中的UI布局路径，用于UI绑定检查
    // 如果用户选择丢弃，将在closeEvent中重置
    if (m_configManager) {
        m_configManager->setUiLayoutPath(filePath);
        qDebug() << "UI布局路径已临时更新到ProductConfigManager:" << filePath;
    }
    
    // 加载布局后重置修改状态 - 通过ProductConfigManager管理
    if (m_configManager) {
        m_configManager->setUiLayoutModified(false);
    }
    
    qDebug() << "Layout loaded from" << filePath;
    
    return true;
}

void UILayoutWindow::loadWidgetRecursive(QDomElement widgetElem, QWidget *parentWidget, bool isInLayout)
{
    if (widgetElem.isNull() || !parentWidget) {
        return;
    }

    QString widgetType = widgetElem.attribute("class");
    QString widgetName = widgetElem.attribute("name");

    // 解析控件属性
    QString text;
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;

    QDomNodeList widgetChildren = widgetElem.childNodes();
    for (int j = 0; j < widgetChildren.size(); ++j) {
        QDomNode childNode = widgetChildren.at(j);
        if (!childNode.isElement()) {
            continue;
        }

        QDomElement childElem = childNode.toElement();
        if (childElem.tagName() == "property") {
            QString propName = childElem.attribute("name");

            if (propName == "geometry") {
                QDomElement rectElem = childElem.firstChildElement("rect");
                if (!rectElem.isNull()) {
                    x = rectElem.firstChildElement("x").text().toInt();
                    y = rectElem.firstChildElement("y").text().toInt();
                    width = rectElem.firstChildElement("width").text().toInt();
                    height = rectElem.firstChildElement("height").text().toInt();
                }
            } else if (propName == "text") {
                QDomElement stringElem = childElem.firstChildElement("string");
                if (!stringElem.isNull()) {
                    text = stringElem.text();
                }
            } else if (propName == "windowTitle") {
                QDomElement stringElem = childElem.firstChildElement("string");
                if (!stringElem.isNull()) {
                    text = stringElem.text();
                }
            } else if (propName == "placeholderText") {
                QDomElement stringElem = childElem.firstChildElement("string");
                if (!stringElem.isNull()) {
                    // 占位符文本可以在创建控件后设置
                }
            }
        }
    }

    qDebug() << "Loading widget:" << widgetType << "name:" << widgetName << "at (" << x << "," << y << ") size:" << width << "x" << height << "text:" << text;

    // 创建 LayoutItem
    LayoutItem *item = new LayoutItem(widgetType, text);
    item->setPos(QPoint(x, y));
    item->setSize(QSize(width, height));
    item->setTabIndex(-1); // 暂时不处理Tab页

    // 将控件添加到父窗口
    addWidgetToEditArea(item, parentWidget);
    QWidget *newWidget = m_widgetItemMap.key(item);

    // 只有当控件不在布局中时，才查找并加载控件的布局
    if (!isInLayout) {
        // 查找并加载布局
        QDomElement layoutElem;
        for (int j = 0; j < widgetChildren.size(); ++j) {
            QDomNode childNode = widgetChildren.at(j);
            if (!childNode.isElement()) {
                continue;
            }

            QDomElement childElem = childNode.toElement();
            if (childElem.tagName() == "layout") {
                layoutElem = childElem;
                break;
            }
        }

        if (!layoutElem.isNull()) {
            // 加载布局和布局中的控件
            loadLayoutRecursive(layoutElem, newWidget);
        }
    } else {
        // 如果控件在布局中，不需要设置位置和大小，布局会自动管理
        if (newWidget) {
            newWidget->move(0, 0);
            newWidget->resize(100, 100);
        }
    }

    // 加载子控件
    for (int j = 0; j < widgetChildren.size(); ++j) {
        QDomNode childNode = widgetChildren.at(j);
        if (!childNode.isElement()) {
            continue;
        }

        QDomElement childElem = childNode.toElement();
        if (childElem.tagName() == "widget") {
            loadWidgetRecursive(childElem, newWidget, false);
        }
    }
}

void UILayoutWindow::loadLayoutRecursive(QDomElement layoutElem, QWidget *parentWidget)
{
    if (layoutElem.isNull() || !parentWidget) {
        return;
    }

    QString layoutType = layoutElem.attribute("class");
    qDebug() << "Loading layout:" << layoutType;

    // 创建布局管理器
    QLayout *layout = nullptr;
    if (layoutType == "QVBoxLayout") {
        layout = new QVBoxLayout(parentWidget);
    } else if (layoutType == "QHBoxLayout") {
        layout = new QHBoxLayout(parentWidget);
    } else if (layoutType == "QGridLayout") {
        layout = new QGridLayout(parentWidget);
    } else if (layoutType == "QFormLayout") {
        layout = new QFormLayout(parentWidget);
    }

    if (!layout) {
        qDebug() << "Unsupported layout type:" << layoutType;
        return;
    }

    // 加载布局中的控件
    QDomNodeList layoutChildren = layoutElem.childNodes();
    for (int i = 0; i < layoutChildren.size(); ++i) {
        QDomNode childNode = layoutChildren.at(i);
        if (!childNode.isElement()) {
            continue;
        }

        QDomElement childElem = childNode.toElement();
        if (childElem.tagName() == "item") {
            // 加载布局项中的控件
            QDomNodeList itemChildren = childElem.childNodes();
            for (int j = 0; j < itemChildren.size(); ++j) {
                QDomNode itemChildNode = itemChildren.at(j);
                if (!itemChildNode.isElement()) {
                    continue;
                }

                QDomElement itemChildElem = itemChildNode.toElement();
                if (itemChildElem.tagName() == "widget") {
                    // 加载控件并将其添加到布局中
                    loadWidgetRecursive(itemChildElem, parentWidget, true);
                    // 获取最后一个添加的控件
                    QWidget *widget = nullptr;
                    if (!m_widgetItemMap.isEmpty()) {
                        widget = m_widgetItemMap.lastKey();
                    }
                    if (widget) {
                        layout->addWidget(widget);
                    }
                } else if (itemChildElem.tagName() == "layout") {
                    // 递归加载嵌套布局
                    loadLayoutRecursive(itemChildElem, parentWidget);
                }
            }
        }
    }

    // 设置布局到父窗口
    parentWidget->setLayout(layout);
}

QString UILayoutWindow::getCurrentLayoutPath() const
{
    return m_currentLayoutPath;
}

void UILayoutWindow::on_actionSave_Layout_triggered()
{
    QString filePath;
    if (m_currentLayoutPath.isEmpty()) {
        filePath = QFileDialog::getSaveFileName(this, "保存", "", "UI文件 (*.ui);;多界面文件 (*.json)");
        if (filePath.isEmpty()) {
            return;
        }
        // 确保UI文件的扩展名是.ui
        if (!filePath.endsWith(".ui") && !filePath.endsWith(".json")) {
            filePath += ".ui";
        }
        m_currentLayoutPath = filePath;
        setWindowTitle(QString("UI布局编辑器 - %1").arg(QFileInfo(m_currentLayoutPath).fileName()));
    } else {
        filePath = m_currentLayoutPath;
    }

    // 根据文件扩展名选择保存格式
    if (filePath.endsWith(".json")) {
        // 保存为多界面JSON格式
        if (!m_interfaceManager) return;
        QJsonDocument jsonDoc(m_interfaceManager->toJson());
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << jsonDoc.toJson(QJsonDocument::Indented);
            file.close();
            qDebug() << "多界面布局保存到" << filePath;
            // 保存后重置修改状态并更新UI布局路径 - 通过ProductConfigManager管理
            if (m_configManager) {
                m_configManager->setUiLayoutPath(filePath);
                m_configManager->setUiLayoutModified(false);
                qDebug() << "UI布局路径已更新到ProductConfigManager:" << filePath;
            }
        }
    } else {
        // 确保UI文件的扩展名是.ui
        if (!filePath.endsWith(".ui")) {
            filePath += ".ui";
        }
        // 保存为传统DOM格式（单个界面）
        QDomDocument doc;
        QDomElement root = doc.createElement("ui");
        root.setAttribute("version", "4.0");
        doc.appendChild(root);

        // 添加class元素
        QDomElement classElem = doc.createElement("class");
        classElem.appendChild(doc.createTextNode("MainWindow"));
        root.appendChild(classElem);

        // 添加MainWindow widget
        QDomElement mainWindow = doc.createElement("widget");
        mainWindow.setAttribute("class", "QMainWindow");
        mainWindow.setAttribute("name", "MainWindow");
        root.appendChild(mainWindow);

        // 添加MainWindow geometry
        QDomElement mainWindowGeometry = doc.createElement("property");
        mainWindowGeometry.setAttribute("name", "geometry");
        QDomElement mainWindowRect = doc.createElement("rect");
        QDomElement mainWindowX = doc.createElement("x");
        mainWindowX.appendChild(doc.createTextNode("0"));
        QDomElement mainWindowY = doc.createElement("y");
        mainWindowY.appendChild(doc.createTextNode("0"));
        QDomElement mainWindowWidth = doc.createElement("width");
        mainWindowWidth.appendChild(doc.createTextNode("800"));
        QDomElement mainWindowHeight = doc.createElement("height");
        mainWindowHeight.appendChild(doc.createTextNode("600"));
        mainWindowRect.appendChild(mainWindowX);
        mainWindowRect.appendChild(mainWindowY);
        mainWindowRect.appendChild(mainWindowWidth);
        mainWindowRect.appendChild(mainWindowHeight);
        mainWindowGeometry.appendChild(mainWindowRect);
        mainWindow.appendChild(mainWindowGeometry);

        // 添加MainWindow windowTitle
        QDomElement mainWindowTitle = doc.createElement("property");
        mainWindowTitle.setAttribute("name", "windowTitle");
        QDomElement mainWindowTitleString = doc.createElement("string");
        mainWindowTitleString.appendChild(doc.createTextNode("MainWindow"));
        mainWindowTitle.appendChild(mainWindowTitleString);
        mainWindow.appendChild(mainWindowTitle);

        // 添加centralWidget
        QDomElement centralWidget = doc.createElement("widget");
        centralWidget.setAttribute("class", "QWidget");
        centralWidget.setAttribute("name", "centralwidget");
        mainWindow.appendChild(centralWidget);

        // 递归保存控件和布局
        saveWidgetRecursive(centralWidget, m_editAreaWidget, doc);

        // 添加menubar
        QDomElement menubar = doc.createElement("widget");
        menubar.setAttribute("class", "QMenuBar");
        menubar.setAttribute("name", "menubar");
        mainWindow.appendChild(menubar);

        // 添加menubar geometry
        QDomElement menubarGeometry = doc.createElement("property");
        menubarGeometry.setAttribute("name", "geometry");
        QDomElement menubarRect = doc.createElement("rect");
        QDomElement menubarX = doc.createElement("x");
        menubarX.appendChild(doc.createTextNode("0"));
        QDomElement menubarY = doc.createElement("y");
        menubarY.appendChild(doc.createTextNode("0"));
        QDomElement menubarWidth = doc.createElement("width");
        menubarWidth.appendChild(doc.createTextNode("800"));
        QDomElement menubarHeight = doc.createElement("height");
        menubarHeight.appendChild(doc.createTextNode("20"));
        menubarRect.appendChild(menubarX);
        menubarRect.appendChild(menubarY);
        menubarRect.appendChild(menubarWidth);
        menubarRect.appendChild(menubarHeight);
        menubarGeometry.appendChild(menubarRect);
        menubar.appendChild(menubarGeometry);

        // 添加statusbar
        QDomElement statusbar = doc.createElement("widget");
        statusbar.setAttribute("class", "QStatusBar");
        statusbar.setAttribute("name", "statusbar");
        mainWindow.appendChild(statusbar);

        // 添加resources元素
        QDomElement resources = doc.createElement("resources");
        root.appendChild(resources);

        // 添加connections元素
        QDomElement connections = doc.createElement("connections");
        root.appendChild(connections);

        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            doc.save(out, 2); // 缩进2个空格
            file.close();
            qDebug() << "Layout saved to" << filePath;
            // 保存后重置修改状态并更新UI布局路径 - 通过ProductConfigManager管理
            if (m_configManager) {
                m_configManager->setUiLayoutPath(filePath);
                m_configManager->setUiLayoutModified(false);
                qDebug() << "UI布局路径已更新到ProductConfigManager:" << filePath;
            }
        }
    }
}

// 递归保存控件和布局的辅助函数
void UILayoutWindow::saveWidgetRecursive(QDomElement parentElem, QWidget *parentWidget, QDomDocument &doc) {
    if (!parentWidget) {
        return;
    }

    // 保存父控件的所有子控件
    QList<QWidget*> children = parentWidget->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly);
    for (QWidget *child : children) {
        // 跳过编辑区本身和控制点
        if (child == m_editAreaWidget || child->objectName() == "handle") {
            continue;
        }

        // 找到对应的LayoutItem
        LayoutItem *item = m_widgetItemMap.value(child);
        if (!item) {
            continue;
        }

        QString widgetType = item->widgetType();
        QString text = item->text();
        QPoint pos = item->pos();
        QSize size = item->size();

        // 创建widget元素
        QDomElement widgetElem = doc.createElement("widget");
        widgetElem.setAttribute("class", widgetType);
        widgetElem.setAttribute("name", child->objectName());

        // 添加geometry属性
        QDomElement geometryElem = doc.createElement("property");
        geometryElem.setAttribute("name", "geometry");
        QDomElement rectElem = doc.createElement("rect");
        QDomElement xElem = doc.createElement("x");
        xElem.appendChild(doc.createTextNode(QString::number(pos.x())));
        QDomElement yElem = doc.createElement("y");
        yElem.appendChild(doc.createTextNode(QString::number(pos.y())));
        QDomElement widthElem = doc.createElement("width");
        widthElem.appendChild(doc.createTextNode(QString::number(size.width())));
        QDomElement heightElem = doc.createElement("height");
        heightElem.appendChild(doc.createTextNode(QString::number(size.height())));
        rectElem.appendChild(xElem);
        rectElem.appendChild(yElem);
        rectElem.appendChild(widthElem);
        rectElem.appendChild(heightElem);
        geometryElem.appendChild(rectElem);
        widgetElem.appendChild(geometryElem);

        // 添加text属性
        if (!text.isEmpty()) {
            QDomElement textElem = doc.createElement("property");
            textElem.setAttribute("name", "text");
            QDomElement stringElem = doc.createElement("string");
            stringElem.appendChild(doc.createTextNode(text));
            textElem.appendChild(stringElem);
            widgetElem.appendChild(textElem);
        }

        // 检查子控件是否有布局
        QLayout *layout = child->layout();
        if (layout) {
            // 创建layout元素
            QString layoutType = layout->metaObject()->className();
            layoutType = layoutType.replace("Layout", ""); // 移除Layout后缀
            QDomElement layoutElem = doc.createElement("layout");
            layoutElem.setAttribute("class", layoutType);

            // 保存布局中的所有控件
            int itemCount = layout->count();
            for (int i = 0; i < itemCount; ++i) {
                QLayoutItem *layoutItem = layout->itemAt(i);
                if (!layoutItem) {
                    continue;
                }

                QWidget *layoutWidget = layoutItem->widget();
                if (layoutWidget) {
                    // 创建item元素
                    QDomElement itemElem = doc.createElement("item");

                    // 递归保存布局中的控件
                    saveWidgetRecursive(itemElem, layoutWidget, doc);

                    layoutElem.appendChild(itemElem);
                }
            }

            widgetElem.appendChild(layoutElem);
        } else {
            // 没有布局，直接保存子控件
            saveWidgetRecursive(widgetElem, child, doc);
        }

        parentElem.appendChild(widgetElem);
    }
}

void UILayoutWindow::on_actionSave_As_Layout_triggered()
{
    QString filePath = QFileDialog::getSaveFileName(this, "另存为", "", "UI文件 (*.ui);;多界面文件 (*.json)");
    if (filePath.isEmpty()) {
        return;
    }
    m_currentLayoutPath = filePath;
    setWindowTitle(QString("UI布局编辑器 - %1").arg(QFileInfo(m_currentLayoutPath).fileName()));
    on_actionSave_Layout_triggered();
}

void UILayoutWindow::saveLayoutState()
{
    // 保存当前布局状态到撤销栈
    QJsonArray itemsArray;
    for (LayoutItem *item : m_layoutItems) {
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
    QJsonDocument jsonDoc(itemsArray);
    QByteArray stateData = jsonDoc.toJson(QJsonDocument::Compact);
    
    // 限制撤销栈的大小
    if (m_undoStack.size() >= m_maxUndoSteps) {
        m_undoStack.removeFirst();
    }
    m_undoStack.append(stateData);
    
    // 通知ProductConfigManager UI布局已修改
    if (m_configManager) {
        m_configManager->setUiLayoutModified(true);
        qDebug() << "UILayoutWindow: UI布局修改，通知ProductConfigManager设置UI布局修改状态";
    }

    // 清空重做栈
    m_redoStack.clear();
}

bool UILayoutWindow::restoreLayoutState(QList<QByteArray> &stack)
{
    if (stack.isEmpty()) {
        return false;
    }
    
    // 保存当前状态
    QByteArray currentState;
    QJsonArray currentItemsArray;
    for (LayoutItem *item : m_layoutItems) {
        QJsonObject itemObj;
        itemObj["widgetType"] = item->widgetType();
        itemObj["text"] = item->text();
        itemObj["x"] = item->pos().x();
        itemObj["y"] = item->pos().y();
        itemObj["width"] = item->size().width();
        itemObj["height"] = item->size().height();
        itemObj["tabIndex"] = item->tabIndex();
        currentItemsArray.append(itemObj);
    }
    QJsonDocument currentJsonDoc(currentItemsArray);
    currentState = currentJsonDoc.toJson(QJsonDocument::Compact);
    
    // 根据栈的类型决定保存到哪个栈
    if (stack == m_undoStack) {
        // 撤销操作，保存到重做栈
        m_redoStack.append(currentState);
    } else if (stack == m_redoStack) {
        // 重做操作，保存到撤销栈
        m_undoStack.append(currentState);
    }
    
    // 获取要恢复的状态
    QByteArray stateData = stack.last();
    stack.removeLast();
    
    // 清空当前布局
    for (auto it = m_widgetItemMap.begin(); it != m_widgetItemMap.end(); ++it) {
        QWidget *widget = it.key();
        delete widget;
        delete it.value();
    }
    m_widgetItemMap.clear();
    m_layoutItems.clear();
    m_selectedWidget = nullptr;
    
    // 解析并恢复布局
    QJsonParseError parseError;
    QJsonDocument restoreJsonDoc = QJsonDocument::fromJson(stateData, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "解析JSON失败:" << parseError.errorString();
        return false;
    }
    
    QJsonArray restoreItemsArray = restoreJsonDoc.array();
    for (const QJsonValue &itemValue : restoreItemsArray) {
        QJsonObject itemObj = itemValue.toObject();
        QString widgetType = itemObj["widgetType"].toString();
        QString text = itemObj["text"].toString();
        int x = itemObj["x"].toInt();
        int y = itemObj["y"].toInt();
        int width = itemObj["width"].toInt();
        int height = itemObj["height"].toInt();
        int tabIndex = itemObj["tabIndex"].toInt();
        
        // 创建 LayoutItem
        LayoutItem *layoutItem = new LayoutItem(widgetType, text);
        layoutItem->setPos(QPoint(x, y));
        layoutItem->setSize(QSize(width, height));
        layoutItem->setTabIndex(tabIndex);
        
        // 创建控件并添加到编辑区
        addWidgetToEditArea(layoutItem, m_editAreaWidget);
    }
    
    // 如果没有控件，重置选中的控件并隐藏控制点
    if (m_layoutItems.isEmpty()) {
        qDebug() << "restoreLayoutState: m_layoutItems is empty, hiding handles";
        m_selectedWidget = nullptr;
        m_editAreaWidget->setHandles(QList<QRect>());
    } else {
        qDebug() << "restoreLayoutState: m_layoutItems has" << m_layoutItems.size() << "items";
    }
    
    m_editAreaWidget->repaint();
    return true;
}

void UILayoutWindow::on_actionUndo_triggered()
{
    // 实现撤销功能
    restoreLayoutState(m_undoStack);
}

void UILayoutWindow::on_actionRedo_triggered()
{
    // 实现重做功能
    restoreLayoutState(m_redoStack);
}

void UILayoutWindow::on_actionLoad_Layout_triggered()
{
    QString filePath = QFileDialog::getOpenFileName(this, "加载UI布局", "", "UI文件 (*.ui)");
    if (filePath.isEmpty()) {
        return;
    }

    if (!loadLayout(filePath)) {
        QMessageBox::critical(this, "错误", "无法加载布局文件: " + filePath);
    }
}



void UILayoutWindow::on_actionNew_Layout_triggered()
{
    // 保存当前状态到撤销栈
    qDebug()<<"on_actionNew_Layout_triggered: 保存当前状态到撤销栈";
    saveLayoutState();
    
    // 清除当前场景
    for (auto it = m_widgetItemMap.begin(); it != m_widgetItemMap.end(); ++it) {
        QWidget *widget = it.key();
        delete widget;
        delete it.value();
    }
    m_widgetItemMap.clear();
    m_layoutItems.clear();
    m_selectedWidget = nullptr;
    m_editAreaWidget->setHandles(QList<QRect>());
    m_editAreaWidget->repaint();
    
    // 清空重做栈
    m_redoStack.clear();
    
    // 新建布局，初始路径为空
    m_currentLayoutPath = "";
    setWindowTitle(QString("UI布局编辑器 - 新建布局.ui"));
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
    // 设置一个简单的布局管理器，避免布局计算错误
    QVBoxLayout *layout = new QVBoxLayout(m_editAreaWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    m_editAreaWidget->setLayout(layout);
    m_editAreaWidget->setStyleSheet("background-color: #f0f0f0; border: 1px solid #ccc;");

    // 安全地替换splitter中的QGraphicsView为我们的编辑区widget
    if (ui->graphicsView && ui->splitter) {
        int index = ui->splitter->indexOf(ui->graphicsView);
        if (index != -1) {
            ui->splitter->replaceWidget(index, m_editAreaWidget);
            // 安全删除，避免重复删除
            ui->graphicsView->setParent(nullptr);
            ui->graphicsView->deleteLater();
            ui->graphicsView = nullptr;
            qDebug() << "UILayoutWindow: 成功替换graphicsView为editAreaWidget";
        } else {
            qDebug() << "UILayoutWindow: graphicsView不在splitter中，直接添加到splitter";
            ui->splitter->addWidget(m_editAreaWidget);
        }
    } else {
        qDebug() << "UILayoutWindow: graphicsView或splitter为nullptr，直接添加到splitter";
        if (ui->splitter) {
            ui->splitter->addWidget(m_editAreaWidget);
        } else {
            qCritical() << "UILayoutWindow: splitter为nullptr，无法添加editAreaWidget";
        }
    }

    // 连接双击事件信号
    connect(m_editAreaWidget, &EditAreaWidget::doubleClicked, this, &UILayoutWindow::handleDoubleClick);
}

void UILayoutWindow::handleDoubleClick(const QPoint &pos)
{
    // 获取点击位置的控件
    QWidget *widget = m_editAreaWidget->childAt(pos);
    
    // 处理QLabel
    QLabel *label = qobject_cast<QLabel*>(widget);
    if (label) {
        // 创建一个临时的QLineEdit来编辑文本
        QLineEdit *edit = new QLineEdit(m_editAreaWidget);
        edit->setText(label->text());
        // 设置位置和大小与QLabel相同
        QPoint globalPos = label->mapTo(m_editAreaWidget, QPoint(0, 0));
        edit->setGeometry(globalPos.x(), globalPos.y(), label->width(), label->height());
        edit->selectAll();
        edit->setFocusPolicy(Qt::StrongFocus);
        edit->setStyleSheet("border: 2px solid blue; background-color: white;");
        edit->raise();
        edit->show();
        edit->activateWindow();
        edit->setFocus();
        
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
        return;
    }

    // 处理QTabWidget
    QTabWidget *tabWidget = qobject_cast<QTabWidget*>(widget);
    if (tabWidget) {
        qDebug() << "处理QTabWidget双击事件:" << tabWidget;
        
        // 找到点击的Tab页索引
        QPoint localPos = tabWidget->mapFromParent(pos);
        QRect tabBarRect = tabWidget->tabBar()->geometry();
        if (tabBarRect.contains(localPos)) {
            QPoint tabBarPos = tabWidget->tabBar()->mapFromParent(localPos);
            int tabIndex = tabWidget->tabBar()->tabAt(tabBarPos);
            if (tabIndex != -1) {
                qDebug() << "双击Tab页索引:" << tabIndex << "当前标题:" << tabWidget->tabText(tabIndex);
                
                // 创建临时QLineEdit用于编辑标题
                m_tabTitleEdit = new QLineEdit(m_editAreaWidget);
                m_tabTitleEdit->setText(tabWidget->tabText(tabIndex));
                // 设置位置和大小
                QPoint globalTabPos = tabWidget->tabBar()->mapTo(m_editAreaWidget, QPoint());
                QRect tabRect = tabWidget->tabBar()->tabRect(tabIndex);
                m_tabTitleEdit->setGeometry(globalTabPos.x() + tabRect.x(), globalTabPos.y() + tabRect.y(), tabRect.width(), tabRect.height());
                m_tabTitleEdit->selectAll();
                m_tabTitleEdit->setFocusPolicy(Qt::StrongFocus);
                m_tabTitleEdit->setStyleSheet("border: 2px solid blue; background-color: white; padding: 0;");
                m_tabTitleEdit->raise();
                m_tabTitleEdit->show();
                
                // 确保获得焦点
                m_tabTitleEdit->activateWindow();
                m_tabTitleEdit->setFocus();
                
                // 记录当前正在编辑的TabWidget和索引
                m_editingTabWidget = tabWidget;
                
                // 连接信号槽，在编辑完成后更新Tab页标题
                connect(m_tabTitleEdit, &QLineEdit::editingFinished, [=]() {
                    if (m_editingTabWidget && m_tabTitleEdit) {
                        // 更新Tab页标题
                        m_editingTabWidget->setTabText(tabIndex, m_tabTitleEdit->text());
                        qDebug() << "编辑完成，更新Tab页标题为:" << m_tabTitleEdit->text();
                        
                        // 清理
                        m_tabTitleEdit->deleteLater();
                        m_tabTitleEdit = nullptr;
                        m_editingTabWidget = nullptr;
                    }
                });
                return;
            }
        }
        return;
    }

    // 如果没有找到直接的QLabel或QTabWidget，可能点击在TabWidget的子控件上
    // 递归查找父控件
    QWidget *parentWidget = widget;
    while (parentWidget && parentWidget != m_editAreaWidget) {
        parentWidget = parentWidget->parentWidget();
        
        tabWidget = qobject_cast<QTabWidget*>(parentWidget);
        if (tabWidget) {
            // 找到点击的Tab页索引
            QPoint localPos = tabWidget->mapFromParent(pos);
            QRect tabBarRect = tabWidget->tabBar()->geometry();
            if (tabBarRect.contains(localPos)) {
                QPoint tabBarPos = tabWidget->tabBar()->mapFromParent(localPos);
                int tabIndex = tabWidget->tabBar()->tabAt(tabBarPos);
                if (tabIndex != -1) {
                    // 创建临时QLineEdit用于编辑标题
                    m_tabTitleEdit = new QLineEdit(m_editAreaWidget);
                    m_tabTitleEdit->setText(tabWidget->tabText(tabIndex));
                    // 设置位置和大小
                    QPoint globalTabPos = tabWidget->tabBar()->mapTo(m_editAreaWidget, QPoint());
                    QRect tabRect = tabWidget->tabBar()->tabRect(tabIndex);
                    m_tabTitleEdit->setGeometry(globalTabPos.x() + tabRect.x(), globalTabPos.y() + tabRect.y(), tabRect.width(), tabRect.height());
                    m_tabTitleEdit->selectAll();
                    m_tabTitleEdit->setFocusPolicy(Qt::StrongFocus);
                    m_tabTitleEdit->setStyleSheet("border: 2px solid blue; background-color: white; padding: 0;");
                    m_tabTitleEdit->raise();
                    m_tabTitleEdit->show();
                    
                    // 确保获得焦点
                    m_tabTitleEdit->activateWindow();
                    m_tabTitleEdit->setFocus();
                    
                    // 记录当前正在编辑的TabWidget和索引
                    m_editingTabWidget = tabWidget;
                    
                    // 连接信号槽，在编辑完成后更新Tab页标题
                    connect(m_tabTitleEdit, &QLineEdit::editingFinished, [=]() {
                        if (m_editingTabWidget && m_tabTitleEdit) {
                            // 更新Tab页标题
                            m_editingTabWidget->setTabText(tabIndex, m_tabTitleEdit->text());
                            qDebug() << "编辑完成，更新Tab页标题为:" << m_tabTitleEdit->text();
                            
                            // 清理
                            m_tabTitleEdit->deleteLater();
                            m_tabTitleEdit = nullptr;
                            m_editingTabWidget = nullptr;
                        }
                    });
                }
            }
            return;
        }
    }
}

void UILayoutWindow::initWidgetLibrary()
{
    // 设置树控件不显示列头
    ui->widgetListWidget->setHeaderHidden(true);

    // 创建分类节点
    QTreeWidgetItem *layoutsNode = new QTreeWidgetItem(ui->widgetListWidget, QStringList() << "Layouts");
    QTreeWidgetItem *spacersNode = new QTreeWidgetItem(ui->widgetListWidget, QStringList() << "Spacers");
    QTreeWidgetItem *buttonsNode = new QTreeWidgetItem(ui->widgetListWidget, QStringList() << "Buttons");
    QTreeWidgetItem *itemViewsNode = new QTreeWidgetItem(ui->widgetListWidget, QStringList() << "Item Views (Model-Based)");
    QTreeWidgetItem *itemWidgetsNode = new QTreeWidgetItem(ui->widgetListWidget, QStringList() << "Item Widgets (Item-Based)");
    QTreeWidgetItem *containersNode = new QTreeWidgetItem(ui->widgetListWidget, QStringList() << "Containers");
    QTreeWidgetItem *inputWidgetsNode = new QTreeWidgetItem(ui->widgetListWidget, QStringList() << "Input Widgets");
    QTreeWidgetItem *displayWidgetsNode = new QTreeWidgetItem(ui->widgetListWidget, QStringList() << "Display Widgets");

    // 添加Layouts分类
    QStringList layoutsTypes = {
        "Vertical Layout",
        "Horizontal Layout",
        "Grid Layout",
        "Form Layout"
    };
    foreach (const QString &type, layoutsTypes) {
        new QTreeWidgetItem(layoutsNode, QStringList() << type);
    }

    // 添加Spacers分类
    QStringList spacersTypes = {
        "Horizontal Spacer",
        "Vertical Spacer"
    };
    foreach (const QString &type, spacersTypes) {
        new QTreeWidgetItem(spacersNode, QStringList() << type);
    }

    // 添加Buttons分类
    QStringList buttonsTypes = {
        "Push Button",
        "Tool Button",
        "Radio Button",
        "Check Box",
        "Command Link Button",
        "Dialog Button Box"
    };
    foreach (const QString &type, buttonsTypes) {
        QTreeWidgetItem *item = new QTreeWidgetItem(buttonsNode, QStringList() << type);
    }

    // 添加Item Views (Model-Based)分类
    QStringList itemViewsTypes = {
        "List View",
        "Tree View",
        "Table View",
        "Column View",
        "Undo View"
    };
    foreach (const QString &type, itemViewsTypes) {
        new QTreeWidgetItem(itemViewsNode, QStringList() << type);
    }

    // 添加Item Widgets (Item-Based)分类
    QStringList itemWidgetsTypes = {
        "List Widget",
        "Tree Widget",
        "Table Widget"
    };
    foreach (const QString &type, itemWidgetsTypes) {
        new QTreeWidgetItem(itemWidgetsNode, QStringList() << type);
    }

    // 添加Containers分类
    QStringList containersTypes = {
        "Group Box",
        "Scroll Area",
        "Tool Box",
        "Tab Widget",
        "Stacked Widget",
        "Frame",
        "Widget",
        "MDI Area",
        "Dock Widget",
        "QAxWidget"
    };
    foreach (const QString &type, containersTypes) {
        new QTreeWidgetItem(containersNode, QStringList() << type);
    }

    // 添加Input Widgets分类
    QStringList inputWidgetsTypes = {
        "Combo Box",
        "Font Combo Box",
        "Line Edit",
        "Text Edit",
        "Plain Text Edit",
        "Spin Box",
        "Double Spin Box",
        "Time Edit",
        "Date Edit",
        "Date/Time Edit",
        "Dial",
        "Horizontal Scroll Bar",
        "Vertical Scroll Bar",
        "Horizontal Slider",
        "Vertical Slider",
        "Key Sequence Edit"
    };
    foreach (const QString &type, inputWidgetsTypes) {
        new QTreeWidgetItem(inputWidgetsNode, QStringList() << type);
    }

    // 添加Display Widgets分类
    QStringList displayWidgetsTypes = {
        "Label",
        "Text Browser",
        "Graphics View",
        "Calendar Widget",
        "LCD Number",
        "Progress Bar",
        "Horizontal Line",
        "Vertical Line",
        "OpenGL Widget",
        "QQuickWidget"
    };
    foreach (const QString &type, displayWidgetsTypes) {
        QTreeWidgetItem *item = new QTreeWidgetItem(displayWidgetsNode, QStringList() << type);
    }

    // 展开所有分类
    ui->widgetListWidget->expandAll();



}

void UILayoutWindow::on_widgetListWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    // 当用户双击控件库中的控件时，直接在场景中央添加该控件
    if (!item) {
        return;
    }

    QString widgetType = item->text(column);
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
    // 处理QDockWidget位置和状态变化
    QDockWidget *dockWidget = qobject_cast<QDockWidget*>(obj);
    if (dockWidget) {
        if (event->type() == QEvent::WindowStateChange) {
            // 更新对应的LayoutItem
            LayoutItem *item = m_widgetItemMap.value(dockWidget);
            if (item) {
                // 获取停靠区域
                Qt::DockWidgetArea area = Qt::NoDockWidgetArea;
                QMainWindow *mainWindow = qobject_cast<QMainWindow*>(dockWidget->parentWidget());
                if (mainWindow) {
                    area = mainWindow->dockWidgetArea(dockWidget);
                }
                item->setDockArea(area);
                item->setFloating(dockWidget->isFloating());
            }
        }
    }
    // 处理控件的双击事件
    if (event->type() == QEvent::MouseButtonDblClick) {
        qDebug() << "双击事件触发，对象类型:" << obj->metaObject()->className();
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            // 记录鼠标位置
            m_lastMousePos = mouseEvent->globalPosition().toPoint();
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
            
            // 处理QTabWidget的Tab页标题编辑
            QTabWidget *tabWidget = qobject_cast<QTabWidget*>(obj);
            if (tabWidget) {
                qDebug() << "处理QTabWidget双击事件:" << tabWidget;
                
                // 将事件转换为QMouseEvent
                QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
                // 获取TabBar
                QTabBar *tabBar = tabWidget->tabBar();
                // 获取鼠标双击的位置
                QPoint mousePos = mouseEvent->pos();
                // 检查鼠标位置是否在TabBar上
                if (tabBar && tabBar->rect().contains(mousePos)) {
                    // 获取被双击的Tab页索引
                    int tabIndex = tabBar->tabAt(mousePos);
                    if (tabIndex != -1) {
                        qDebug() << "双击Tab页索引:" << tabIndex << "当前标题:" << tabWidget->tabText(tabIndex);
                        
                        // 创建临时QLineEdit用于编辑标题
                        m_tabTitleEdit = new QLineEdit(tabWidget);
                        m_tabTitleEdit->setText(tabWidget->tabText(tabIndex));
                        // 设置位置和大小
                        QRect tabRect = tabBar->tabRect(tabIndex);
                        m_tabTitleEdit->setGeometry(tabRect);
                        m_tabTitleEdit->selectAll();
                        m_tabTitleEdit->setFocusPolicy(Qt::StrongFocus);
                        m_tabTitleEdit->setStyleSheet("border: 2px solid blue; background-color: white; padding: 0;");
                        m_tabTitleEdit->raise();
                        m_tabTitleEdit->show();
                        
                        // 确保获得焦点
                        m_tabTitleEdit->activateWindow();
                        m_tabTitleEdit->setFocus();
                        
                        // 记录当前正在编辑的TabWidget
                        m_editingTabWidget = tabWidget;
                        
                        // 连接信号槽，在编辑完成后更新Tab页标题
                        connect(m_tabTitleEdit, &QLineEdit::editingFinished, [=]() {
                            if (m_editingTabWidget && m_tabTitleEdit) {
                                // 更新Tab页标题
                                m_editingTabWidget->setTabText(tabIndex, m_tabTitleEdit->text());
                                qDebug() << "编辑完成，更新Tab页标题为:" << m_tabTitleEdit->text();
                                
                                // 更新对应的LayoutItem的文本属性
                                LayoutItem *item = m_widgetItemMap.value(m_editingTabWidget);
                                if (item) {
                                    // 注意：TabWidget的文本属性可能需要特殊处理，这里暂时不更新
                                    // 因为TabWidget的文本可能不是直接存储在LayoutItem中的
                                }
                                
                                // 清理
                                m_tabTitleEdit->deleteLater();
                                m_tabTitleEdit = nullptr;
                                m_editingTabWidget = nullptr;
                            }
                        });
                        
                        return true;
                    }
                }
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
                    // 保存当前状态到撤销栈
                    qDebug()<<"onMousePress: 保存当前状态到撤销栈";
                    saveLayoutState();
                    
                    // 选择该控件并更新控制点
                    m_selectedWidget = widget;
                    updateHandles(widget);
                    updatePropertiesEditor(m_selectedWidget);
                    update(); // 重新绘制
                }
                
                m_currentWidget = widget;
                m_mousePressPos = mouseEvent->globalPosition().toPoint();
                m_widgetPos = widget->pos();
                
                return true;
            }
        }
    }
    
    // 处理双击编辑事件
    if (event->type() == QEvent::MouseButtonDblClick) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        QWidget *widget = qobject_cast<QWidget*>(obj);
        if (widget) {
            // 判断控件类型
            if (QLabel *label = qobject_cast<QLabel*>(widget)) {
                // 编辑QLabel文本
                bool ok;
                QString newText = QInputDialog::getText(this, "编辑文本", "请输入新的文本:", QLineEdit::Normal, label->text(), &ok);
                if (ok && !newText.isEmpty()) {
                    label->setText(newText);
                    // 更新LayoutItem
                    LayoutItem *item = m_widgetItemMap.value(widget);
                    if (item) {
                        item->setText(newText);
                    }
                }
            } else if (QTabWidget *tabWidget = qobject_cast<QTabWidget*>(widget)) {
                // 编辑当前选中的Tab页标题
                int currentIndex = tabWidget->currentIndex();
                if (currentIndex != -1) {
                    QString currentText = tabWidget->tabText(currentIndex);
                    bool ok;
                    QString newText = QInputDialog::getText(this, "编辑Tab页标题", "请输入新的标题:", QLineEdit::Normal, currentText, &ok);
                    if (ok && !newText.isEmpty()) {
                        tabWidget->setTabText(currentIndex, newText);
                        // 注意：Tab页标题不会保存在LayoutItem中，因为LayoutItem主要保存控件的基本属性
                    }
                }
            }
            return true;
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
                    QPoint delta = mouseEvent->globalPosition().toPoint() - m_mousePressPos;
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
                    m_mousePressPos = mouseEvent->globalPosition().toPoint();
                    return true;
                } else if (m_moving) {
                    // 移动控件
                    QPoint delta = mouseEvent->globalPosition().toPoint() - m_mousePressPos;
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
            // 保存当前状态到撤销栈
            qDebug()<<"onLayoutItemDoubleClicked: 保存当前状态到撤销栈";
            saveLayoutState();
            label->setText(text);
            
            // 更新LayoutItem的文本
            LayoutItem *item = m_widgetItemMap.value(m_selectedWidget);
            if (item) {
                item->setText(text);
            }
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
                    // 保存当前状态到撤销栈
                    qDebug()<<"onLayoutItemDoubleClicked: 保存当前状态到撤销栈";
                    saveLayoutState();
                    tabWidget->setTabText(tabIndex, text);
                    
                    // 更新LayoutItem的文本
                    LayoutItem *item = m_widgetItemMap.value(m_selectedWidget);
                    if (item) {
                        item->setText(text);
                    }
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

// 处理编辑区双击事件
void UILayoutWindow::onEditAreaDoubleClicked(const QPoint &pos) {
    // 将点击位置转换为编辑区的本地坐标
    QPoint localPos = pos;
    
    // 遍历所有控件，找到被双击的控件
    for (QWidget *widget : m_widgetItemMap.keys()) {
        if (widget->geometry().contains(localPos)) {
            // 保存当前状态到撤销栈
            qDebug()<<"onEditAreaDoubleClicked: 保存当前状态到撤销栈";
            saveLayoutState();
            
            // 选择该控件并更新控制点
            m_selectedWidget = widget;
            updateHandles(widget);
            updatePropertiesEditor(m_selectedWidget);
            update(); // 重新绘制
            
            // 处理双击编辑
            onLayoutItemDoubleClicked();
            return;
        }
    }
}

// 处理控制点释放
void UILayoutWindow::onHandleReleased() {
    // 可以在这里添加释放后的处理逻辑
}

// 处理窗口关闭事件
void UILayoutWindow::closeEvent(QCloseEvent *event)
{
    // 检查是否有未保存的修改 - 通过ProductConfigManager管理
    bool isModified = m_configManager ? m_configManager->isUiLayoutModified() : false;
    qDebug() << "UILayoutWindow::closeEvent - isUiLayoutModified:" << isModified;
    
    if (isModified) {
        QMessageBox::StandardButton button = QMessageBox::question(this, 
            "保存更改", 
            "UI布局有未保存的更改。是否保存？",
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        
        if (button == QMessageBox::Save) {
            // 保存UI布局文件
            if (m_currentLayoutPath.isEmpty()) {
                // 如果还没有保存过，弹出另存为对话框
                QString fileName = QFileDialog::getSaveFileName(this, 
                    "保存UI布局文件", 
                    QDir::current().filePath("untitled.ui"), 
                    "UI Files (*.ui)");
                
                if (fileName.isEmpty()) {
                    // 用户取消了保存，取消关闭
                    event->ignore();
                    return;
                }
                
                m_currentLayoutPath = fileName;
            }
            
            // 保存UI布局文件
            on_actionSave_Layout_triggered();
            qDebug() << "UI布局文件已自动保存:" << m_currentLayoutPath;
            
            // 更新ProductConfigManager中的UI布局路径
            if (m_configManager) {
                m_configManager->setUiLayoutPath(m_currentLayoutPath);
                qDebug() << "UI布局路径已更新到ProductConfigManager:" << m_currentLayoutPath;
            }
            
            // 将UI布局路径写入产品JSON文件
            if (!m_productFilePath.isEmpty() && !m_currentLayoutPath.isEmpty()) {
                // 读取产品JSON文件
                QFile productFile(m_productFilePath);
                if (productFile.open(QIODevice::ReadWrite | QIODevice::Text)) {
                    QJsonDocument jsonDoc = QJsonDocument::fromJson(productFile.readAll());
                    if (!jsonDoc.isNull()) {
                        QJsonObject productObj = jsonDoc.object();
                        // 更新uiLayoutPath字段
                        productObj["uiLayoutPath"] = m_currentLayoutPath;
                        
                        // 写回文件
                        productFile.resize(0); // 清空文件
                        productFile.write(QJsonDocument(productObj).toJson(QJsonDocument::Indented));
                        qDebug() << "UI布局路径已写入产品JSON文件:" << m_currentLayoutPath;
                    }
                    productFile.close();
                }
            }
        } else if (button == QMessageBox::Discard) {
            // 用户选择丢弃保存，重置修改状态 - 通过ProductConfigManager管理
            if (m_configManager) {
                m_configManager->setUiLayoutModified(false);
                m_configManager->setUiLayoutChanged(false);
                qDebug() << "UILayoutWindow: 用户选择丢弃保存，重置UI布局修改状态和路径改变状态";
            }
        } else if (button == QMessageBox::Cancel) {
            // 取消关闭
            event->ignore();
            return;
        }
    } else {
        // 只有在有UI布局路径且产品文件路径有效时，才写入路径信息
        // 但仅在真正有修改时才需要写入，避免只是打开界面就触发保存
        qDebug() << "UILayoutWindow: 没有修改，不执行保存操作";
    }
    
    // 接受关闭事件
    event->accept();
}

// 产品配置集成功能实现
void UILayoutWindow::updateProductContext()
{
    if (!m_configManager) {
        return;
    }
    
    // 获取产品功能特性
    Product product = m_configManager->getProduct();
    QList<ProductFeature> features = product.features();
    
    // 更新UI以反映产品上下文
    // 例如：在控件库中高亮显示与产品功能相关的控件
    // 或者根据产品类型调整可用的控件类型
    
    qDebug() << "UILayoutWindow: 更新产品上下文，产品功能数量:" << features.size();
}

void UILayoutWindow::syncWithProductFeatures()
{
    if (!m_configManager) {
        return;
    }
    
    // 获取产品功能特性
    Product product = m_configManager->getProduct();
    QList<ProductFeature> features = product.features();
    
    // 获取现有的控件绑定
    QMap<QString, QString> bindings = m_configManager->getFeatureWidgetBindings();
    
    // 同步功能特性与UI控件
    for (const ProductFeature &feature : features) {
        QString featureName = feature.name;
        QString widgetId = bindings.value(featureName);
        
        if (!widgetId.isEmpty()) {
            // 查找对应的控件
            for (QWidget *widget : m_widgetItemMap.keys()) {
                QString currentWidgetId = widget->objectName();
                if (currentWidgetId == widgetId) {
                    // 更新控件文本以匹配功能特性
                    if (QLabel *label = qobject_cast<QLabel*>(widget)) {
                        label->setText(featureName);
                    } else if (QPushButton *button = qobject_cast<QPushButton*>(widget)) {
                        button->setText(featureName);
                    }
                    
                    // 更新LayoutItem
                    LayoutItem *item = m_widgetItemMap.value(widget);
                    if (item) {
                        item->setText(featureName);
                    }
                    break;
                }
            }
        }
    }
    
    qDebug() << "UILayoutWindow: 与产品功能特性同步完成";
}

void UILayoutWindow::applyLayoutTemplate(const QString &templateName)
{
    if (!m_configManager) {
        return;
    }
    
    QString templatePath = m_configManager->getLayoutTemplate(templateName);
    if (!templatePath.isEmpty()) {
        // 加载模板布局
        if (loadLayout(templatePath)) {
            qDebug() << "UILayoutWindow: 应用布局模板:" << templateName;
        }
    }
}

void UILayoutWindow::suggestLayoutForFeatures()
{
    if (!m_configManager) {
        return;
    }
    
    Product product = m_configManager->getProduct();
    QList<ProductFeature> features = product.features();
    
    if (features.isEmpty()) {
        return;
    }
    
    // 根据功能特性数量和建议布局
    int featureCount = features.size();
    
    if (featureCount <= 3) {
        // 简单布局建议：单列
        suggestSimpleColumnLayout();
    } else if (featureCount <= 6) {
        // 中等布局建议：两列
        suggestTwoColumnLayout();
    } else {
        // 复杂布局建议：Tab页或网格
        suggestTabbedLayout();
    }
}

void UILayoutWindow::suggestSimpleColumnLayout()
{
    if (!m_configManager) {
        return;
    }
    
    Product product = m_configManager->getProduct();
    QList<ProductFeature> features = product.features();
    
    // 清空当前布局
    clearLayout();
    
    // 创建垂直布局
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setSpacing(10);
    layout->setContentsMargins(20, 20, 20, 20);
    
    // 为每个功能特性创建对应的控件
    for (const ProductFeature &feature : features) {
        QString widgetType = suggestWidgetTypeForFeature(feature);
        
        // 创建控件
        QWidget *widget = createWidgetForFeature(feature, widgetType);
        if (widget) {
            layout->addWidget(widget);
        }
    }
    
    // 添加弹性空间
    layout->addStretch();
    
    // 应用布局到中央控件
    QWidget *centralWidget = ui->graphicsView;
    if (centralWidget) {
        centralWidget->setLayout(layout);
    }
    
    QMessageBox::information(this, "布局建议", "已应用简单单列布局建议");
}

void UILayoutWindow::suggestTwoColumnLayout()
{
    if (!m_configManager) {
        return;
    }
    
    Product product = m_configManager->getProduct();
    QList<ProductFeature> features = product.features();
    
    // 清空当前布局
    clearLayout();
    
    // 创建水平布局作为主布局
    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    
    // 创建两列垂直布局
    QVBoxLayout *leftColumn = new QVBoxLayout();
    QVBoxLayout *rightColumn = new QVBoxLayout();
    
    leftColumn->setSpacing(10);
    rightColumn->setSpacing(10);
    
    // 将功能特性分配到两列
    int halfCount = features.size() / 2;
    
    for (int i = 0; i < features.size(); ++i) {
        const ProductFeature &feature = features[i];
        QString widgetType = suggestWidgetTypeForFeature(feature);
        QWidget *widget = createWidgetForFeature(feature, widgetType);
        
        if (widget) {
            if (i < halfCount) {
                leftColumn->addWidget(widget);
            } else {
                rightColumn->addWidget(widget);
            }
        }
    }
    
    // 添加弹性空间
    leftColumn->addStretch();
    rightColumn->addStretch();
    
    // 将两列添加到主布局
    mainLayout->addLayout(leftColumn);
    mainLayout->addLayout(rightColumn);
    
    // 设置两列的比例
    mainLayout->setStretchFactor(leftColumn, 1);
    mainLayout->setStretchFactor(rightColumn, 1);
    
    // 应用布局到中央控件
    QWidget *centralWidget = ui->graphicsView;
    if (centralWidget) {
        centralWidget->setLayout(mainLayout);
    }
    
    QMessageBox::information(this, "布局建议", "已应用两列布局建议");
}

void UILayoutWindow::suggestTabbedLayout()
{
    if (!m_configManager) {
        return;
    }
    
    Product product = m_configManager->getProduct();
    QList<ProductFeature> features = product.features();
    
    // 清空当前布局
    clearLayout();
    
    // 创建Tab控件
    QTabWidget *tabWidget = new QTabWidget();
    
    // 根据功能特性分组创建Tab页
    QMap<QString, QList<ProductFeature>> featureGroups;
    
    // 按功能特性名称的第一个字符分组（简单分组策略）
    for (const ProductFeature &feature : features) {
        QString groupKey = feature.name.left(1).toUpper();
        featureGroups[groupKey].append(feature);
    }
    
    // 如果分组太多，合并为更少的组
    if (featureGroups.size() > 5) {
        QMap<QString, QList<ProductFeature>> mergedGroups;
        int groupIndex = 0;
        int featuresPerGroup = features.size() / 3 + 1;
        
        QList<ProductFeature> currentGroup;
        for (const ProductFeature &feature : features) {
            currentGroup.append(feature);
            if (currentGroup.size() >= featuresPerGroup) {
                mergedGroups[QString("组%1").arg(groupIndex + 1)] = currentGroup;
                currentGroup.clear();
                groupIndex++;
            }
        }
        
        if (!currentGroup.isEmpty()) {
            mergedGroups[QString("组%1").arg(groupIndex + 1)] = currentGroup;
        }
        
        featureGroups = mergedGroups;
    }
    
    // 为每个分组创建Tab页
    for (auto it = featureGroups.begin(); it != featureGroups.end(); ++it) {
        QString groupName = it.key();
        QList<ProductFeature> groupFeatures = it.value();
        
        // 创建Tab页内容控件
        QWidget *tabPage = new QWidget();
        QVBoxLayout *tabLayout = new QVBoxLayout(tabPage);
        tabLayout->setSpacing(10);
        tabLayout->setContentsMargins(15, 15, 15, 15);
        
        // 为分组中的每个功能特性创建控件
        for (const ProductFeature &feature : groupFeatures) {
            QString widgetType = suggestWidgetTypeForFeature(feature);
            QWidget *widget = createWidgetForFeature(feature, widgetType);
            if (widget) {
                tabLayout->addWidget(widget);
            }
        }
        
        tabLayout->addStretch();
        
        // 添加Tab页
        tabWidget->addTab(tabPage, groupName);
    }
    
    // 应用布局到中央控件
    QWidget *centralWidget = ui->graphicsView;
    if (centralWidget) {
        QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
        mainLayout->addWidget(tabWidget);
    }
    
    QMessageBox::information(this, "布局建议", "已应用Tab页布局建议");
}

QWidget* UILayoutWindow::createWidgetForFeature(const ProductFeature &feature, const QString &widgetType)
{
    QWidget *widget = nullptr;
    
    // 根据控件类型创建对应的控件
    if (widgetType == "QLabel") {
        QLabel *label = new QLabel(feature.name);
        label->setTextInteractionFlags(Qt::TextEditorInteraction); // 允许编辑文本
        widget = label;
    } else if (widgetType == "QLineEdit") {
        QLineEdit *lineEdit = new QLineEdit();
        lineEdit->setPlaceholderText(feature.name);
        widget = lineEdit;
    } else if (widgetType == "QTextEdit") {
        QTextEdit *textEdit = new QTextEdit();
        textEdit->setPlaceholderText(feature.description);
        widget = textEdit;
    } else if (widgetType == "QPushButton") {
        QPushButton *button = new QPushButton(feature.name);
        widget = button;
    } else if (widgetType == "QCheckBox") {
        QCheckBox *checkBox = new QCheckBox(feature.name);
        widget = checkBox;
    } else if (widgetType == "QRadioButton") {
        QRadioButton *radioButton = new QRadioButton(feature.name);
        widget = radioButton;
    } else if (widgetType == "QComboBox") {
        QComboBox *comboBox = new QComboBox();
        comboBox->addItem(feature.name);
        widget = comboBox;
    } else if (widgetType == "QSpinBox") {
        QSpinBox *spinBox = new QSpinBox();
        widget = spinBox;
    } else if (widgetType == "QSlider") {
        QSlider *slider = new QSlider(Qt::Horizontal);
        widget = slider;
    } else if (widgetType == "QProgressBar") {
        QProgressBar *progressBar = new QProgressBar();
        widget = progressBar;
    } else if (widgetType == "QTabWidget") {
        QTabWidget *tabWidget = new QTabWidget();
        // 为TabWidget添加一个默认的Tab页
        QWidget *tabPage = new QWidget();
        tabWidget->addTab(tabPage, feature.name);
        widget = tabWidget;
    } else {
        // 默认使用QLabel
        QLabel *label = new QLabel(feature.name);
        label->setTextInteractionFlags(Qt::TextEditorInteraction);
        widget = label;
    }
    
    if (widget) {
        // 设置控件属性
        widget->setObjectName(generateWidgetId(widgetType));
        widget->setMinimumSize(100, 30);
        
        // 为控件添加样式，使其更容易识别
        widget->setStyleSheet("border: 1px solid #ccc; padding: 5px; background-color: #f9f9f9;");
        
        // 绑定功能特性
        if (m_configManager) {
            m_configManager->bindProductFeatureToWidget(feature.name, widget->objectName());
        }
    }
    
    return widget;
}

void UILayoutWindow::clearLayout()
{
    // 获取中央控件
    QWidget *centralWidget = ui->graphicsView;
    if (!centralWidget) {
        return;
    }
    
    // 删除所有子控件
    QLayout *layout = centralWidget->layout();
    if (layout) {
        QLayoutItem *item;
        while ((item = layout->takeAt(0)) != nullptr) {
            if (item->widget()) {
                item->widget()->deleteLater();
            }
            delete item;
        }
        delete layout;
    }
    
    // 清空控件映射
    m_widgetItemMap.clear();
    
    // 清空布局项列表
    qDeleteAll(m_layoutItems);
    m_layoutItems.clear();
}

void UILayoutWindow::saveCurrentLayoutAsTemplate(const QString &templateName)
{
    if (!m_configManager) {
        QMessageBox::warning(this, "错误", "产品配置管理器未初始化");
        return;
    }
    
    if (templateName.isEmpty()) {
        QMessageBox::warning(this, "错误", "模板名称不能为空");
        return;
    }
    
    // 获取当前布局路径
    QString currentLayoutPath = getCurrentLayoutPath();
    if (currentLayoutPath.isEmpty()) {
        // 如果没有保存的布局，先保存当前布局
        on_actionSave_Layout_triggered();
        currentLayoutPath = getCurrentLayoutPath();
        
        if (currentLayoutPath.isEmpty()) {
            QMessageBox::warning(this, "错误", "请先保存当前布局");
            return;
        }
    }
    
    // 保存为模板
    m_configManager->saveLayoutTemplate(templateName, currentLayoutPath);
    
    QMessageBox::information(this, "成功", QString("布局已保存为模板: %1").arg(templateName));
}

void UILayoutWindow::showTemplateSelectionDialog()
{
    if (!m_configManager) {
        QMessageBox::warning(this, "错误", "产品配置管理器未初始化");
        return;
    }
    
    // 获取可用的模板列表
    QStringList templates = m_configManager->getAvailableLayoutTemplates();
    
    if (templates.isEmpty()) {
        QMessageBox::information(this, "提示", "没有可用的布局模板");
        return;
    }
    
    // 显示模板选择对话框
    bool ok;
    QString selectedTemplate = QInputDialog::getItem(this, "选择布局模板", 
                                                     "请选择要应用的布局模板:", 
                                                     templates, 0, false, &ok);
    
    if (ok && !selectedTemplate.isEmpty()) {
        applyLayoutTemplate(selectedTemplate);
    }
}

void UILayoutWindow::applySmartTemplate()
{
    if (!m_configManager) {
        QMessageBox::warning(this, "错误", "产品配置管理器未初始化");
        return;
    }
    
    // 获取产品功能特性
    Product product = m_configManager->getProduct();
    QList<ProductFeature> features = product.features();
    
    if (features.isEmpty()) {
        QMessageBox::information(this, "提示", "当前产品没有定义功能特性");
        return;
    }
    
    // 分析功能特性，选择最合适的模板
    QString bestTemplate = selectBestTemplateForFeatures(features);
    
    if (!bestTemplate.isEmpty()) {
        // 应用选定的模板
        applyLayoutTemplate(bestTemplate);
        QMessageBox::information(this, "智能模板", QString("已应用智能模板: %1").arg(bestTemplate));
    } else {
        // 没有合适的模板，使用智能布局建议
        suggestLayoutForFeatures();
        QMessageBox::information(this, "智能模板", "已根据功能特性生成智能布局");
    }
}

QString UILayoutWindow::selectBestTemplateForFeatures(const QList<ProductFeature> &features)
{
    if (!m_configManager) {
        return QString();
    }
    
    // 获取可用的模板
    QStringList templates = m_configManager->getAvailableLayoutTemplates();
    
    if (templates.isEmpty()) {
        return QString();
    }
    
    // 简单的模板匹配算法
    // 根据功能特性数量选择最合适的模板
    int featureCount = features.size();
    
    // 分析功能特性的类型分布
    int inputCount = 0, displayCount = 0, actionCount = 0;
    for (const ProductFeature &feature : features) {
        QString widgetType = suggestWidgetTypeForFeature(feature);
        if (widgetType == "QLineEdit" || widgetType == "QTextEdit" || widgetType == "QSpinBox" || 
            widgetType == "QSlider" || widgetType == "QComboBox") {
            inputCount++;
        } else if (widgetType == "QLabel" || widgetType == "QTextBrowser" || widgetType == "QProgressBar") {
            displayCount++;
        } else if (widgetType == "QPushButton" || widgetType == "QCheckBox" || widgetType == "QRadioButton") {
            actionCount++;
        }
    }
    
    // 根据模板名称和功能特性匹配度选择最佳模板
    QString bestTemplate;
    double bestScore = 0.0;
    
    for (const QString &templateName : templates) {
        double score = calculateTemplateMatchScore(templateName, featureCount, inputCount, displayCount, actionCount);
        
        if (score > bestScore) {
            bestScore = score;
            bestTemplate = templateName;
        }
    }
    
    // 如果匹配度高于阈值，则使用该模板
    if (bestScore > 0.6) {
        return bestTemplate;
    }
    
    return QString();
}

double UILayoutWindow::calculateTemplateMatchScore(const QString &templateName, int featureCount, int inputCount, int displayCount, int actionCount)
{
    double score = 0.0;
    
    // 简单的模板名称匹配算法
    QString lowerName = templateName.toLower();
    
    // 根据模板名称中的关键词计算匹配度
    if (lowerName.contains("simple") || lowerName.contains("basic")) {
        // 简单模板适合少量功能特性
        if (featureCount <= 3) {
            score += 0.8;
        } else if (featureCount <= 6) {
            score += 0.5;
        }
    } else if (lowerName.contains("two") || lowerName.contains("column")) {
        // 两列模板适合中等数量的功能特性
        if (featureCount > 3 && featureCount <= 8) {
            score += 0.8;
        } else if (featureCount <= 12) {
            score += 0.6;
        }
    } else if (lowerName.contains("tab") || lowerName.contains("page")) {
        // Tab页模板适合大量功能特性
        if (featureCount > 8) {
            score += 0.9;
        } else if (featureCount > 5) {
            score += 0.7;
        }
    } else if (lowerName.contains("form") || lowerName.contains("input")) {
        // 表单模板适合输入型功能特性
        if (inputCount > displayCount + actionCount) {
            score += 0.8;
        }
    } else if (lowerName.contains("dashboard") || lowerName.contains("display")) {
        // 仪表板模板适合显示型功能特性
        if (displayCount > inputCount + actionCount) {
            score += 0.8;
        }
    } else if (lowerName.contains("control") || lowerName.contains("action")) {
        // 控制面板模板适合操作型功能特性
        if (actionCount > inputCount + displayCount) {
            score += 0.8;
        }
    }
    
    // 根据功能特性数量调整分数
    if (featureCount <= 3) {
        score *= 1.2; // 少量功能特性，简单模板更合适
    } else if (featureCount >= 10) {
        score *= 1.1; // 大量功能特性，复杂模板更合适
    }
    
    return qMin(score, 1.0); // 确保分数不超过1.0
}

QString UILayoutWindow::generateWidgetId(const QString &widgetType) const
{
    // 生成唯一的控件ID
    QString baseId = widgetType;
    baseId.remove("Q"); // 移除Q前缀
    QString uniqueId = QUuid::createUuid().toString();
    uniqueId.remove("{").remove("}").remove("-");
    
    return QString("%1_%2").arg(baseId).arg(uniqueId.left(8));
}

void UILayoutWindow::updateWidgetBindings()
{
    if (!m_configManager) {
        return;
    }
    
    // 更新所有控件的绑定信息
    for (QWidget *widget : m_widgetItemMap.keys()) {
        QString widgetId = widget->objectName();
        if (widgetId.isEmpty()) {
            // 为控件生成唯一ID
            LayoutItem *item = m_widgetItemMap.value(widget);
            if (item) {
                widgetId = generateWidgetId(item->widgetType());
                widget->setObjectName(widgetId);
            }
        }
        
        // 检查是否有功能特性绑定
        QString featureName = m_configManager->getFeatureForWidget(widgetId);
        if (!featureName.isEmpty()) {
            // 更新控件显示以反映绑定
            qDebug() << "UILayoutWindow: 控件" << widgetId << "绑定到功能特性:" << featureName;
        }
    }
}

// 功能特性关联映射相关实现
void UILayoutWindow::onBindFeatureButtonClicked()
{
    // 获取当前选中的控件
    QWidget *selectedWidget = m_selectedWidget;
    if (!selectedWidget) {
        QMessageBox::information(this, "提示", "请先选择一个控件");
        return;
    }
    
    // 获取产品功能特性列表
    if (!m_configManager) {
        QMessageBox::warning(this, "警告", "产品配置管理器未初始化");
        return;
    }
    
    Product product = m_configManager->getProduct();
    if (product.name().isEmpty()) {
        QMessageBox::warning(this, "警告", "未加载产品信息");
        return;
    }
    
    QList<ProductFeature> features = product.features();
    if (features.isEmpty()) {
        QMessageBox::information(this, "提示", "当前产品没有定义功能特性");
        return;
    }
    
    // 提取功能特性名称列表
    QStringList featureNames;
    for (const ProductFeature &feature : features) {
        featureNames << feature.name;
    }
    
    // 显示功能特性选择对话框
    bool ok;
    QString feature = QInputDialog::getItem(this, "选择功能特性", 
                                           "请选择要绑定的功能特性:", 
                                           featureNames, 0, false, &ok);
    
    if (ok && !feature.isEmpty()) {
        // 绑定功能特性到控件
        QString widgetId = selectedWidget->objectName();
        if (widgetId.isEmpty()) {
            // 为控件生成唯一ID
            LayoutItem *item = m_widgetItemMap.value(selectedWidget);
            if (item) {
                widgetId = generateWidgetId(item->widgetType());
                selectedWidget->setObjectName(widgetId);
            } else {
                widgetId = generateWidgetId("QWidget");
                selectedWidget->setObjectName(widgetId);
            }
        }
        
        // 保存绑定关系
        m_configManager->bindProductFeatureToWidget(feature, widgetId);
        
        QMessageBox::information(this, "成功", QString("功能特性 '%1' 已绑定到控件").arg(feature));
        updateFeaturesTreeWidget();
    } else {
        QMessageBox::warning(this, "失败", "绑定功能特性失败");
    }
}

void UILayoutWindow::onUnbindFeatureButtonClicked()
{
    // 获取当前选中的功能特性项
    QTreeWidgetItem *selectedItem = ui->featuresTreeWidget->currentItem();
    if (!selectedItem) {
        QMessageBox::information(this, "提示", "请先选择一个功能特性项");
        return;
    }
    
    QString feature = selectedItem->text(0);
    QString widgetId = selectedItem->data(0, Qt::UserRole).toString();
    
    if (feature.isEmpty() || widgetId.isEmpty()) {
        QMessageBox::warning(this, "错误", "无效的功能特性项");
        return;
    }
    
    // 确认解绑
    int result = QMessageBox::question(this, "确认解绑", 
                                      QString("确定要解绑功能特性 '%1' 吗?").arg(feature),
                                      QMessageBox::Yes | QMessageBox::No);
    
    if (result == QMessageBox::Yes) {
        // 解绑功能特性
        m_configManager->unbindProductFeatureFromWidget(feature, widgetId);
        
        QMessageBox::information(this, "成功", QString("功能特性 '%1' 已解绑").arg(feature));
        updateFeaturesTreeWidget();
    }
}

void UILayoutWindow::onSyncFeaturesButtonClicked()
{
    if (!m_configManager) {
        QMessageBox::warning(this, "警告", "产品配置管理器未初始化");
        return;
    }
    
    // 同步产品功能特性
    Product product = m_configManager->getProduct();
    if (product.name().isEmpty()) {
        QMessageBox::warning(this, "警告", "未加载产品信息");
        return;
    }
    
    QList<ProductFeature> features = product.features();
    
    // 获取当前绑定的功能特性
    QMap<QString, QString> bindings = m_configManager->getFeatureWidgetBindings();
    
    // 更新功能特性树控件
    updateFeaturesTreeWidget();
    
    QMessageBox::information(this, "同步完成", 
                           QString("已同步 %1 个功能特性，当前绑定 %2 个控件").arg(features.size()).arg(bindings.size()));
}

void UILayoutWindow::onFeaturesTreeWidgetItemDoubleClicked(QTreeWidgetItem *item, int column)
{
    if (column == 1) { // 关联控件列
        QString widgetId = item->data(0, Qt::UserRole).toString();
        if (!widgetId.isEmpty()) {
            // 查找并选中对应的控件
            QWidget *widget = findWidgetById(widgetId);
            if (widget) {
                // 选中控件
                m_selectedWidget = widget;
                updatePropertiesEditor(widget);
                updateHandles(widget);
                
                // 滚动到控件位置
                QPoint widgetPos = widget->mapTo(m_editAreaWidget, QPoint(0, 0));
                // 确保控件可见（注释掉不兼容的代码）
                // ui->graphicsView->ensureVisible(QRect(widgetPos, widget->size()));
            }
        }
    }
}

void UILayoutWindow::updateFeaturesTreeWidget()
{
    if (!m_configManager) return;
    
    ui->featuresTreeWidget->clear();
    
    // 设置功能特性树状视图的列标题
    ui->featuresTreeWidget->setHeaderLabels(QStringList() << "功能特性" << "关联控件" << "状态" << "优先级" << "描述");
    
    // 获取产品功能特性
    Product product = m_configManager->getProduct();
    if (product.name().isEmpty()) return;
    
    QList<ProductFeature> features = product.features();
    QMap<QString, QString> bindings = m_configManager->getFeatureWidgetBindings();
    
    // 按优先级排序功能特性
    std::sort(features.begin(), features.end(), [](const ProductFeature &a, const ProductFeature &b) {
        return a.priority > b.priority; // 优先级高的在前
    });
    
    for (const ProductFeature &feature : features) {
        QString featureName = feature.name;
        QTreeWidgetItem *item = new QTreeWidgetItem(ui->featuresTreeWidget);
        item->setText(0, featureName);
        item->setText(3, QString::number(feature.priority)); // 优先级
        item->setText(4, feature.description); // 描述
        
        // 设置工具提示，显示完整的功能特性信息
        QString tooltip = QString("功能特性: %1\n优先级: %2\n描述: %3")
                         .arg(featureName)
                         .arg(feature.priority)
                         .arg(feature.description);
        item->setToolTip(0, tooltip);
        
        // 检查是否有绑定
        if (bindings.contains(featureName)) {
            QString widgetId = bindings.value(featureName);
            QString widgetName = m_configManager->getWidgetIdForFeature(featureName);
            
            item->setText(1, widgetName.isEmpty() ? "已绑定" : widgetName);
            item->setText(2, "已绑定");
            item->setData(0, Qt::UserRole, widgetId);
            item->setForeground(2, QBrush(Qt::darkGreen));
            
            // 为已绑定的功能特性添加图标
            item->setIcon(0, QIcon(":/icons/bound.png"));
        } else {
            item->setText(1, "未绑定");
            item->setText(2, "未绑定");
            item->setForeground(2, QBrush(Qt::darkRed));
            
            // 为未绑定的功能特性添加图标
            item->setIcon(0, QIcon(":/icons/unbound.png"));
            
            // 根据优先级设置不同的背景色
            if (feature.priority >= 8) {
                item->setBackground(0, QBrush(QColor(255, 240, 240))); // 高优先级
            } else if (feature.priority >= 5) {
                item->setBackground(0, QBrush(QColor(255, 255, 240))); // 中优先级
            }
        }
    }
    
    // 自动调整列宽
    for (int i = 0; i < ui->featuresTreeWidget->columnCount(); ++i) {
        ui->featuresTreeWidget->resizeColumnToContents(i);
    }
    
    // 更新状态栏显示产品上下文信息
    updateStatusBarWithProductContext();
    
    // 添加上下文菜单支持
    ui->featuresTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->featuresTreeWidget, &QTreeWidget::customContextMenuRequested,
            this, &UILayoutWindow::showFeatureContextMenu);
}

// 产品上下文感知相关方法实现
void UILayoutWindow::updateStatusBarWithProductContext()
{
    if (!m_configManager) return;
    
    Product product = m_configManager->getProduct();
    if (product.name().isEmpty()) return;
    
    // 获取产品信息
    QString productName = product.name();
    QString productVersion = product.version();
    QList<ProductFeature> features = product.features();
    QMap<QString, QString> bindings = m_configManager->getFeatureWidgetBindings();
    
    // 计算统计信息
    int totalFeatures = features.size();
    int boundFeatures = bindings.size();
    int unboundFeatures = totalFeatures - boundFeatures;
    
    // 计算高优先级未绑定功能特性数量
    int highPriorityUnbound = 0;
    for (const ProductFeature &feature : features) {
        if (feature.priority >= 8 && !bindings.contains(feature.name)) {
            highPriorityUnbound++;
        }
    }
    
    // 更新状态栏显示
    QString statusText = QString("产品: %1 v%2 | 功能特性: %3/%4 已绑定 | 高优先级未绑定: %5")
                        .arg(productName)
                        .arg(productVersion)
                        .arg(boundFeatures)
                        .arg(totalFeatures)
                        .arg(highPriorityUnbound);
    
    // 根据绑定状态设置不同的颜色
    if (highPriorityUnbound > 0) {
        statusText += " ⚠"; // 警告图标
        ui->statusbar->setStyleSheet("color: #d35400;"); // 橙色警告色
    } else if (unboundFeatures > 0) {
        ui->statusbar->setStyleSheet("color: #f39c12;"); // 黄色提示色
    } else {
        ui->statusbar->setStyleSheet("color: #27ae60;"); // 绿色成功色
    }
    
    ui->statusbar->showMessage(statusText);
}



void UILayoutWindow::highlightRelevantWidgets(const QString &featureName)
{
    if (!m_configManager) return;
    
    // 清除之前的高亮
    for (auto it = m_widgetItemMap.begin(); it != m_widgetItemMap.end(); ++it) {
        QWidget *widget = it.key();
        widget->setStyleSheet(""); // 清除样式
    }
    
    // 高亮与指定功能特性相关的控件
    QMap<QString, QString> bindings = m_configManager->getFeatureWidgetBindings();
    
    if (bindings.contains(featureName)) {
        QString widgetId = bindings.value(featureName);
        QWidget *widget = findWidgetById(widgetId);
        if (widget) {
            // 高亮显示已绑定的控件
            widget->setStyleSheet("border: 3px solid #27ae60; background-color: #d5f5e3;");
        }
    } else {
        // 高亮显示可能相关的控件类型
        Product product = m_configManager->getProduct();
        ProductFeature feature;
        for (const ProductFeature &f : product.features()) {
            if (f.name == featureName) {
                feature = f;
                break;
            }
        }
        
        if (!feature.name.isEmpty()) {
            QString suggestedType = suggestWidgetTypeForFeature(feature);
            
            // 高亮显示建议的控件类型
            for (auto it = m_widgetItemMap.begin(); it != m_widgetItemMap.end(); ++it) {
                QWidget *widget = it.key();
                LayoutItem *item = it.value();
                
                if (item->widgetType() == suggestedType) {
                    widget->setStyleSheet("border: 3px solid #f39c12; background-color: #fef9e7;");
                }
            }
        }
    }
}

void UILayoutWindow::showFeatureContextMenu(const QPoint &pos)
{
    QTreeWidgetItem *item = ui->featuresTreeWidget->itemAt(pos);
    if (!item) return;
    
    QString featureName = item->text(0);
    
    QMenu contextMenu(this);
    
    // 添加上下文菜单项
    QAction *bindAction = contextMenu.addAction("绑定到控件");
    QAction *highlightAction = contextMenu.addAction("高亮相关控件");
    QAction *suggestAction = contextMenu.addAction("获取布局建议");
    contextMenu.addSeparator();
    QAction *infoAction = contextMenu.addAction("查看功能特性详情");
    
    // 执行菜单操作
    QAction *selectedAction = contextMenu.exec(ui->featuresTreeWidget->viewport()->mapToGlobal(pos));
    
    if (selectedAction == bindAction) {
        // 绑定功能特性
        onBindFeatureButtonClicked();
    } else if (selectedAction == highlightAction) {
        // 高亮相关控件
        highlightRelevantWidgets(featureName);
    } else if (selectedAction == suggestAction) {
        // 获取布局建议
        suggestLayoutForFeatures();
    } else if (selectedAction == infoAction) {
        // 显示功能特性详情
        Product product = m_configManager->getProduct();
        for (const ProductFeature &feature : product.features()) {
            if (feature.name == featureName) {
                QString info = QString("功能特性详情:\n\n"
                                      "名称: %1\n"
                                      "优先级: %2\n"
                                      "描述: %3\n"
                                      "建议控件类型: %4")
                              .arg(feature.name)
                              .arg(feature.priority)
                              .arg(feature.description)
                              .arg(suggestWidgetTypeForFeature(feature));
                
                QMessageBox::information(this, "功能特性详情", info);
                break;
            }
        }
    }
}

// 辅助方法：根据功能特性建议控件类型
QString UILayoutWindow::suggestWidgetTypeForFeature(const ProductFeature &feature)
{
    QString description = feature.description.toLower();
    QString name = feature.name.toLower();
    
    // 根据功能特性描述和名称推断合适的控件类型
    if (description.contains("按钮") || name.contains("button") || name.contains("btn")) {
        return "QPushButton";
    } else if (description.contains("标签") || name.contains("label") || name.contains("lbl")) {
        return "QLabel";
    } else if (description.contains("输入") || name.contains("input") || name.contains("edit")) {
        return "QLineEdit";
    } else if (description.contains("文本") || name.contains("text")) {
        return "QTextEdit";
    } else if (description.contains("选择") || name.contains("select") || name.contains("combo")) {
        return "QComboBox";
    } else if (description.contains("列表") || name.contains("list")) {
        return "QListWidget";
    } else if (description.contains("树") || name.contains("tree")) {
        return "QTreeWidget";
    } else if (description.contains("表格") || name.contains("table")) {
        return "QTableWidget";
    } else if (description.contains("进度") || name.contains("progress")) {
        return "QProgressBar";
    } else if (description.contains("滑动") || name.contains("slider")) {
        return "QSlider";
    } else if (description.contains("复选框") || name.contains("check") || name.contains("checkbox")) {
        return "QCheckBox";
    } else if (description.contains("单选") || name.contains("radio")) {
        return "QRadioButton";
    } else if (description.contains("分组") || name.contains("group")) {
        return "QGroupBox";
    } else if (description.contains("标签页") || name.contains("tab")) {
        return "QTabWidget";
    }
    
    // 默认返回通用控件
    return "QWidget";
}

QWidget* UILayoutWindow::findWidgetById(const QString &widgetId)
{
    // 遍历所有控件查找匹配的widgetId
    for (auto it = m_widgetItemMap.begin(); it != m_widgetItemMap.end(); ++it) {
        QWidget *widget = it.key();
        QString id = widget->objectName();
        if (id == widgetId) {
            return widget;
        }
    }
    return nullptr;
}

void UILayoutWindow::onActionSuggestLayoutTriggered()
{
    // 调用布局建议功能
    suggestLayoutForFeatures();
}

void UILayoutWindow::onActionEventActionEditorTriggered()
{
    // 检查界面管理器是否有效
    if (!m_interfaceManager) {
        qCritical() << "UILayoutWindow: m_interfaceManager is nullptr, cannot open event-action editor";
        QMessageBox::warning(this, "错误", "界面管理器未初始化，无法打开事件-动作编辑器");
        return;
    }
    
    // 检查当前界面是否有效
    if (!m_interfaceManager->currentInterface()) {
        qCritical() << "UILayoutWindow: current interface is nullptr, cannot open event-action editor";
        QMessageBox::warning(this, "错误", "当前界面未设置，无法打开事件-动作编辑器");
        return;
    }
    qDebug()<<"current interface name:"<<m_interfaceManager->currentInterface()->name();
    // 创建或显示事件-动作编辑器窗口
    if (!m_eventActionEditor) {
        qDebug()<<"create EventActionEditor";
        m_eventActionEditor = new EventActionEditor(this);
        m_eventActionEditor->setWindowTitle("事件-动作编辑器");
        m_eventActionEditor->resize(800, 600);
        
        // 设置窗口标志，使其为模态对话框
        m_eventActionEditor->setWindowFlags(Qt::Dialog);
        m_eventActionEditor->setWindowModality(Qt::ApplicationModal);
    }
    
    // 显示事件-动作编辑器
    m_eventActionEditor->show();
    m_eventActionEditor->raise();
    m_eventActionEditor->activateWindow();
}

// 多界面管理相关方法实现
void UILayoutWindow::setupInterfaceManagementUI()
{
    // 创建界面管理工具栏
    QToolBar *interfaceToolBar = new QToolBar("界面管理", this);
    interfaceToolBar->setMovable(false);
    
    // 添加界面列表下拉框
    QComboBox *interfaceComboBox = new QComboBox(interfaceToolBar);
    interfaceComboBox->setObjectName("interfaceComboBox");
    interfaceComboBox->setMinimumWidth(150);
    interfaceComboBox->setToolTip("选择要编辑的界面");
    
    // 添加界面管理按钮
    QAction *addInterfaceAction = new QAction("新建界面", this);
    QAction *deleteInterfaceAction = new QAction("删除界面", this);
    QAction *renameInterfaceAction = new QAction("重命名界面", this);
    QAction *copyInterfaceAction = new QAction("复制界面", this);
    
    interfaceToolBar->addWidget(new QLabel("界面:"));
    interfaceToolBar->addWidget(interfaceComboBox);
    interfaceToolBar->addSeparator();
    interfaceToolBar->addAction(addInterfaceAction);
    interfaceToolBar->addAction(deleteInterfaceAction);
    interfaceToolBar->addAction(renameInterfaceAction);
    interfaceToolBar->addAction(copyInterfaceAction);
    
    // 将工具栏添加到主窗口
    addToolBar(Qt::TopToolBarArea, interfaceToolBar);
    
    // 连接信号槽
    connect(interfaceComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &UILayoutWindow::onInterfaceComboBoxChanged);
    connect(addInterfaceAction, &QAction::triggered, this, &UILayoutWindow::onAddInterfaceAction);
    connect(deleteInterfaceAction, &QAction::triggered, this, &UILayoutWindow::onDeleteInterfaceAction);
    connect(renameInterfaceAction, &QAction::triggered, this, &UILayoutWindow::onRenameInterfaceAction);
    connect(copyInterfaceAction, &QAction::triggered, this, &UILayoutWindow::onCopyInterfaceAction);
    
    // 更新界面列表
    updateInterfaceList();
}

void UILayoutWindow::updateInterfaceList()
{
    // 检查界面管理器是否有效
    if (!m_interfaceManager) {
        qCritical() << "UILayoutWindow: m_interfaceManager is nullptr, cannot update interface list";
        return;
    }
    
    // 查找界面管理工具栏中的下拉框
    QComboBox *interfaceComboBox = findChild<QComboBox*>("interfaceComboBox");
    if (!interfaceComboBox) return;
    
    // 保存当前选中的界面
    QString currentInterfaceName = interfaceComboBox->currentText();
    
    // 清空下拉框
    interfaceComboBox->clear();
    
    // 添加所有界面到下拉框
    QList<UIInterface*> interfaces = m_interfaceManager->interfaces();
    for (UIInterface *interface : interfaces) {
        interfaceComboBox->addItem(interface->name(), QVariant::fromValue(interface));
    }
    
    // 恢复选中的界面
    int index = interfaceComboBox->findText(currentInterfaceName);
    if (index >= 0) {
        interfaceComboBox->setCurrentIndex(index);
    } else if (interfaceComboBox->count() > 0) {
        interfaceComboBox->setCurrentIndex(0);
    }
    
    // 更新窗口标题显示当前界面
    if (m_interfaceManager->currentInterface()) {
        setWindowTitle(QString("UI布局编辑器 - %1").arg(m_interfaceManager->currentInterface()->name()));
    }
}

void UILayoutWindow::switchToInterface(UIInterface *interface)
{
    if (!interface || !m_interfaceManager) return;
    
    // 保存当前界面状态
    saveCurrentInterfaceState();
    
    // 切换到新界面
    m_interfaceManager->setCurrentInterface(interface);
    
    // 加载新界面状态
    loadInterfaceState(interface);
    
    // 更新窗口标题显示当前界面
    if (m_interfaceManager->currentInterface()) {
        setWindowTitle(QString("UI布局编辑器 - %1").arg(m_interfaceManager->currentInterface()->name()));
    }
}

void UILayoutWindow::saveCurrentInterfaceState()
{
    if (!m_interfaceManager || !m_interfaceManager->currentInterface()) return;
    
    // 获取当前界面
    UIInterface *currentInterface = m_interfaceManager->currentInterface();
    
    // 清空当前界面的布局项
    currentInterface->clearLayoutItems();
    
    // 保存所有布局项到当前界面
    for (LayoutItem *item : m_layoutItems) {
        currentInterface->addLayoutItem(item);
    }
}

void UILayoutWindow::loadInterfaceState(UIInterface *interface)
{
    if (!interface) return;
    
    // 清空当前编辑区的所有控件
    clearEditArea();
    
    // 加载界面中的布局项
    QList<LayoutItem*> layoutItems = interface->layoutItems();
    for (LayoutItem *item : layoutItems) {
        // 创建控件并添加到编辑区
        QWidget *widget = qobject_cast<QWidget*>(item->createWidget(m_editAreaWidget));
        if (widget) {
            widget->setGeometry(QRect(item->pos(), item->size()));
            widget->show();
            
            // 添加到映射关系
            m_widgetItemMap[widget] = item;
            m_layoutItems.append(item);
        }
    }
    
    // 更新属性编辑器
    updatePropertiesEditor(nullptr);
}

// 界面管理相关的槽函数
void UILayoutWindow::onInterfaceComboBoxChanged(int index)
{
    QComboBox *comboBox = qobject_cast<QComboBox*>(sender());
    if (!comboBox) return;
    
    // 防止递归调用：如果当前界面已经是目标界面，则不进行切换
    if (m_interfaceManager && m_interfaceManager->currentInterface()) {
        UIInterface *currentInterface = m_interfaceManager->currentInterface();
        if (index >= 0) {
            UIInterface *targetInterface = comboBox->itemData(index).value<UIInterface*>();
            if (targetInterface && targetInterface == currentInterface) {
                return; // 已经是当前界面，不需要切换
            }
        }
    }
    
    if (index >= 0) {
        UIInterface *interface = comboBox->itemData(index).value<UIInterface*>();
        if (interface) {
            switchToInterface(interface);
        }
    }
}

void UILayoutWindow::onAddInterfaceAction()
{
    if (!m_interfaceManager) return;
    
    bool ok;
    QString interfaceName = QInputDialog::getText(this, "新建界面", "请输入界面名称:", 
                                                 QLineEdit::Normal, "新界面", &ok);
    if (ok && !interfaceName.isEmpty()) {
        UIInterface *newInterface = m_interfaceManager->createInterface(interfaceName);
        if (newInterface) {
            updateInterfaceList();
            
            // 切换到新创建的界面
            switchToInterface(newInterface);
        }
    }
}

void UILayoutWindow::onDeleteInterfaceAction()
{
    if (!m_interfaceManager) return;
    
    UIInterface *currentInterface = m_interfaceManager->currentInterface();
    if (!currentInterface) return;
    
    if (m_interfaceManager->interfaces().count() <= 1) {
        QMessageBox::warning(this, "删除界面", "至少需要保留一个界面！");
        return;
    }
    
    int result = QMessageBox::question(this, "删除界面", 
                                       QString("确定要删除界面 '%1' 吗？").arg(currentInterface->name()),
                                       QMessageBox::Yes | QMessageBox::No);
    
    if (result == QMessageBox::Yes) {
        m_interfaceManager->removeInterface(currentInterface);
        updateInterfaceList();
    }
}

void UILayoutWindow::onRenameInterfaceAction()
{
    if (!m_interfaceManager) return;
    
    UIInterface *currentInterface = m_interfaceManager->currentInterface();
    if (!currentInterface) return;
    
    bool ok;
    QString newName = QInputDialog::getText(this, "重命名界面", "请输入新名称:", 
                                           QLineEdit::Normal, currentInterface->name(), &ok);
    if (ok && !newName.isEmpty()) {
        m_interfaceManager->renameInterface(currentInterface, newName);
        updateInterfaceList();
    }
}

void UILayoutWindow::onCopyInterfaceAction()
{
    if (!m_interfaceManager) return;
    
    UIInterface *currentInterface = m_interfaceManager->currentInterface();
    if (!currentInterface) return;
    
    bool ok;
    QString newName = QInputDialog::getText(this, "复制界面", "请输入新界面名称:", 
                                           QLineEdit::Normal, 
                                           QString("%1_副本").arg(currentInterface->name()), &ok);
    if (ok && !newName.isEmpty()) {
        UIInterface *copiedInterface = m_interfaceManager->cloneInterface(currentInterface, newName);
        if (copiedInterface) {
            updateInterfaceList();
            
            // 切换到新复制的界面
            switchToInterface(copiedInterface);
        }
    }
}

void UILayoutWindow::clearEditArea()
{
    // 删除所有控件
    for (QWidget *widget : m_widgetItemMap.keys()) {
        delete widget;
    }
    
    // 清空映射关系
    m_widgetItemMap.clear();
    m_layoutItems.clear();
    
    // 重置选中的控件
    m_selectedWidget = nullptr;
    
    // 隐藏控制点
    if (m_editAreaWidget) {
        m_editAreaWidget->setHandles(QList<QRect>());
    }
}
