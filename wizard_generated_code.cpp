// 自动生成的向导应用程序代码
// 生成时间: 2026-02-03 21:50:36

#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QFile>
#include <QUiLoader>
#include <QDebug>
#include <QStackedWidget>

class Page0;
class Page1;
class Page2;
class Page3;
class Page4;
class Page5;

class WizardMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit WizardMainWindow(QWidget *parent = nullptr);
    ~WizardMainWindow();

private slots:
    void goToPage0();
    void goToPage1();
    void goToPage2();
    void goToPage3();
    void goToPage4();
    void goToPage5();
    void onWizardFinished();

private:
    QStackedWidget *m_stackedWidget;
    int m_currentPageIndex;
    int m_totalPages;
};

// 页面 1: 43214
class Page0 : public QWidget
{
    Q_OBJECT
public:
    explicit Page0(QWidget *parent = nullptr);
    ~Page%1();

signals:
    void nextPageRequested();
    void previousPageRequested();

private:
    QWidget *m_uiWidget;
};

Page0::Page0(QWidget *parent)
    : QWidget(parent)
    , m_uiWidget(nullptr)
{
    // 加载UI文件
    QString uiFilePath = "D:/virtualMachine/github/0203/kimi/software-editor/product_configurations/product_12345/ui_layouts/43214.ui";
    QFile uiFile(uiFilePath);
    if (uiFile.open(QIODevice::ReadOnly)) {
        QUiLoader loader;
        m_uiWidget = loader.load(&uiFile, this);
        uiFile.close();

        if (m_uiWidget) {
            m_uiWidget->setParent(this);
            // 保持原UI的几何位置
            QRect geom = m_uiWidget->geometry();
            if (geom.isEmpty()) {
                geom = QRect(10, 10, 400, 300);
            }
            m_uiWidget->setGeometry(geom);

            // 连接所有按钮的点击信号
            QList<QPushButton*> buttons = m_uiWidget->findChildren<QPushButton*>();
            for (QPushButton* btn : buttons) {
                connect(btn, &QPushButton::clicked, this, [this]() {
                    emit nextPageRequested();
                });
            }
        }
    }

    // 设置窗口大小
    setMinimumSize(600, 400);
}

Page0::~Page0()
{
}

// 页面 2: 1208
class Page1 : public QWidget
{
    Q_OBJECT
public:
    explicit Page1(QWidget *parent = nullptr);
    ~Page%1();

signals:
    void nextPageRequested();
    void previousPageRequested();

private:
    QWidget *m_uiWidget;
};

Page1::Page1(QWidget *parent)
    : QWidget(parent)
    , m_uiWidget(nullptr)
{
    // 加载UI文件
    QString uiFilePath = "D:/virtualMachine/github/0203/kimi/software-editor/product_configurations/product_12345/ui_layouts/1208.ui";
    QFile uiFile(uiFilePath);
    if (uiFile.open(QIODevice::ReadOnly)) {
        QUiLoader loader;
        m_uiWidget = loader.load(&uiFile, this);
        uiFile.close();

        if (m_uiWidget) {
            m_uiWidget->setParent(this);
            // 保持原UI的几何位置
            QRect geom = m_uiWidget->geometry();
            if (geom.isEmpty()) {
                geom = QRect(10, 10, 400, 300);
            }
            m_uiWidget->setGeometry(geom);

            // 连接所有按钮的点击信号
            QList<QPushButton*> buttons = m_uiWidget->findChildren<QPushButton*>();
            for (QPushButton* btn : buttons) {
                connect(btn, &QPushButton::clicked, this, [this]() {
                    emit nextPageRequested();
                });
            }
        }
    }

    // 设置窗口大小
    setMinimumSize(600, 400);
}

Page1::~Page1()
{
}

// 页面 3: 1209
class Page2 : public QWidget
{
    Q_OBJECT
public:
    explicit Page2(QWidget *parent = nullptr);
    ~Page%1();

signals:
    void nextPageRequested();
    void previousPageRequested();

private:
    QWidget *m_uiWidget;
};

