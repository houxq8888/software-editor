// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#ifndef QDESIGNER_H
#define QDESIGNER_H

#include <QtCore/qpointer.h>
#include <QtWidgets/qapplication.h>

QT_BEGIN_NAMESPACE

#define qDesigner \
    (static_cast<QDesigner*>(QCoreApplication::instance()))

class QDesignerWorkbench;
class QDesignerToolWindow;
class MainWindowBase;
class QDesignerServer;
class QDesignerClient;
class QErrorMessage;
class QCommandLineParser;
struct Options;

class QDesigner: public QApplication
{
    Q_OBJECT
public:
    enum ParseArgumentsResult {
        ParseArgumentsSuccess,
        ParseArgumentsError,
        ParseArgumentsHelpRequested
    };

    QDesigner(int &argc, char **argv);
    ~QDesigner() override;

    ParseArgumentsResult parseCommandLineArguments();
    
    // 添加手动设置文件路径的方法
    void setFilesToOpen(const QStringList &files);
    
    // 添加预设置文件路径的方法，在构造函数阶段调用
    void setPredefinedFiles(const QStringList &files);

    QDesignerWorkbench *workbench() const;
    QDesignerServer *server() const;
    bool isServerOrClientEnabled() const;
    MainWindowBase *mainWindow() const;
    void setMainWindow(MainWindowBase *tw);

    // 添加备份文件清理功能
    void cleanupBackupFiles();

protected:
    bool event(QEvent *ev) override;

signals:
    void initialized();
    

public slots:
    void showErrorMessage(const QString &message);

private:
    void showErrorMessageBox(const QString &);

    QDesignerServer *m_server = nullptr;
    QDesignerClient *m_client = nullptr;
    QDesignerWorkbench *m_workbench = nullptr;
    QPointer<MainWindowBase> m_mainWindow;
    QPointer<QErrorMessage> m_errorMessageDialog;

    QString m_initializationErrors;
    QString m_lastErrorMessage;
    
    // 预设置的文件路径，在构造函数阶段设置
    QStringList m_predefinedFiles;
};

QT_END_NAMESPACE

#endif // QDESIGNER_H
