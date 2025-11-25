#ifndef EVENTACTIONEDITOR_H
#define EVENTACTIONEDITOR_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QMenu>
#include <QAction>
#include <QUndoStack>
#include "eventactionmodel.h"

// 流程图节点类型
enum class NodeType {
    EventNode,      // 事件节点
    ActionNode,     // 动作节点
    ConditionNode,  // 条件节点
    StartNode,      // 开始节点
    EndNode         // 结束节点
};

// 流程图节点
class FlowNode : public QGraphicsItem
{
public:
    FlowNode(NodeType type, const QString &title, const QString &description, QGraphicsItem *parent = nullptr);
    
    // QGraphicsItem 接口
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    
    // 节点属性
    NodeType nodeType() const { return m_type; }
    QString title() const { return m_title; }
    QString description() const { return m_description; }
    QString nodeId() const { return m_nodeId; }
    
    void setTitle(const QString &title) { m_title = title; }
    void setDescription(const QString &description) { m_description = description; }
    
    // 连接点管理
    void addConnection(FlowNode *targetNode);
    void removeConnection(FlowNode *targetNode);
    QList<FlowNode*> connections() const { return m_connections; }
    
    // 事件-动作关联
    void setEventDefinition(const EventDefinition &event) { m_event = event; }
    EventDefinition eventDefinition() const { return m_event; }
    void setActionDefinition(const ActionDefinition &action) { m_action = action; }
    ActionDefinition actionDefinition() const { return m_action; }
    
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    
private:
    NodeType m_type;
    QString m_nodeId;
    QString m_title;
    QString m_description;
    QList<FlowNode*> m_connections;
    EventDefinition m_event;
    ActionDefinition m_action;
    QPointF m_dragStartPos;
    
    QColor getNodeColor() const;
    QRectF getNodeRect() const;
};

// 连接线
class ConnectionLine : public QGraphicsItem
{
public:
    ConnectionLine(FlowNode *sourceNode, FlowNode *targetNode, QGraphicsItem *parent = nullptr);
    
    // QGraphicsItem 接口
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    
    FlowNode* sourceNode() const { return m_sourceNode; }
    FlowNode* targetNode() const { return m_targetNode; }
    
    void updatePosition();
    
private:
    FlowNode *m_sourceNode;
    FlowNode *m_targetNode;
    QPointF m_sourcePoint;
    QPointF m_targetPoint;
};

// 事件-动作编辑器主界面
class EventActionEditor : public QWidget
{
    Q_OBJECT

public:
    explicit EventActionEditor(QWidget *parent = nullptr);
    ~EventActionEditor();
    
    // 模型管理
    void setModel(EventActionModel *model);
    EventActionModel* model() const { return m_model; }
    
    // 文件操作
    bool loadFromFile(const QString &filePath);
    bool saveToFile(const QString &filePath);
    
    // 模板应用
    void applyTemplate(const QString &templateName);
    
    // 测试和调试
    void startDebugMode();
    void stopDebugMode();
    void executeRule(const QString &ruleId);
    
public slots:
    void onRuleAdded(const QString &ruleId);
    void onRuleRemoved(const QString &ruleId);
    void onRuleModified(const QString &ruleId);
    
private slots:
    void onAddEventNode();
    void onAddActionNode();
    void onAddConditionNode();
    void onDeleteSelectedNodes();
    void onConnectNodes();
    void onDisconnectNodes();
    void onNodeProperties();
    void onZoomIn();
    void onZoomOut();
    void onFitToView();
    void onValidateRules();
    void onTestRules();
    
private:
    void setupUI();
    void setupToolbar();
    void setupContextMenu();
    void createDefaultNodes();
    void updateScene();
    void clearScene();
    FlowNode* createNode(NodeType type, const QString &title, const QString &description, const QPointF &pos);
    void createConnection(FlowNode *source, FlowNode *target);
    void removeConnection(FlowNode *source, FlowNode *target);
    
    // 辅助函数
    QString getEventTypeString(EventType type);
    QString getActionTypeString(ActionType type);
    
    // 撤销/重做命令
    class AddNodeCommand;
    class RemoveNodeCommand;
    class ConnectNodesCommand;
    class DisconnectNodesCommand;
    
    QGraphicsView *m_graphicsView;
    QGraphicsScene *m_scene;
    EventActionModel *m_model;
    QUndoStack *m_undoStack;
    QMenu *m_contextMenu;
    QAction *m_addEventAction;
    QAction *m_addActionAction;
    QAction *m_addConditionAction;
    QAction *m_deleteAction;
    QAction *m_connectAction;
    QAction *m_disconnectAction;
    QAction *m_propertiesAction;
    
    QMap<QString, FlowNode*> m_ruleNodes;
    QList<ConnectionLine*> m_connections;
    bool m_debugMode;
};

#endif // EVENTACTIONEDITOR_H