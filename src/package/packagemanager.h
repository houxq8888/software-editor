#ifndef PACKAGEMANAGER_H
#define PACKAGEMANAGER_H

#include <QObject>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QProgressBar>
#include <QTextEdit>
#include <QGroupBox>
#include <QCheckBox>
#include <QComboBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>
#include "ipackageservice.h"
#include "product.h"

class PackageManager : public QDialog
{
    Q_OBJECT

public:
    explicit PackageManager(QWidget *parent = nullptr);
    ~PackageManager();
    
    // 设置配置但不开始打包
    void setConfiguration(const Product &product, const PackageConfig &config);
    
    // 开始智能打包
    void startSmartPackage(const Product &product, const PackageConfig &config);
    
    // 开始标准打包
    void startPackage(const PackageConfig &config);
    
    // 获取打包服务接口
    IPackageService *getPackageService() const { return m_packageService; }
    
    // 取消打包
    void cancelPackage();

signals:
    void progressChanged(int progress, const QString &message);
    void packageFinished(bool success, const QString &resultPath);
    void errorOccurred(const QString &error);

private slots:
    void onDetectToolsClicked();
    void onSelectOutputDirClicked();
    void onSelectUILayoutClicked();
    void onPackageClicked();
    void onProgressChanged(int value, const QString &message);
    void onPackageFinished(bool success, const QString &outputPath);
    void onErrorOccurred(const QString &error);

private:
    void setupUI();
    void setupConnections();
    void updateConfigFromUI();
    void updateUIFromConfig();
    void showStatusMessage(const QString &message, bool isError = false);

    // UI组件
    QVBoxLayout *m_mainLayout;
    QGroupBox *m_configGroup;
    QGroupBox *m_toolsGroup;
    QGroupBox *m_packageGroup;
    
    // 配置相关
    QLineEdit *m_nameEdit;
    QLineEdit *m_versionEdit;
    QLineEdit *m_developerEdit;
    QLineEdit *m_outputDirEdit;
    QLineEdit *m_uiLayoutEdit;
    QCheckBox *m_createInstallerCheck;
    
    // 工具路径相关
    QLineEdit *m_qtDirEdit;
    QLineEdit *m_mingwDirEdit;
    QLineEdit *m_cmakeDirEdit;
    QPushButton *m_detectToolsButton;
    
    // 打包控制
    QPushButton *m_packageButton;
    QProgressBar *m_progressBar;
    QTextEdit *m_logTextEdit;
    
    // 逻辑层服务
    IPackageService *m_packageService;
    PackageConfig m_currentConfig;
    
    // 状态管理
    bool m_isPackaging;
    QTimer *m_statusTimer;
    
    // 进度条状态标识
    enum ProgressState {
        Idle,           // 空闲状态
        DetectingTools, // 检测开发工具
        Packaging       // 打包过程
    };
    ProgressState m_currentProgressState;
};

#endif // PACKAGEMANAGER_H