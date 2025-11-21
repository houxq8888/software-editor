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

PackageWorker::PackageWorker(QObject *parent)
    : QObject(parent)
{
}

void PackageWorker::packageSoftware(const PackageConfig::PackageSettings &settings)
{
    try {
        // 验证设置
        if (settings.name.isEmpty()) {
            throw QString("软件名称不能为空");
        }
        if (settings.outputDir.isEmpty()) {
            throw QString("输出目录不能为空");
        }
        
        emit progressChanged(5, "开始打包过程...");
        
        // 准备输出目录
        QDir outputDir(settings.outputDir);
        if (!outputDir.exists()) {
            if (!outputDir.mkpath(".")) {
                throw QString("无法创建输出目录: %1").arg(settings.outputDir);
            }
        }
        
        emit progressChanged(10, "准备输出目录完成");
        
        // 检查UI布局文件是否存在
        QString uiLayoutPath = QDir::currentPath() + "/ui_layouts/" + settings.name + ".xml";
        if (!QFile::exists(uiLayoutPath)) {
            throw QString("UI布局文件不存在: %1").arg(uiLayoutPath);
        }
        
        emit progressChanged(15, "找到UI布局文件");
        
        // 生成UI布局到C++代码的转换
        emit progressChanged(20, "转换UI布局为C++代码...");
        QString cppCode = generateCppCodeFromUILayout(uiLayoutPath, settings);
        
        // 生成完整的C++项目文件
        emit progressChanged(30, "生成C++项目文件...");
        generateCppProjectFiles(settings, cppCode, outputDir.absolutePath());
        
        emit progressChanged(40, "C++项目文件生成完成");
        
        // 执行CMake构建
        emit progressChanged(50, "执行CMake构建...");
        
        QProcess cmakeProcess;
        cmakeProcess.setWorkingDirectory(outputDir.absolutePath());
        
        // 生成CMakeLists.txt
        QString cmakeLists = generateCmakeLists(settings);
        QFile cmakeFile(outputDir.absoluteFilePath("CMakeLists.txt"));
        if (cmakeFile.open(QIODevice::WriteOnly)) {
            cmakeFile.write(cmakeLists.toUtf8());
            cmakeFile.close();
        }
        
        // 执行CMake配置
        cmakeProcess.start("cmake", {"-G", "MinGW Makefiles", "-DCMAKE_BUILD_TYPE=Release", "."});
        if (!cmakeProcess.waitForFinished(120000)) { // 2分钟超时
            throw QString("CMake配置超时");
        }
        
        if (cmakeProcess.exitCode() != 0) {
            QString errorOutput = QString::fromLocal8Bit(cmakeProcess.readAllStandardError());
            throw QString("CMake配置失败: %1").arg(errorOutput);
        }
        
        emit progressChanged(65, "CMake配置完成");
        
        // 执行构建
        cmakeProcess.start("cmake", {"--build", ".", "--config", "Release"});
        if (!cmakeProcess.waitForFinished(300000)) { // 5分钟超时
            throw QString("构建过程超时");
        }
        
        if (cmakeProcess.exitCode() != 0) {
            QString errorOutput = QString::fromLocal8Bit(cmakeProcess.readAllStandardError());
            throw QString("构建失败: %1").arg(errorOutput);
        }
        
        emit progressChanged(80, "构建完成");
        
        // 复制可执行文件和依赖
        QString exePath = outputDir.absoluteFilePath(settings.name + ".exe");
        
        if (settings.includeDependencies) {
            emit progressChanged(85, "复制依赖文件...");
            copyDependencies(exePath, outputDir.absolutePath());
        }
        
        // 设置应用程序图标
        if (!settings.iconPath.isEmpty() && QFile::exists(settings.iconPath)) {
            emit progressChanged(90, "设置应用程序图标...");
            setApplicationIcon(exePath, settings.iconPath);
        }
        
        // 创建安装包
        if (settings.createInstaller) {
            emit progressChanged(95, "创建安装包...");
            createInstaller(settings, exePath);
        }
        
        emit progressChanged(100, "打包完成！");
        emit finished(true, outputDir.absolutePath());
        
    } catch (const QString &error) {
        emit errorOccurred(error);
        emit finished(false, "");
    }
}

