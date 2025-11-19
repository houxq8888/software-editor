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

void PreviewWindow::setLayoutItems(const QList<LayoutItem *> &items){ 
    // 清除之前的预览控件
    qDeleteAll(m_widgetMap.values());
    m_widgetMap.clear();

    // 遍历所有布局项并创建真实控件
    foreach (LayoutItem *item, items) {
        if (!item) continue;

        QWidget *widget = nullptr;
        QString widgetType = item->widgetType();

        // 根据控件类型创建真实的Qt控件
        if (widgetType == "QPushButton") {
            widget = new QPushButton("Button", m_previewWidget);
        } else if (widgetType == "QLineEdit") {
            QLineEdit *lineEdit = new QLineEdit(m_previewWidget);
            lineEdit->setText("Line Edit");
            widget = lineEdit;
        } else if (widgetType == "QLabel") {
            QString text = item->text().isEmpty() ? "Label" : item->text();
            widget = new QLabel(text, m_previewWidget);
        } else if (widgetType == "QCheckBox") {
            widget = new QCheckBox("Check Box", m_previewWidget);
        } else if (widgetType == "QRadioButton") {
            widget = new QRadioButton("Radio Button", m_previewWidget);
        } else if (widgetType == "QTextEdit") {
            QTextEdit *textEdit = new QTextEdit(m_previewWidget);
            textEdit->setText("Text Edit");
            widget = textEdit;
        } else if (widgetType == "QComboBox") {
            QComboBox *comboBox = new QComboBox(m_previewWidget);
            comboBox->addItem("Option 1");
            comboBox->addItem("Option 2");
            comboBox->addItem("Option 3");
            widget = comboBox;
        } else if (widgetType == "QSpinBox") {
            widget = new QSpinBox(m_previewWidget);
        } else if (widgetType == "QSlider") {
            QSlider *slider = new QSlider(Qt::Horizontal, m_previewWidget);
            widget = slider;
        } else if (widgetType == "QProgressBar") {
            QProgressBar *progressBar = new QProgressBar(m_previewWidget);
            progressBar->setValue(50);
            widget = progressBar;
        } else if (widgetType == "QCalendarWidget") {
            widget = new QCalendarWidget(m_previewWidget);
        } else if (widgetType == "QGroupBox") {
            QGroupBox *groupBox = new QGroupBox("Group Box", m_previewWidget);
            widget = groupBox;
        } else if (widgetType == "QTabWidget") {
            QTabWidget *tabWidget = new QTabWidget(m_previewWidget);
            QStringList tabs = item->text().split("|");
            if (tabs.isEmpty()) {
                tabs << "Tab 1" << "Tab 2";
            }
            for (const QString &tabName : tabs) {
                QWidget *tab = new QWidget();
                tabWidget->addTab(tab, tabName);
            }
            widget = tabWidget;
        } else if (widgetType == "QListWidget") {
            QListWidget *listWidget = new QListWidget(m_previewWidget);
            listWidget->addItem("Item 1");
            listWidget->addItem("Item 2");
            listWidget->addItem("Item 3");
            widget = listWidget;
        } else if (widgetType == "QTableWidget") {
            QTableWidget *tableWidget = new QTableWidget(3, 3, m_previewWidget);
            widget = tableWidget;
        } else {
            // 默认创建一个QWidget
            widget = new QWidget(m_previewWidget);
            widget->setStyleSheet("background-color: lightgray; border: 1px solid gray;");
        }

        if (widget) {
            // 设置控件位置和大小
            widget->setGeometry(item->pos().x(), item->pos().y(), item->width(), item->height());
            widget->show();

            // 保存映射关系
            m_widgetMap[item] = widget;
        }
    }

    // 更新预览
    m_previewWidget->update();
}
