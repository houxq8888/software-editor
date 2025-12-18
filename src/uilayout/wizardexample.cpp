#include "wizard.h"
#include "uiinterfacemanager.h"
#include "statemachine.h"
#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QDebug>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>

// 创建简单的向导页面UI界面
UIInterface* createWizardPage(UIInterfaceManager* manager, const QString& pageId, const QString& title, const QString& description) {
    UIInterface* ui = manager->createInterface(title, description);
    ui->setTitle(title);
    ui->setDescription(description);
    ui->setSize(QSize(600, 400));
    ui->setIsMainWindow(false);

    // 注意：当前LayoutItem类的结构与示例代码中的预期不同，
    // 我们暂时简化这个函数，只创建UIInterface对象，不设置布局

    return ui;
}

// 向导示例应用程序
class WizardExample : public QMainWindow {
    Q_OBJECT

public:
    WizardExample(QWidget *parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("Wizard Function Example");
        setGeometry(100, 100, 800, 600);

        // 创建主布局
        QWidget* centralWidget = new QWidget(this);
        QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
        
        // 创建标题
        QLabel* titleLabel = new QLabel("Wizard Function Demo");
        titleLabel->setAlignment(Qt::AlignCenter);
        titleLabel->setFont(QFont("Arial", 18, QFont::Bold));
        mainLayout->addWidget(titleLabel);

        // 创建启动向导按钮
        QPushButton* startWizardBtn = new QPushButton("Start Wizard");
        mainLayout->addWidget(startWizardBtn, 0, Qt::AlignCenter);
        
        // 创建状态标签
        statusLabel = new QLabel("Ready to start wizard");
        statusLabel->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(statusLabel);

        setCentralWidget(centralWidget);

        // 初始化UI管理器和状态机管理器
        uiManager = new UIInterfaceManager(this);
        stateMachineManager = new StateMachineManager(this);
        stateMachineManager->setUiInterfaceManager(uiManager);

        // 初始化向导
        wizardManager = new WizardManager(this);
        stateMachineManager->setWizardManager(wizardManager);

        // 创建向导
        createSampleWizard();

        // 连接信号
        connect(startWizardBtn, &QPushButton::clicked, this, &WizardExample::startWizard);
    }

    ~WizardExample() override {
        // 清理资源
        delete wizardManager;
        delete stateMachineManager;
        delete uiManager;
    }

private slots:
    void startWizard() {
        // 启动向导
        Wizard* wizard = wizardManager->findWizard("sampleWizard");
        if (wizard) {
            if (wizard->start()) {
                statusLabel->setText("向导已启动");
                qDebug() << "Wizard started successfully";
            } else {
                statusLabel->setText("向导启动失败");
                qDebug() << "Wizard failed to start";
            }
        } else {
            statusLabel->setText("未找到向导");
            qDebug() << "Wizard not found";
        }
    }

private:
    void createSampleWizard() {
        // 创建向导
        Wizard* wizard = wizardManager->createWizard("sampleWizard", "This is a demo of wizard functionality");
        wizard->setName("sampleWizard");
        wizard->setDescription("This is a demo of wizard functionality");

        // 创建向导页面
        UIInterface* page1 = createWizardPage(uiManager, "page1", "Welcome to Wizard", "This is the first step of the wizard, you will enter your personal information here.");
        UIInterface* page2 = createWizardPage(uiManager, "page2", "Select Options", "Please select the options you want.");
        UIInterface* page3 = createWizardPage(uiManager, "page3", "Confirm Information", "Please confirm if your information is correct.");

        // 添加向导页面
        WizardPage wizardPage1;
        wizardPage1.pageId = "page1";
        wizardPage1.title = "Welcome to Wizard";
        wizardPage1.description = "This is the first step of the wizard, you will enter your personal information here.";
        wizardPage1.uiInterface = page1;
        wizardPage1.isStartPage = true;
        wizardPage1.nextPageId = "page2";
        wizardPage1.enableNextButton = true;
        wizardPage1.enableBackButton = false;
        wizardPage1.enableFinishButton = false;
        wizardPage1.enableCancelButton = true;
        wizard->addPage(wizardPage1);

        WizardPage wizardPage2;
        wizardPage2.pageId = "page2";
        wizardPage2.title = "Select Options";
        wizardPage2.description = "Please select the options you want.";
        wizardPage2.uiInterface = page2;
        wizardPage2.previousPageId = "page1";
        wizardPage2.nextPageId = "page3";
        wizardPage2.enableNextButton = true;
        wizardPage2.enableBackButton = true;
        wizardPage2.enableFinishButton = false;
        wizardPage2.enableCancelButton = true;
        wizard->addPage(wizardPage2);

        WizardPage wizardPage3;
        wizardPage3.pageId = "page3";
        wizardPage3.title = "Confirm Information";
        wizardPage3.description = "Please confirm if your information is correct.";
        wizardPage3.uiInterface = page3;
        wizardPage3.isFinalPage = true;
        wizardPage3.previousPageId = "page2";
        wizardPage3.enableNextButton = false;
        wizardPage3.enableBackButton = true;
        wizardPage3.enableFinishButton = true;
        wizardPage3.enableCancelButton = true;
        wizard->addPage(wizardPage3);

        // 连接向导信号
        connect(wizard, &Wizard::started, this, [=]() {
            statusLabel->setText("向导已开始");
            qDebug() << "Wizard has started";
        });

        connect(wizard, &Wizard::finished, this, [=](Wizard::WizardResult result) {
            QString resultStr;
            switch (result) {
            case Wizard::WizardResult::Finished:
                resultStr = "Wizard completed";
                break;
            case Wizard::WizardResult::Canceled:
                resultStr = "Wizard canceled";
                break;
            case Wizard::WizardResult::Failed:
                resultStr = "Wizard failed";
                break;
            }
            statusLabel->setText(resultStr);
            qDebug() << "Wizard completion status: " << resultStr;
        });

        connect(wizard, &Wizard::pageChanged, this, [=](const WizardPage& previousPage, const WizardPage& currentPage) {
            statusLabel->setText(QString("Page switched: %1 -> %2").arg(previousPage.title).arg(currentPage.title));
            qDebug() << QString("Page switched: %1 -> %2").arg(previousPage.title).arg(currentPage.title);
        });

        connect(wizard, &Wizard::navigationEnabledChanged, this, [=](bool canGoNext, bool canGoPrevious, bool canFinish, bool canCancel) {
            qDebug() << QString("Navigation button status: Next=%1, Previous=%2, Finish=%3, Cancel=%4").arg(canGoNext).arg(canGoPrevious).arg(canFinish).arg(canCancel);
        });

        // 设置页面验证器
        wizard->setPageValidator("page1", [=]() {
            // 验证页面1
            qDebug() << "Validating page 1";
            return true; // 总是通过验证
        });

        wizard->setPageValidator("page2", [=]() {
            // 验证页面2
            qDebug() << "Validating page 2";
            return true; // 总是通过验证
        });
    }

    QLabel* statusLabel;
    UIInterfaceManager* uiManager;
    StateMachineManager* stateMachineManager;
    WizardManager* wizardManager;
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    WizardExample example;
    example.show();
    return app.exec();
}

#include "wizardexample.moc"
