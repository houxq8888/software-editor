#ifndef DESIGNERWINDOW_H
#define DESIGNERWINDOW_H

#include <QMainWindow>
#include <QtDesigner/qdesigner_components.h>
#include <QtDesigner/QDesignerFormEditorInterface>

class DesignerWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit DesignerWindow(QWidget *parent = nullptr);
    ~DesignerWindow() override;

private:
    QDesignerFormEditorInterface *m_formEditor = nullptr;
};

#endif // DESIGNERWINDOW_H