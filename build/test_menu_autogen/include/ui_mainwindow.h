/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionNew;
    QAction *actionOpen;
    QAction *actionSave;
    QAction *actionSave_As;
    QAction *actionExit;
    QAction *actionAbout;
    QAction *actionOpen_UI_Layout_Editor;
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QTabWidget *tabWidget;
    QWidget *basicInfoTab;
    QVBoxLayout *verticalLayout_2;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QLabel *nameLabel;
    QLineEdit *nameLineEdit;
    QLabel *versionLabel;
    QLineEdit *versionLineEdit;
    QLabel *categoryLabel;
    QLineEdit *categoryLineEdit;
    QLabel *developerLabel;
    QLineEdit *developerLineEdit;
    QLabel *websiteLabel;
    QLineEdit *websiteLineEdit;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_2;
    QLabel *iconPathLabel;
    QLineEdit *iconPathLineEdit;
    QPushButton *iconBrowseButton;
    QLabel *screenshotPathLabel;
    QLineEdit *screenshotPathLineEdit;
    QPushButton *screenshotBrowseButton;
    QGroupBox *groupBox_3;
    QVBoxLayout *verticalLayout_3;
    QTextEdit *descriptionTextEdit;
    QWidget *featuresTab;
    QVBoxLayout *verticalLayout_4;
    QListWidget *featuresListWidget;
    QGroupBox *groupBox_4;
    QGridLayout *gridLayout_3;
    QLabel *featureNameLabel;
    QLineEdit *featureNameLineEdit;
    QLabel *featureDescriptionLabel;
    QTextEdit *featureDescriptionTextEdit;
    QPushButton *addFeatureButton;
    QPushButton *removeFeatureButton;
    QGroupBox *groupBox_5;
    QHBoxLayout *horizontalLayout;
    QPushButton *saveButton;
    QPushButton *loadButton;
    QPushButton *exportButton;
    QMenuBar *menuBar;
    QMenu *menu;
    QMenu *menu_3;
    QMenu *menu_2;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(800, 600);
        actionNew = new QAction(MainWindow);
        actionNew->setObjectName("actionNew");
        actionOpen = new QAction(MainWindow);
        actionOpen->setObjectName("actionOpen");
        actionSave = new QAction(MainWindow);
        actionSave->setObjectName("actionSave");
        actionSave_As = new QAction(MainWindow);
        actionSave_As->setObjectName("actionSave_As");
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName("actionExit");
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName("actionAbout");
        actionOpen_UI_Layout_Editor = new QAction(MainWindow);
        actionOpen_UI_Layout_Editor->setObjectName("actionOpen_UI_Layout_Editor");
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName("centralWidget");
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setObjectName("verticalLayout");
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName("tabWidget");
        basicInfoTab = new QWidget();
        basicInfoTab->setObjectName("basicInfoTab");
        verticalLayout_2 = new QVBoxLayout(basicInfoTab);
        verticalLayout_2->setObjectName("verticalLayout_2");
        groupBox = new QGroupBox(basicInfoTab);
        groupBox->setObjectName("groupBox");
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setObjectName("gridLayout");
        nameLabel = new QLabel(groupBox);
        nameLabel->setObjectName("nameLabel");

        gridLayout->addWidget(nameLabel, 0, 0, 1, 1);

        nameLineEdit = new QLineEdit(groupBox);
        nameLineEdit->setObjectName("nameLineEdit");

        gridLayout->addWidget(nameLineEdit, 0, 1, 1, 1);

        versionLabel = new QLabel(groupBox);
        versionLabel->setObjectName("versionLabel");

        gridLayout->addWidget(versionLabel, 1, 0, 1, 1);

        versionLineEdit = new QLineEdit(groupBox);
        versionLineEdit->setObjectName("versionLineEdit");

        gridLayout->addWidget(versionLineEdit, 1, 1, 1, 1);

        categoryLabel = new QLabel(groupBox);
        categoryLabel->setObjectName("categoryLabel");

        gridLayout->addWidget(categoryLabel, 2, 0, 1, 1);

        categoryLineEdit = new QLineEdit(groupBox);
        categoryLineEdit->setObjectName("categoryLineEdit");

        gridLayout->addWidget(categoryLineEdit, 2, 1, 1, 1);

        developerLabel = new QLabel(groupBox);
        developerLabel->setObjectName("developerLabel");

        gridLayout->addWidget(developerLabel, 3, 0, 1, 1);

        developerLineEdit = new QLineEdit(groupBox);
        developerLineEdit->setObjectName("developerLineEdit");

        gridLayout->addWidget(developerLineEdit, 3, 1, 1, 1);

        websiteLabel = new QLabel(groupBox);
        websiteLabel->setObjectName("websiteLabel");

        gridLayout->addWidget(websiteLabel, 4, 0, 1, 1);

        websiteLineEdit = new QLineEdit(groupBox);
        websiteLineEdit->setObjectName("websiteLineEdit");

        gridLayout->addWidget(websiteLineEdit, 4, 1, 1, 1);


        verticalLayout_2->addWidget(groupBox);

        groupBox_2 = new QGroupBox(basicInfoTab);
        groupBox_2->setObjectName("groupBox_2");
        gridLayout_2 = new QGridLayout(groupBox_2);
        gridLayout_2->setObjectName("gridLayout_2");
        iconPathLabel = new QLabel(groupBox_2);
        iconPathLabel->setObjectName("iconPathLabel");

        gridLayout_2->addWidget(iconPathLabel, 0, 0, 1, 1);

        iconPathLineEdit = new QLineEdit(groupBox_2);
        iconPathLineEdit->setObjectName("iconPathLineEdit");

        gridLayout_2->addWidget(iconPathLineEdit, 0, 1, 1, 1);

        iconBrowseButton = new QPushButton(groupBox_2);
        iconBrowseButton->setObjectName("iconBrowseButton");

        gridLayout_2->addWidget(iconBrowseButton, 0, 2, 1, 1);

        screenshotPathLabel = new QLabel(groupBox_2);
        screenshotPathLabel->setObjectName("screenshotPathLabel");

        gridLayout_2->addWidget(screenshotPathLabel, 1, 0, 1, 1);

        screenshotPathLineEdit = new QLineEdit(groupBox_2);
        screenshotPathLineEdit->setObjectName("screenshotPathLineEdit");

        gridLayout_2->addWidget(screenshotPathLineEdit, 1, 1, 1, 1);

        screenshotBrowseButton = new QPushButton(groupBox_2);
        screenshotBrowseButton->setObjectName("screenshotBrowseButton");

        gridLayout_2->addWidget(screenshotBrowseButton, 1, 2, 1, 1);


        verticalLayout_2->addWidget(groupBox_2);

        groupBox_3 = new QGroupBox(basicInfoTab);
        groupBox_3->setObjectName("groupBox_3");
        verticalLayout_3 = new QVBoxLayout(groupBox_3);
        verticalLayout_3->setObjectName("verticalLayout_3");
        descriptionTextEdit = new QTextEdit(groupBox_3);
        descriptionTextEdit->setObjectName("descriptionTextEdit");

        verticalLayout_3->addWidget(descriptionTextEdit);


        verticalLayout_2->addWidget(groupBox_3);

        tabWidget->addTab(basicInfoTab, QString());
        featuresTab = new QWidget();
        featuresTab->setObjectName("featuresTab");
        verticalLayout_4 = new QVBoxLayout(featuresTab);
        verticalLayout_4->setObjectName("verticalLayout_4");
        featuresListWidget = new QListWidget(featuresTab);
        featuresListWidget->setObjectName("featuresListWidget");
        featuresListWidget->setAlternatingRowColors(true);

        verticalLayout_4->addWidget(featuresListWidget);

        groupBox_4 = new QGroupBox(featuresTab);
        groupBox_4->setObjectName("groupBox_4");
        gridLayout_3 = new QGridLayout(groupBox_4);
        gridLayout_3->setObjectName("gridLayout_3");
        featureNameLabel = new QLabel(groupBox_4);
        featureNameLabel->setObjectName("featureNameLabel");

        gridLayout_3->addWidget(featureNameLabel, 0, 0, 1, 1);

        featureNameLineEdit = new QLineEdit(groupBox_4);
        featureNameLineEdit->setObjectName("featureNameLineEdit");

        gridLayout_3->addWidget(featureNameLineEdit, 0, 1, 1, 1);

        featureDescriptionLabel = new QLabel(groupBox_4);
        featureDescriptionLabel->setObjectName("featureDescriptionLabel");

        gridLayout_3->addWidget(featureDescriptionLabel, 1, 0, 1, 1);

        featureDescriptionTextEdit = new QTextEdit(groupBox_4);
        featureDescriptionTextEdit->setObjectName("featureDescriptionTextEdit");

        gridLayout_3->addWidget(featureDescriptionTextEdit, 1, 1, 1, 1);

        addFeatureButton = new QPushButton(groupBox_4);
        addFeatureButton->setObjectName("addFeatureButton");

        gridLayout_3->addWidget(addFeatureButton, 2, 0, 1, 1);

        removeFeatureButton = new QPushButton(groupBox_4);
        removeFeatureButton->setObjectName("removeFeatureButton");

        gridLayout_3->addWidget(removeFeatureButton, 2, 1, 1, 1);


        verticalLayout_4->addWidget(groupBox_4);

        tabWidget->addTab(featuresTab, QString());

        verticalLayout->addWidget(tabWidget);

        groupBox_5 = new QGroupBox(centralWidget);
        groupBox_5->setObjectName("groupBox_5");
        horizontalLayout = new QHBoxLayout(groupBox_5);
        horizontalLayout->setObjectName("horizontalLayout");
        saveButton = new QPushButton(groupBox_5);
        saveButton->setObjectName("saveButton");

        horizontalLayout->addWidget(saveButton);

        loadButton = new QPushButton(groupBox_5);
        loadButton->setObjectName("loadButton");

        horizontalLayout->addWidget(loadButton);

        exportButton = new QPushButton(groupBox_5);
        exportButton->setObjectName("exportButton");

        horizontalLayout->addWidget(exportButton);


        verticalLayout->addWidget(groupBox_5);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName("menuBar");
        menuBar->setGeometry(QRect(0, 0, 800, 40));
        menu = new QMenu(menuBar);
        menu->setObjectName("menu");
        menu_3 = new QMenu(menuBar);
        menu_3->setObjectName("menu_3");
        menu_2 = new QMenu(menuBar);
        menu_2->setObjectName("menu_2");
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName("mainToolBar");
        MainWindow->addToolBar(Qt::ToolBarArea::LeftToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName("statusBar");
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menu->menuAction());
        menuBar->addAction(menu_3->menuAction());
        menuBar->addAction(menu_2->menuAction());
        menu->addAction(actionNew);
        menu->addAction(actionOpen);
        menu->addAction(actionSave);
        menu->addAction(actionSave_As);
        menu->addAction(actionExit);
        menu_3->addAction(actionOpen_UI_Layout_Editor);
        menu_2->addAction(actionAbout);
        mainToolBar->addAction(actionNew);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "\350\275\257\344\273\266\344\272\247\345\223\201\347\274\226\350\276\221\347\225\214\351\235\242", nullptr));
        actionNew->setText(QCoreApplication::translate("MainWindow", "\346\226\260\345\273\272", nullptr));
