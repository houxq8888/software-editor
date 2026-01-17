#include "statemachineeditor_v2.h"
#include "uiinterface.h"
#include "wizard.h"
#include <QMessageBox>
#include <QDebug>
#include <QFileDialog>
#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QGroupBox>
#include <QLabel>
#include <QApplication>
#include <QMainWindow>
#include <QDialog>
#include <QWizard>
#include <QWizardPage>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>

UIFilePreviewItem::UIFilePreviewItem(const ProductUIFile &uiFile, QWidget *parent)
    : QWidget(parent)
    , m_uiFile(uiFile)
    , m_uiInterface(nullptr)
    , m_controlCount(0)
    , m_uiType("unknown")
    , m_mainWidgetClass("QWidget")
{}

void StateMachineEditorV2::runWizard()
{
    qDebug() << "[DEBUG] runWizard() function called";

    // 1. 查找主界面
    UIFilePreviewItem *mainUiItem = nullptr;
    bool hasMainUi = false;

    for (int i = 0; i < m_uiFilesListWidget->count(); ++i) {
        QListWidgetItem *listItem = m_uiFilesListWidget->item(i);
        UIFilePreviewItem *uiItem = qobject_cast<UIFilePreviewItem*>(m_uiFilesListWidget->itemWidget(listItem));

        if (uiItem && uiItem->isMainInterface()) {
            mainUiItem = uiItem;
            hasMainUi = true;
            qDebug() << "[DEBUG] Main interface found: " << uiItem->fileName();
            break;
        }
    }

    if (!hasMainUi) {
        QMessageBox::critical(this, "运行向导", "请先选择一个主界面，作为程序的第一个窗口。\n\n您可以在UI文件列表中点击'设为主界面'按钮来设置主界面。");
        return;
    }

    qDebug() << "[DEBUG] Main interface found, proceeding to create wizard flow";

    // 2. 创建向导并添加页面
    Wizard *wizard = m_wizardManager->createWizard("MainApplicationWizard", "主应用程序向导");

    if (!wizard) {
        qDebug() << "[ERROR] Failed to create wizard";
        QMessageBox::critical(this, "错误", "无法创建向导实例");
        return;
    }

    // 设置当前向导
    m_wizardManager->setCurrentWizard(wizard);

    // 3. 创建所有UI界面的向导页
    // 收集所有UI界面（包括主界面）
    QList<UIFilePreviewItem*> allUiItems;
    for (int i = 0; i < m_uiFilesListWidget->count(); ++i) {
        QListWidgetItem *listItem = m_uiFilesListWidget->item(i);
        UIFilePreviewItem *uiItem = qobject_cast<UIFilePreviewItem*>(m_uiFilesListWidget->itemWidget(listItem));

        if (uiItem) {
            allUiItems.append(uiItem);
        }
    }

    qDebug() << "[DEBUG] Total UI items found: " << allUiItems.size();

    // 确保主界面在第一个位置
    if (mainUiItem && allUiItems.contains(mainUiItem)) {
        allUiItems.removeOne(mainUiItem);
        allUiItems.prepend(mainUiItem);
    }

    // 4. 添加所有UI界面到向导
    QMap<QString, QString> uiFilePathToPageIdMap;
    for (int i = 0; i < allUiItems.size(); ++i) {
        UIFilePreviewItem *uiItem = allUiItems[i];
        QString pageId = "page_" + QString::number(i);
        uiFilePathToPageIdMap[uiItem->filePath()] = pageId;

        WizardPage page;
        page.pageId = pageId;
        page.title = uiItem->fileName();
        page.description = "UI界面: " + uiItem->fileName();
        page.isStartPage = (i == 0);
        page.isFinalPage = (i == allUiItems.size() - 1);
        page.enableNextButton = (i < allUiItems.size() - 1);
        page.enableBackButton = (i > 0);
        page.enableFinishButton = (i == allUiItems.size() - 1);

        // 设置默认的前后页ID（用于孤立UI）
        if (i > 0) {
            page.previousPageId = "page_" + QString::number(i - 1);
        }
        if (i < allUiItems.size() - 1) {
            page.nextPageId = "page_" + QString::number(i + 1);
        }

        // 关联UI文件
        qDebug() << "[DEBUG] UI file path: " << uiItem->filePath();

        // 从uiItem获取UIInterface
        page.uiInterface = uiItem->uiInterface();
        qDebug() << "[DEBUG] Page uiInterface from uiItem: " << page.uiInterface;

        // 验证uiInterface是否有效
        if (page.uiInterface) {
            qDebug() << "[DEBUG] Page uiInterface is valid: " << page.uiInterface->name();
        }

        wizard->addPage(page);
        qDebug() << "[DEBUG] Page added to wizard: " << uiItem->fileName() << " with ID: " << pageId;
    }

    // 5. 处理UI控件事件的跳转逻辑
    for (int i = 0; i < allUiItems.size(); ++i) {
        UIFilePreviewItem *uiItem = allUiItems[i];
        QString pageId = "page_" + QString::number(i);

        // 获取当前页面
        WizardPage *page = wizard->findPage(pageId);
        if (!page || !page->uiInterface) continue;

        // 暂时跳过控件事件处理，因为UIInterface中没有controls()方法
        // 后续可以在UIInterface中添加控件管理功能
        qDebug() << "[DEBUG] Skipping control event handling for page: " << pageId;
    }

    // 6. 检查向导是否有足够的页面
    if (wizard->allPages().isEmpty()) {
        qDebug() << "[ERROR] Wizard has no pages";
        QMessageBox::critical(this, "错误", "向导未初始化，至少需要一个页面才能运行。");
        return;
    }

    // 6. 生成QT代码
    qDebug() << "[DEBUG] Generating QT code for wizard flow...";
    QString qtCode = generateQtCodeForWizard(wizard, mainUiItem, allUiItems);

    // 保存生成的代码到临时文件或显示给用户
    if (!qtCode.isEmpty()) {
        qDebug() << "[DEBUG] QT code generated successfully";

        // 这里可以选择将代码保存到文件或显示在对话框中
        // 暂时保存到临时文件
        QString tempFile = QDir::tempPath() + "/wizard_generated_code.cpp";
        QFile file(tempFile);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << qtCode;
            file.close();
            qDebug() << "[DEBUG] Generated code saved to: " << tempFile;
        }
    }

    // 7. 启动向导
    if (wizard->start()) {
        qDebug() << "[DEBUG] Wizard started successfully";

        // 弹出向导运行效果
        m_wizardPreviewWidget->loadWizard(wizard);
        m_wizardPreviewWidget->show();
        m_wizardPreviewWidget->raise();
        m_wizardPreviewWidget->activateWindow();

        QMessageBox::information(this, "运行向导", "向导已成功启动！\n\n向导运行效果已弹出，您可以在新窗口中体验向导流程。\n\n主界面是起始页，点击'下一步'按钮将跳转到下一个界面。\n\nQT代码已生成并保存到临时文件。");
    } else {
        qDebug() << "[ERROR] Failed to start wizard";
        QMessageBox::critical(this, "错误", "无法启动向导，向导可能未正确初始化。");
    }

    qDebug() << "[DEBUG] runWizard() function completed successfully";
}

