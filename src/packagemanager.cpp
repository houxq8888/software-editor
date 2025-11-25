#include "packagemanager.h"
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QDebug>
#include <QStandardPaths>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNodeList>
#include <QDateTime>
#include <QTextStream>
#include <QRegularExpression>
#include <QCoreApplication>

PackageWorker::PackageWorker(QObject *parent)
    : QObject(parent)
{
}

QString PackageWorker::generateCppCodeFromUILayout(const QString &uiLayoutPath, const SmartPackageConfig::SmartPackageSettings &settings)
{
    // 读取UI布局XML文件
    QFile layoutFile(uiLayoutPath);
    if (!layoutFile.open(QIODevice::ReadOnly)) {
        throw QString("无法打开UI布局文件: %1").arg(uiLayoutPath);
    }
    
    // 创建XML文档对象
    QDomDocument doc;
    QString errorMsg;
    int errorLine, errorColumn;
    
    if (!doc.setContent(&layoutFile, &errorMsg, &errorLine, &errorColumn)) {
        layoutFile.close();
        throw QString("UI布局文件XML格式错误: %1 (行%2, 列%3)").arg(errorMsg).arg(errorLine).arg(errorColumn);
    }
    
    layoutFile.close();
    
    // 获取根元素
    QDomElement root = doc.documentElement();
    if (root.isNull()) {
        throw QString("UI布局文件格式错误: 缺少根元素");
    }
    
    // 生成C++代码
    QStringList codeLines;
    
    // 头文件包含
    codeLines << "#include <QApplication>";
    codeLines << "#include <QMainWindow>";
    codeLines << "#include <QVBoxLayout>";
    codeLines << "#include <QHBoxLayout>";
    codeLines << "#include <QLabel>";
    codeLines << "#include <QPushButton>";
    codeLines << "#include <QLineEdit>";
    codeLines << "#include <QTabWidget>";
    codeLines << "#include <QTextEdit>";
    codeLines << "#include <QMessageBox>";
    codeLines << "";
    
    // 主窗口类定义
    codeLines << QString("class %1Window : public QMainWindow").arg(settings.name);
    codeLines << "{";
    codeLines << "    Q_OBJECT";
    codeLines << "";
    codeLines << "public:";
    codeLines << QString("    %1Window(QWidget *parent = nullptr) : QMainWindow(parent)").arg(settings.name);
    codeLines << "    {";
    codeLines << "        setupUI();";
    codeLines << "        setWindowTitle(\"" + settings.name + "\");";
    codeLines << "        resize(800, 600);";
    codeLines << "    }";
    codeLines << "";
    codeLines << "private:";
    codeLines << "    void setupUI();";
    codeLines << "";
    codeLines << "    // UI组件声明";
    
    // 根据UI布局生成组件声明
    QDomNodeList widgetNodes = root.elementsByTagName("widget");
    for (int i = 0; i < widgetNodes.count(); i++) {
        QDomElement widget = widgetNodes.at(i).toElement();
        if (widget.isNull()) continue;
        
        QString className = widget.attribute("class");
        QString name = widget.attribute("name");
        
        if (className == "QLabel") {
            codeLines << QString("    QLabel *%1;").arg(name);
        } else if (className == "QPushButton") {
            codeLines << QString("    QPushButton *%1;").arg(name);
        } else if (className == "QLineEdit") {
            codeLines << QString("    QLineEdit *%1;").arg(name);
        } else if (className == "QTabWidget") {
            codeLines << QString("    QTabWidget *%1;").arg(name);
        } else if (className == "QTextEdit") {
            codeLines << QString("    QTextEdit *%1;").arg(name);
        }
    }
    
    codeLines << "};";
    codeLines << "";
    
    // UI设置函数实现
    codeLines << QString("void %1Window::setupUI()").arg(settings.name);
    codeLines << "{";
    codeLines << "    QWidget *centralWidget = new QWidget(this);";
    codeLines << "    setCentralWidget(centralWidget);";
    codeLines << "";
    codeLines << "    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);";
    codeLines << "";
    
    // 根据UI布局生成组件创建和布局代码
    for (int i = 0; i < widgetNodes.count(); i++) {
        QDomElement widget = widgetNodes.at(i).toElement();
        if (widget.isNull()) continue;
        
        QString className = widget.attribute("class");
        QString name = widget.attribute("name");
        
        // 获取widget的文本内容
        QString text;
        QDomElement propertyElement = widget.firstChildElement("property");
        while (!propertyElement.isNull()) {
            if (propertyElement.attribute("name") == "text") {
                QDomElement stringElement = propertyElement.firstChildElement("string");
                if (!stringElement.isNull()) {
                    text = stringElement.text();
                }
                break;
            }
            propertyElement = propertyElement.nextSiblingElement("property");
        }
        
        codeLines << QString("    // %1").arg(name);
        
        if (className == "QLabel") {
            codeLines << QString("    %1 = new QLabel(\"%2\", this);").arg(name, text);
        } else if (className == "QPushButton") {
            codeLines << QString("    %1 = new QPushButton(\"%2\", this);").arg(name, text);
        } else if (className == "QLineEdit") {
            codeLines << QString("    %1 = new QLineEdit(this);").arg(name);
            if (!text.isEmpty()) {
                codeLines << QString("    %1->setPlaceholderText(\"%2\");").arg(name, text);
            }
        } else if (className == "QTabWidget") {
            codeLines << QString("    %1 = new QTabWidget(this);").arg(name);
            // 处理tab页
            QDomNodeList tabNodes = widget.elementsByTagName("widget");
            for (int j = 0; j < tabNodes.count(); j++) {
                QDomElement tabWidget = tabNodes.at(j).toElement();
                if (tabWidget.isNull()) continue;
                
                QString tabName = tabWidget.attribute("name");
                QString tabText;
                QDomElement tabProperty = tabWidget.firstChildElement("property");
                while (!tabProperty.isNull()) {
                    if (tabProperty.attribute("name") == "text") {
                        QDomElement stringElement = tabProperty.firstChildElement("string");
                        if (!stringElement.isNull()) {
                            tabText = stringElement.text();
                        }
                        break;
                    }
                    tabProperty = tabProperty.nextSiblingElement("property");
                }
                
                if (!tabText.isEmpty()) {
                    codeLines << QString("    %1->addTab(new QWidget(), \"%2\");").arg(name, tabText);
                }
            }
        } else if (className == "QTextEdit") {
            codeLines << QString("    %1 = new QTextEdit(this);").arg(name);
        }
        
        codeLines << QString("    mainLayout->addWidget(%1);").arg(name);
        codeLines << "";
    }
    
    codeLines << "}";
    codeLines << "";
    
    // main函数
    codeLines << "int main(int argc, char *argv[])";
    codeLines << "{";
    codeLines << "    QApplication app(argc, argv);";
    codeLines << "    ";
    codeLines << QString("    %1Window window;").arg(settings.name);
    codeLines << "    window.show();";
    codeLines << "    ";
    codeLines << "    return app.exec();";
    codeLines << "}";
    
    return codeLines.join("\n");
}