void PackageWorker::packageSoftware(const Product &product, const PackageConfig::PackageSettings &settings)
{
    try {
        emit progressChanged(10, "检查UI布局文件...");
        
        // 使用Product对象中的uiLayoutPath字段
        QString uiLayoutPath = product.uiLayoutPath();
        if (uiLayoutPath.isEmpty()) {
            throw QString("产品未绑定UI布局文件，请先为产品绑定UI布局");
        }
        
        if (!QFile::exists(uiLayoutPath)) {
            throw QString("UI布局文件不存在: %1").arg(uiLayoutPath);
        }
        
        emit progressChanged(15, "找到UI布局文件");
        
        // 准备输出目录
        QDir outputDir(settings.outputDir);
        if (!outputDir.exists()) {
            if (!outputDir.mkpath(".")) {
                throw QString("无法创建输出目录: %1").arg(settings.outputDir);
            }
        }
        
        // 生成UI布局到C++代码的转换
        emit progressChanged(20, "转换UI布局为C++代码...");
        QString cppCode = generateCppCodeFromUILayout(uiLayoutPath, settings);
        
        // 生成完整的C++项目文件
        emit progressChanged(30, "生成C++项目文件...");
        generateCppProjectFiles(settings, cppCode, outputDir.absolutePath());
        
        emit progressChanged(40, "C++项目文件生成完成");
        
        // 执行CMake构建
        emit progressChanged(50, "执行CMake构建...");
        
        QProcess cmakeProcess;
        cmakeProcess.setWorkingDirectory(outputDir.absolutePath());
        
        // 生成CMakeLists.txt
        QString cmakeLists = generateCmakeLists(settings);
        QFile cmakeFile(outputDir.absoluteFilePath("CMakeLists.txt"));
        if (cmakeFile.open(QIODevice::WriteOnly)) {
            cmakeFile.write(cmakeLists.toUtf8());
            cmakeFile.close();
        }
        
        // 执行CMake配置
        cmakeProcess.start("cmake", {"-G", "MinGW Makefiles", "-DCMAKE_BUILD_TYPE=Release", "."});
        if (!cmakeProcess.waitForFinished(120000)) { // 2分钟超时
            throw QString("CMake配置超时");
        }
        
        if (cmakeProcess.exitCode() != 0) {
            QString errorOutput = QString::fromLocal8Bit(cmakeProcess.readAllStandardError());
            throw QString("CMake配置失败: %1").arg(errorOutput);
        }
        
        emit progressChanged(65, "CMake配置完成");
        
        // 执行构建
        cmakeProcess.start("cmake", {"--build", ".", "--config", "Release"});
        if (!cmakeProcess.waitForFinished(300000)) { // 5分钟超时
            throw QString("构建过程超时");
        }
        
        if (cmakeProcess.exitCode() != 0) {
            QString errorOutput = QString::fromLocal8Bit(cmakeProcess.readAllStandardError());
            throw QString("构建失败: %1").arg(errorOutput);
        }
        
        emit progressChanged(80, "构建完成");
        
        // 复制可执行文件和依赖
        QString exePath = outputDir.absoluteFilePath(settings.name + ".exe");
        
        if (settings.includeDependencies) {
            emit progressChanged(85, "复制依赖文件...");
            copyDependencies(exePath, outputDir.absolutePath());
        }
        
        // 设置应用程序图标
        if (!settings.iconPath.isEmpty() && QFile::exists(settings.iconPath)) {
            emit progressChanged(90, "设置应用程序图标...");
            setApplicationIcon(exePath, settings.iconPath);
        }
        
        // 创建安装包
        if (settings.createInstaller) {
            emit progressChanged(95, "创建安装包...");
            createInstaller(settings, exePath);
        }
        
        emit progressChanged(100, "打包完成！");
        emit finished(true, outputDir.absolutePath());
        
    } catch (const QString &error) {
        emit errorOccurred(error);
        emit finished(false, "");
    }
}

