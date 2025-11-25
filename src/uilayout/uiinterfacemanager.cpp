#include "uiinterfacemanager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>

UIInterfaceManager::UIInterfaceManager(QObject *parent)
    : QObject(parent)
    , m_currentInterface(nullptr)
    , m_mainInterface(nullptr)
{
    // 创建默认的主界面
    UIInterface *mainInterface = createInterface("主界面", "应用程序的主界面");
    mainInterface->setIsMainWindow(true);
    setMainInterface(mainInterface);
    setCurrentInterface(mainInterface);
}

UIInterfaceManager::~UIInterfaceManager()
{
    qDeleteAll(m_interfaces);
    qDeleteAll(m_templates);
}

QList<UIInterface*> UIInterfaceManager::interfaces() const
{
    return m_interfaces;
}

UIInterface* UIInterfaceManager::currentInterface() const
{
    return m_currentInterface;
}

void UIInterfaceManager::setCurrentInterface(UIInterface *interface)
{
    if (m_currentInterface != interface && m_interfaces.contains(interface)) {
        UIInterface *oldInterface = m_currentInterface;
        m_currentInterface = interface;
        emit currentInterfaceChanged(m_currentInterface);
    }
}

void UIInterfaceManager::setCurrentInterfaceById(const QString &interfaceId)
{
    UIInterface *interface = findInterface(interfaceId);
    if (interface) {
        setCurrentInterface(interface);
    }
}

UIInterface* UIInterfaceManager::createInterface(const QString &name, const QString &description)
{
    QString interfaceName = name.isEmpty() ? QString("界面%1").arg(m_interfaces.size() + 1) : name;
    UIInterface *interface = new UIInterface(interfaceName, description);
    m_interfaces.append(interface);
    
    emit interfaceAdded(interface);
    
    // 如果没有当前界面，设置为当前界面
    if (!m_currentInterface) {
        setCurrentInterface(interface);
    }
    
    return interface;
}

bool UIInterfaceManager::removeInterface(UIInterface *interface)
{
    if (interface && m_interfaces.contains(interface)) {
        // 不能删除主界面
        if (interface == m_mainInterface) {
            qWarning() << "Cannot remove main interface";
            return false;
        }
        
        // 如果删除的是当前界面，需要切换到其他界面
        if (interface == m_currentInterface) {
            if (m_interfaces.size() > 1) {
                // 切换到主界面或其他界面
                UIInterface *newCurrent = (m_mainInterface && m_mainInterface != interface) ? 
                                         m_mainInterface : m_interfaces.first();
                setCurrentInterface(newCurrent);
            } else {
                m_currentInterface = nullptr;
            }
        }
        
        m_interfaces.removeOne(interface);
        emit interfaceRemoved(interface);
        delete interface;
        return true;
    }
    return false;
}

bool UIInterfaceManager::removeInterfaceById(const QString &interfaceId)
{
    UIInterface *interface = findInterface(interfaceId);
    return removeInterface(interface);
}

UIInterface* UIInterfaceManager::findInterface(const QString &interfaceId) const
{
    for (UIInterface *interface : m_interfaces) {
        if (interface->id() == interfaceId) {
            return interface;
        }
    }
    return nullptr;
}

UIInterface* UIInterfaceManager::findInterfaceByName(const QString &name) const
{
    for (UIInterface *interface : m_interfaces) {
        if (interface->name() == name) {
            return interface;
        }
    }
    return nullptr;
}

UIInterface* UIInterfaceManager::cloneInterface(UIInterface *interface, const QString &newName)
{
    if (!interface) return nullptr;
    
    QString cloneName = newName.isEmpty() ? interface->name() + "_副本" : newName;
    UIInterface *clonedInterface = interface->clone();
    clonedInterface->setName(cloneName);
    m_interfaces.append(clonedInterface);
    
    emit interfaceAdded(clonedInterface);
    return clonedInterface;
}

