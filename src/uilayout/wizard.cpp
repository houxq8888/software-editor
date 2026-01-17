#include "wizard.h"
#include <QUuid>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QDebug>
#include <QTimer>
#include <QWidget>
#include "statemachine.h"
#include "uiinterfacemanager.h"

Wizard::Wizard(QObject *parent) : QObject(parent), 
                                 m_id(QUuid::createUuid().toString()),
                                 m_started(false),
                                 m_completed(false),
                                 m_autoAdvance(false),
                                 m_result(WizardResult::Canceled),
                                 m_stateMachineManager(nullptr) {}

Wizard::~Wizard() {
    qDeleteAll(m_pages);
    m_pages.clear();
    clearAllValidators();
}

QString Wizard::name() const {
    return m_name;
}

void Wizard::setName(const QString &name) {
    m_name = name;
}

QString Wizard::description() const {
    return m_description;
}

void Wizard::setDescription(const QString &description) {
    m_description = description;
}

QString Wizard::id() const {
    return m_id;
}

void Wizard::addPage(const WizardPage &page) {
    WizardPage* newPage = new WizardPage(page);
    m_pages.append(newPage);

    if (page.isStartPage) {
        m_startPageId = page.pageId;
    }

    if (page.isFinalPage) {
        m_finalPageId = page.pageId;
    }
}

bool Wizard::removePage(const QString &pageId) {
    for (int i = 0; i < m_pages.size(); ++i) {
        if (m_pages.at(i)->pageId == pageId) {
            delete m_pages.takeAt(i);
            return true;
        }
    }
    return false;
}

WizardPage* Wizard::findPage(const QString &pageId) const {
    for (WizardPage* page : m_pages) {
        if (page->pageId == pageId) {
            return page;
        }
    }
    return nullptr;
}

WizardPage* Wizard::currentPage() const {
    return findPage(m_currentPageId);
}

QList<WizardPage*> Wizard::allPages() const {
    return m_pages;
}

bool Wizard::start() {
    if (m_started) {
        return false;
    }

    if (m_startPageId.isEmpty()) {
        // 如果没有设置起始页，使用第一个页
        if (!m_pages.isEmpty()) {
            m_startPageId = m_pages.first()->pageId;
        } else {
            return false;
        }
    }

    m_currentPageId = m_startPageId;
    m_started = true;
    m_completed = false;
    m_result = WizardResult::Canceled;
    m_pageHistory.clear();
    m_pageHistory.push(m_currentPageId);

    // 显示当前页面
    WizardPage* currentPage = findPage(m_currentPageId);
    qDebug() << "[DEBUG] Wizard::start() - currentPage: " << currentPage;
    qDebug() << "[DEBUG] Wizard::start() - currentPageId: " << m_currentPageId;
    qDebug() << "[DEBUG] Wizard::start() - total pages: " << m_pages.size();
    
    if (currentPage) {
        qDebug() << "[DEBUG] Wizard::start() - currentPage found: " << currentPage->pageId;
        qDebug() << "[DEBUG] Wizard::start() - currentPage->uiInterface: " << currentPage->uiInterface;
        
        // 只有当 uiInterface 不为空时才尝试显示
        if (currentPage->uiInterface) {
            QWidget* widget = currentPage->uiInterface->widget();
            qDebug() << "[DEBUG] Wizard::start() - widget: " << widget;
            if (widget) {
                qDebug() << "[DEBUG] Wizard::start() - showing widget: " << widget;
                widget->show();
            } else {
                qDebug() << "[ERROR] Wizard::start() - widget is null for uiInterface: " << currentPage->uiInterface;
            }
        } else {
            qDebug() << "[ERROR] Wizard::start() - currentPage has no uiInterface: " << currentPage->pageId;
        }
    } else {
        qDebug() << "[ERROR] Wizard::start() - currentPage is null for pageId: " << m_currentPageId;
    }

    emit started();
    emit navigationEnabledChanged(canGoNext(), canGoPrevious(), canGoNext() && isOnLastPage(), true);
    return true;
}

