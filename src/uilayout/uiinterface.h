#ifndef UIINTERFACE_H
#define UIINTERFACE_H

#include <QString>
#include <QWidget>
#include <QList>
#include <QJsonObject>

class UIInterface : public QObject
{
    Q_OBJECT
public:
    explicit UIInterface(const QString &name, const QString &description = QString(), QObject *parent = nullptr);
    UIInterface(); // 默认构造函数
    ~UIInterface();

    QString id() const { return m_id; }
    void setId(const QString &id) { m_id = id; }
    
    QString name() const { return m_name; }
    QString description() const { return m_description; }
    
    void setName(const QString &name) { m_name = name; }
    void setDescription(const QString &description) { m_description = description; }
    
    // 创建UI界面的Widget
    QWidget* widget();
    
    // 控件事件处理
    void handleControlEvent(const QString &controlName, const QString &eventType);
    
    // UI跳转逻辑
    void setNextUI(const QString &nextUIName) { m_nextUIName = nextUIName; }
    QString nextUI() const { return m_nextUIName; }
    
    // 判断是否有跳转逻辑
    bool hasJumpLogic() const { return !m_nextUIName.isEmpty(); }
    
    // 是否为主窗口
    bool isMainWindow() const { return m_isMainWindow; }
    void setIsMainWindow(bool isMain) { m_isMainWindow = isMain; }
    
    // 克隆和序列化
    UIInterface* clone() const;
    QJsonObject toJson() const;
    bool fromJson(const QJsonObject &json);

signals:
    void jumpToUI(const QString &uiName);

private:
    QString m_id; // 唯一标识符
    QString m_name;
    QString m_description;
    QString m_nextUIName; // 下一个要跳转到的UI名称
    QWidget* m_widget; // 缓存的Widget对象
    bool m_isMainWindow; // 是否为主窗口
};

#endif // UIINTERFACE_H