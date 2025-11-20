#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QMainWindow>
using namespace Qt;
#include "mainwindow.h"

int main(int argc, char *argv[]) { 
    // 将日志输出到文件
    QFile *logFile = new QFile("debug.log");
    if (logFile->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) { 
        QTextStream *logStream = new QTextStream(logFile);
        qInstallMessageHandler([](QtMsgType type, const QMessageLogContext &context, const QString &msg) { 
            QString logEntry = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz") + " " + msg;
            // 输出到控制台
            QTextStream(stdout) << logEntry << endl;
        });
    }

    QApplication app(argc, argv);
    MainWindow window;
    window.resize(1000, 800);
    window.show();
    return app.exec();
}