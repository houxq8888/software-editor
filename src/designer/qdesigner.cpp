// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

// designer
#include "qdesigner.h"
#include "qdesigner_actions.h"
#include "qdesigner_server.h"
#include "qdesigner_settings.h"
#include "qdesigner_workbench.h"
#include "mainwindow.h"
#include <QtDesigner/abstractintegration.h>
#include <QtDesigner/abstractformeditor.h>
#include <qdesigner_propertysheet_p.h>

#include <QtGui/qevent.h>
#include <QtWidgets/qmessagebox.h>
#include <QtGui/qicon.h>
#include <QtWidgets/qerrormessage.h>
#include <QtCore/qmetaobject.h>
#include <QtCore/qdir.h>
#include <QtCore/qfile.h>
#include <QtCore/qlibraryinfo.h>
#include <QtCore/qlocale.h>
#include <QtCore/qtextstream.h>
#include <QtCore/qtimer.h>
#include <QtCore/qtranslator.h>
#include <QtCore/qfileinfo.h>
#include <QtCore/qdebug.h>
#include <QtCore/qcommandlineparser.h>
#include <QtCore/qcommandlineoption.h>
#include <QtCore/qversionnumber.h>
#include <QtCore/qvariant.h>

#include <QtDesigner/QDesignerComponents>

#include <optional>

QT_BEGIN_NAMESPACE

using namespace Qt::StringLiterals;

static constexpr auto designerApplicationName = "Designer"_L1;
static constexpr auto designerDisplayName = "Qt Widgets Designer"_L1;
static constexpr auto designerWarningPrefix = "Designer: "_L1;
static QtMessageHandler previousMessageHandler = nullptr;

static void designerMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    // Only Designer warnings are displayed as box
    QDesigner *designerApp = qDesigner;
    if (type != QtWarningMsg || !designerApp || !msg.startsWith(designerWarningPrefix)) {
        previousMessageHandler(type, context, msg);
        return;
    }
    designerApp->showErrorMessage(msg);
}

QDesigner::QDesigner(int &argc, char **argv)
    : QApplication(argc, argv)
    , m_predefinedFiles() // 初始化预设置文件列表为空
{
    setOrganizationName(u"QtProject"_s);
    QGuiApplication::setApplicationDisplayName(designerDisplayName);
    setApplicationName(designerApplicationName);
    QDesignerComponents::initializeResources();

#if !defined(Q_OS_MACOS) && !defined(Q_OS_WIN)
    setWindowIcon(QIcon(u":/qt-project.org/designer/images/designer.png"_s));
#endif
}

QDesigner::~QDesigner()
{
    // 确保在析构时清理备份文件
    cleanupBackupFiles();
    
    delete m_workbench;
    delete m_server;
    delete m_client;
}

void QDesigner::cleanupBackupFiles()
{
    // 清理Qt Designer的备份文件，防止"last session was not terminated correctly"错误
    if (m_workbench && m_workbench->core()) {
        QDesignerSettings settings(m_workbench->core());
        settings.clearBackup();
        qDebug() << "Qt Designer back up file clear";
    }
}

void QDesigner::showErrorMessage(const QString &message)
{
    // strip the prefix
    const QString qMessage =
        message.right(message.size() - int(designerWarningPrefix.size()));
    // If there is no main window yet, just store the message.
    // The QErrorMessage would otherwise be hidden by the main window.
    if (m_mainWindow) {
        showErrorMessageBox(qMessage);
    } else {
        const QMessageLogContext emptyContext;
        previousMessageHandler(QtWarningMsg, emptyContext, message); // just in case we crash
        m_initializationErrors += qMessage;
        m_initializationErrors += u'\n';
    }
}

void QDesigner::showErrorMessageBox(const QString &msg)
{
    // Manually suppress consecutive messages.
    // This happens if for example sth is wrong with custom widget creation.
    // The same warning will be displayed by Widget box D&D and form Drop
    // while trying to create instance.
    if (m_errorMessageDialog && m_lastErrorMessage == msg)
        return;

    if (!m_errorMessageDialog) {
        m_lastErrorMessage.clear();
        m_errorMessageDialog = new QErrorMessage(m_mainWindow);
        const QString title = QCoreApplication::translate("QDesigner", "%1 - warning").arg(designerApplicationName);
        m_errorMessageDialog->setWindowTitle(title);
        m_errorMessageDialog->setMinimumSize(QSize(600, 250));
    }
    m_errorMessageDialog->showMessage(msg);
    m_lastErrorMessage = msg;
}

QDesignerWorkbench *QDesigner::workbench() const
{
    return m_workbench;
}

QDesignerServer *QDesigner::server() const
{
    return m_server;
}

