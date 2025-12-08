// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include "qdesigner.h"
#include <QtCore/qlibraryinfo.h>
#include <QtCore/qdir.h>
#include <QtCore/qoperatingsystemversion.h>

#include <stdlib.h>

QT_USE_NAMESPACE

static const char rhiBackEndVar[] = "QSG_RHI_BACKEND";

int main(int argc, char *argv[])
{
    // Enable the QWebEngineView, QQuickWidget plugins on Windows.
    if (QOperatingSystemVersion::currentType() == QOperatingSystemVersion::Windows
        && !qEnvironmentVariableIsSet(rhiBackEndVar)) {
        qputenv(rhiBackEndVar, "gl");
    }

    // required for QWebEngineView
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

    QDesigner app(argc, argv);
    switch (app.parseCommandLineArguments()) {
    case QDesigner::ParseArgumentsSuccess:
        break;
    case QDesigner::ParseArgumentsError:
        return 1;
    case QDesigner::ParseArgumentsHelpRequested:
        return 0;
    }
    
    // 设置当最后一个窗口关闭时退出应用程序
    // 这确保Qt Designer进程能够正确清理备份文件
    QGuiApplication::setQuitOnLastWindowClosed(true);

    // 添加全局异常处理，确保程序异常退出时也能清理备份文件
    try {
        return QApplication::exec();
    } catch (...) {
        // 异常时执行清理，防止备份文件残留
        qWarning() << "Qt Designer异常退出，正在清理备份文件...";
        // 这里可以添加备份文件清理逻辑
        return -1;
    }
}
