/********************************************************************************
** Form generated from reading UI file 'uilayoutwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_UILAYOUTWINDOW_H
#define UI_UILAYOUTWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "uilayoutwindow.h"

QT_BEGIN_NAMESPACE

class Ui_UILayoutWindow
{
public:
    QAction *actionSave_Layout;
    QAction *actionLoad_Layout;
    QAction *actionUndo;
    QAction *actionRedo;
    QAction *actionPreview;
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout;
    QSplitter *splitter;
    QDockWidget *dockWidget;
    QWidget *dockWidgetContents;
    QVBoxLayout *verticalLayout_2;
    QLabel *label;
    CustomListWidget *widgetListWidget;
    QGraphicsView *graphicsView;
    QMenuBar *menubar;
    QMenu *menu;
    QMenu *menu_2;
    QToolBar *mainToolBar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *UILayoutWindow)
    {
        if (UILayoutWindow->objectName().isEmpty())
            UILayoutWindow->setObjectName("UILayoutWindow");
        UILayoutWindow->resize(1000, 700);
        actionSave_Layout = new QAction(UILayoutWindow);
        actionSave_Layout->setObjectName("actionSave_Layout");
        actionLoad_Layout = new QAction(UILayoutWindow);
        actionLoad_Layout->setObjectName("actionLoad_Layout");
        actionUndo = new QAction(UILayoutWindow);
        actionUndo->setObjectName("actionUndo");
        actionRedo = new QAction(UILayoutWindow);
        actionRedo->setObjectName("actionRedo");
        actionPreview = new QAction(UILayoutWindow);
        actionPreview->setObjectName("actionPreview");
        centralwidget = new QWidget(UILayoutWindow);
        centralwidget->setObjectName("centralwidget");
        horizontalLayout = new QHBoxLayout(centralwidget);
        horizontalLayout->setObjectName("horizontalLayout");
        splitter = new QSplitter(centralwidget);
        splitter->setObjectName("splitter");
        splitter->setOrientation(Qt::Horizontal);
        dockWidget = new QDockWidget(splitter);
        dockWidget->setObjectName("dockWidget");
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName("dockWidgetContents");
        verticalLayout_2 = new QVBoxLayout(dockWidgetContents);
        verticalLayout_2->setObjectName("verticalLayout_2");
        label = new QLabel(dockWidgetContents);
        label->setObjectName("label");

        verticalLayout_2->addWidget(label);

        widgetListWidget = new CustomListWidget(dockWidgetContents);
        widgetListWidget->setObjectName("widgetListWidget");
        widgetListWidget->setDragEnabled(true);

        verticalLayout_2->addWidget(widgetListWidget);

        dockWidget->setWidget(dockWidgetContents);
        splitter->addWidget(dockWidget);

        graphicsView = new QGraphicsView(splitter);
        graphicsView->setObjectName("graphicsView");
        graphicsView->setAcceptDrops(true);
        splitter->addWidget(graphicsView);
        horizontalLayout->addWidget(splitter);

        UILayoutWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(UILayoutWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1000, 26));
        menu = new QMenu(menubar);
        menu->setObjectName("menu");
        menu_2 = new QMenu(menubar);
        menu_2->setObjectName("menu_2");
        UILayoutWindow->setMenuBar(menubar);
        mainToolBar = new QToolBar(UILayoutWindow);
        mainToolBar->setObjectName("mainToolBar");
        UILayoutWindow->addToolBar(Qt::ToolBarArea::LeftToolBarArea, mainToolBar);
        statusbar = new QStatusBar(UILayoutWindow);
        statusbar->setObjectName("statusbar");
        UILayoutWindow->setStatusBar(statusbar);

        menubar->addAction(menu->menuAction());
        menubar->addAction(menu_2->menuAction());
        menu->addAction(actionSave_Layout);
        menu->addAction(actionLoad_Layout);
        menu_2->addAction(actionUndo);
        menu_2->addAction(actionRedo);
        mainToolBar->addAction(actionPreview);

        retranslateUi(UILayoutWindow);

        QMetaObject::connectSlotsByName(UILayoutWindow);
    } // setupUi

    void retranslateUi(QMainWindow *UILayoutWindow)
    {
        UILayoutWindow->setWindowTitle(QCoreApplication::translate("UILayoutWindow", "UI\345\270\203\345\261\200\347\274\226\350\276\221\345\231\250", nullptr));
        actionSave_Layout->setText(QCoreApplication::translate("UILayoutWindow", "\344\277\235\345\255\230\345\270\203\345\261\200", nullptr));
        actionLoad_Layout->setText(QCoreApplication::translate("UILayoutWindow", "\345\212\240\350\275\275\345\270\203\345\261\200", nullptr));
        actionUndo->setText(QCoreApplication::translate("UILayoutWindow", "\346\222\244\351\224\200", nullptr));
        actionRedo->setText(QCoreApplication::translate("UILayoutWindow", "\351\207\215\345\201\232", nullptr));
        actionPreview->setText(QCoreApplication::translate("UILayoutWindow", "\351\242\204\350\247\210", nullptr));
        label->setText(QCoreApplication::translate("UILayoutWindow", "\350\207\252\345\256\232\344\271\211\346\216\247\344\273\266\345\272\223", nullptr));
        menu->setTitle(QCoreApplication::translate("UILayoutWindow", "\346\226\207\344\273\266", nullptr));
        menu_2->setTitle(QCoreApplication::translate("UILayoutWindow", "\347\274\226\350\276\221", nullptr));
    } // retranslateUi

};

namespace Ui {
    class UILayoutWindow: public Ui_UILayoutWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_UILAYOUTWINDOW_H
