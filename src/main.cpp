#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QMainWindow>
#include <QSettings>
#include <QCoreApplication>
#include <QDir>
#include <QProcess>
#include <QProcessEnvironment>
using namespace Qt;
#include "mainwindow.h"
#include "uilayoutwindow.h"

// 设置PowerShell编码配置
void setupPowerShellEncoding() {
    // 设置控制台编码为UTF-8
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    
    // 设置PowerShell编码相关环境变量
    qputenv("PYTHONIOENCODING", "utf-8");
    qputenv("LANG", "zh_CN.UTF-8");
    qputenv("LC_ALL", "zh_CN.UTF-8");
    
    // 设置Windows控制台编码
    #ifdef Q_OS_WIN
        // 设置控制台代码页为UTF-8 (65001)
        system("chcp 65001 > nul");
        
        // 设置PowerShell编码
        QProcess process;
        process.start("powershell", QStringList() << "-Command" << "[Console]::OutputEncoding = [System.Text.Encoding]::UTF8");
        process.waitForFinished();
        
        process.start("powershell", QStringList() << "-Command" << "[Console]::InputEncoding = [System.Text.Encoding]::UTF8");
        process.waitForFinished();
    #endif
    
    // 设置Qt文本编码（Qt6中默认使用UTF-8）
    // QTextCodec在Qt6中已被移除，Qt6默认使用UTF-8编码
    
    qDebug() << "PowerShell编码设置完成：UTF-8";
}

// 加载应用程序配置
void loadApplicationConfig() {
    QString configPath = QCoreApplication::applicationDirPath() + "/config/powershell_config.ini";
    
    // 如果配置文件不存在，使用默认配置
    if (!QFile::exists(configPath)) {
        // 创建配置目录
        QDir configDir(QCoreApplication::applicationDirPath() + "/config");
        if (!configDir.exists()) {
            configDir.mkpath(".");
        }
        
        // 创建默认配置文件
        QSettings settings(configPath, QSettings::IniFormat);
        settings.setValue("PowerShell/encoding", "UTF-8");
        settings.setValue("PowerShell/console_output_codepage", "65001");
        settings.setValue("PowerShell/input_codepage", "65001");
        settings.setValue("Application/name", "软件编辑器");
        settings.setValue("Application/version", "1.0.0");
        settings.setValue("Settings/use_utf8_encoding", true);
        settings.setValue("Settings/force_console_utf8", true);
        settings.sync();
        
        qDebug() << "创建默认配置文件:" << configPath;
    }
    
    // 加载配置
    QSettings settings(configPath, QSettings::IniFormat);
    bool useUtf8 = settings.value("Settings/use_utf8_encoding", true).toBool();
    
    if (useUtf8) {
        setupPowerShellEncoding();
    }
    
    qDebug() << "应用程序配置加载完成";
}

int main(int argc, char *argv[]) { 
    QApplication app(argc, argv);
    
    // 设置应用程序属性
    QCoreApplication::setApplicationName("软件编辑器");
    QCoreApplication::setApplicationVersion("1.0.0");
    QCoreApplication::setOrganizationName("软件工作室");
    
    // 将日志输出到文件
    QString logPath = QCoreApplication::applicationDirPath() + "/debug.log";
    QFile *logFile = new QFile(logPath);
    if (logFile->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) { 
        QTextStream *logStream = new QTextStream(logFile);
        qInstallMessageHandler([](QtMsgType type, const QMessageLogContext &context, const QString &msg) { 
            QString logEntry = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz") + " " + msg;
            // 输出到控制台
            QTextStream(stdout) << logEntry << endl;
        });
    } else {
        // 如果日志文件打开失败，清理资源
        delete logFile;
        qDebug() << "无法打开日志文件:" << logPath;
    }
    
    // 加载配置并设置PowerShell编码
    loadApplicationConfig();
    UILayoutWindow window;
    // MainWindow window;
    window.resize(1000, 800);
    window.show();
    return app.exec();
}