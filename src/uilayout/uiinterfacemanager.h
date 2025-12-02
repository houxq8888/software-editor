#ifndef UIINTERFACEMANAGER_H
#define UIINTERFACEMANAGER_H

#include <QObject>
#include <QList>
#include <QString>
#include <QJsonObject>
#include "uiinterface.h"

// UI界面管理器类，管理多个UI界面
class UIInterfaceManager : public QObject
{
    Q_OBJECT

public:
    explicit UIInterfaceManager(QObject *parent = nullptr);
    ~UIInterfaceManager() override;

    // 界面管理
    QList<UIInterface*> interfaces() const;
    UIInterface* currentInterface() const;
    void setCurrentInterface(UIInterface *interface);
    void setCurrentInterfaceById(const QString &interfaceId);
    
    // 界面操作
    UIInterface* createInterface(const QString &name = "", const QString &description = "");
    bool removeInterface(UIInterface *interface);
    bool removeInterfaceById(const QString &interfaceId);
    UIInterface* findInterface(const QString &interfaceId) const;
    UIInterface* findInterfaceByName(const QString &name) const;
    
    // 界面复制
    UIInterface* cloneInterface(UIInterface *interface, const QString &newName = "");
    
    // 界面重命名
    bool renameInterface(UIInterface *interface, const QString &newName);
    
    // 设置主界面
    void setMainInterface(UIInterface *interface);
    UIInterface* mainInterface() const;
    
    // 序列化和反序列化
    QJsonObject toJson() const;
    bool fromJson(const QJsonObject &json);
    
    // 文件操作
    bool saveToFile(const QString &filePath);
    bool loadFromFile(const QString &filePath);
    
    // 模板管理
    void addTemplate(UIInterface *templateInterface);
    QList<UIInterface*> templates() const;
    UIInterface* createFromTemplate(const QString &templateName, const QString &newName = "");

signals:
    void interfaceAdded(UIInterface *interface);
    void interfaceRemoved(UIInterface *interface);
    void currentInterfaceChanged(UIInterface *interface);
    void interfaceRenamed(UIInterface *interface, const QString &oldName);

private:
    QList<UIInterface*> m_interfaces;
    QList<UIInterface*> m_templates;
    UIInterface *m_currentInterface;
    UIInterface *m_mainInterface;
};

#endif // UIINTERFACEMANAGER_H