QString StateMachineEditorV2::generateQtCodeForWizard(Wizard *wizard, UIFilePreviewItem *mainUiItem, QList<UIFilePreviewItem*> allUiItems)
{
    if (!wizard) {
        qDebug() << "[ERROR] Invalid wizard parameter for generateQtCodeForWizard";
        return QString();
    }

    QStringList codeLines;

    // 头文件包含
    codeLines << "#include <QApplication>";
    codeLines << "#include <QMainWindow>";
    codeLines << "#include <QDialog>";
    codeLines << "#include <QWizard>";
    codeLines << "#include <QWizardPage>";
    codeLines << "#include <QPushButton>";
    codeLines << "#include <QVBoxLayout>";
    codeLines << "#include <QLabel>";
    codeLines << "#include <QMessageBox>";
    codeLines << "#include <QFile>";
    codeLines << "#include <QTextStream>";
    codeLines << "";

    // 命名空间
    codeLines << "using namespace std;";
    codeLines << "";

    // 主函数
    codeLines << "int main(int argc, char *argv[])";
    codeLines << "{";
    codeLines << "    QApplication app(argc, argv);";
    codeLines << "    ";
    codeLines << "    // 设置应用程序信息";
    codeLines << QString("    app.setApplicationName(\"%1\");").arg(wizard->name().isEmpty() ? "Wizard Application" : wizard->name());
    codeLines << "    app.setApplicationVersion(\"1.0\");";
    codeLines << "    ";

    // 创建向导对象
    codeLines << "    // 创建向导";
    codeLines << QString("    QWizard *wizard = new QWizard();");
    codeLines << QString("    wizard->setWindowTitle(\"%1\");").arg(wizard->name().isEmpty() ? "Wizard Application" : wizard->name());
    if (!wizard->description().isEmpty()) {
        codeLines << QString("    // 向导描述: %1").arg(wizard->description());
    }
    codeLines << "    wizard->resize(800, 600);";
    codeLines << "    ";

    // 生成向导页面代码
    QList<WizardPage*> pages = wizard->allPages();
    for (int i = 0; i < pages.size(); ++i) {
        WizardPage *page = pages[i];
        codeLines << QString("    // 创建向导页面 %1: %2").arg(i+1).arg(page->title.isEmpty() ? "Untitled Page" : page->title);
        codeLines << QString("    QWizardPage *page%1 = new QWizardPage(wizard);").arg(i+1);
        codeLines << QString("    page%1->setTitle(\"%2\");").arg(i+1).arg(page->title.isEmpty() ? QString("Page %1").arg(i+1) : page->title);

        if (!page->description.isEmpty()) {
            codeLines << QString("    page%1->setSubTitle(\"%2\");").arg(i+1).arg(page->description);
        }

        // 添加页面内容布局
        codeLines << QString("    QVBoxLayout *layout%1 = new QVBoxLayout(page%1);").arg(i+1);

        // 如果有UI界面信息，添加说明
        if (page->uiInterface) {
            codeLines << QString("    // 该页面关联UI界面: %1").arg(page->uiInterface->name());
            codeLines << QString("    QLabel *uiLabel%1 = new QLabel(\"UI界面: %2\", page%1);").arg(i+1).arg(page->uiInterface->name());
            if (!page->uiInterface->description().isEmpty()) {
                codeLines << QString("    QLabel *descLabel%1 = new QLabel(\"%2\", page%1);").arg(i+1).arg(page->uiInterface->description());
                codeLines << QString("    layout%1->addWidget(descLabel%1);").arg(i+1);
            }
            codeLines << QString("    layout%1->addWidget(uiLabel%1);").arg(i+1);
        }

        // 添加页面完成
        codeLines << QString("    wizard->addPage(page%1);").arg(i+1);
        codeLines << "    ";
    }

    // 设置向导按钮
    codeLines << "    // 设置向导按钮";
    codeLines << "    wizard->setOption(QWizard::HaveNextButton, true);";
    codeLines << "    wizard->setOption(QWizard::HaveBackButton, true);";
    codeLines << "    wizard->setOption(QWizard::HaveFinishButton, true);";
    codeLines << "    wizard->setOption(QWizard::HaveCancelButton, true);";
    codeLines << "    ";

    // 显示向导
    codeLines << "    // 显示向导";
    codeLines << "    wizard->show();";
    codeLines << "    ";

    // 执行应用程序
    codeLines << "    return app.exec();";
    codeLines << "}";

    return codeLines.join("\n");
}