bool UIInterfaceManager::renameInterface(UIInterface *interface, const QString &newName)
{
    if (!interface || newName.isEmpty()) return false;
    
    QString oldName = interface->name();
    interface->setName(newName);
    emit interfaceRenamed(interface, oldName);
    return true;
}

void UIInterfaceManager::setMainInterface(UIInterface *interface)
{
    if (interface && m_interfaces.contains(interface)) {
        // 取消之前的主界面标记
        if (m_mainInterface) {
            m_mainInterface->setIsMainWindow(false);
        }
        
        m_mainInterface = interface;
        m_mainInterface->setIsMainWindow(true);
    }
}

UIInterface* UIInterfaceManager::mainInterface() const
{
    return m_mainInterface;
}

QJsonObject UIInterfaceManager::toJson() const
{
    QJsonObject json;
    
    // 序列化界面列表
    QJsonArray interfacesArray;
    for (UIInterface *interface : m_interfaces) {
        interfacesArray.append(interface->toJson());
    }
    json["interfaces"] = interfacesArray;
    
    // 序列化当前界面ID
    if (m_currentInterface) {
        json["currentInterfaceId"] = m_currentInterface->id();
    }
    
    // 序列化主界面ID
    if (m_mainInterface) {
        json["mainInterfaceId"] = m_mainInterface->id();
    }
    
    return json;
}

bool UIInterfaceManager::fromJson(const QJsonObject &json)
{
    if (json.isEmpty()) return false;
    
    // 清除现有数据
    qDeleteAll(m_interfaces);
    m_interfaces.clear();
    m_currentInterface = nullptr;
    m_mainInterface = nullptr;
    
    // 加载界面列表
    QJsonArray interfacesArray = json["interfaces"].toArray();
    for (const QJsonValue &interfaceValue : interfacesArray) {
        QJsonObject interfaceJson = interfaceValue.toObject();
        UIInterface *interface = new UIInterface();
        if (interface->fromJson(interfaceJson)) {
            m_interfaces.append(interface);
        } else {
            delete interface;
        }
    }
    
    // 设置当前界面
    QString currentInterfaceId = json["currentInterfaceId"].toString();
    if (!currentInterfaceId.isEmpty()) {
        setCurrentInterfaceById(currentInterfaceId);
    }
    
    // 设置主界面
    QString mainInterfaceId = json["mainInterfaceId"].toString();
    if (!mainInterfaceId.isEmpty()) {
        UIInterface *mainInterface = findInterface(mainInterfaceId);
        if (mainInterface) {
            setMainInterface(mainInterface);
        }
    }
    
    // 如果没有界面，创建默认界面
    if (m_interfaces.isEmpty()) {
        createInterface("主界面", "应用程序的主界面");
    }
    
    // 如果没有当前界面，设置为第一个界面
    if (!m_currentInterface && !m_interfaces.isEmpty()) {
        setCurrentInterface(m_interfaces.first());
    }
    
    return true;
}

bool UIInterfaceManager::saveToFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open file for writing:" << filePath;
        return false;
    }
    
    QJsonDocument doc(toJson());
    file.write(doc.toJson());
    file.close();
    
    return true;
}

bool UIInterfaceManager::loadFromFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open file for reading:" << filePath;
        return false;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) {
        qWarning() << "Failed to parse JSON from file:" << filePath;
        return false;
    }
    
    return fromJson(doc.object());
}

void UIInterfaceManager::addTemplate(UIInterface *templateInterface)
{
    if (templateInterface && !m_templates.contains(templateInterface)) {
        m_templates.append(templateInterface);
    }
}

QList<UIInterface*> UIInterfaceManager::templates() const
{
    return m_templates;
}

UIInterface* UIInterfaceManager::createFromTemplate(const QString &templateName, const QString &newName)
{
    for (UIInterface *templateInterface : m_templates) {
        if (templateInterface->name() == templateName) {
            return cloneInterface(templateInterface, newName);
        }
    }
    return nullptr;
}