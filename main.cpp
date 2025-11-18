#include <QApplication>
#include <QMainWindow>
#include <QDebug>
#include <QTextStream>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    // 启用调试输出
    qSetMessagePattern("%{type} %{function} %{line}: %{message}");
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    return app.exec();
}