static void showHelp(QCommandLineParser &parser, const QString &errorMessage = QString())
{
    QString text;
    QTextStream str(&text);
    str << "<html><head/><body>";
    if (!errorMessage.isEmpty())
        str << "<p>" << errorMessage << "</p>";
    str << "<pre>" << parser.helpText().toHtmlEscaped() << "</pre></body></html>";
    QMessageBox box(errorMessage.isEmpty() ? QMessageBox::Information : QMessageBox::Warning,
                    QGuiApplication::applicationDisplayName(), text,
                    QMessageBox::Ok);
    box.setTextInteractionFlags(Qt::TextBrowserInteraction);
    box.exec();
}

struct Options
{
    QStringList files;
    QString resourceDir{QLibraryInfo::path(QLibraryInfo::TranslationsPath)};
    QStringList pluginPaths;
    std::optional<QVersionNumber> qtVersion;
    bool server{false};
    quint16 clientPort{0};
    bool enableInternalDynamicProperties{false};
};

static inline QDesigner::ParseArgumentsResult
    parseDesignerCommandLineArguments(QCommandLineParser &parser, Options *options,
                                      QString *errorMessage)
{
    parser.setApplicationDescription(u"Qt Widgets Designer " QT_VERSION_STR "\n\nUI designer for QWidget-based applications."_s);
    const QCommandLineOption helpOption = parser.addHelpOption();
    parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
    const QCommandLineOption serverOption(u"server"_s,
                                          u"Server mode"_s);
    parser.addOption(serverOption);
    const QCommandLineOption clientOption(u"client"_s,
                                          u"Client mode"_s,
                                          u"port"_s);
    parser.addOption(clientOption);
    const QCommandLineOption resourceDirOption(u"resourcedir"_s,
                                          u"Resource directory"_s,
                                          u"directory"_s);
    parser.addOption(resourceDirOption);
    const QCommandLineOption internalDynamicPropertyOption(u"enableinternaldynamicproperties"_s,
                                          u"Enable internal dynamic properties"_s);
    parser.addOption(internalDynamicPropertyOption);
    const QCommandLineOption pluginPathsOption(u"plugin-path"_s,
                                               u"Default plugin path list"_s,
                                               u"path"_s);
    parser.addOption(pluginPathsOption);

    const QCommandLineOption qtVersionOption(u"qt-version"_s,
                                             u"Qt Version for writing .ui files"_s,
                                             u"version"_s);
    parser.addOption(qtVersionOption);

    parser.addPositionalArgument(u"files"_s,
                                 u"The UI files to open."_s);

    if (!parser.parse(QCoreApplication::arguments())) {
        *errorMessage = parser.errorText();
        return QDesigner::ParseArgumentsError;
    }

    if (parser.isSet(helpOption))
        return QDesigner::ParseArgumentsHelpRequested;
    // There is no way to retrieve the complete help text from QCommandLineParser,
    // so, call process() to display it.
    if (parser.isSet(u"help-all"_s))
        parser.process(QCoreApplication::arguments()); // exits
    options->server = parser.isSet(serverOption);
    if (parser.isSet(clientOption)) {
        bool ok;
        options->clientPort = parser.value(clientOption).toUShort(&ok);
        if (!ok) {
            *errorMessage = u"Non-numeric argument specified for -client"_s;
            return QDesigner::ParseArgumentsError;
        }
    }
    if (parser.isSet(resourceDirOption))
        options->resourceDir = parser.value(resourceDirOption);
    const auto pluginPathValues = parser.values(pluginPathsOption);
    for (const auto &pluginPath : pluginPathValues)
        options->pluginPaths.append(pluginPath.split(QDir::listSeparator(), Qt::SkipEmptyParts));

    if (parser.isSet(qtVersionOption))
        options->qtVersion = QVersionNumber::fromString(parser.value(qtVersionOption));

    options->enableInternalDynamicProperties = parser.isSet(internalDynamicPropertyOption);
    options->files = parser.positionalArguments();
    return QDesigner::ParseArgumentsSuccess;
}

void QDesigner::setPredefinedFiles(const QStringList &files)
{
    // 在构造函数阶段设置预定义文件路径
    qDebug() << "set predefined file path:" << files;
    m_predefinedFiles = files;
}

