#ifndef PACKAGEDIALOG_H
#define PACKAGEDIALOG_H

#include <QDialog>
#include <QProgressBar>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QTextEdit>
#include <QFileDialog>
#include "packageconfig.h"
#include "packagemanager.h"
#include "product.h"

class PackageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PackageDialog(QWidget *parent = nullptr);
    ~PackageDialog();
    
    // 设置产品信息
    void setProductInfo(const QJsonObject &productInfo);
    void setProductInfo(const Product &product);
    
    // 获取打包配置
    PackageConfig::PackageSettings getPackageSettings() const;

public slots:
    void updateProgress(int progress, const QString &message);
    void packageFinished();

signals:
    void packageRequested(const Product &product, const PackageConfig::PackageSettings &settings);

private slots:
    void onBrowseOutputDir();
    void onBrowseIcon();
    void onStartPackage();
    void onCancelPackage();
    void onPackageProgress(int progress, const QString &message);
    void onPackageFinished(bool success, const QString &resultPath);
    void onPackageError(const QString &error);

private:
    void setupUI();
    void setupConnections();
    void updateUIState(bool packaging);
    
    // UI组件
    QLineEdit *m_nameEdit;
    QLineEdit *m_versionEdit;
    QLineEdit *m_developerEdit;
    QTextEdit *m_descriptionEdit;
    QLineEdit *m_iconPathEdit;
    QLineEdit *m_outputDirEdit;
    QCheckBox *m_createInstallerCheck;
    QCheckBox *m_includeDepsCheck;
    
    QProgressBar *m_progressBar;
    QLabel *m_statusLabel;
    QPushButton *m_startButton;
    QPushButton *m_cancelButton;
    QPushButton *m_closeButton;
    
    PackageManager *m_packageManager;
    PackageConfig *m_packageConfig;
    Product m_product; // 存储当前产品信息
};

#endif // PACKAGEDIALOG_H