bool Wizard::goToNextPage() {
    if (!canGoNext()) {
        return false;
    }

    WizardPage* currentPage = findPage(m_currentPageId);
    if (!currentPage) {
        return false;
    }

    // 验证当前页面
    if (!validateCurrentPage()) {
        return false;
    }

    QString nextPageId;
    if (!currentPage->nextPageId.isEmpty()) {
        nextPageId = currentPage->nextPageId;
    } else {
        // 自动查找下一页
        for (WizardPage* page : m_pages) {
            if (page->previousPageId == m_currentPageId) {
                nextPageId = page->pageId;
                break;
            }
        }
    }

    if (nextPageId.isEmpty()) {
        return false;
    }

    return goToPage(nextPageId);
}

bool Wizard::goToPreviousPage() {
    if (!canGoPrevious()) {
        return false;
    }

    // 从历史记录中弹出当前页
    m_pageHistory.pop();
    if (m_pageHistory.isEmpty()) {
        return false;
    }

    // 获取上一页ID
    QString previousPageId = m_pageHistory.top();
    return goToPage(previousPageId);
}

bool Wizard::goToPage(const QString &pageId) {
    WizardPage* newPage = findPage(pageId);
    if (!newPage) {
        return false;
    }

    WizardPage* oldPage = findPage(m_currentPageId);
    if (!oldPage) {
        return false;
    }

    QString oldPageId = m_currentPageId;
    m_currentPageId = pageId;

    // 更新历史记录
    if (m_pageHistory.top() != pageId) {
        m_pageHistory.push(pageId);
    }

    // 隐藏旧页面
    if (oldPage->uiInterface) {
        QWidget* oldWidget = oldPage->uiInterface->widget();
        if (oldWidget) {
            oldWidget->hide();
        }
    }

    // 显示新页面
    if (newPage->uiInterface) {
        QWidget* newWidget = newPage->uiInterface->widget();
        if (newWidget) {
            newWidget->show();
        }
    }

    emit pageChanged(*oldPage, *newPage);
    emit navigationEnabledChanged(canGoNext(), canGoPrevious(), isOnLastPage(), true);

    // 如果是自动前进，且不是最后一页，自动进入下一页
    if (m_autoAdvance && !isOnLastPage()) {
        QTimer::singleShot(100, this, [this]() {
            goToNextPage();
        });
    }

    return true;
}

void Wizard::finish() {
    if (!m_started || m_completed) {
        return;
    }

    // 验证当前页面
    if (!validateCurrentPage()) {
        return;
    }

    m_completed = true;
    m_result = WizardResult::Finished;

    emit finished(m_result);
}

void Wizard::cancel() {
    if (!m_started || m_completed) {
        return;
    }

    m_completed = true;
    m_result = WizardResult::Canceled;

    emit canceled();
    emit finished(m_result);
}

void Wizard::setResult(WizardResult result) {
    m_result = result;
}

Wizard::WizardResult Wizard::result() const {
    return m_result;
}

bool Wizard::isStarted() const {
    return m_started;
}

bool Wizard::isCompleted() const {
    return m_completed;
}

bool Wizard::canGoNext() const {
    WizardPage* currentPage = findPage(m_currentPageId);
    if (!currentPage) {
        return false;
    }

    if (currentPage->isFinalPage) {
        return false;
    }

    if (!currentPage->nextPageId.isEmpty()) {
        return currentPage->enableNextButton;
    }

    // 检查是否有下一页
    for (WizardPage* page : m_pages) {
        if (page->previousPageId == m_currentPageId) {
            return currentPage->enableNextButton;
        }
    }

    return false;
}

bool Wizard::canGoPrevious() const {
    if (m_pageHistory.size() <= 1) {
        return false;
    }

    WizardPage* currentPage = findPage(m_currentPageId);
    if (!currentPage) {
        return false;
    }

    return currentPage->enableBackButton;
}

