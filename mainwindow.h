#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QStandardItem>
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

    void on_iconBrowseButton_clicked();
    void on_screenshotBrowseButton_clicked();

    void on_addFeatureButton_clicked();
    void on_removeFeatureButton_clicked();
    void on_featuresListView_clicked(const QModelIndex &index);

    void updateFeatureEditors();

private:
    Ui::MainWindow *ui;
    Product m_product;
    QString m_currentFile;
    bool m_isModified;
    QStandardItemModel *m_featuresModel;

    void clearProductData();
    void loadProductData(const Product &product);
    void saveProductData();
    void setModified(bool modified);
    bool saveChanges();
    void updateFeaturesList();
    ProductFeature getCurrentFeatureFromEditors() const;
};

#endif // MAINWINDOW_H