void PackageWorker::generateCppProjectFiles(const SmartPackageConfig::SmartPackageSettings &settings, const QString &cppCode, const QString &outputDir)
{
    // 生成main.cpp文件
    QFile mainCppFile(outputDir + "/main.cpp");
    if (mainCppFile.open(QIODevice::WriteOnly)) {
        mainCppFile.write(cppCode.toUtf8());
        mainCppFile.close();
    } else {
        throw QString("无法创建main.cpp文件");
    }
    
    // 如果存在UI布局文件，复制到输出目录
    if (!settings.uiLayoutPath.isEmpty() && QFile::exists(settings.uiLayoutPath)) {
        QString uiFileName = QFileInfo(settings.uiLayoutPath).fileName();
        QString destUiPath = outputDir + "/" + uiFileName;
        
        if (QFile::copy(settings.uiLayoutPath, destUiPath)) {
            qDebug() << "UI文件已复制到:" << destUiPath;
        } else {
            qWarning() << "无法复制UI文件:" << settings.uiLayoutPath << "到" << destUiPath;
        }
    }
    
    // 生成CMakeLists.txt文件
    QString cmakeContent = generateCmakeLists(settings);
    QFile cmakeFile(outputDir + "/CMakeLists.txt");
    if (cmakeFile.open(QIODevice::WriteOnly)) {
        cmakeFile.write(cmakeContent.toUtf8());
        cmakeFile.close();
    } else {
        throw QString("无法创建CMakeLists.txt文件");
    }
}

