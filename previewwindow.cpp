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

PreviewWindow::PreviewWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::PreviewWindow)
{
    ui->setupUi(this);

    // 设置主窗口属性
    setWindowTitle("UI布局预览");
    setMinimumSize(800, 600);

    // 使用ui中的previewWidget
    m_previewWidget = ui->previewWidget;
    m_previewWidget->setMinimumSize(800, 600);
}

PreviewWindow::~PreviewWindow()
{
    delete ui;
    qDeleteAll(m_widgetMap.values());
    m_widgetMap.clear();
}
QWidget* PreviewWindow::createWidgetFromType(const QString &widgetType, QWidget *parent) {
    qDebug() << "Creating widget of type:" << widgetType;
    // 根据控件类型创建真实的Qt控件
    if (widgetType == "QPushButton") {
        return new QPushButton("Button", parent);
    } else if (widgetType == "QLineEdit") {
        QLineEdit *lineEdit = new QLineEdit(parent);
        lineEdit->setText("Line Edit");
        return lineEdit;
    } else if (widgetType == "QLabel") {
        return new QLabel("Label", parent);
    } else if (widgetType == "QCheckBox") {
        return new QCheckBox("Check Box", parent);
    } else if (widgetType == "QRadioButton") {
        return new QRadioButton("Radio Button", parent);
    } else if (widgetType == "QTextEdit") {
        QTextEdit *textEdit = new QTextEdit(parent);
        textEdit->setText("Text Edit");
        return textEdit;
    } else if (widgetType == "QComboBox") {
        QComboBox *comboBox = new QComboBox(parent);
        comboBox->addItem("Option 1");
        comboBox->addItem("Option 2");
        comboBox->addItem("Option 3");
        return comboBox;
    } else if (widgetType == "QSpinBox") {
        return new QSpinBox(parent);
    } else if (widgetType == "QSlider") {
        return new QSlider(Qt::Horizontal, parent);
    } else if (widgetType == "QProgressBar") {
        QProgressBar *progressBar = new QProgressBar(parent);
        progressBar->setValue(50);
        return progressBar;
    } else if (widgetType == "QCalendarWidget") {
        return new QCalendarWidget(parent);
    } else if (widgetType == "QGroupBox") {
        return new QGroupBox("Group Box", parent);
    } else if (widgetType == "QTabWidget") {
        QTabWidget *tabWidget = new QTabWidget(parent);
        return tabWidget;
    } else if (widgetType == "QListWidget") {
        QListWidget *listWidget = new QListWidget(parent);
        listWidget->addItem("Item 1");
        listWidget->addItem("Item 2");
        listWidget->addItem("Item 3");
        return listWidget;
    } else if (widgetType == "QTableWidget") {
        return new QTableWidget(3, 3, parent);
    } else {
        // 默认返回一个QWidget
        QWidget *widget = new QWidget(parent);
        widget->setStyleSheet("background-color: lightgray; border: 1px solid gray;");
        return widget;
    }
}

void PreviewWindow::setLayoutItems(const QList<LayoutItem *> &items){ 
    // 清除之前的预览控件
    qDebug() << "Clearing previous widgets from m_widgetMap";
    qDeleteAll(m_widgetMap.values());
    m_widgetMap.clear();

    // 设置previewWidget为容器
    QWidget *containerWidget = m_previewWidget;

    // 确保预览容器没有布局，以便手动定位控件
    if (containerWidget->layout()) {
        delete containerWidget->layout();
        containerWidget->setLayout(nullptr);
    }
    
    // 遍历所有布局项并创建真实控件
    foreach (LayoutItem *item, items) {
        if (!item) continue;

        QString widgetType = item->widgetType();
        qDebug() << "Processing item with widgetType:" << widgetType;

        QWidget *widget = createWidgetFromType(widgetType, containerWidget);
        if (!widget) {
            qDebug() << "Failed to create widget of type:" << widgetType;
            continue;
        }

        // 设置控件位置和大小
        widget->move(item->pos());
        widget->resize(item->size());
        widget->show();

        // 保存映射关系
        m_widgetMap[item] = widget;
        qDebug() << "Added widget to m_widgetMap";
    }

    // 更新预览
    m_previewWidget->update();
    qDebug() << "Updated m_previewWidget";
}
