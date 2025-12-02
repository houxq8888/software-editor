#include <QApplication>
#include <QProcess>
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // 创建主窗口
    QMainWindow mainWindow;
    mainWindow.setWindowTitle("软件编辑器");
    mainWindow.resize(400, 200);

    // 创建中央部件和布局
    QWidget *centralWidget = new QWidget(&mainWindow);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    // 创建按钮
    QPushButton *openDesignerButton = new QPushButton("打开Qt Designer", centralWidget);
    QPushButton *openUIFileButton = new QPushButton("打开UI文件", centralWidget);

    // 添加按钮到布局
    layout->addWidget(openDesignerButton);
    layout->addWidget(openUIFileButton);

    // 设置中央部件
    mainWindow.setCentralWidget(centralWidget);

    // 打开Qt Designer的槽函数
    QObject::connect(openDesignerButton, &QPushButton::clicked, [&]() {
        QProcess designerProcess;
        QString designerPath = "designer.exe"; // 假设Qt Designer在系统PATH中

        // 启动Qt Designer
        if (designerProcess.startDetached(designerPath)) {
            QMessageBox::information(&mainWindow, "成功", "Qt Designer已启动");
        } else {
            QMessageBox::critical(&mainWindow, "失败", "无法启动Qt Designer。请确保Qt Designer已正确安装并添加到系统PATH中。");
        }
    });

    // 打开UI文件的槽函数
    QObject::connect(openUIFileButton, &QPushButton::clicked, [&]() {
        QString uiFilePath = QFileDialog::getOpenFileName(&mainWindow, "打开UI文件", "", "UI文件 (*.ui)");

        if (!uiFilePath.isEmpty()) {
            QProcess designerProcess;
            QString designerPath = "designer.exe"; // 假设Qt Designer在系统PATH中

            // 启动Qt Designer并打开指定的UI文件
            if (designerProcess.startDetached(designerPath, QStringList() << uiFilePath)) {
                QMessageBox::information(&mainWindow, "成功", "已在Qt Designer中打开UI文件");
            } else {
                QMessageBox::critical(&mainWindow, "失败", "无法启动Qt Designer。请确保Qt Designer已正确安装并添加到系统PATH中。");
            }
        }
    });

    // 显示主窗口
    mainWindow.show();

    return app.exec();
}