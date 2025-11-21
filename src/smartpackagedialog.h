#ifndef SMARTPACKAGEDIALOG_H
#define SMARTPACKAGEDIALOG_H

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
#include <QRadioButton>
#include <QButtonGroup>
#include "packageconfig.h"
#include "smartpackageconfig.h"
#include "packagemanager.h"
#include "product.h"

class SmartPackageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SmartPackageDialog(QWidget *parent = nullptr);
    ~SmartPackageDialog();
    
    // 设置产品信息
    void setProductInfo(const QJsonObject &productInfo);
    void setProductInfo(const Product &product);
    
    // 获取打包配置
    SmartPackageConfig::SmartPackageSettings getSmartPackageSettings() const;

public slots:
    void updateProgress(int progress, const QString &message);
    void packageFinished();

signals:
    void smartPackageRequested(const Product &product, const SmartPackageConfig::SmartPackageSettings &settings);

private slots:
    void onBrowseOutputDir();
    void onBrowseIcon();
    void onBrowseQtPath();
    void onBrowseMingwPath();
    void onBrowseCmakePath();
    void onStartPackage();
    void onCancelPackage();
    void onPackageProgress(int progress, const QString &message);
    void onPackageFinished(bool success, const QString &resultPath);
    void onPackageError(const QString &error);
    void onDetectTools();
    void onPackageModeChanged();

private:
    void setupUI();
    void setupConnections();
    void updateUIState(bool packaging);
    void updateSmartPackageInfo();
    bool validateManualPaths();
    
    // UI组件
    QLineEdit *m_nameEdit;
    QLineEdit *m_versionEdit;
    QLineEdit *m_developerEdit;
    QTextEdit *m_descriptionEdit;
    QLineEdit *m_iconPathEdit;
    QLineEdit *m_outputDirEdit;
    QCheckBox *m_createInstallerCheck;
    QCheckBox *m_includeDepsCheck;
    
    // 智能打包特有组件
    QRadioButton *m_normalPackageRadio;
    QRadioButton *m_smartPackageRadio;
    QGroupBox *m_smartPackageGroup;
    QLineEdit *m_qtPathEdit;
    QLineEdit *m_mingwPathEdit;
    QLineEdit *m_cmakePathEdit;
    QLabel *m_qtPathLabel;
    QLabel *m_mingwPathLabel;
    QLabel *m_cmakePathLabel;
    QPushButton *m_detectToolsButton;
    QLabel *m_smartPackageInfoLabel;
    
    QProgressBar *m_progressBar;
    QLabel *m_statusLabel;
    QPushButton *m_startButton;
    QPushButton *m_cancelButton;
    QPushButton *m_closeButton;
    
    PackageManager *m_packageManager;
    SmartPackageConfig *m_smartPackageConfig;
    Product m_product; // 存储当前产品信息
};

#endif // SMARTPACKAGEDIALOG_H