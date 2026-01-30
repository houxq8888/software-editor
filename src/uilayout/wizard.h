#ifndef WIZARD_H
#define WIZARD_H

#include <QObject>
#include <QString>
#include <QList>
#include <QStack>
#include <QMap>
#include "uiinterface.h"

// 前向声明
class StateMachineManager;

class WizardPage {
public:
    QString pageId;
    QString title;
    QString description;
    UIInterface* uiInterface;
    QString filePath;
    bool isStartPage;
    bool isFinalPage;
    QString nextPageId;
    QString previousPageId;
    QString completionAction;
    QString cancelAction;
    bool enableNextButton;
    bool enableBackButton;
    bool enableFinishButton;
    bool enableCancelButton;
    QMap<QString, QString> buttonTransitions;

    WizardPage() : uiInterface(nullptr), isStartPage(false), isFinalPage(false), 
                 enableNextButton(true), enableBackButton(true), 
                 enableFinishButton(false), enableCancelButton(true) {}
};

class Wizard : public QObject
{
    Q_OBJECT

public:
    explicit Wizard(QObject *parent = nullptr);
    ~Wizard() override;

    enum class WizardResult {
        Canceled,
        Finished,
        Failed
    };

    // 基本信息
    QString name() const;
    void setName(const QString &name);

    QString description() const;
    void setDescription(const QString &description);

    QString id() const;

    // 页面管理
    void addPage(const WizardPage &page);
    bool removePage(const QString &pageId);
    WizardPage* findPage(const QString &pageId) const;
    WizardPage* currentPage() const;
    QList<WizardPage*> allPages() const;

    // 向导控制
    bool start();
    bool goToNextPage();
    bool goToPreviousPage();
    bool goToPage(const QString &pageId);
    void finish();
    void cancel();
    void setResult(WizardResult result);
    WizardResult result() const;

    // 状态检查
    bool isStarted() const;
    bool isCompleted() const;
    bool canGoNext() const;
    bool canGoPrevious() const;
    bool isOnFirstPage() const;
    bool isOnLastPage() const;

    // 向导配置
    void setAutoAdvance(bool autoAdvance);
    bool autoAdvance() const;

    // 与状态机集成
    void setStateMachineManager(StateMachineManager *manager);
    StateMachineManager* stateMachineManager() const;

    // 序列化和反序列化
    QJsonObject toJson() const;
    bool fromJson(const QJsonObject &json);

    // 页面验证
    bool validateCurrentPage();
    void setPageValidator(const QString &pageId, const std::function<bool()> &validator);
    void clearAllValidators();

signals:
    void started();
    void finished(WizardResult result);
    void canceled();
    void pageChanged(const WizardPage &previousPage, const WizardPage &currentPage);
    void nextButtonClicked();
    void previousButtonClicked();
    void finishButtonClicked();
    void cancelButtonClicked();
    void navigationEnabledChanged(bool canGoNext, bool canGoPrevious, bool canFinish, bool canCancel);

private:
    QString m_id;
    QString m_name;
    QString m_description;
    QList<WizardPage*> m_pages;
    QString m_currentPageId;
    QString m_startPageId;
    QString m_finalPageId;
    bool m_started;
    bool m_completed;
    bool m_autoAdvance;
    WizardResult m_result;
    StateMachineManager *m_stateMachineManager;
    QMap<QString, std::function<bool()>> m_pageValidators;
    QStack<QString> m_pageHistory;
};

class WizardManager : public QObject
{
    Q_OBJECT

public:
    explicit WizardManager(QObject *parent = nullptr);
    ~WizardManager() override;

    // 向导管理
    QList<Wizard*> wizards() const;
    Wizard* createWizard(const QString &name, const QString &description);
    bool removeWizard(Wizard *wizard);
    Wizard* findWizard(const QString &wizardId) const;

    // 当前向导
    Wizard* currentWizard() const;
    void setCurrentWizard(Wizard *wizard);

    // 序列化和反序列化
    QJsonObject toJson() const;
    bool fromJson(const QJsonObject &json);

    // 文件操作
    bool saveToFile(const QString &filePath);
    bool loadFromFile(const QString &filePath);

signals:
    void wizardAdded(Wizard *wizard);
    void wizardRemoved(Wizard *wizard);
    void currentWizardChanged(Wizard *wizard);

private:
    QList<Wizard*> m_wizards;
    Wizard *m_currentWizard;
};

#endif // WIZARD_H