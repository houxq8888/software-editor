#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include "product.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionNew_triggered();
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionSave_As_triggered();
    void on_actionExit_triggered();
    void on_actionAbout_triggered();
    void on_actionOpen_UI_Layout_Editor_triggered();
    void on_iconBrowseButton_clicked();
    void on_screenshotBrowseButton_clicked();
    void on_addFeatureButton_clicked();
    void on_saveButton_clicked();
    void on_loadButton_clicked();
    void on_removeFeatureButton_clicked();
    void on_featuresListWidget_itemClicked(QListWidgetItem *item);

    void updateFeatureEditors();

private:
    Ui::MainWindow *ui;
    Product m_product;
    QString m_currentFile;
    bool m_isModified;
    QListWidget *m_featuresListWidget;

    void clearProductData();
    void loadProductData(const Product &product);
    void saveProductData();
    void setModified(bool modified);
    bool saveChanges();
    void updateFeaturesList();
    ProductFeature getCurrentFeatureFromEditors() const;
};

#endif // MAINWINDOW_H