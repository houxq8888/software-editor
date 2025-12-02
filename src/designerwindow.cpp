#include "designerwindow.h"
#include <QCoreApplication>
#include <QtCore/qdir.h>
#include <QtCore/qoperatingsystemversion.h>
#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerFormWindowManagerInterface>
#include <QtDesigner/QDesignerFormWindowInterface>
#include <QtDesigner/QDesignerComponents>

static const char rhiBackEndVar[] = "QSG_RHI_BACKEND";

DesignerWindow::DesignerWindow(QWidget *parent) : QMainWindow(parent)
{
    // Enable the QWebEngineView, QQuickWidget plugins on Windows.
    if (QOperatingSystemVersion::currentType() == QOperatingSystemVersion::Windows
        && !qEnvironmentVariableIsSet(rhiBackEndVar)) {
        qputenv(rhiBackEndVar, "gl");
    }

    // required for QWebEngineView
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

    // Create Designer components
    QStringList pluginPaths;
    // 初始化Qt Designer表单编辑器
    m_formEditor = new QDesignerFormEditorInterface();

    // Set the window title
    setWindowTitle("Qt Widgets Designer");

    // 创建一个新的表单窗口
    QDesignerFormWindowManagerInterface *manager = m_formEditor->formWindowManager();
    QDesignerFormWindowInterface *formWindow = manager->createFormWindow();
    formWindow->show();
}

DesignerWindow::~DesignerWindow()
{
    delete m_formEditor;
}