bool Wizard::isOnFirstPage() const {
    return m_currentPageId == m_startPageId;
}

bool Wizard::isOnLastPage() const {
    return m_currentPageId == m_finalPageId;
}

void Wizard::setAutoAdvance(bool autoAdvance) {
    m_autoAdvance = autoAdvance;
}

bool Wizard::autoAdvance() const {
    return m_autoAdvance;
}

void Wizard::setStateMachineManager(StateMachineManager *manager) {
    m_stateMachineManager = manager;
}

StateMachineManager* Wizard::stateMachineManager() const {
    return m_stateMachineManager;
}

QJsonObject Wizard::toJson() const {
    QJsonObject json;

    json["id"] = m_id;
    json["name"] = m_name;
    json["description"] = m_description;
    json["started"] = m_started;
    json["completed"] = m_completed;
    json["autoAdvance"] = m_autoAdvance;
    json["currentPageId"] = m_currentPageId;
    json["startPageId"] = m_startPageId;
    json["finalPageId"] = m_finalPageId;

    QJsonArray pagesArray;
    for (WizardPage* page : m_pages) {
        QJsonObject pageJson;
        pageJson["pageId"] = page->pageId;
        pageJson["title"] = page->title;
        pageJson["description"] = page->description;
        if (page->uiInterface) {
            pageJson["uiInterfaceId"] = page->uiInterface->id();
        }
        pageJson["isStartPage"] = page->isStartPage;
        pageJson["isFinalPage"] = page->isFinalPage;
        pageJson["nextPageId"] = page->nextPageId;
        pageJson["previousPageId"] = page->previousPageId;
        pageJson["completionAction"] = page->completionAction;
        pageJson["cancelAction"] = page->cancelAction;
        pageJson["enableNextButton"] = page->enableNextButton;
        pageJson["enableBackButton"] = page->enableBackButton;
        pageJson["enableFinishButton"] = page->enableFinishButton;
        pageJson["enableCancelButton"] = page->enableCancelButton;
        pagesArray.append(pageJson);
    }
    json["pages"] = pagesArray;

    return json;
}

bool Wizard::fromJson(const QJsonObject &json) {
    if (json.isEmpty()) {
        return false;
    }

    m_id = json["id"].toString();
    m_name = json["name"].toString();
    m_description = json["description"].toString();
    m_started = json["started"].toBool();
    m_completed = json["completed"].toBool();
    m_autoAdvance = json["autoAdvance"].toBool();
    m_currentPageId = json["currentPageId"].toString();
    m_startPageId = json["startPageId"].toString();
    m_finalPageId = json["finalPageId"].toString();

    // 清空现有页面
    qDeleteAll(m_pages);
    m_pages.clear();

    QJsonArray pagesArray = json["pages"].toArray();
    for (const QJsonValue &pageValue : pagesArray) {
        QJsonObject pageJson = pageValue.toObject();
        WizardPage page;

        page.pageId = pageJson["pageId"].toString();
        page.title = pageJson["title"].toString();
        page.description = pageJson["description"].toString();
        page.isStartPage = pageJson["isStartPage"].toBool();
        page.isFinalPage = pageJson["isFinalPage"].toBool();
        page.nextPageId = pageJson["nextPageId"].toString();
        page.previousPageId = pageJson["previousPageId"].toString();
        page.completionAction = pageJson["completionAction"].toString();
        page.cancelAction = pageJson["cancelAction"].toString();
        page.enableNextButton = pageJson["enableNextButton"].toBool();
        page.enableBackButton = pageJson["enableBackButton"].toBool();
        page.enableFinishButton = pageJson["enableFinishButton"].toBool();
        page.enableCancelButton = pageJson["enableCancelButton"].toBool();

        // 查找UI界面
        QString uiInterfaceId = pageJson["uiInterfaceId"].toString();
        if (!uiInterfaceId.isEmpty() && m_stateMachineManager) {
            UIInterfaceManager* uiManager = m_stateMachineManager->uiInterfaceManager();
            if (uiManager) {
                page.uiInterface = uiManager->findInterface(uiInterfaceId);
            }
        }

        addPage(page);
    }

    return true;
}

