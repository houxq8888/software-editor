#include "previewwindow.h"
#include "ui_previewwindow.h"
#include "uilayoutwindow.h"
#include <QDebug>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QSlider>
#include <QCheckBox>
#include <QRadioButton>
#include <QCalendarWidget>
#include <QGroupBox>
#include <QTabWidget>
#include <QTableWidget>
#include <QTreeWidget>
#include <QListWidget>
#include <QDockWidget>
#include <QFrame>
#include <QToolBar>
#include <QVBoxLayout>
#include <QProgressBar>
#include <QScreen>
#include <QGuiApplication>
#include <QInputDialog>
#include <QMouseEvent>
#include <QTabBar>
#include <QMessageBox>

PreviewWindow::PreviewWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::PreviewWindow)
{
    ui->setupUi(this);

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

    // 设置主窗口属性
    setWindowTitle("UI布局预览");
    setMinimumSize(800, 600);

    // 设置预览容器的最小尺寸
    ui->previewWidget->setMinimumSize(800, 600);
}

PreviewWindow::~PreviewWindow()
{
    delete ui;
    qDeleteAll(m_widgetMap.values());
    m_widgetMap.clear();
    m_widgetToItemMap.clear();
}
QWidget* PreviewWindow::createWidgetFromType(const QString &widgetType, QWidget *parent) {
    qDebug() << "Creating widget of type:" << widgetType;
    // 默认返回nullptr，由调用者根据LayoutItem的text属性设置
    return nullptr;
}

