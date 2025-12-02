#include <QApplication>
#include <QTest>
#include <QTimer>
#include <QMainWindow>
#include <QListWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QPushButton>
#include <QMenu>
#include <QAction>
#include "uilayoutwindow.h"

class UITest : public QObject
{
    Q_OBJECT
public:
    explicit UITest(QObject *parent = nullptr) : QObject(parent) {}

private slots:
    void initTestCase()
    {
        m_app = new QApplication(m_argc, m_argv);
        m_window = new UILayoutWindow();
        m_window->show();
        QTest::qWait(500); // Wait for window to initialize
    }

    void testWidgetLibraryLoad()
    {
        // Test if widget library is loaded correctly
        QListWidget *listWidget = m_window->findChild<QListWidget*>("widgetListWidget");
        QVERIFY(listWidget != nullptr);
        QCOMPARE(listWidget->count(), 13); // Should have 13 widget types
        QVERIFY(listWidget->item(0)->text() == "QLabel");
        QVERIFY(listWidget->item(1)->text() == "QPushButton");
    }

    void testDragAndDropFunctionality()
    {
        // Test drag and drop functionality
        QListWidget *listWidget = m_window->findChild<QListWidget*>("widgetListWidget");
        QGraphicsView *graphicsView = m_window->findChild<QGraphicsView*>("graphicsView");
        QVERIFY(listWidget != nullptr);
        QVERIFY(graphicsView != nullptr);
        QGraphicsScene *scene = graphicsView->scene();
        QVERIFY(scene != nullptr);

        // Get initial item count
        int initialItemCount = scene->items().count();

        // Simulate clicking the first item (QLabel)
        QListWidgetItem *item = listWidget->item(0);
        QVERIFY(item != nullptr);

        // Simulate mouse press and drag
        QPoint listPos = listWidget->visualItemRect(item).center();
        QPoint viewPos = graphicsView->viewport()->rect().center();

        // Simulate drag operation
        QTest::mousePress(listWidget->viewport(), Qt::LeftButton, Qt::NoModifier, listPos);
        QTest::mouseMove(graphicsView->viewport(), viewPos, 500);
        QTest::mouseRelease(graphicsView->viewport(), Qt::LeftButton, Qt::NoModifier, viewPos, 500);

        // Check if new item was added
        int newItemCount = scene->items().count();
        QVERIFY(newItemCount > initialItemCount);
        qDebug() << "Drag and drop test: Items before:" << initialItemCount << ", after:" << newItemCount;
    }

    void cleanupTestCase()
    {
        delete m_window;
        delete m_app;
    }

private:
    int m_argc = 0;
    char **m_argv = nullptr;
    QApplication *m_app = nullptr;
    UILayoutWindow *m_window = nullptr;
};

QTEST_MAIN(UITest)
#include "uitest.moc"