void QDesigner::setFilesToOpen(const QStringList &files)
{
    // 手动设置要打开的文件列表，模拟命令行参数传入的效果
    qDebug() << "manually set files to open:" << files;
    
    bool suppressNewFormShow = false;
    
    // 记录是否有任何UI文件被成功打开
    bool anyFileOpened = false;
    
    for (auto fileName : files) {
        // Ensure absolute paths for recent file list to be unique
        const QFileInfo fi(fileName);
        if (fi.exists() && fi.isRelative())
            fileName = fi.absoluteFilePath();
            
        // 检查文件扩展名，区分UI文件和产品配置文件
        if (fileName.endsWith(".json", Qt::CaseInsensitive)) {
            // 如果是JSON文件，认为是产品配置文件，不直接作为UI文件打开
            qDebug() << "detected product config file:" << fileName;
            qDebug() << "product config file will be handled by ProductMainWindow, not opened as UI file";
            // 产品配置文件不抑制新建表单对话框
        } else if (fileName.endsWith(".ui", Qt::CaseInsensitive)) {
            // 如果是UI文件，正常打开
            if (m_workbench->readInForm(fileName)) {
                // 如果成功打开了UI文件，则抑制新建表单对话框的显示
                suppressNewFormShow = true;
                anyFileOpened = true;
                qDebug() << "successfully opened UI file:" << fileName;
            } else {
                qDebug() << "failed to open UI file:" << fileName;
                // 即使打开失败，如果有UI文件传入，也应该抑制新建表单对话框
                suppressNewFormShow = true;
            }
        } else {
            // 其他文件类型，尝试作为UI文件打开
            if (m_workbench->readInForm(fileName)) {
                suppressNewFormShow = true;
                anyFileOpened = true;
                qDebug() << "successfully opened file (non-standard extension):" << fileName;
            } else {
                qDebug() << "failed to open file (non-standard extension):" << fileName;
                // 即使打开失败，如果有文件传入，也应该抑制新建表单对话框
                suppressNewFormShow = true;
            }
        }
    }

    // 如果有UI文件被传入（无论是否成功打开），都应该抑制新建表单对话框
    if (!files.isEmpty()) {
        suppressNewFormShow = true;
        qDebug() << "any file opened, suppress new form display";
    }

    // 如果已经有表单窗口打开，也应该抑制新建表单对话框
    if (m_workbench->formWindowCount() > 0) {
        suppressNewFormShow = true;
        qDebug() << "form window already opened, suppress new form display";
    }

    // 关键修复：将抑制标志设置到workbench对象中，确保所有showNewForm()调用都能正确抑制
    m_workbench->setSuppressNewFormShow(suppressNewFormShow);
    qDebug() << "set suppressNewFormShow flag: suppressNewFormShow =" << suppressNewFormShow;
    qDebug() << "workbench suppressNewFormShow flag: m_workbench->suppressNewFormShow() =" << m_workbench->suppressNewFormShow();
}

QDesigner::ParseArgumentsResult QDesigner::parseCommandLineArguments()
{
    QString errorMessage;
    Options options;
    QCommandLineParser parser;
    const ParseArgumentsResult result = parseDesignerCommandLineArguments(parser, &options, &errorMessage);
    if (result != ParseArgumentsSuccess) {
        showHelp(parser, errorMessage);
        return result;
    }
    
    // 关键修改：如果存在预设置的文件路径，则优先使用预设置的文件
    if (!m_predefinedFiles.isEmpty()) {
        qDebug() << "use predefined file path:" << m_predefinedFiles;
        options.files = m_predefinedFiles;
        // 清空预设置文件，避免重复使用
        m_predefinedFiles.clear();
    }
    
    // initialize the sub components
    if (options.clientPort)
        m_client = new QDesignerClient(options.clientPort, this);
    if (options.server) {
        m_server = new QDesignerServer();
        printf("%d\n", m_server->serverPort());
        fflush(stdout);
    }
    if (options.enableInternalDynamicProperties)
        QDesignerPropertySheet::setInternalDynamicPropertiesEnabled(true);

    std::unique_ptr<QTranslator> designerTranslator(new QTranslator(this));
    if (designerTranslator->load(QLocale(), u"designer"_s, u"_"_s, options.resourceDir)) {
        installTranslator(designerTranslator.release());
        std::unique_ptr<QTranslator> qtTranslator(new QTranslator(this));
        if (qtTranslator->load(QLocale(), u"qt"_s, u"_"_s, options.resourceDir))
            installTranslator(qtTranslator.release());
    }

    m_workbench = new QDesignerWorkbench(options.pluginPaths);

    emit initialized();
    previousMessageHandler = qInstallMessageHandler(designerMessageHandler); // Warn when loading faulty forms
    Q_ASSERT(previousMessageHandler);

    bool suppressNewFormShow = false;
    
    // 记录是否有任何UI文件被成功打开
    bool anyFileOpened = false;
    
    for (auto fileName : std::as_const(options.files)) {
        // Ensure absolute paths for recent file list to be unique
        const QFileInfo fi(fileName);
        if (fi.exists() && fi.isRelative())
            fileName = fi.absoluteFilePath();
            
        // 检查文件扩展名，区分UI文件和产品配置文件
        if (fileName.endsWith(".json", Qt::CaseInsensitive)) {
            // 如果是JSON文件，认为是产品配置文件，不直接作为UI文件打开
            qDebug() << "detected product config file:" << fileName;
            qDebug() << "product config file will be handled by ProductMainWindow, not open as UI file";
            // 产品配置文件不抑制新建表单对话框
        } else if (fileName.endsWith(".ui", Qt::CaseInsensitive)) {
            // 如果是UI文件，正常打开
            if (m_workbench->readInForm(fileName)) {
                // 如果成功打开了UI文件，则抑制新建表单对话框的显示
                suppressNewFormShow = true;
                anyFileOpened = true;
                qDebug() << "success open UI file:" << fileName;
            } else {
                qDebug() << "open UI file failed:" << fileName;
                // 即使打开失败，如果有UI文件传入，也应该抑制新建表单对话框
                suppressNewFormShow = true;
            }
        } else {
            // 其他文件类型，尝试作为UI文件打开
            if (m_workbench->readInForm(fileName)) {
                suppressNewFormShow = true;
                anyFileOpened = true;
                qDebug() << "success open file(non-standard extension):" << fileName;
            } else {
                qDebug() << "open file failed(non-standard extension):" << fileName;
                // 即使打开失败，如果有文件传入，也应该抑制新建表单对话框
                suppressNewFormShow = true;
            }
        }
    }

    // 如果有UI文件被传入（无论是否成功打开），都应该抑制新建表单对话框
    if (!options.files.isEmpty()) {
        suppressNewFormShow = true;
        qDebug()<<"has file input, suppress new form display";
    }

    // 如果已经有表单窗口打开，也应该抑制新建表单对话框
    if (m_workbench->formWindowCount() > 0) {
        suppressNewFormShow = true;
        qDebug()<<"has form window opened, suppress new form display";
    }

    // 关键修复：将抑制标志设置到workbench对象中，确保所有showNewForm()调用都能正确抑制
    m_workbench->setSuppressNewFormShow(suppressNewFormShow);
    qDebug()<<"set suppressNewFormShow flag: suppressNewFormShow =" << suppressNewFormShow;
    qDebug() << "workbench suppressNewFormShow flag: m_workbench->suppressNewFormShow() =" << m_workbench->suppressNewFormShow();

    if (options.qtVersion.has_value()) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 9, 0)
        m_workbench->core()->integration()->setQtVersion(options.qtVersion.value());