QString PackageWorker::generateCmakeLists(const SmartPackageConfig::SmartPackageSettings &settings)
{
    QStringList cmakeLines;
    
    // 清理工程名，移除非字母数字下划线字符，但保留横杠
    // 使用uniqueId生成文件名，避免中文问题
    QString cleanProjectName = settings.uniqueId.isEmpty() ? settings.name : settings.uniqueId;
    cleanProjectName.replace(QRegularExpression("[^a-zA-Z0-9_-]"), "_");
    if (cleanProjectName.isEmpty()) {
        cleanProjectName = "SoftwareProject";
    }
    
    cmakeLines << "cmake_minimum_required(VERSION 3.16)";
    cmakeLines << "";
    cmakeLines << "project(" + cleanProjectName + ")";
    cmakeLines << "";
    cmakeLines << "set(CMAKE_CXX_STANDARD 17)";
    cmakeLines << "set(CMAKE_CXX_STANDARD_REQUIRED ON)";
    cmakeLines << "";
    cmakeLines << "# 查找Qt6";
    cmakeLines << "find_package(Qt6 REQUIRED COMPONENTS Core Widgets)";
    cmakeLines << "";
    cmakeLines << "# 设置自动包含moc文件";
    cmakeLines << "set(CMAKE_AUTOMOC ON)";
    cmakeLines << "set(CMAKE_AUTORCC ON)";
    cmakeLines << "set(CMAKE_AUTOUIC ON)";
    cmakeLines << "";
    
    // 如果有UI文件，添加到源文件列表
    QStringList sourceFiles;
    sourceFiles << "main.cpp";
    
    if (!settings.uiLayoutPath.isEmpty() && QFile::exists(settings.uiLayoutPath)) {
        QString uiFileName = QFileInfo(settings.uiLayoutPath).fileName();
        sourceFiles << uiFileName;
        cmakeLines << "# UI文件将自动被uic工具处理";
    }
    
    cmakeLines << "# 添加可执行文件";
    cmakeLines << "add_executable(" + cleanProjectName + " " + sourceFiles.join(" ") + ")";
    cmakeLines << "";
    cmakeLines << "# 链接Qt库";
    cmakeLines << "target_link_libraries(" + cleanProjectName + " Qt6::Core Qt6::Widgets)";
    cmakeLines << "";
    cmakeLines << "# Windows特定设置";
    cmakeLines << "if(WIN32)";
    cmakeLines << "    # 设置子系统为Windows";
    cmakeLines << "    set_target_properties(" + cleanProjectName + " PROPERTIES WIN32_EXECUTABLE TRUE)";
    cmakeLines << "    ";
    cmakeLines << "    # 设置应用程序图标";
    if (!settings.iconPath.isEmpty()) {
        cmakeLines << "    if(EXISTS \"" + settings.iconPath + "\")";
        cmakeLines << "        set(APP_ICON \"" + settings.iconPath + "\")";
        cmakeLines << "    endif()";
    }
    cmakeLines << "endif()";
    
    return cmakeLines.join("\n");
}