#if QT_CONFIG(shortcut)
        actionNew->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+N", nullptr));
#endif // QT_CONFIG(shortcut)
        actionOpen->setText(QCoreApplication::translate("MainWindow", "\346\211\223\345\274\200", nullptr));
#if QT_CONFIG(shortcut)
        actionOpen->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+O", nullptr));
#endif // QT_CONFIG(shortcut)
        actionSave->setText(QCoreApplication::translate("MainWindow", "\344\277\235\345\255\230", nullptr));
#if QT_CONFIG(shortcut)
        actionSave->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+S", nullptr));
#endif // QT_CONFIG(shortcut)
        actionSave_As->setText(QCoreApplication::translate("MainWindow", "\345\217\246\345\255\230\344\270\272", nullptr));
#if QT_CONFIG(shortcut)
        actionSave_As->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Shift+S", nullptr));
#endif // QT_CONFIG(shortcut)
        actionExit->setText(QCoreApplication::translate("MainWindow", "\351\200\200\345\207\272", nullptr));
        actionAbout->setText(QCoreApplication::translate("MainWindow", "\345\205\263\344\272\216", nullptr));
        actionOpen_UI_Layout_Editor->setText(QCoreApplication::translate("MainWindow", "\346\211\223\345\274\200UI\345\270\203\345\261\200\347\274\226\350\276\221\345\231\250", nullptr));
