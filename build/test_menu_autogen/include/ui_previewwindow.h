/********************************************************************************
** Form generated from reading UI file 'previewwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PREVIEWWINDOW_H
#define UI_PREVIEWWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PreviewWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QWidget *previewWidget;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *PreviewWindow)
    {
        if (PreviewWindow->objectName().isEmpty())
            PreviewWindow->setObjectName("PreviewWindow");
        PreviewWindow->resize(800, 600);
        centralwidget = new QWidget(PreviewWindow);
        centralwidget->setObjectName("centralwidget");
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName("verticalLayout");
        previewWidget = new QWidget(centralwidget);
        previewWidget->setObjectName("previewWidget");
        previewWidget->setStyleSheet(QString::fromUtf8("background-color: white;"));

        verticalLayout->addWidget(previewWidget);

        PreviewWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(PreviewWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 26));
        PreviewWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(PreviewWindow);
        statusbar->setObjectName("statusbar");
        PreviewWindow->setStatusBar(statusbar);

        retranslateUi(PreviewWindow);

        QMetaObject::connectSlotsByName(PreviewWindow);
    } // setupUi

    void retranslateUi(QMainWindow *PreviewWindow)
    {
        PreviewWindow->setWindowTitle(QCoreApplication::translate("PreviewWindow", "UI\345\270\203\345\261\200\351\242\204\350\247\210", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PreviewWindow: public Ui_PreviewWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PREVIEWWINDOW_H