bool PackageWorker::copyDependencies(const QString &exePath, const QString &outputDir)
{
    if (!QFile::exists(exePath)) {
        qWarning() << "可执行文件不存在:" << exePath;
        return false;
    }
    
    QDir outputDirObj(outputDir);
    if (!outputDirObj.exists()) {
        if (!outputDirObj.mkpath(".")) {
            qWarning() << "无法创建输出目录:" << outputDir;
            return false;
        }
    }
    
    bool success = true;
    
    // 1. 使用windeployqt工具复制Qt依赖
    qDebug() << "开始复制Qt依赖...";
    QProcess deployProcess;
    
    // 使用正确的windeployqt路径
    QString windeployqtPath = "D:/Qt/6.9.1/mingw_64/bin/windeployqt.exe";
    
    // 检查windeployqt是否可用
    deployProcess.start(windeployqtPath, {"--version"});
    if (!deployProcess.waitForFinished(5000)) {
        qWarning() << "windeployqt工具不可用，将跳过Qt依赖复制";
        success = false;
    } else {
        // 重新执行windeployqt复制依赖
        deployProcess.start(windeployqtPath, {"--release", "--no-compiler-runtime", "--dir", outputDir, exePath});
        
        if (!deployProcess.waitForFinished(180000)) { // 3分钟超时
            qWarning() << "复制Qt依赖文件超时";
            success = false;
        } else if (deployProcess.exitCode() != 0) {
            QString errorOutput = QString::fromLocal8Bit(deployProcess.readAllStandardError());
            qWarning() << "复制Qt依赖文件失败:" << errorOutput;
            success = false;
        } else {
            qDebug() << "Qt依赖复制完成";
        }
    }
    
    // 2. 复制MinGW运行时依赖
    qDebug() << "开始复制MinGW运行时依赖...";
    QStringList mingwDlls = {
        "libgcc_s_seh-1.dll",
        "libstdc++-6.dll", 
        "libwinpthread-1.dll"
    };
    
    // 自动检测MinGW路径
    QString mingwBinDir = findMingwBinDir();
    
    if (mingwBinDir.isEmpty()) {
        qWarning() << "未找到MinGW安装路径，将跳过MinGW依赖复制";
        success = false;
    } else {
        qDebug() << "使用MinGW路径:" << mingwBinDir;
        
        for (const QString &dllName : mingwDlls) {
            QString sourcePath = mingwBinDir + "/" + dllName;
            QString destPath = outputDir + "/" + dllName;
            
            if (QFile::exists(sourcePath) && !QFile::exists(destPath)) {
                if (QFile::copy(sourcePath, destPath)) {
                    qDebug() << "复制MinGW DLL:" << dllName;
                } else {
                    qWarning() << "无法复制MinGW DLL:" << dllName;
                    success = false;
                }
            }
        }
    }
    
    // 3. 复制系统依赖（如果缺少）
    qDebug() << "检查系统依赖...";
    QStringList systemDlls = {
        "VCRUNTIME140.dll",
        "VCRUNTIME140_1.dll", 
        "MSVCP140.dll",
        "ucrtbase.dll"
    };
    
    for (const QString &dllName : systemDlls) {
        QString destPath = outputDir + "/" + dllName;
        if (!QFile::exists(destPath)) {
            qWarning() << "缺少系统DLL:" << dllName << "（需要安装Visual C++ Redistributable）";
        }
    }
    
    // 4. 复制Qt插件
    qDebug() << "开始复制Qt插件...";
    QStringList qtPlugins = {
        "platforms",
        "styles", 
        "imageformats",
        "iconengines"
    };
    
    // 自动检测Qt插件路径
    QString qtPluginsDir = findQtPluginsDir();
    
    if (qtPluginsDir.isEmpty()) {
        qWarning() << "未找到Qt插件路径，将跳过Qt插件复制";
        success = false;
    } else {
        qDebug() << "使用Qt插件路径:" << qtPluginsDir;
        
        for (const QString &pluginDir : qtPlugins) {
            QString sourceDir = qtPluginsDir + "/" + pluginDir;
            QString destDir = outputDir + "/" + pluginDir;
            
            if (QDir(sourceDir).exists() && !QDir(destDir).exists()) {
                if (copyDirectory(sourceDir, destDir)) {
                    qDebug() << "复制Qt插件目录:" << pluginDir;
                } else {
                    qWarning() << "无法复制Qt插件目录:" << pluginDir;
                    success = false;
                }
            }
        }
    }
    
    // 5. 复制主程序到输出目录（如果不在同一目录）
    QFileInfo exeInfo(exePath);
    QString destExePath = outputDir + "/" + exeInfo.fileName();
    if (exePath != destExePath && !QFile::exists(destExePath)) {
        if (QFile::copy(exePath, destExePath)) {
            qDebug() << "复制主程序到输出目录";
        } else {
            qWarning() << "无法复制主程序到输出目录";
            success = false;
        }
    }
    
    // 6. 生成依赖报告
    qDebug() << "生成依赖报告...";
    generateDependencyReport(exePath, outputDir);
    
    if (success) {
        qDebug() << "依赖复制完成";
    } else {
        qWarning() << "依赖复制过程中出现错误";
    }
    
    return success;
}

