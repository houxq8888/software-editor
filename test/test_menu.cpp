#include <QApplication>
#include <QTest>
#include <QMenuBar>
#include <QMenu>
#include "uilayoutwindow.h"

class TestMenu : public QObject
{
    Q_OBJECT

private slots:
    void testSaveAsMenuExists();
};

void TestMenu::testSaveAsMenuExists()
{
    // 创建应用程序
    int argc = 0;
    char **argv = nullptr;
    QApplication app(argc, argv);
    
    // 创建主窗口
    UILayoutWindow window;
    
    // 获取菜单栏
    QMenuBar *menuBar = window.findChild<QMenuBar*>("menubar");
    QVERIFY(menuBar != nullptr);
    
    // 获取"文件"菜单
    QMenu *fileMenu = menuBar->findChild<QMenu*>("menu");
    QVERIFY(fileMenu != nullptr);
    
    // 检查"另存为"菜单项是否存在
    QAction *saveAsAction = fileMenu->findChild<QAction*>("actionSave_As_Layout");
    QVERIFY(saveAsAction != nullptr);
    
    // 检查菜单项是否可见
    QVERIFY(saveAsAction->isVisible());
    
    // 检查菜单项文本是否正确
    QCOMPARE(saveAsAction->text(), QString("另存为"));
}

QTEST_MAIN(TestMenu)
#include "test_menu.moc"