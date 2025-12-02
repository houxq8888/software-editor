#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QTimer>
#include <QDateTime>
#include "product.h"
#include "packagemanager.h"
#include "productconfigmanager.h"

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
    void on_actionSmart_Package_Software_triggered();
    void on_actionOpen_UI_Layout_Editor_triggered();
    
    // UI layout window management
    void onUILayoutWindowClosed();
    void on_iconBrowseButton_clicked();
    void on_screenshotBrowseButton_clicked();
    void on_addFeatureButton_clicked();
    void on_saveButton_clicked();
    void on_loadButton_clicked();
    void on_removeFeatureButton_clicked();
    void on_featuresListWidget_itemClicked(QListWidgetItem *item);

    void updateFeatureEditors();
    void onPackageProgress(int progress, const QString &message);
    void onPackageFinished(bool success, const QString &message);
    void onPackageError(const QString &error);

private:
    Ui::MainWindow *ui;
    Product m_product;
    QString m_currentFile;
    PackageManager *m_packageManager;
    ProductConfigManager *m_configManager;
    
    // 日志捕获相关成员
    QStringList m_logMessages;
    QTimer *m_logTimer;
    static const int MAX_LOG_MESSAGES = 10; // 最大显示日志数量

    void clearProductData();
    void loadProductData(const Product &product);
    void saveProductData();
    void setModified(bool modified);
    bool saveChanges();
    void updateFeaturesList();
    ProductFeature getCurrentFeatureFromEditors() const;
    void updateStatusBar();
    
    // 窗口关闭事件处理
    void closeEvent(QCloseEvent *event) override;
    
    // 检查基本信息TAB页是否被修改
    bool isBasicInfoModified() const;
    
    // 检查功能特性TAB页是否被修改
    bool isFeaturesTabModified() const;
    
    // 日志捕获相关函数
    void setupLogCapture();
    void addLogMessage(const QString &message);
    void updateStatusBarWithLogs();
    void cleanupLogCapture();
    static void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);
};

#endif // MAINWINDOW_H