void PackageWorker::createInstaller(const SmartPackageConfig::SmartPackageSettings &settings, const QString &exePath)
{
    // 检查NSIS工具是否可用 - 直接使用系统已安装的NSIS工具
QString makensisPath;

// 直接检查常见NSIS安装路径
QStringList commonPaths = {
    "C:/Program Files (x86)/NSIS/makensis.exe",
    "C:/Program Files/NSIS/makensis.exe",
    QCoreApplication::applicationDirPath() + "/makensis.exe"
};

for (const QString &path : commonPaths) {
    if (QFile::exists(path)) {
        makensisPath = path;
        qDebug() << "找到NSIS工具:" << makensisPath;
        break;
    }
}

if (makensisPath.isEmpty()) {
    throw QString("NSIS工具不可用，请安装NSIS工具");
}
    
    QProcess nsisCheckProcess;
    nsisCheckProcess.start(makensisPath, {"/VERSION"});
    
    if (!nsisCheckProcess.waitForFinished(5000)) {
        QString error = QString::fromLocal8Bit(nsisCheckProcess.readAllStandardError());
        throw QString("NSIS工具检查超时，错误信息: %1").arg(error);
    }
    
    if (nsisCheckProcess.exitCode() != 0) {
        QString errorOutput = QString::fromLocal8Bit(nsisCheckProcess.readAllStandardError());
        QString standardOutput = QString::fromLocal8Bit(nsisCheckProcess.readAllStandardOutput());
        throw QString("NSIS工具执行失败，退出码: %1，错误输出: %2，标准输出: %3")
            .arg(nsisCheckProcess.exitCode())
            .arg(errorOutput)
            .arg(standardOutput);
    }
    
    // 使用smartpackageconfig中的完整NSIS实现，避免重复代码
    SmartPackageConfig config;
    if (!config.createNsisInstaller(settings, exePath)) {
        throw QString("NSIS安装包生成失败: %1").arg(config.getLastError());
    }
}

void PackageWorker::setApplicationIcon(const QString &exePath, const QString &iconPath)
{
    // 在Windows上使用rcedit工具设置应用程序图标
    QString rceditPath = "rcedit.exe";
    if (QFile::exists(rceditPath)) {
        QProcess rceditProcess;
        rceditProcess.start(rceditPath, {exePath, "--set-icon", iconPath});
        
        if (!rceditProcess.waitForFinished(30000)) { // 30秒超时
            qWarning() << "设置应用程序图标超时";
        }
    }
}



PackageManager::PackageManager(QObject *parent)
    : QObject(parent)
    , m_worker(nullptr)
    , m_workerThread(nullptr)
    , m_isPackaging(false)
{
    setupWorker();
}

PackageManager::~PackageManager()
{
    cleanupWorker();
}

void PackageManager::startPackage(const SmartPackageConfig::SmartPackageSettings &settings)
{
    if (m_isPackaging) {
        emit errorOccurred("当前正在打包，请等待完成");
        return;
    }
    
    m_isPackaging = true;
    emit progressChanged(0, "开始打包过程...");
    
    // 在工作线程中执行打包，传递PackageSettings对象
    QMetaObject::invokeMethod(m_worker, "smartPackageSoftware", Qt::QueuedConnection, 
                              Q_ARG(SmartPackageConfig::SmartPackageSettings, settings));
}

void PackageManager::startPackage(const Product &product, const SmartPackageConfig::SmartPackageSettings &settings)
{
    if (m_isPackaging) {
        emit errorOccurred("当前正在打包，请等待完成");
        return;
    }
    
    m_isPackaging = true;
    emit progressChanged(0, "开始打包过程...");
    
    // 在工作线程中执行打包，传递Product对象
    QMetaObject::invokeMethod(m_worker, "smartPackageSoftware", Qt::QueuedConnection, 
                              Q_ARG(Product, product),
                              Q_ARG(SmartPackageConfig::SmartPackageSettings, settings));
}

void PackageManager::cancelPackage()
{
    if (m_workerThread && m_workerThread->isRunning()) {
        m_workerThread->quit();
        m_workerThread->wait();
        m_isPackaging = false;
        emit packageFinished(false, "");
    }
}

bool PackageManager::isPackagingToolsAvailable() const
{
    // 检查CMake是否可用
    QProcess cmakeProcess;
    cmakeProcess.start("cmake", {"--version"});
    if (!cmakeProcess.waitForFinished(5000)) {
        return false;
    }
    
    return cmakeProcess.exitCode() == 0;
}

bool PackageManager::isPackaging() const
{
    return m_isPackaging;
}

void PackageManager::setupWorker()
{
    m_workerThread = new QThread(this);
    m_worker = new PackageWorker();
    m_worker->moveToThread(m_workerThread);
    
    connect(m_workerThread, &QThread::finished, m_worker, &QObject::deleteLater);
    connect(m_worker, &PackageWorker::progressChanged, this, &PackageManager::onWorkerProgress);
    connect(m_worker, &PackageWorker::finished, this, &PackageManager::onWorkerFinished);
    connect(m_worker, &PackageWorker::errorOccurred, this, &PackageManager::onWorkerError);
    
    m_workerThread->start();
}

void PackageManager::cleanupWorker()
{
    if (m_workerThread) {
        m_workerThread->quit();
        m_workerThread->wait();
        delete m_workerThread;
        m_workerThread = nullptr;
    }
    m_worker = nullptr;
}

