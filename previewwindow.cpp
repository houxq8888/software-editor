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

void PreviewWindow::setLayoutItems(const QList<LayoutItem *> &items)
{
    // 清除之前的预览控件
    qDeleteAll(m_widgetMap.values());
    m_widgetMap.clear();
    
    // 创建一个布局管理器
    QLayout *existingLayout = m_previewWidget->layout();
    if (existingLayout) {
        delete existingLayout;
    }
    QVBoxLayout *layout = new QVBoxLayout(m_previewWidget);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(20);
    
    // 创建所有预览控件
    foreach (LayoutItem *item, items) {
        QWidget *widget = nullptr;
        QString widgetType = item->widgetType();
        QString text = item->text();
        
        // 根据控件类型创建实际的Qt控件
        if (widgetType == "QLabel") {
            QLabel *label = new QLabel(text, m_previewWidget);
            widget = label;
        } else if (widgetType == "QLineEdit") {
            QLineEdit *lineEdit = new QLineEdit(text, m_previewWidget);
            widget = lineEdit;
        } else if (widgetType == "QPushButton") {
            QPushButton *button = new QPushButton(text.isEmpty() ? "Button" : text, m_previewWidget);
            widget = button;
        } else if (widgetType == "QTextEdit") {
            QTextEdit *textEdit = new QTextEdit(text.isEmpty() ? "Text Edit" : text, m_previewWidget);
            widget = textEdit;
        } 
        // 支持更多控件类型
        else if (widgetType == "QCheckBox") {
            QCheckBox *checkBox = new QCheckBox(text.isEmpty() ? "CheckBox" : text, m_previewWidget);
            widget = checkBox;
        } else if (widgetType == "QRadioButton") {
            QRadioButton *radioButton = new QRadioButton(text.isEmpty() ? "RadioButton" : text, m_previewWidget);
            widget = radioButton;
        } else if (widgetType == "QComboBox") {
            QComboBox *comboBox = new QComboBox(m_previewWidget);
            comboBox->addItem(text.isEmpty() ? "Option 1" : text);
            comboBox->addItem("Option 2");
            comboBox->addItem("Option 3");
            widget = comboBox;
        } else if (widgetType == "QSpinBox") {
            QSpinBox *spinBox = new QSpinBox(m_previewWidget);
            if (!text.isEmpty()) {
                spinBox->setValue(text.toInt());
            }
            widget = spinBox;
        } else if (widgetType == "QSlider") {
            QSlider *slider = new QSlider(Qt::Horizontal, m_previewWidget);
            if (!text.isEmpty()) {
                slider->setValue(text.toInt());
            }
            widget = slider;
        }
        
        if (widget) {
            // 设置控件大小
            widget->setFixedSize(item->width(), item->height());
            
            // 添加到布局中
            layout->addWidget(widget);
            
            // 保存映射关系
            m_widgetMap.insert(item, widget);
        }
    }

    // 创建一个布局管理器
    QVBoxLayout *mainLayout = new QVBoxLayout(m_previewWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    m_previewWidget->setLayout(mainLayout);

    // 创建一个容器widget来容纳所有预览控件
    QWidget *containerWidget = new QWidget(m_previewWidget);
    containerWidget->setStyleSheet("background-color: transparent;");
    containerWidget->setFixedSize(800, 600);
    mainLayout->addWidget(containerWidget);

    // 遍历所有布局项并创建真实控件
    foreach (LayoutItem *item, items) {
        if (!item) continue;

        QWidget *widget = nullptr;
        QString widgetType = item->widgetType();

        // 根据控件类型创建真实的Qt控件
        if (widgetType == "QPushButton") {
            widget = new QPushButton("Button", containerWidget);
        } else if (widgetType == "QLineEdit") {
            QLineEdit *lineEdit = new QLineEdit(containerWidget);
            lineEdit->setText("Line Edit");
            widget = lineEdit;
        } else if (widgetType == "QLabel") {
            widget = new QLabel("Label", containerWidget);
        } else if (widgetType == "QCheckBox") {
            widget = new QCheckBox("Check Box", containerWidget);
        } else if (widgetType == "QRadioButton") {
            widget = new QRadioButton("Radio Button", containerWidget);
        } else if (widgetType == "QTextEdit") {
            QTextEdit *textEdit = new QTextEdit(containerWidget);
            textEdit->setText("Text Edit");
            widget = textEdit;
        } else if (widgetType == "QComboBox") {
            QComboBox *comboBox = new QComboBox(containerWidget);
            comboBox->addItem("Option 1");
            comboBox->addItem("Option 2");
            comboBox->addItem("Option 3");
            widget = comboBox;
        } else if (widgetType == "QSpinBox") {
            widget = new QSpinBox(containerWidget);
        } else if (widgetType == "QSlider") {
            QSlider *slider = new QSlider(Qt::Horizontal, containerWidget);
            widget = slider;
        } else if (widgetType == "QProgressBar") {
            QProgressBar *progressBar = new QProgressBar(containerWidget);
            progressBar->setValue(50);
            widget = progressBar;
        } else if (widgetType == "QCalendarWidget") {
            widget = new QCalendarWidget(containerWidget);
        } else if (widgetType == "QGroupBox") {
            QGroupBox *groupBox = new QGroupBox("Group Box", containerWidget);
            widget = groupBox;
        } else if (widgetType == "QTabWidget") {
            QTabWidget *tabWidget = new QTabWidget(containerWidget);
            QWidget *tab1 = new QWidget();
            QWidget *tab2 = new QWidget();
            tabWidget->addTab(tab1, "Tab 1");
            tabWidget->addTab(tab2, "Tab 2");
            widget = tabWidget;
        } else if (widgetType == "QListWidget") {
            QListWidget *listWidget = new QListWidget(containerWidget);
            listWidget->addItem("Item 1");
            listWidget->addItem("Item 2");
            listWidget->addItem("Item 3");
            widget = listWidget;
        } else if (widgetType == "QTableWidget") {
            QTableWidget *tableWidget = new QTableWidget(3, 3, containerWidget);
            widget = tableWidget;
        } else {
            // 默认创建一个QWidget
            widget = new QWidget(containerWidget);
            widget->setStyleSheet("background-color: lightgray; border: 1px solid gray;");
        }

        if (widget) {
            // 设置控件位置和大小
            widget->setGeometry(item->pos().x(), item->pos().y(), item->width(), item->height());
            widget->setParent(containerWidget);
            widget->show();

            // 保存映射关系
            m_widgetMap[item] = widget;
        }
    }

    // 更新预览
    m_previewWidget->update();
}