#else
        auto version = QVariant::fromValue(options.qtVersion.value());
        m_workbench->core()->integration()->setProperty("qtVersion", version);
#endif
    }

    // Show up error box with parent now if something went wrong
    if (m_initializationErrors.isEmpty()) {
        qDebug()<<"check showNewForm condition:";
        qDebug() << "  isServerOrClientEnabled() =" << isServerOrClientEnabled();
        qDebug() << "  m_workbench->suppressNewFormShow() =" << m_workbench->suppressNewFormShow();
        qDebug() << "  !isServerOrClientEnabled() =" << !isServerOrClientEnabled();
        qDebug() << "  !m_workbench->suppressNewFormShow() =" << !m_workbench->suppressNewFormShow();
        qDebug()<<"final condition: !isServerOrClientEnabled() && !m_workbench->suppressNewFormShow() =" << (!isServerOrClientEnabled() && !m_workbench->suppressNewFormShow());
        
        if (!isServerOrClientEnabled() && !m_workbench->suppressNewFormShow())
        {
            qDebug() << "showNewForm !isServerOrClientEnabled() && !m_workbench->suppressNewFormShow()";
            m_workbench->showNewForm();
        }
    } else {
        showErrorMessageBox(m_initializationErrors);
        m_initializationErrors.clear();
    }
    return result;
}

bool QDesigner::isServerOrClientEnabled() const
{
    return m_server || m_client;
}

bool QDesigner::event(QEvent *ev)
{
    bool eaten;
    switch (ev->type()) {
    case QEvent::FileOpen:
        m_workbench->readInForm(static_cast<QFileOpenEvent *>(ev)->file());
        m_workbench->requestActivate();
        eaten = true;
        break;
    case QEvent::Close: {
        QCloseEvent *closeEvent = static_cast<QCloseEvent *>(ev);
        closeEvent->setAccepted(m_workbench->handleClose());
        if (closeEvent->isAccepted()) {
            // We're going down, make sure that we don't get our settings saved twice.
            if (m_mainWindow)
                m_mainWindow->setCloseEventPolicy(MainWindowBase::AcceptCloseEvents);
            eaten = QApplication::event(ev);
        }
        eaten = true;
        break;
    }
    default:
        eaten = QApplication::event(ev);
        break;
    }
    return eaten;
}

void QDesigner::setMainWindow(MainWindowBase *tw)
{
    m_mainWindow = tw;
}

MainWindowBase *QDesigner::mainWindow() const
{
    return m_mainWindow;
}

QT_END_NAMESPACE
