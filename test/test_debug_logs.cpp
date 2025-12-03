#include <QApplication>
#include <QDebug>
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

class TestWindow : public QMainWindow {
    Q_OBJECT

public:
    TestWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
        QWidget *centralWidget = new QWidget(this);
        QVBoxLayout *layout = new QVBoxLayout(centralWidget);
        
        QPushButton *testButton = new QPushButton("测试调试日志", centralWidget);
        connect(testButton, &QPushButton::clicked, this, &TestWindow::testDebugLogs);
        
        layout->addWidget(testButton);
        centralWidget->setLayout(layout);
        setCentralWidget(centralWidget);
    }

private slots:
    void testDebugLogs() {
        qDebug() << "测试: 调试日志正常工作";
        qDebug() << "UILayoutWindow: QLabel文本修改，设置m_isModified = true";
        qDebug() << "UILayoutWindow: Tab页标题修改，设置m_isModified = true";
        qDebug() << "UILayoutWindow::closeEvent - m_isModified:" << true;
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    TestWindow window;
    window.show();
    
    return app.exec();
}

#include "test_debug_logs.moc"