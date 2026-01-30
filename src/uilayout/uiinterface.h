#ifndef UIINTERFACE_H
#define UIINTERFACE_H

#include <QObject>
#include <QString>
#include <QList>
#include <QJsonObject>
#include "layoutitem.h"

// UI界面类，表示一个完整的UI界面
class UIInterface : public QObject
{
    Q_OBJECT

public:
    explicit UIInterface(const QString &name = "", const QString &description = "", QObject *parent = nullptr);
    ~UIInterface() override = default;

    // 基本信息
    QString name() const;
    void setName(const QString &name);
    
    QString description() const;
    void setDescription(const QString &description);
    
    QString id() const;
    
    // 布局项管理
    QList<LayoutItem*> layoutItems() const;
    void addLayoutItem(LayoutItem *item);
    void removeLayoutItem(LayoutItem *item);
    void clearLayoutItems();
    
    // 查找布局项
    LayoutItem* findLayoutItem(const QString &widgetId) const;
    
    // 序列化和反序列化
    QJsonObject toJson() const;
    bool fromJson(const QJsonObject &json);
    
    // 复制功能
    UIInterface* clone() const;
    
    // 界面属性
    QSize size() const;
    void setSize(const QSize &size);
    
    QString title() const;
    void setTitle(const QString &title);
    
    bool isMainWindow() const;
    void setIsMainWindow(bool isMain);

    // 事件处理
    void triggerButtonClick(const QString &buttonName);
    void triggerMenuAction(const QString &actionName);
    
    // 获取界面控件信息
    QList<QString> getButtonNames() const;
    QList<QString> getMenuActionNames() const;
    
    // 界面控件创建
    QWidget* widget();
    
    // 从UI文件加载内容
    bool loadFromFile(const QString &filePath);

signals:
    void buttonClicked(const QString &buttonName);
    void menuActionTriggered(const QString &actionName);
    void interfaceActivated();
    void interfaceDeactivated();

private:
    QString m_id;
    QString m_name;
    QString m_description;
    QString m_title;
    QSize m_size;
    bool m_isMainWindow;
    QList<LayoutItem*> m_layoutItems;
};

#endif // UIINTERFACE_H