Page2::Page2(QWidget *parent)
    : QWidget(parent)
    , m_uiWidget(nullptr)
{
    // 加载UI文件
    QString uiFilePath = "D:/virtualMachine/github/0203/kimi/software-editor/product_configurations/product_12345/ui_layouts/1209.ui";
    QFile uiFile(uiFilePath);
    if (uiFile.open(QIODevice::ReadOnly)) {
        QUiLoader loader;
        m_uiWidget = loader.load(&uiFile, this);
        uiFile.close();

        if (m_uiWidget) {
            m_uiWidget->setParent(this);
            // 保持原UI的几何位置
            QRect geom = m_uiWidget->geometry();
            if (geom.isEmpty()) {
                geom = QRect(10, 10, 400, 300);
            }
            m_uiWidget->setGeometry(geom);

            // 连接所有按钮的点击信号
            QList<QPushButton*> buttons = m_uiWidget->findChildren<QPushButton*>();
            for (QPushButton* btn : buttons) {
                connect(btn, &QPushButton::clicked, this, [this]() {
                    emit nextPageRequested();
                });
            }
        }
    }

    // 设置窗口大小
    setMinimumSize(600, 400);
}

Page2::~Page2()
{
}

// 页面 4: 1209-1
class Page3 : public QWidget
{
    Q_OBJECT
public:
    explicit Page3(QWidget *parent = nullptr);
    ~Page%1();

signals:
    void nextPageRequested();
    void previousPageRequested();

private:
    QWidget *m_uiWidget;
};

Page3::Page3(QWidget *parent)
    : QWidget(parent)
    , m_uiWidget(nullptr)
{
    // 加载UI文件
    QString uiFilePath = "D:/virtualMachine/github/0203/kimi/software-editor/product_configurations/product_12345/ui_layouts/1209-1.ui";
    QFile uiFile(uiFilePath);
    if (uiFile.open(QIODevice::ReadOnly)) {
        QUiLoader loader;
        m_uiWidget = loader.load(&uiFile, this);
        uiFile.close();

        if (m_uiWidget) {
            m_uiWidget->setParent(this);
            // 保持原UI的几何位置
            QRect geom = m_uiWidget->geometry();
            if (geom.isEmpty()) {
                geom = QRect(10, 10, 400, 300);
            }
            m_uiWidget->setGeometry(geom);

            // 连接所有按钮的点击信号
            QList<QPushButton*> buttons = m_uiWidget->findChildren<QPushButton*>();
            for (QPushButton* btn : buttons) {
                connect(btn, &QPushButton::clicked, this, [this]() {
                    emit nextPageRequested();
                });
            }
        }
    }

    // 设置窗口大小
    setMinimumSize(600, 400);
}

Page3::~Page3()
{
}

// 页面 5: 1209-2
class Page4 : public QWidget
{
    Q_OBJECT
public:
    explicit Page4(QWidget *parent = nullptr);
    ~Page%1();

signals:
    void nextPageRequested();
    void previousPageRequested();

private:
    QWidget *m_uiWidget;
};

Page4::Page4(QWidget *parent)
    : QWidget(parent)
    , m_uiWidget(nullptr)
{
    // 加载UI文件
    QString uiFilePath = "D:/virtualMachine/github/0203/kimi/software-editor/product_configurations/product_12345/ui_layouts/1209-2.ui";
    QFile uiFile(uiFilePath);
    if (uiFile.open(QIODevice::ReadOnly)) {
        QUiLoader loader;
        m_uiWidget = loader.load(&uiFile, this);
        uiFile.close();

        if (m_uiWidget) {
            m_uiWidget->setParent(this);
            // 保持原UI的几何位置
            QRect geom = m_uiWidget->geometry();
            if (geom.isEmpty()) {
                geom = QRect(10, 10, 400, 300);
            }
            m_uiWidget->setGeometry(geom);

            // 连接所有按钮的点击信号
            QList<QPushButton*> buttons = m_uiWidget->findChildren<QPushButton*>();
            for (QPushButton* btn : buttons) {
                connect(btn, &QPushButton::clicked, this, [this]() {
                    emit nextPageRequested();
                });
            }
        }
    }

    // 设置窗口大小
    setMinimumSize(600, 400);
}

Page4::~Page4()
{
}

// 页面 6: untitled
class Page5 : public QWidget
{
    Q_OBJECT
public:
    explicit Page5(QWidget *parent = nullptr);
    ~Page%1();

signals:
    void nextPageRequested();
    void previousPageRequested();

private:
    QWidget *m_uiWidget;
};