void PreviewWindow::setLayoutItems(const QList<LayoutItem *> &items){ 
    // 清除之前的预览控件
    qDebug() << "PreviewWindow::setLayoutItems: 开始设置布局项，共" << items.size() << "个布局项";
    qDebug() << "PreviewWindow::setLayoutItems: Clearing previous widgets from m_widgetMap";
    qDeleteAll(m_widgetMap.values());
    m_widgetMap.clear();

    // 设置previewWidget为容器
    QWidget *containerWidget = ui->previewWidget;
    if (!containerWidget) {
        qCritical() << "PreviewWindow::setLayoutItems: ui->previewWidget为空！";
        return;
    }
    qDebug() << "PreviewWindow::setLayoutItems: 使用containerWidget:" << containerWidget;

    // 确保预览容器没有布局，以便手动定位控件
    if (containerWidget->layout()) {
        delete containerWidget->layout();
        containerWidget->setLayout(nullptr);
        qDebug() << "PreviewWindow::setLayoutItems: 删除了容器的旧布局";
    }

    // 遍历所有布局项并创建真实控件
    QTabWidget *tabWidget = nullptr;
    QWidget *tabPage1 = nullptr;
    QWidget *tabPage2 = nullptr;

    // 首先遍历查找用户拖入的QTabWidget
    foreach (LayoutItem *item, items) {
        if (!item) {
            qDebug() << "PreviewWindow::setLayoutItems: 跳过空的LayoutItem";
            continue;
        }
        qDebug() << "PreviewWindow::setLayoutItems: 处理LayoutItem:" << item << "类型:" << item->widgetType();

        if (item->widgetType() == "QTabWidget") {
            // 使用LayoutItem自己的createWidget方法创建用户拖入的QTabWidget
            tabWidget = qobject_cast<QTabWidget *>(item->createWidget(containerWidget));
            if (tabWidget) {
                // 设置TabWidget位置和大小
                tabWidget->move(item->pos());
                tabWidget->resize(item->size());
                tabWidget->show();

                // 保存映射关系
                m_widgetMap[item] = tabWidget;
                m_widgetToItemMap[tabWidget] = item; // 建立反向映射
                qDebug() << "PreviewWindow::setLayoutItems: Added user-dragged QTabWidget to m_widgetMap and m_widgetToItemMap" << tabWidget;

                // 获取TabWidget的Tab页
                if (tabWidget->count() >= 2) {
                    tabPage1 = tabWidget->widget(0);
                    tabPage2 = tabWidget->widget(1);
                    qDebug() << "PreviewWindow::setLayoutItems: 获取了用户拖入的TabWidget的Tab页:" << tabPage1 << tabPage2;
                } else {
                    // 如果TabWidget没有足够的Tab页，则创建默认Tab页
                    tabPage1 = new QWidget(tabWidget);
                    tabWidget->addTab(tabPage1, "Tab 1");
                    tabPage2 = new QWidget(tabWidget);
                    tabWidget->addTab(tabPage2, "Tab 2");
                    qDebug() << "PreviewWindow::setLayoutItems: 为用户拖入的TabWidget创建了默认Tab页:" << tabPage1 << tabPage2;
                }
                break;
            } else {
                qDebug() << "PreviewWindow::setLayoutItems: 创建用户拖入的TabWidget失败";
            }
        }
    }

    // 再次遍历，将其他控件添加到对应的Tab页或直接添加到容器
    foreach (LayoutItem *item, items) {
        if (!item) {
            qDebug() << "PreviewWindow::setLayoutItems: 跳过空的LayoutItem";
            continue;
        }

        // 跳过已经处理过的QTabWidget
        if (item->widgetType() == "QTabWidget") {
            qDebug() << "PreviewWindow::setLayoutItems: 跳过已经处理过的QTabWidget";
            continue;
        }
        qDebug() << "PreviewWindow::setLayoutItems: 处理非QTabWidget的控件:" << item << "类型:" << item->widgetType();

        // 确定控件的父容器
        QWidget *parentWidget = containerWidget;
        int tabIndex = item->tabIndex();
        qDebug() << "PreviewWindow::setLayoutItems: 控件所属Tab页索引:" << tabIndex;
        
        // 如果存在TabWidget且Tab页索引有效，则将控件添加到对应的Tab页
        if (tabWidget) {
            if (tabIndex == 0) {
                if (tabPage1) {
                    parentWidget = tabPage1;
                    qDebug() << "PreviewWindow::setLayoutItems: 将控件添加到Tab页1:" << tabPage1;
                } else {
                    qDebug() << "PreviewWindow::setLayoutItems: tabPage1为空，将控件添加到容器";
                }
            } else if (tabIndex == 1) {
                if (tabPage2) {
                    parentWidget = tabPage2;
                    qDebug() << "PreviewWindow::setLayoutItems: 将控件添加到Tab页2:" << tabPage2;
                } else {
                    qDebug() << "PreviewWindow::setLayoutItems: tabPage2为空，将控件添加到容器";
                }
            }
        } else {
            // 没有TabWidget时，直接将控件添加到容器
            qDebug() << "PreviewWindow::setLayoutItems: 没有TabWidget，将控件直接添加到容器";
        }
        if (!parentWidget) {
            qCritical() << "PreviewWindow::setLayoutItems: 父容器为空！";
            continue;
        }

        // 使用LayoutItem自己的createWidget方法创建控件
        QObject *createdObj = item->createWidget(parentWidget);
        if (!createdObj) {
            qDebug() << "PreviewWindow::setLayoutItems: Failed to create widget of type:" << item->widgetType();
            continue;
        }

        // 检查是否是Widget
        QWidget *widget = qobject_cast<QWidget*>(createdObj);
        if (!widget) {
            // 是布局类，不需要设置位置和大小
            continue;
        }
        qDebug() << "PreviewWindow::setLayoutItems: 创建控件成功:" << widget;

        // 设置控件位置和大小
        widget->move(item->pos());
        widget->resize(item->size());
        widget->show();

        // 打印控件坐标和大小信息
        qDebug() << "PreviewWindow::setLayoutItems: 控件类型:" << item->widgetType() 
                 << " 位置:" << item->pos() 
                 << " 大小:" << item->size() 
                 << " 所属Tab页:" << (item->tabIndex() + 1)
                 << " 父容器:" << parentWidget->objectName()
                 << " 在预览窗口中的位置:" << widget->pos() 
                 << " 在预览窗口中的大小:" << widget->size();

        // 保存映射关系
        m_widgetMap[item] = widget;
        m_widgetToItemMap[widget] = item; // 建立反向映射
        qDebug() << "PreviewWindow::setLayoutItems: Added widget to m_widgetMap and m_widgetToItemMap" << widget;
    }

    // 更新预览
    ui->previewWidget->update();
    qDebug() << "PreviewWindow::setLayoutItems: Updated ui->previewWidget" << ui->previewWidget;
}