#if QT_CONFIG(shortcut)
        actionOpen_UI_Layout_Editor->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+L", nullptr));
#endif // QT_CONFIG(shortcut)
        groupBox->setTitle(QCoreApplication::translate("MainWindow", "\344\272\247\345\223\201\345\237\272\346\234\254\344\277\241\346\201\257", nullptr));
        nameLabel->setText(QCoreApplication::translate("MainWindow", "\344\272\247\345\223\201\345\220\215\347\247\260\357\274\232", nullptr));
        nameLineEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "\350\276\223\345\205\245\344\272\247\345\223\201\345\220\215\347\247\260", nullptr));
        versionLabel->setText(QCoreApplication::translate("MainWindow", "\347\211\210\346\234\254\357\274\232", nullptr));
        versionLineEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "\350\276\223\345\205\245\347\211\210\346\234\254\345\217\267", nullptr));
        categoryLabel->setText(QCoreApplication::translate("MainWindow", "\345\210\206\347\261\273\357\274\232", nullptr));
        categoryLineEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "\350\276\223\345\205\245\344\272\247\345\223\201\345\210\206\347\261\273", nullptr));
        developerLabel->setText(QCoreApplication::translate("MainWindow", "\345\274\200\345\217\221\350\200\205\357\274\232", nullptr));
        developerLineEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "\350\276\223\345\205\245\345\274\200\345\217\221\350\200\205\345\220\215\347\247\260", nullptr));
        websiteLabel->setText(QCoreApplication::translate("MainWindow", "\347\275\221\347\253\231\357\274\232", nullptr));
        websiteLineEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "\350\276\223\345\205\245\344\272\247\345\223\201\347\275\221\347\253\231", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("MainWindow", "\346\226\207\344\273\266\350\256\276\347\275\256", nullptr));
        iconPathLabel->setText(QCoreApplication::translate("MainWindow", "\345\233\276\346\240\207\350\267\257\345\276\204\357\274\232", nullptr));
        iconPathLineEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "\351\200\211\346\213\251\344\272\247\345\223\201\345\233\276\346\240\207", nullptr));
        iconBrowseButton->setText(QCoreApplication::translate("MainWindow", "\346\265\217\350\247\210...", nullptr));
        screenshotPathLabel->setText(QCoreApplication::translate("MainWindow", "\346\210\252\345\233\276\350\267\257\345\276\204\357\274\232", nullptr));
        screenshotPathLineEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "\351\200\211\346\213\251\344\272\247\345\223\201\346\210\252\345\233\276", nullptr));
        screenshotBrowseButton->setText(QCoreApplication::translate("MainWindow", "\346\265\217\350\247\210...", nullptr));
        groupBox_3->setTitle(QCoreApplication::translate("MainWindow", "\344\272\247\345\223\201\346\217\217\350\277\260", nullptr));
        descriptionTextEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "\350\276\223\345\205\245\344\272\247\345\223\201\346\217\217\350\277\260", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(basicInfoTab), QCoreApplication::translate("MainWindow", "\345\237\272\346\234\254\344\277\241\346\201\257", nullptr));
        groupBox_4->setTitle(QCoreApplication::translate("MainWindow", "\347\274\226\350\276\221\345\212\237\350\203\275", nullptr));
        featureNameLabel->setText(QCoreApplication::translate("MainWindow", "\345\212\237\350\203\275\345\220\215\347\247\260\357\274\232", nullptr));
        featureNameLineEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "\350\276\223\345\205\245\345\212\237\350\203\275\345\220\215\347\247\260", nullptr));
        featureDescriptionLabel->setText(QCoreApplication::translate("MainWindow", "\345\212\237\350\203\275\346\217\217\350\277\260\357\274\232", nullptr));
        featureDescriptionTextEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "\350\276\223\345\205\245\345\212\237\350\203\275\346\217\217\350\277\260", nullptr));
        addFeatureButton->setText(QCoreApplication::translate("MainWindow", "\346\267\273\345\212\240\345\212\237\350\203\275", nullptr));
        removeFeatureButton->setText(QCoreApplication::translate("MainWindow", "\345\210\240\351\231\244\345\212\237\350\203\275", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(featuresTab), QCoreApplication::translate("MainWindow", "\345\212\237\350\203\275\347\211\271\346\200\247", nullptr));
        groupBox_5->setTitle(QCoreApplication::translate("MainWindow", "\346\223\215\344\275\234", nullptr));
        saveButton->setText(QCoreApplication::translate("MainWindow", "\344\277\235\345\255\230", nullptr));
        loadButton->setText(QCoreApplication::translate("MainWindow", "\345\212\240\350\275\275", nullptr));
        exportButton->setText(QCoreApplication::translate("MainWindow", "\345\257\274\345\207\272", nullptr));
        menu->setTitle(QCoreApplication::translate("MainWindow", "\346\226\207\344\273\266", nullptr));
        menu_3->setTitle(QCoreApplication::translate("MainWindow", "UI\345\270\203\345\261\200", nullptr));
        menu_2->setTitle(QCoreApplication::translate("MainWindow", "\345\270\256\345\212\251", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