void PackageManager::onWorkerProgress(int progress, const QString &message)
{
    emit progressChanged(progress, message);
}

void PackageManager::onWorkerFinished(bool success, const QString &resultPath)
{
    m_isPackaging = false;
    emit packageFinished(success, resultPath);
}

void PackageManager::onWorkerError(const QString &error)
{
    emit errorOccurred(error);
}

// 智能打包方法实现
bool PackageWorker::executeSmartPackageLogic(const SmartPackageConfig::SmartPackageSettings &settings, const QString &uiLayoutPath)
{
    // 准备输出目录
    QDir outputDir(settings.outputDir);
    if (!outputDir.exists()) {
        if (!outputDir.mkpath(".")) {
            throw QString("无法创建输出目录: %1").arg(settings.outputDir);
        }
    }
    
    emit progressChanged(10, "准备输出目录完成");
    
    // 检查UI布局文件是否存在
    if (!QFile::exists(uiLayoutPath)) {
        throw QString("UI布局文件不存在: %1").arg(uiLayoutPath);
    }
    
    emit progressChanged(15, "找到UI布局文件");
    
    // 使用智能打包配置生成项目文件
    emit progressChanged(20, "生成智能CMake配置...");
    
    SmartPackageConfig smartConfig;
    
    // 检测开发工具路径
    emit progressChanged(25, "检测开发工具路径...");
    if (!smartConfig.detectDevelopmentTools()) {
        throw QString("无法检测到必要的开发工具（Qt、MinGW、CMake）");
    }
    
    // 生成智能CMakeLists.txt
    emit progressChanged(30, "生成智能CMakeLists.txt...");
    QString cmakeLists = smartConfig.generateSmartCmakeLists(settings);
    
    // 生成main.cpp
    emit progressChanged(35, "生成主程序文件...");
    QString mainCpp = smartConfig.generateSmartMainCpp(settings);
    
    // 保存项目文件
    emit progressChanged(40, "保存项目文件...");
    
    // 保存CMakeLists.txt
    QFile cmakeFile(outputDir.absoluteFilePath("CMakeLists.txt"));
    if (cmakeFile.open(QIODevice::WriteOnly)) {
        cmakeFile.write(cmakeLists.toUtf8());
        cmakeFile.close();
    } else {
        throw QString("无法创建CMakeLists.txt文件");
    }
    
    // 保存main.cpp
    QFile mainFile(outputDir.absoluteFilePath("main.cpp"));
    if (mainFile.open(QIODevice::WriteOnly)) {
        mainFile.write(mainCpp.toUtf8());
        mainFile.close();
    } else {
        throw QString("无法创建main.cpp文件");
    }
    
    // 执行智能打包
    emit progressChanged(45, "执行智能打包流程...");
    
    // 连接SmartPackageConfig的信号
    connect(&smartConfig, &SmartPackageConfig::progressChanged, this, &PackageWorker::progressChanged);
    
    // 使用lambda捕获正确的文件路径
    QString finalResultPath;
    QMetaObject::Connection packageFinishedConnection = connect(&smartConfig, &SmartPackageConfig::packageFinished, 
        [&](bool success, const QString &resultPath) {
            if (success) {
                finalResultPath = resultPath;
            }
        });
    
    if (!smartConfig.executeSmartPackage(settings)) {
        qDebug()<<smartConfig.getLastError();
        disconnect(&smartConfig, &SmartPackageConfig::progressChanged, this, &PackageWorker::progressChanged);
        disconnect(packageFinishedConnection);
        throw QString("智能打包执行失败");
    }
    
    // 断开连接
    disconnect(&smartConfig, &SmartPackageConfig::progressChanged, this, &PackageWorker::progressChanged);
    disconnect(packageFinishedConnection);
    
    emit progressChanged(100, "智能打包完成！");
    
    // 使用SmartPackageConfig返回的正确文件路径
    if (!finalResultPath.isEmpty()) {
        emit finished(true, finalResultPath);
    } else {
        // 如果SmartPackageConfig没有返回路径，则使用输出目录作为后备
        emit finished(true, outputDir.absolutePath());
    }
    
    return true;
}