Page5::Page5(QWidget *parent)
    : QWidget(parent)
    , m_uiWidget(nullptr)
{
    // 加载UI文件
    QString uiFilePath = "D:/virtualMachine/github/0203/kimi/software-editor/product_configurations/product_12345/ui_layouts/untitled.ui";
    QFile uiFile(uiFilePath);
    if (uiFile.open(QIODevice::ReadOnly)) {
        QUiLoader loader;
        m_uiWidget = loader.load(&uiFile, this);
        uiFile.close();

        if (m_uiWidget) {
            m_uiWidget->setParent(this);
            // 保持原UI的几何位置
            QRect geom = m_uiWidget->geometry();
            if (geom.isEmpty()) {
                geom = QRect(10, 10, 400, 300);
            }
            m_uiWidget->setGeometry(geom);

            // 连接所有按钮的点击信号
            QList<QPushButton*> buttons = m_uiWidget->findChildren<QPushButton*>();
            for (QPushButton* btn : buttons) {
                connect(btn, &QPushButton::clicked, this, [this]() {
                    emit nextPageRequested();
                });
            }
        }
    }

    // 设置窗口大小
    setMinimumSize(600, 400);
}

Page5::~Page5()
{
}

WizardMainWindow::WizardMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_stackedWidget(new QStackedWidget(this))
    , m_currentPageIndex(0)
    , m_totalPages(6)
{
    setWindowTitle("MainApplicationWizard");
    resize(1024, 768);

    // 创建所有页面
    Page0 *page0 = new Page0(this);
    connect(page0, &Page0::nextPageRequested, this, &WizardMainWindow::goToPage0);
    m_stackedWidget->addWidget(page0);

    Page1 *page1 = new Page1(this);
    connect(page1, &Page1::nextPageRequested, this, &WizardMainWindow::goToPage1);
    m_stackedWidget->addWidget(page1);

    Page2 *page2 = new Page2(this);
    connect(page2, &Page2::nextPageRequested, this, &WizardMainWindow::goToPage2);
    m_stackedWidget->addWidget(page2);

    Page3 *page3 = new Page3(this);
    connect(page3, &Page3::nextPageRequested, this, &WizardMainWindow::goToPage3);
    m_stackedWidget->addWidget(page3);

    Page4 *page4 = new Page4(this);
    connect(page4, &Page4::nextPageRequested, this, &WizardMainWindow::goToPage4);
    m_stackedWidget->addWidget(page4);

    Page5 *page5 = new Page5(this);
    connect(page5, &Page5::nextPageRequested, this, &WizardMainWindow::goToPage5);
    m_stackedWidget->addWidget(page5);

    setCentralWidget(m_stackedWidget);
}

WizardMainWindow::~WizardMainWindow()
{
}

void WizardMainWindow::goToPage0()
{
    // 跳转到页面 1
    m_stackedWidget->setCurrentIndex(1);
    m_currentPageIndex = 1;
}

void WizardMainWindow::goToPage1()
{
    // 跳转到页面 2
    m_stackedWidget->setCurrentIndex(2);
    m_currentPageIndex = 2;
}

void WizardMainWindow::goToPage2()
{
    // 跳转到页面 3
    m_stackedWidget->setCurrentIndex(3);
    m_currentPageIndex = 3;
}

void WizardMainWindow::goToPage3()
{
    // 跳转到页面 4
    m_stackedWidget->setCurrentIndex(4);
    m_currentPageIndex = 4;
}

void WizardMainWindow::goToPage4()
{
    // 跳转到页面 5
    m_stackedWidget->setCurrentIndex(5);
    m_currentPageIndex = 5;
}

void WizardMainWindow::goToPage5()
{
    // 最后一个页面，显示完成提示
    QMessageBox::information(this, "完成", "恭喜！您已完成所有步骤。");
    onWizardFinished();
}

void WizardMainWindow::onWizardFinished()
{
    // 向导完成处理
    qDebug() << "Wizard finished";
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("MainApplicationWizard");
    app.setApplicationVersion("1.0");

    WizardMainWindow window;
    window.show();

    return app.exec();
}

#include "main.moc"