bool Wizard::validateCurrentPage() {
    WizardPage* currentPage = findPage(m_currentPageId);
    if (!currentPage) {
        return true;
    }

    // 查找当前页面的验证器
    auto validator = m_pageValidators.find(m_currentPageId);
    if (validator != m_pageValidators.end()) {
        return validator.value()();
    }

    return true;
}

void Wizard::setPageValidator(const QString &pageId, const std::function<bool()> &validator) {
    m_pageValidators[pageId] = validator;
}

void Wizard::clearAllValidators() {
    m_pageValidators.clear();
}

// --------------------- WizardManager --------------------- //

WizardManager::WizardManager(QObject *parent) : QObject(parent), 
                                               m_currentWizard(nullptr) {}

WizardManager::~WizardManager() {
    qDeleteAll(m_wizards);
    m_wizards.clear();
}

QList<Wizard*> WizardManager::wizards() const {
    return m_wizards;
}

Wizard* WizardManager::createWizard(const QString &name, const QString &description) {
    Wizard* newWizard = new Wizard(this);
    newWizard->setName(name);
    newWizard->setDescription(description);
    m_wizards.append(newWizard);
    emit wizardAdded(newWizard);
    return newWizard;
}

bool WizardManager::removeWizard(Wizard *wizard) {
    if (m_wizards.contains(wizard)) {
        m_wizards.removeOne(wizard);
        if (m_currentWizard == wizard) {
            m_currentWizard = nullptr;
        }
        emit wizardRemoved(wizard);
        wizard->deleteLater();
        return true;
    }
    return false;
}

Wizard* WizardManager::findWizard(const QString &wizardId) const {
    for (Wizard* wizard : m_wizards) {
        if (wizard->id() == wizardId || wizard->name() == wizardId) {
            return wizard;
        }
    }
    return nullptr;
}

Wizard* WizardManager::currentWizard() const {
    return m_currentWizard;
}

void WizardManager::setCurrentWizard(Wizard *wizard) {
    if (m_currentWizard != wizard && m_wizards.contains(wizard)) {
        m_currentWizard = wizard;
        emit currentWizardChanged(wizard);
    }
}

QJsonObject WizardManager::toJson() const {
    QJsonObject json;
    QJsonArray wizardsArray;

    for (Wizard* wizard : m_wizards) {
        wizardsArray.append(wizard->toJson());
    }

    json["wizards"] = wizardsArray;
    if (m_currentWizard) {
        json["currentWizardId"] = m_currentWizard->id();
    }

    return json;
}

bool WizardManager::fromJson(const QJsonObject &json) {
    if (json.isEmpty()) {
        return false;
    }

    // 清空现有向导
    qDeleteAll(m_wizards);
    m_wizards.clear();
    m_currentWizard = nullptr;

    QJsonArray wizardsArray = json["wizards"].toArray();
    for (const QJsonValue &wizardValue : wizardsArray) {
        QJsonObject wizardJson = wizardValue.toObject();
        Wizard* newWizard = new Wizard(this);
        if (newWizard->fromJson(wizardJson)) {
            m_wizards.append(newWizard);
        } else {
            delete newWizard;
        }
    }

    // 恢复当前向导
    QString currentWizardId = json["currentWizardId"].toString();
    if (!currentWizardId.isEmpty()) {
        m_currentWizard = findWizard(currentWizardId);
    }

    return true;
}

bool WizardManager::saveToFile(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QJsonObject json = toJson();
    QJsonDocument doc(json);
    file.write(doc.toJson());
    file.close();

    return true;
}

bool WizardManager::loadFromFile(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        return false;
    }

    return fromJson(doc.object());
}