void PackageWorker::smartPackageSoftware(const SmartPackageConfig::SmartPackageSettings &settings)
{
    try {
        // 验证设置
        if (settings.name.isEmpty()) {
            throw QString("软件名称不能为空");
        }
        if (settings.outputDir.isEmpty()) {
            throw QString("输出目录不能为空");
        }
        
        emit progressChanged(5, "开始智能打包过程...");
        
        // 检查UI布局文件是否存在
        QString uiLayoutPath = QDir::currentPath() + "/ui_layouts/" + settings.name + ".xml";
        
        // 执行公共的智能打包逻辑
        executeSmartPackageLogic(settings, uiLayoutPath);
        
    } catch (const QString &error) {
        emit errorOccurred(error);
        emit finished(false, "");
    }
}

void PackageWorker::smartPackageSoftware(const Product &product, const SmartPackageConfig::SmartPackageSettings &settings)
{
    try {
        emit progressChanged(10, "检查UI布局文件...");
        
        // 使用Product对象中的uiLayoutPath字段
        QString uiLayoutPath = product.uiLayoutPath();
        if (uiLayoutPath.isEmpty()) {
            throw QString("产品未绑定UI布局文件，请先为产品绑定UI布局");
        }
        
        // 执行公共的智能打包逻辑
        executeSmartPackageLogic(settings, uiLayoutPath);
        
    } catch (const QString &error) {
        emit errorOccurred(error);
        emit finished(false, "");
    }
}

void PackageManager::startSmartPackage(const SmartPackageConfig::SmartPackageSettings &settings)
{
    if (m_isPackaging) {
        emit errorOccurred("当前正在打包，请等待完成");
        return;
    }
    
    m_isPackaging = true;
    emit progressChanged(0, "开始智能打包过程...");
    
    // 在工作线程中执行智能打包
    QMetaObject::invokeMethod(m_worker, "smartPackageSoftware", Qt::QueuedConnection, 
                              Q_ARG(SmartPackageConfig::SmartPackageSettings, settings));
}

void PackageManager::startSmartPackage(const Product &product, const SmartPackageConfig::SmartPackageSettings &settings)
{
    if (m_isPackaging) {
        emit errorOccurred("当前正在打包，请等待完成");
        return;
    }
    
    m_isPackaging = true;
    emit progressChanged(0, "开始智能打包过程...");
    
    // 在工作线程中执行智能打包
    QMetaObject::invokeMethod(m_worker, "smartPackageSoftware", Qt::QueuedConnection, 
                              Q_ARG(Product, product),
                              Q_ARG(SmartPackageConfig::SmartPackageSettings, settings));
}

// 复制目录及其内容
bool PackageWorker::copyDirectory(const QString &sourceDir, const QString &destinationDir)
{
    QDir source(sourceDir);
    QDir destination(destinationDir);
    
    if (!source.exists()) {
        qWarning() << "源目录不存在:" << sourceDir;
        return false;
    }
    
    if (!destination.exists()) {
        if (!destination.mkpath(".")) {
            qWarning() << "无法创建目标目录:" << destinationDir;
            return false;
        }
    }
    
    // 复制所有文件
    QFileInfoList files = source.entryInfoList(QDir::Files);
    for (const QFileInfo &file : files) {
        QString sourcePath = file.absoluteFilePath();
        QString destPath = destination.absoluteFilePath(file.fileName());
        
        if (QFile::exists(destPath)) {
            QFile::remove(destPath);
        }
        
        if (!QFile::copy(sourcePath, destPath)) {
            qWarning() << "无法复制文件:" << sourcePath << "->" << destPath;
            return false;
        }
    }
    
    // 递归复制子目录
    QFileInfoList dirs = source.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QFileInfo &dir : dirs) {
        QString sourceSubDir = dir.absoluteFilePath();
        QString destSubDir = destination.absoluteFilePath(dir.fileName());
        
        if (!copyDirectory(sourceSubDir, destSubDir)) {
            return false;
        }
    }
    
    return true;
}

QString PackageWorker::findMingwBinDir()
{
    // 常见MinGW安装路径
    QStringList possiblePaths = {
        "D:/Qt/Tools/mingw1310_64/bin",
        "D:/Qt/Tools/mingw1120_64/bin", 
        "D:/Qt/Tools/mingw900_64/bin",
        "C:/Qt/Tools/mingw1310_64/bin",
        "C:/Qt/Tools/mingw1120_64/bin",
        "C:/Qt/Tools/mingw900_64/bin",
        "C:/mingw64/bin",
        "C:/mingw/bin"
    };
    
    for (const QString &path : possiblePaths) {
        if (QDir(path).exists()) {
            // 检查是否包含必要的DLL文件
            QString testDll = path + "/libgcc_s_seh-1.dll";
            if (QFile::exists(testDll)) {
                return path;
            }
        }
    }
    
    // 尝试从环境变量中查找
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString pathEnv = env.value("PATH");
    QStringList pathList = pathEnv.split(";");
    
    for (const QString &path : pathList) {
        if (path.contains("mingw", Qt::CaseInsensitive) && QDir(path).exists()) {
            QString testDll = path + "/libgcc_s_seh-1.dll";
            if (QFile::exists(testDll)) {
                return path;
            }
        }
    }
    
    return QString();
}

