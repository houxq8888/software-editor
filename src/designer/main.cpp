// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include "qdesigner.h"
#include "../product/productconfigmanager.h"
#include <QtCore/qlibraryinfo.h>
#include <QtCore/qdir.h>
#include <QtCore/qoperatingsystemversion.h>

#include <stdlib.h>

QT_USE_NAMESPACE

static const char rhiBackEndVar[] = "QSG_RHI_BACKEND";

int main(int argc, char *argv[]) {
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
         // 优先加载产品配置文件中uiLayoutPath指定的UI文件
         ProductConfigManager configManager;
         QString uiLayoutPath = configManager.getUiLayoutPath();
         if (!uiLayoutPath.isEmpty() && QFile::exists(uiLayoutPath)) {
             app.openDocument(uiLayoutPath);
         } else if (argc > 1) {
             // 如果产品配置文件中没有指定UI文件路径，则使用命令行参数中的UI文件路径
             QString uiFilePath = argv[1];
             if (QFile::exists(uiFilePath)) {
                 app.openDocument(uiFilePath);
             }
         }
         break;
    case QDesigner::ParseArgumentsError:
        return 1;
    case QDesigner::ParseArgumentsHelpRequested:
        return 0;
    }
    QGuiApplication::setQuitOnLastWindowClosed(false);

    return QApplication::exec();
}