QString PackageWorker::generateCppCodeFromUILayout(const QString &uiLayoutPath, const PackageConfig::PackageSettings &settings)
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

void PackageWorker::generateCppProjectFiles(const PackageConfig::PackageSettings &settings, const QString &cppCode, const QString &outputDir)
{
    // 生成main.cpp文件
    QFile mainCppFile(outputDir + "/main.cpp");
    if (mainCppFile.open(QIODevice::WriteOnly)) {
        mainCppFile.write(cppCode.toUtf8());
        mainCppFile.close();
    } else {
        throw QString("无法创建main.cpp文件");
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

QString PackageWorker::generateCmakeLists(const PackageConfig::PackageSettings &settings)
{
    QStringList cmakeLines;
    
    // 清理工程名，移除中文字符
    QString cleanProjectName = settings.name;
    cleanProjectName.replace(QRegularExpression("[^a-zA-Z0-9_]"), "_");
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
    cmakeLines << "# 添加可执行文件";
    cmakeLines << "add_executable(" + cleanProjectName + " main.cpp)";
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

void PackageWorker::copyDependencies(const QString &exePath, const QString &outputDir)
{
    // 使用windeployqt工具复制Qt依赖
    QProcess deployProcess;
    deployProcess.start("windeployqt", {"--release", "--no-compiler-runtime", "--dir", outputDir, exePath});
    
    if (!deployProcess.waitForFinished(120000)) { // 2分钟超时
        qWarning() << "复制依赖文件超时";
        return;
    }
    
    if (deployProcess.exitCode() != 0) {
        QString errorOutput = QString::fromLocal8Bit(deployProcess.readAllStandardError());
        qWarning() << "复制依赖文件失败:" << errorOutput;
    }
}

void PackageWorker::createInstaller(const PackageConfig::PackageSettings &settings, const QString &exePath)
{
    // 生成NSIS安装脚本
    QString nsisScript = createSimpleNsisScript(settings, exePath);
    
    // 保存NSIS脚本文件
    QDir outputDir(settings.outputDir);
    QString scriptPath = outputDir.absoluteFilePath("installer.nsi");
    
    QFile scriptFile(scriptPath);
    if (!scriptFile.open(QIODevice::WriteOnly)) {
        throw QString("无法创建NSIS安装脚本文件");
    }
    
    scriptFile.write(nsisScript.toUtf8());
    scriptFile.close();
    
    // 执行NSIS编译
    QProcess nsisProcess;
    nsisProcess.setWorkingDirectory(outputDir.absolutePath());
    nsisProcess.start("makensis", {scriptPath});
    
    if (!nsisProcess.waitForFinished(180000)) { // 3分钟超时
        throw QString("NSIS编译超时");
    }
    
    if (nsisProcess.exitCode() != 0) {
        QString errorOutput = QString::fromLocal8Bit(nsisProcess.readAllStandardError());
        throw QString("NSIS编译失败: %1").arg(errorOutput);
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

QString PackageWorker::createSimpleNsisScript(const PackageConfig::PackageSettings &settings, const QString &exePath)
{
    QString script;
    
    // 使用QStringList来构建脚本，避免复杂的字符串连接
    QStringList scriptLines;
    
    scriptLines << QString("Name \"%1\"").arg(settings.name);
    scriptLines << QString("OutFile \"%1_Setup.exe\"").arg(settings.name);
    scriptLines << QString("InstallDir \"$PROGRAMFILES\\%1\\%2\"").arg(settings.developer, settings.name);
    scriptLines << "";
    
    scriptLines << "Section \"Main Program\"";
    scriptLines << "    SetOutPath \"$INSTDIR\"";
    scriptLines << QString("    File \"%1\"").arg(QFileInfo(exePath).fileName());
    scriptLines << "    File \"*.dll\"";
    scriptLines << "";
    scriptLines << "    ; 创建开始菜单快捷方式";
    scriptLines << QString("    CreateShortcut \"$SMPROGRAMS\\%1.lnk\" \"$INSTDIR\\%1.exe\"").arg(settings.name);
    scriptLines << "";
    scriptLines << "    ; 创建桌面快捷方式";
    scriptLines << QString("    CreateShortcut \"$DESKTOP\\%1.lnk\" \"$INSTDIR\\%1.exe\"").arg(settings.name);
    scriptLines << "";
    scriptLines << "    ; 写入卸载信息";
    scriptLines << "    WriteUninstaller \"$INSTDIR\\Uninstall.exe\"";
    scriptLines << QString("    WriteRegStr HKLM \"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\%1\" \"DisplayName\" \"%1\"").arg(settings.name);
    scriptLines << QString("    WriteRegStr HKLM \"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\%1\" \"UninstallString\" \"$INSTDIR\\Uninstall.exe\"").arg(settings.name);
    scriptLines << "SectionEnd";
    scriptLines << "";
    scriptLines << "Section \"Uninstall\"";
    scriptLines << "    Delete \"$INSTDIR\\*.*\"";
    scriptLines << "    RMDir /r \"$INSTDIR\"";
    scriptLines << "";
    scriptLines << QString("    Delete \"$SMPROGRAMS\\%1.lnk\"").arg(settings.name);
    scriptLines << QString("    Delete \"$DESKTOP\\%1.lnk\"").arg(settings.name);
    scriptLines << "";
    scriptLines << QString("    DeleteRegKey HKLM \"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\%1\"").arg(settings.name);
    scriptLines << "SectionEnd";
    
    script = scriptLines.join("\n");
    return script;
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

void PackageManager::startPackage(const PackageConfig::PackageSettings &settings)
{
    if (m_isPackaging) {
        emit errorOccurred("当前正在打包，请等待完成");
        return;
    }
    
    m_isPackaging = true;
    emit progressChanged(0, "开始打包过程...");
    
    // 在工作线程中执行打包，传递PackageSettings对象
    QMetaObject::invokeMethod(m_worker, "packageSoftware", Qt::QueuedConnection, 
                              Q_ARG(PackageConfig::PackageSettings, settings));
}

void PackageManager::startPackage(const Product &product, const PackageConfig::PackageSettings &settings)
{
    if (m_isPackaging) {
        emit errorOccurred("当前正在打包，请等待完成");
        return;
    }
    
    m_isPackaging = true;
    emit progressChanged(0, "开始打包过程...");
    
    // 在工作线程中执行打包，传递Product对象
    QMetaObject::invokeMethod(m_worker, "packageSoftware", Qt::QueuedConnection, 
                              Q_ARG(Product, product),
                              Q_ARG(PackageConfig::PackageSettings, settings));
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
        
        // 准备输出目录
        QDir outputDir(settings.outputDir);
        if (!outputDir.exists()) {
            if (!outputDir.mkpath(".")) {
                throw QString("无法创建输出目录: %1").arg(settings.outputDir);
            }
        }
        
        emit progressChanged(10, "准备输出目录完成");
        
        // 检查UI布局文件是否存在
        QString uiLayoutPath = QDir::currentPath() + "/ui_layouts/" + settings.name + ".xml";
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
        if (!smartConfig.executeSmartPackage(settings)) {
            qDebug()<<smartConfig.getLastError();
            throw QString("智能打包执行失败");
        }
        
        emit progressChanged(100, "智能打包完成！");
        emit finished(true, outputDir.absolutePath());
        
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
        
        if (!QFile::exists(uiLayoutPath)) {
            throw QString("UI布局文件不存在: %1").arg(uiLayoutPath);
        }
        
        emit progressChanged(15, "找到UI布局文件");
        
        // 准备输出目录
        QDir outputDir(settings.outputDir);
        if (!outputDir.exists()) {
            if (!outputDir.mkpath(".")) {
                throw QString("无法创建输出目录: %1").arg(settings.outputDir);
            }
        }
        
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
        if (!smartConfig.executeSmartPackage(settings)) {
            qDebug()<<smartConfig.getLastError();
            throw QString("智能打包执行失败");
        }
        
        emit progressChanged(100, "智能打包完成！");
        emit finished(true, outputDir.absolutePath());
        
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