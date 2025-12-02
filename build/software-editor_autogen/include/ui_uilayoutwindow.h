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
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "customtreewidget.h"

QT_BEGIN_NAMESPACE

class Ui_UILayoutWindow
{
public:
    QAction *actionNew_Layout;
    QAction *actionSave_Layout;
    QAction *actionSave_As_Layout;
    QAction *actionLoad_Layout;
    QAction *actionUndo;
    QAction *actionRedo;
    QAction *actionPreview;
    QAction *actionSuggestLayout;
    QAction *actionEventActionEditor;
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout;
    QSplitter *splitter;
    QWidget *leftPanelWidget;
    QVBoxLayout *verticalLayout_2;
    QLabel *label;
    QLineEdit *searchLineEdit;
    CustomTreeWidget *widgetListWidget;
    QWidget *graphicsView;
    QWidget *rightPanelWidget;
    QVBoxLayout *verticalLayout_3;
    QTabWidget *rightTabWidget;
    QWidget *propertiesTab;
    QVBoxLayout *verticalLayout_4;
    QLabel *propertiesLabel;
    QTreeWidget *propertiesTreeWidget;
    QWidget *featuresTab;
    QVBoxLayout *verticalLayout_5;
    QLabel *featuresLabel;
    QTreeWidget *featuresTreeWidget;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *bindFeatureButton;
    QPushButton *unbindFeatureButton;
    QPushButton *syncFeaturesButton;
    QMenuBar *menubar;
    QMenu *menu;
    QMenu *menu_2;
    QMenu *menu_3;
    QToolBar *mainToolBar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *UILayoutWindow)
    {
        if (UILayoutWindow->objectName().isEmpty())
            UILayoutWindow->setObjectName("UILayoutWindow");
        UILayoutWindow->resize(1200, 700);
        actionNew_Layout = new QAction(UILayoutWindow);
        actionNew_Layout->setObjectName("actionNew_Layout");
        actionSave_Layout = new QAction(UILayoutWindow);
        actionSave_Layout->setObjectName("actionSave_Layout");
        actionSave_As_Layout = new QAction(UILayoutWindow);
        actionSave_As_Layout->setObjectName("actionSave_As_Layout");
        actionLoad_Layout = new QAction(UILayoutWindow);
        actionLoad_Layout->setObjectName("actionLoad_Layout");
        actionUndo = new QAction(UILayoutWindow);
        actionUndo->setObjectName("actionUndo");
        actionRedo = new QAction(UILayoutWindow);
        actionRedo->setObjectName("actionRedo");
        actionPreview = new QAction(UILayoutWindow);
        actionPreview->setObjectName("actionPreview");
        actionSuggestLayout = new QAction(UILayoutWindow);
        actionSuggestLayout->setObjectName("actionSuggestLayout");
        actionEventActionEditor = new QAction(UILayoutWindow);
        actionEventActionEditor->setObjectName("actionEventActionEditor");
        centralwidget = new QWidget(UILayoutWindow);
        centralwidget->setObjectName("centralwidget");
        horizontalLayout = new QHBoxLayout(centralwidget);
        horizontalLayout->setObjectName("horizontalLayout");
        splitter = new QSplitter(centralwidget);
        splitter->setObjectName("splitter");
        splitter->setOrientation(Qt::Orientation::Horizontal);
        splitter->setHandleWidth(8);
        splitter->setChildrenCollapsible(false);
        leftPanelWidget = new QWidget(splitter);
        leftPanelWidget->setObjectName("leftPanelWidget");
        verticalLayout_2 = new QVBoxLayout(leftPanelWidget);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(leftPanelWidget);
        label->setObjectName("label");

        verticalLayout_2->addWidget(label);

        searchLineEdit = new QLineEdit(leftPanelWidget);
        searchLineEdit->setObjectName("searchLineEdit");

        verticalLayout_2->addWidget(searchLineEdit);

        widgetListWidget = new CustomTreeWidget(leftPanelWidget);
        widgetListWidget->setObjectName("widgetListWidget");
        widgetListWidget->setDragEnabled(true);

        verticalLayout_2->addWidget(widgetListWidget);

        splitter->addWidget(leftPanelWidget);
        graphicsView = new QWidget(splitter);
        graphicsView->setObjectName("graphicsView");
        graphicsView->setAcceptDrops(true);
        splitter->addWidget(graphicsView);
        rightPanelWidget = new QWidget(splitter);
        rightPanelWidget->setObjectName("rightPanelWidget");
        verticalLayout_3 = new QVBoxLayout(rightPanelWidget);
        verticalLayout_3->setObjectName("verticalLayout_3");
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        rightTabWidget = new QTabWidget(rightPanelWidget);
        rightTabWidget->setObjectName("rightTabWidget");
        propertiesTab = new QWidget();
        propertiesTab->setObjectName("propertiesTab");
        verticalLayout_4 = new QVBoxLayout(propertiesTab);
        verticalLayout_4->setObjectName("verticalLayout_4");
        propertiesLabel = new QLabel(propertiesTab);
        propertiesLabel->setObjectName("propertiesLabel");

        verticalLayout_4->addWidget(propertiesLabel);

        propertiesTreeWidget = new QTreeWidget(propertiesTab);
        propertiesTreeWidget->setObjectName("propertiesTreeWidget");

        verticalLayout_4->addWidget(propertiesTreeWidget);

        rightTabWidget->addTab(propertiesTab, QString());
        featuresTab = new QWidget();
        featuresTab->setObjectName("featuresTab");
        verticalLayout_5 = new QVBoxLayout(featuresTab);
        verticalLayout_5->setObjectName("verticalLayout_5");
        featuresLabel = new QLabel(featuresTab);
        featuresLabel->setObjectName("featuresLabel");

        verticalLayout_5->addWidget(featuresLabel);

        featuresTreeWidget = new QTreeWidget(featuresTab);
        featuresTreeWidget->setObjectName("featuresTreeWidget");

        verticalLayout_5->addWidget(featuresTreeWidget);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        bindFeatureButton = new QPushButton(featuresTab);
        bindFeatureButton->setObjectName("bindFeatureButton");

        horizontalLayout_2->addWidget(bindFeatureButton);

        unbindFeatureButton = new QPushButton(featuresTab);
        unbindFeatureButton->setObjectName("unbindFeatureButton");

        horizontalLayout_2->addWidget(unbindFeatureButton);


        verticalLayout_5->addLayout(horizontalLayout_2);

        syncFeaturesButton = new QPushButton(featuresTab);
        syncFeaturesButton->setObjectName("syncFeaturesButton");

        verticalLayout_5->addWidget(syncFeaturesButton);

        rightTabWidget->addTab(featuresTab, QString());

        verticalLayout_3->addWidget(rightTabWidget);

        splitter->addWidget(rightPanelWidget);

        horizontalLayout->addWidget(splitter);

        UILayoutWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(UILayoutWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1200, 21));
        menu = new QMenu(menubar);
        menu->setObjectName("menu");
        menu_2 = new QMenu(menubar);
        menu_2->setObjectName("menu_2");
        menu_3 = new QMenu(menubar);
        menu_3->setObjectName("menu_3");
        UILayoutWindow->setMenuBar(menubar);
        mainToolBar = new QToolBar(UILayoutWindow);
        mainToolBar->setObjectName("mainToolBar");
        UILayoutWindow->addToolBar(Qt::ToolBarArea::LeftToolBarArea, mainToolBar);
        statusbar = new QStatusBar(UILayoutWindow);
        statusbar->setObjectName("statusbar");
        UILayoutWindow->setStatusBar(statusbar);

        menubar->addAction(menu->menuAction());
        menubar->addAction(menu_2->menuAction());
        menubar->addAction(menu_3->menuAction());
        menu->addAction(actionNew_Layout);
        menu->addAction(actionSave_Layout);
        menu->addAction(actionSave_As_Layout);
        menu->addAction(actionLoad_Layout);
        menu_2->addAction(actionUndo);
        menu_2->addAction(actionRedo);
        menu_3->addAction(actionEventActionEditor);
        mainToolBar->addAction(actionPreview);
        mainToolBar->addAction(actionNew_Layout);
        mainToolBar->addAction(actionSuggestLayout);
        mainToolBar->addAction(actionEventActionEditor);

        retranslateUi(UILayoutWindow);

        rightTabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(UILayoutWindow);
    } // setupUi

    void retranslateUi(QMainWindow *UILayoutWindow)
    {
        UILayoutWindow->setWindowTitle(QCoreApplication::translate("UILayoutWindow", "UI\345\270\203\345\261\200\347\274\226\350\276\221\345\231\250", nullptr));
        actionNew_Layout->setText(QCoreApplication::translate("UILayoutWindow", "\346\226\260\345\273\272", nullptr));
#if QT_CONFIG(shortcut)
        actionNew_Layout->setShortcut(QCoreApplication::translate("UILayoutWindow", "Ctrl+N", nullptr));
#endif // QT_CONFIG(shortcut)
        actionSave_Layout->setText(QCoreApplication::translate("UILayoutWindow", "\344\277\235\345\255\230", nullptr));
#if QT_CONFIG(shortcut)
        actionSave_Layout->setShortcut(QCoreApplication::translate("UILayoutWindow", "Ctrl+S", nullptr));
#endif // QT_CONFIG(shortcut)
        actionSave_As_Layout->setText(QCoreApplication::translate("UILayoutWindow", "\345\217\246\345\255\230\344\270\272", nullptr));
#if QT_CONFIG(shortcut)
        actionSave_As_Layout->setShortcut(QCoreApplication::translate("UILayoutWindow", "Ctrl+Shift+S", nullptr));
#endif // QT_CONFIG(shortcut)
        actionLoad_Layout->setText(QCoreApplication::translate("UILayoutWindow", "\345\212\240\350\275\275", nullptr));
#if QT_CONFIG(shortcut)
        actionLoad_Layout->setShortcut(QCoreApplication::translate("UILayoutWindow", "Ctrl+O", nullptr));
#endif // QT_CONFIG(shortcut)
        actionUndo->setText(QCoreApplication::translate("UILayoutWindow", "\346\222\244\351\224\200(&Z)", nullptr));
#if QT_CONFIG(shortcut)
        actionUndo->setShortcut(QCoreApplication::translate("UILayoutWindow", "Ctrl+Z", nullptr));
#endif // QT_CONFIG(shortcut)
        actionRedo->setText(QCoreApplication::translate("UILayoutWindow", "\346\201\242\345\244\215(&Y)", nullptr));
#if QT_CONFIG(shortcut)
        actionRedo->setShortcut(QCoreApplication::translate("UILayoutWindow", "Ctrl+Y", nullptr));
#endif // QT_CONFIG(shortcut)
        actionPreview->setText(QCoreApplication::translate("UILayoutWindow", "\351\242\204\350\247\210(&P)", nullptr));
#if QT_CONFIG(shortcut)
        actionPreview->setShortcut(QCoreApplication::translate("UILayoutWindow", "Ctrl+P", nullptr));
#endif // QT_CONFIG(shortcut)
        actionSuggestLayout->setText(QCoreApplication::translate("UILayoutWindow", "\345\270\203\345\261\200\345\273\272\350\256\256(&L)", nullptr));
#if QT_CONFIG(shortcut)
        actionSuggestLayout->setShortcut(QCoreApplication::translate("UILayoutWindow", "Ctrl+L", nullptr));
#endif // QT_CONFIG(shortcut)
#if QT_CONFIG(tooltip)
        actionSuggestLayout->setToolTip(QCoreApplication::translate("UILayoutWindow", "\350\216\267\345\217\226\345\237\272\344\272\216\344\272\247\345\223\201\345\212\237\350\203\275\347\211\271\346\200\247\347\232\204\345\270\203\345\261\200\345\273\272\350\256\256", nullptr));
#endif // QT_CONFIG(tooltip)
        actionEventActionEditor->setText(QCoreApplication::translate("UILayoutWindow", "\344\272\213\344\273\266-\345\212\250\344\275\234\347\274\226\350\276\221\345\231\250(&E)", nullptr));
#if QT_CONFIG(shortcut)
        actionEventActionEditor->setShortcut(QCoreApplication::translate("UILayoutWindow", "Ctrl+E", nullptr));
#endif // QT_CONFIG(shortcut)
#if QT_CONFIG(tooltip)
        actionEventActionEditor->setToolTip(QCoreApplication::translate("UILayoutWindow", "\346\211\223\345\274\200\344\272\213\344\273\266-\345\212\250\344\275\234\347\274\226\350\276\221\345\231\250\357\274\214\351\205\215\347\275\256\346\216\247\344\273\266\351\227\264\344\272\244\344\272\222\345\205\263\347\263\273", nullptr));
#endif // QT_CONFIG(tooltip)
        label->setText(QCoreApplication::translate("UILayoutWindow", "\346\216\247\344\273\266\347\233\222\345\255\220", nullptr));
        searchLineEdit->setPlaceholderText(QCoreApplication::translate("UILayoutWindow", "\346\220\234\347\264\242\346\216\247\344\273\266...", nullptr));
        propertiesLabel->setText(QCoreApplication::translate("UILayoutWindow", "\345\261\236\346\200\247\347\274\226\350\276\221\345\231\250", nullptr));
        propertiesTreeWidget->setHeaderLabels(QStringList{
            QCoreApplication::translate("UILayoutWindow", "\345\261\236\346\200\247\345\220\215", nullptr),
            QCoreApplication::translate("UILayoutWindow", "\345\261\236\346\200\247\345\200\274", nullptr)});
        rightTabWidget->setTabText(rightTabWidget->indexOf(propertiesTab), QCoreApplication::translate("UILayoutWindow", "\345\261\236\346\200\247\347\274\226\350\276\221\345\231\250", nullptr));
        featuresLabel->setText(QCoreApplication::translate("UILayoutWindow", "\345\212\237\350\203\275\347\211\271\346\200\247\345\205\263\350\201\224", nullptr));
        featuresTreeWidget->setHeaderLabels(QStringList{
            QCoreApplication::translate("UILayoutWindow", "\345\212\237\350\203\275\347\211\271\346\200\247", nullptr),
            QCoreApplication::translate("UILayoutWindow", "\345\205\263\350\201\224\346\216\247\344\273\266", nullptr),
            QCoreApplication::translate("UILayoutWindow", "\347\212\266\346\200\201", nullptr)});
        bindFeatureButton->setText(QCoreApplication::translate("UILayoutWindow", "\347\273\221\345\256\232\347\211\271\346\200\247", nullptr));
        unbindFeatureButton->setText(QCoreApplication::translate("UILayoutWindow", "\350\247\243\347\273\221\347\211\271\346\200\247", nullptr));
        syncFeaturesButton->setText(QCoreApplication::translate("UILayoutWindow", "\345\220\214\346\255\245\345\212\237\350\203\275\347\211\271\346\200\247", nullptr));
        rightTabWidget->setTabText(rightTabWidget->indexOf(featuresTab), QCoreApplication::translate("UILayoutWindow", "\345\212\237\350\203\275\347\211\271\346\200\247", nullptr));
        menu->setTitle(QCoreApplication::translate("UILayoutWindow", "\346\226\207\344\273\266", nullptr));
        menu_2->setTitle(QCoreApplication::translate("UILayoutWindow", "\347\274\226\350\276\221", nullptr));
        menu_3->setTitle(QCoreApplication::translate("UILayoutWindow", "\344\272\244\344\272\222", nullptr));
    } // retranslateUi

};

namespace Ui {
    class UILayoutWindow: public Ui_UILayoutWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_UILAYOUTWINDOW_H