// 鼠标双击事件处理
void PreviewWindow::mouseDoubleClickEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        QPoint pos = event->pos();
        qDebug() << "PreviewWindow::mouseDoubleClickEvent: 双击位置:" << pos;
        
        // 遍历所有控件，找到被双击的控件
        for (QWidget *widget : m_widgetToItemMap.keys()) {
            if (widget->isVisible() && widget->geometry().contains(pos)) {
                qDebug() << "PreviewWindow::mouseDoubleClickEvent: 找到被双击的控件:" << widget;
                
                LayoutItem *item = m_widgetToItemMap.value(widget);
                if (!item) {
                    qDebug() << "PreviewWindow::mouseDoubleClickEvent: 无法找到对应的LayoutItem";
                    continue;
                }
                
                // 处理QLabel双击编辑
                QLabel *label = qobject_cast<QLabel*>(widget);
                if (label) {
                    qDebug() << "PreviewWindow::mouseDoubleClickEvent: 处理QLabel双击编辑";
                    handleLabelDoubleClick(label, item);
                    return;
                }
                
                // 处理QTabWidget双击编辑
                QTabWidget *tabWidget = qobject_cast<QTabWidget*>(widget);
                if (tabWidget) {
                    qDebug() << "PreviewWindow::mouseDoubleClickEvent: 处理QTabWidget双击编辑";
                    handleTabWidgetDoubleClick(tabWidget, pos, item);
                    return;
                }
                
                qDebug() << "PreviewWindow::mouseDoubleClickEvent: 控件类型不支持双击编辑:" << widget->metaObject()->className();
                break;
            }
        }
    }
    
    // 调用基类处理
    QMainWindow::mouseDoubleClickEvent(event);
}

// 处理QLabel双击编辑
void PreviewWindow::handleLabelDoubleClick(QWidget *label, LayoutItem *item) {
    if (!label || !item) {
        qDebug() << "PreviewWindow::handleLabelDoubleClick: 参数为空";
        return;
    }
    
    QLabel *qlabel = qobject_cast<QLabel*>(label);
    if (!qlabel) {
        qDebug() << "PreviewWindow::handleLabelDoubleClick: 控件不是QLabel类型";
        return;
    }
    
    bool ok;
    QString text = QInputDialog::getText(this, tr("编辑文本"), tr("文本:"), QLineEdit::Normal, qlabel->text(), &ok);
    if (ok && !text.isEmpty()) {
        // 更新Label文本
        qlabel->setText(text);
        
        // 更新LayoutItem的文本
        item->setText(text);
        
        qDebug() << "PreviewWindow::handleLabelDoubleClick: QLabel文本已修改为:" << text;
        
        // 显示成功消息
        QMessageBox::information(this, tr("编辑成功"), tr("文本已成功修改"));
    }
}

// 处理QTabWidget双击编辑
void PreviewWindow::handleTabWidgetDoubleClick(QWidget *tabWidget, const QPoint &pos, LayoutItem *item) {
    if (!tabWidget || !item) {
        qDebug() << "PreviewWindow::handleTabWidgetDoubleClick: 参数为空";
        return;
    }
    
    QTabWidget *qtabWidget = qobject_cast<QTabWidget*>(tabWidget);
    if (!qtabWidget) {
        qDebug() << "PreviewWindow::handleTabWidgetDoubleClick: 控件不是QTabWidget类型";
        return;
    }
    
    // 转换为QTabWidget的本地坐标
    QPoint tabWidgetLocalPos = qtabWidget->mapFromGlobal(pos);
    
    // 获取Tab栏的区域
    QRect tabBarRect = qtabWidget->tabBar()->geometry();
    if (tabBarRect.contains(tabWidgetLocalPos)) {
        // 计算用户点击的是哪个Tab页
        int tabIndex = qtabWidget->tabBar()->tabAt(tabWidgetLocalPos);
        if (tabIndex != -1) {
            bool ok;
            QString text = QInputDialog::getText(this, tr("编辑Tab标题"), tr("标题:"), QLineEdit::Normal, qtabWidget->tabText(tabIndex), &ok);
            if (ok && !text.isEmpty()) {
                // 更新Tab页标题
                qtabWidget->setTabText(tabIndex, text);
                
                // 更新LayoutItem的文本
                item->setText(text);
                
                qDebug() << "PreviewWindow::handleTabWidgetDoubleClick: Tab页标题已修改为:" << text;
                
                // 显示成功消息
                QMessageBox::information(this, tr("编辑成功"), tr("Tab页标题已成功修改"));
            }
        }
    }
}