QString PackageWorker::findQtPluginsDir()
{
    // 常见Qt安装路径
    QStringList possiblePaths = {
        "D:/Qt/6.9.1/mingw_64/plugins",
        "D:/Qt/6.8.0/mingw_64/plugins", 
        "D:/Qt/6.7.0/mingw_64/plugins",
        "C:/Qt/6.9.1/mingw_64/plugins",
        "C:/Qt/6.8.0/mingw_64/plugins",
        "C:/Qt/6.7.0/mingw_64/plugins",
        "D:/Qt/5.15.2/mingw81_64/plugins",
        "C:/Qt/5.15.2/mingw81_64/plugins"
    };
    
    for (const QString &path : possiblePaths) {
        if (QDir(path).exists()) {
            // 检查是否包含平台插件
            QString testPlugin = path + "/platforms";
            if (QDir(testPlugin).exists()) {
                return path;
            }
        }
    }
    
    // 尝试从windeployqt工具推断Qt路径
    QProcess process;
    process.start("windeployqt", {"--version"});
    if (process.waitForFinished(5000) && process.exitCode() == 0) {
        QString output = QString::fromLocal8Bit(process.readAllStandardOutput());
        // 从输出中提取Qt安装路径
        QRegularExpression regex("Qt (\\d+\\.\\d+\\.\\d+)");
        QRegularExpressionMatch match = regex.match(output);
        if (match.hasMatch()) {
            QString version = match.captured(1);
            QStringList candidatePaths = {
                QString("D:/Qt/%1/mingw_64/plugins").arg(version),
                QString("C:/Qt/%1/mingw_64/plugins").arg(version)
            };
            
            for (const QString &path : candidatePaths) {
                if (QDir(path).exists()) {
                    return path;
                }
            }
        }
    }
    
    return QString();
}

// 生成依赖报告
QString PackageWorker::generateDependencyReport(const QString &exePath, const QString &outputDir)
{
    QString report;
    QTextStream stream(&report);
    
    stream << "=== 依赖分析报告 ===\n";
    stream << "生成时间: " << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "\n";
    stream << "可执行文件: " << exePath << "\n";
    stream << "输出目录: " << outputDir << "\n\n";
    
    // 检查可执行文件是否存在
    if (!QFile::exists(exePath)) {
        stream << "错误: 可执行文件不存在\n";
        return report;
    }
    
    // 检查输出目录
    QDir output(outputDir);
    if (!output.exists()) {
        stream << "警告: 输出目录不存在\n";
    }
    
    // 检查已复制的依赖文件
    QFileInfoList files = output.entryInfoList(QDir::Files);
    stream << "已复制的依赖文件 (" << files.size() << " 个):\n";
    for (const QFileInfo &file : files) {
        stream << "  - " << file.fileName() << " (" << file.size() << " 字节)\n";
    }
    
    // 检查Qt插件目录
    QString pluginsDir = outputDir + "/plugins";
    if (QDir(pluginsDir).exists()) {
        QFileInfoList pluginFiles = QDir(pluginsDir).entryInfoList(QDir::Files);
        stream << "\nQt插件文件 (" << pluginFiles.size() << " 个):\n";
        for (const QFileInfo &file : pluginFiles) {
            stream << "  - plugins/" << file.fileName() << " (" << file.size() << " 字节)\n";
        }
    }
    
    // 检查平台目录
    QString platformsDir = outputDir + "/platforms";
    if (QDir(platformsDir).exists()) {
        QFileInfoList platformFiles = QDir(platformsDir).entryInfoList(QDir::Files);
        stream << "\n平台插件文件 (" << platformFiles.size() << " 个):\n";
        for (const QFileInfo &file : platformFiles) {
            stream << "  - platforms/" << file.fileName() << " (" << file.size() << " 字节)\n";
        }
    }
    
    stream << "\n=== 报告结束 ===\n";
    
    return report;
}