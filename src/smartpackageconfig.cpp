#include "smartpackageconfig.h"
#include "packagemanager.h"
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QStandardPaths>
#include <QDateTime>
#include <QTextStream>
#include <QCoreApplication>
#include <QRegularExpression>
#include <QDomDocument>
#include <QDirIterator>

SmartPackageConfig::SmartPackageConfig(QObject *parent)
    : QObject(parent)
{
}

bool SmartPackageConfig::detectDevelopmentTools()
{
    emit progressChanged(10, "开始检测开发工具路径...");
    
    // 检测Qt路径
    QString qtPath = detectQtPath();
    if (qtPath.isEmpty()) {
        emit progressChanged(20, "警告: 未检测到Qt安装路径，请手动设置");
        // 不立即返回false，允许用户手动设置路径
    } else {
        m_settings.qtDir = qtPath;
        emit progressChanged(30, "Qt路径检测完成: " + qtPath);
    }
    
    // 检测MinGW路径
    QString mingwPath = detectMingwPath();
    if (mingwPath.isEmpty()) {
        emit progressChanged(40, "警告: 未检测到MinGW安装路径，请手动设置");
    } else {
        m_settings.mingwDir = mingwPath;
        emit progressChanged(50, "MinGW路径检测完成: " + mingwPath);
    }
    
    // 检测CMake路径
    QString cmakePath = detectCmakePath();
    if (cmakePath.isEmpty()) {
        emit progressChanged(60, "警告: 未检测到CMake安装路径，请手动设置");
    } else {
        m_settings.cmakeDir = cmakePath;
        emit progressChanged(70, "CMake路径检测完成: " + cmakePath);
    }
    
    // 确定配置方案
    if (!qtPath.isEmpty() && !mingwPath.isEmpty() && !cmakePath.isEmpty()) {
        if (qtPath.contains("6.9")) {
            m_settings.detectedConfig = "Qt6.9 + MinGW + CMake";
        } else if (qtPath.contains("6.7")) {
            m_settings.detectedConfig = "Qt6.7 + MinGW + CMake";
        } else {
            m_settings.detectedConfig = "Qt + MinGW + CMake";
        }
        emit progressChanged(90, "开发工具检测完成，配置方案: " + m_settings.detectedConfig);
    } else {
        m_settings.detectedConfig = "部分路径需要手动设置";
        emit progressChanged(90, "开发工具检测完成，部分路径需要手动设置");
    }
    
    return true; // 即使部分路径未检测到，也返回true，允许用户手动设置
}

QString SmartPackageConfig::detectQtPath() const
{
    // 尝试多个可能的Qt安装路径
    QStringList possiblePaths;
    
    // 从环境变量获取
    QString qtdir = qgetenv("QTDIR");
    if (!qtdir.isEmpty() && checkPathExists(qtdir)) {
        return qtdir;
    }
    
    // 常见Qt安装路径
    possiblePaths << "C:/Qt"
                  << "D:/Qt"
                  << "E:/Qt"
                  << QDir::homePath() + "/Qt"
                  << "C:/Program Files/Qt"
                  << "D:/Program Files/Qt";
    
    for (const QString &basePath : possiblePaths) {
        if (checkPathExists(basePath)) {
            QDir qtDir(basePath);
            QStringList versions = qtDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
            
            // 优先选择较新版本
            QStringList sortedVersions = versions;
            std::sort(sortedVersions.begin(), sortedVersions.end(), std::greater<QString>());
            
            for (const QString &version : sortedVersions) {
                QString mingwPath = basePath + "/" + version + "/mingw_64";
                if (checkPathExists(mingwPath)) {
                    return mingwPath;
                }
            }
        }
    }
    
    return QString();
}

QString SmartPackageConfig::detectMingwPath() const
{
    // 优先查找Qt Tools目录下的MinGW工具路径
    QStringList possiblePaths;
    
    // 从Qt安装路径推断Tools目录
    if (!m_settings.qtDir.isEmpty()) {
        // 如果Qt路径是类似 D:/Qt/6.9.1/mingw_64，则推断Tools目录为 D:/Qt/Tools
        QFileInfo qtInfo(m_settings.qtDir);
        if (qtInfo.absolutePath().contains("/Qt/")) {
            QString toolsPath = qtInfo.absolutePath().split("/Qt/")[0] + "/Qt/Tools";
            possiblePaths << toolsPath + "/mingw1310_64"
                          << toolsPath + "/mingw1120_64"
                          << toolsPath + "/mingw";
        }
    }
    
    // 常见Qt Tools目录路径
    possiblePaths << "D:/Qt/Tools/mingw1310_64"
                  << "D:/Qt/Tools/mingw1120_64"
                  << "D:/Qt/Tools/mingw"
                  << "C:/Qt/Tools/mingw1310_64"
                  << "C:/Qt/Tools/mingw1120_64"
                  << "C:/Qt/Tools/mingw"
                  << "E:/Qt/Tools/mingw1310_64"
                  << "E:/Qt/Tools/mingw1120_64"
                  << "E:/Qt/Tools/mingw";
    
    // 常见独立MinGW安装路径
    possiblePaths << "C:/mingw64"
                  << "D:/mingw64"
                  << "C:/MinGW"
                  << "D:/MinGW"
                  << "C:/Program Files/mingw-w64"
                  << "C:/Strawberry/c"
                  << qgetenv("MINGW_HOME");
    
    for (const QString &path : possiblePaths) {
        if (checkPathExists(path)) {
            // 检查是否包含g++编译器
            QString gppPath = path + "/bin/g++.exe";
            if (QFile::exists(gppPath)) {
                return path;
            }
        }
    }
    
    return QString();
}

QString SmartPackageConfig::detectCmakePath() const
{
    // 优先使用用户指定的路径
    QString userCmakePath = "D:/Qt/Tools/CMake_64/bin/cmake.exe";
    if (QFile::exists(userCmakePath)) {
        return QFileInfo(userCmakePath).absolutePath();
    }
    
    // 从环境变量获取
    QString cmakeHome = qgetenv("CMAKE_HOME");
    if (!cmakeHome.isEmpty() && checkPathExists(cmakeHome)) {
        return cmakeHome;
    }
    
    QString pathEnv = qgetenv("PATH");
    QStringList paths = pathEnv.split(";");
    for (const QString &path : paths) {
        if (path.contains("cmake", Qt::CaseInsensitive)) {
            QString cmakeExe = path + "/cmake.exe";
            if (QFile::exists(cmakeExe)) {
                return QFileInfo(cmakeExe).absolutePath();
            }
        }
    }
    
    // 常见CMake安装路径
    QStringList possiblePaths;
    possiblePaths << "C:/Program Files/CMake"
                  << "D:/Program Files/CMake"
                  << "C:/cmake"
                  << "D:/cmake"
                  << QDir::homePath() + "/AppData/Local/Programs/CMake";
    
    for (const QString &path : possiblePaths) {
        QString cmakeExe = path + "/bin/cmake.exe";
        if (QFile::exists(cmakeExe)) {
            return path;
        }
    }
    
    return QString();
}

bool SmartPackageConfig::checkPathExists(const QString &path) const
{
    return QDir(path).exists();
}

QString SmartPackageConfig::executeCommand(const QString &command, const QStringList &args) const
{
    QProcess process;
    process.start(command, args);
    if (!process.waitForFinished(5000)) {
        return QString();
    }
    return QString::fromLocal8Bit(process.readAllStandardOutput());
}

bool SmartPackageConfig::createOutputDirectory(const QString &path) const
{
    QDir dir(path);
    if (!dir.exists()) {
        return dir.mkpath(".");
    }
    return true;
}

SmartPackageConfig::SmartPackageSettings SmartPackageConfig::getDetectedSettings() const
{
    return m_settings;
}

bool SmartPackageConfig::validateConfiguration()
{
    m_lastError.clear();
    
    // 检查Qt路径
    if (m_settings.qtDir.isEmpty()) {
        m_lastError = "Qt路径未设置，请手动设置Qt安装路径";
        return false;
    }
    
    QString qmakePath = m_settings.qtDir + "/bin/qmake.exe";
    if (!QFile::exists(qmakePath)) {
        // 尝试其他可能的qmake路径
        qmakePath = m_settings.qtDir + "/qmake.exe";
        if (!QFile::exists(qmakePath)) {
            m_lastError = "Qt路径无效，未找到qmake.exe，请检查Qt安装路径";
            return false;
        }
    }
    
    // 检查MinGW路径
    if (m_settings.mingwDir.isEmpty()) {
        m_lastError = "MinGW路径未设置，请手动设置MinGW安装路径";
        return false;
    }
    
    QString gppPath = m_settings.mingwDir + "/bin/g++.exe";
    if (!QFile::exists(gppPath)) {
        // 尝试其他可能的g++路径
        gppPath = m_settings.mingwDir + "/g++.exe";
        if (!QFile::exists(gppPath)) {
            m_lastError = "MinGW路径无效，未找到g++.exe，请检查MinGW安装路径";
            return false;
        }
    }
    
    // 检查CMake路径（可选，因为build.bat会处理构建）
    if (!m_settings.cmakeDir.isEmpty()) {
        QString cmakePath = m_settings.cmakeDir + "/bin/cmake.exe";
        if (!QFile::exists(cmakePath)) {
            // 尝试其他可能的cmake路径
            cmakePath = m_settings.cmakeDir + "/cmake.exe";
            if (!QFile::exists(cmakePath)) {
                // 尝试从PATH环境变量查找cmake
                QProcess process;
                process.start("where", {"cmake"});
                if (process.waitForFinished(5000) && process.exitCode() == 0) {
                    QString output = QString::fromLocal8Bit(process.readAllStandardOutput());
                    QStringList paths = output.split("\r\n", Qt::SkipEmptyParts);
                    if (!paths.isEmpty()) {
                        cmakePath = paths.first();
                        m_settings.cmakeDir = QFileInfo(cmakePath).absolutePath();
                    }
                }
            }
        }
    }
    
    return true;
}

// NSIS安装包生成功能
// 检查NSIS工具是否可用
QString SmartPackageConfig::findNsisTool() const
{
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
            logInfo("找到NSIS工具: " + makensisPath);
            break;
        }
    }

    if (makensisPath.isEmpty()) {
        logError("NSIS工具不可用，请安装NSIS工具", false);
    }
    
    return makensisPath;
}

// 生成安装包文件路径
QString SmartPackageConfig::generateInstallerPath(const SmartPackageSettings &settings) const
{
    QString cleanProjectName = settings.uniqueId.isEmpty() ? settings.name : settings.uniqueId;
    cleanProjectName.replace(QRegularExpression("[^a-zA-Z0-9_-]"), "_");
    
    if (cleanProjectName.isEmpty()) {
        cleanProjectName = "SoftwareProject";
    }
    
    return settings.outputDir + "/" + cleanProjectName + "_Setup.exe";
}

// 统一的错误处理和日志输出
void SmartPackageConfig::logError(const QString &errorMessage, bool isCritical) const
{
    if (isCritical) {
        qCritical() << "错误:" << errorMessage;
    } else {
        qWarning() << "警告:" << errorMessage;
    }
}

void SmartPackageConfig::logInfo(const QString &infoMessage) const
{
    qDebug() << "信息:" << infoMessage;
}

void SmartPackageConfig::logProgress(const QString &progressMessage) const
{
    qDebug() << "进度:" << progressMessage;
}

bool SmartPackageConfig::createNsisInstaller(const SmartPackageSettings &settings, const QString &exePath) const
{
    // 检查NSIS工具是否可用
    QString makensisPath = findNsisTool();
    if (makensisPath.isEmpty()) {
        return false;
    }
    
    QProcess nsisCheckProcess;
    nsisCheckProcess.start(makensisPath, {"/VERSION"});
    
    if (!nsisCheckProcess.waitForFinished(5000)) {
        QString error = QString::fromLocal8Bit(nsisCheckProcess.readAllStandardError());
        logError("NSIS工具检查超时，错误信息: " + error, true);
        return false;
    }
    
    if (nsisCheckProcess.exitCode() != 0) {
        QString errorOutput = QString::fromLocal8Bit(nsisCheckProcess.readAllStandardError());
        QString standardOutput = QString::fromLocal8Bit(nsisCheckProcess.readAllStandardOutput());
        logError(QString("NSIS工具执行失败，退出码: %1，错误输出: %2，标准输出: %3")
                 .arg(nsisCheckProcess.exitCode())
                 .arg(errorOutput)
                 .arg(standardOutput), true);
        return false;
    }
    
    logProgress("NSIS工具检查通过，开始生成安装脚本");
    
    // 生成NSIS安装脚本
    QString nsisScript = createNsisScript(settings, exePath);
    
    // 保存NSIS脚本文件
    QDir outputDir(settings.outputDir);
    QString scriptPath = outputDir.absoluteFilePath("installer.nsi");
    
    QFile scriptFile(scriptPath);
    if (!scriptFile.open(QIODevice::WriteOnly)) {
        logError("无法创建NSIS安装脚本文件", true);
        return false;
    }
    
    // 添加UTF-8 BOM并写入UTF-8编码的NSIS脚本
    QByteArray bom;
    bom.append(0xEF);
    bom.append(0xBB);
    bom.append(0xBF);
    scriptFile.write(bom);
    scriptFile.write(nsisScript.toUtf8());
    scriptFile.close();
    
    logProgress("NSIS脚本文件生成完成，开始编译");
    
    // 执行NSIS编译
    QProcess nsisProcess;
    nsisProcess.setWorkingDirectory(outputDir.absolutePath());
    nsisProcess.start(makensisPath, {scriptPath});
    
    if (!nsisProcess.waitForFinished(180000)) { // 3分钟超时
        logError("NSIS编译超时", true);
        return false;
    }
    
    if (nsisProcess.exitCode() != 0) {
        QString errorOutput = QString::fromLocal8Bit(nsisProcess.readAllStandardError());
        logError("NSIS编译失败: " + errorOutput, true);
        return false;
    }
    
    // 检查安装包文件是否实际生成
    QString installerPath = generateInstallerPath(settings);
    
    if (!QFile::exists(installerPath)) {
        logError("NSIS安装包文件未生成: " + installerPath, true);
        return false;
    }
    
    logInfo("NSIS安装包生成成功: " + installerPath);
    return true;
}

QString SmartPackageConfig::createNsisScript(const SmartPackageSettings &settings, const QString &exePath) const
{
    QString script;
    
    // 使用uniqueId生成文件名，避免中文问题，但保留横杠
    QString cleanProjectName = settings.uniqueId.isEmpty() ? settings.name : settings.uniqueId;
    cleanProjectName.replace(QRegularExpression("[^a-zA-Z0-9_-]"), "_");
    
    // 使用QStringList来构建脚本，避免复杂的字符串连接
    QStringList scriptLines;
    
    scriptLines << QString("Name \"%1\"").arg(settings.name);
    scriptLines << QString("OutFile \"%1_Setup.exe\"").arg(cleanProjectName);
    scriptLines << QString("InstallDir \"$PROGRAMFILES\\%1\\%2\"").arg(settings.developer, cleanProjectName);
    scriptLines << "";
    
    scriptLines << "Section \"Main Program\"";
    scriptLines << "    SetOutPath \"$INSTDIR\"";
    scriptLines << QString("    File \"%1\"").arg(QFileInfo(exePath).fileName());
    scriptLines << "    File \"*.dll\"";
    scriptLines << "";
    scriptLines << "    ; 复制Qt插件目录";
    scriptLines << "    SetOutPath \"$INSTDIR\\platforms\"";
    scriptLines << "    File \"platforms\\*.dll\"";
    scriptLines << "    SetOutPath \"$INSTDIR\\styles\"";
    scriptLines << "    File \"styles\\*.dll\"";
    scriptLines << "    SetOutPath \"$INSTDIR\\imageformats\"";
    scriptLines << "    File \"imageformats\\*.dll\"";
    scriptLines << "    SetOutPath \"$INSTDIR\\iconengines\"";
    scriptLines << "    File \"iconengines\\*.dll\"";
    scriptLines << "";
    scriptLines << "    ; 创建开始菜单快捷方式";
    scriptLines << QString("    CreateShortcut \"$SMPROGRAMS\\%1.lnk\" \"$INSTDIR\\%1.exe\"").arg(cleanProjectName);
    scriptLines << "";
    scriptLines << "    ; 创建桌面快捷方式";
    scriptLines << QString("    CreateShortcut \"$DESKTOP\\%1.lnk\" \"$INSTDIR\\%1.exe\"").arg(cleanProjectName);
    scriptLines << "";
    scriptLines << "    ; 写入卸载信息";
    scriptLines << "    WriteUninstaller \"$INSTDIR\\Uninstall.exe\"";
    scriptLines << QString("    WriteRegStr HKLM \"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\%1\" \"DisplayName\" \"%1\"").arg(cleanProjectName);
    scriptLines << QString("    WriteRegStr HKLM \"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\%1\" \"UninstallString\" \"$INSTDIR\\Uninstall.exe\"").arg(cleanProjectName);
    scriptLines << "SectionEnd";
    scriptLines << "";
    scriptLines << "Section \"Uninstall\"";
    scriptLines << "    Delete \"$INSTDIR\\*.*\"";
    scriptLines << "    RMDir /r \"$INSTDIR\"";
    scriptLines << "";
    scriptLines << QString("    Delete \"$SMPROGRAMS\\%1.lnk\"").arg(cleanProjectName);
    scriptLines << QString("    Delete \"$DESKTOP\\%1.lnk\"").arg(cleanProjectName);
    scriptLines << "";
    scriptLines << QString("    DeleteRegKey HKLM \"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\%1\"").arg(cleanProjectName);
    scriptLines << "SectionEnd";
    
    script = scriptLines.join("\n");
    return script;
}

QString SmartPackageConfig::generateSmartCmakeLists(const SmartPackageSettings &settings) const
{
    QString cmakeContent;
    QTextStream stream(&cmakeContent);
    
    // 清理工程名，移除非字母数字下划线字符，但保留横杠
    // 使用uniqueId生成文件名，避免中文问题
    logInfo("generateSmartCmakeLists uniqueId: " + settings.uniqueId);
    QString cleanProjectName = settings.uniqueId.isEmpty() ? settings.name : settings.uniqueId;
    cleanProjectName.replace(QRegularExpression("[^a-zA-Z0-9_-]"), "_");
    if (cleanProjectName.isEmpty()) {
        cleanProjectName = "SoftwareProject";
    }
    
    stream << "cmake_minimum_required(VERSION 3.16)\n";
    stream << "\n";
    stream << "project(" << cleanProjectName << " VERSION " << settings.version << ")\n";
    stream << "\n";
    stream << "# 设置C++标准\n";
    stream << "set(CMAKE_CXX_STANDARD 17)\n";
    stream << "set(CMAKE_CXX_STANDARD_REQUIRED ON)\n";
    stream << "\n";
    stream << "# 查找Qt6\n";
    stream << "find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)\n";
    stream << "\n";
    stream << "# 设置自动处理\n";
    stream << "set(CMAKE_AUTOMOC ON)\n";
    stream << "set(CMAKE_AUTORCC ON)\n";
    stream << "set(CMAKE_AUTOUIC ON)\n";
    stream << "\n";
    stream << "# 添加可执行文件\n";
    stream << "add_executable(" << cleanProjectName << " main.cpp)\n";
    stream << "\n";
    stream << "# 链接Qt库\n";
    stream << "target_link_libraries(" << cleanProjectName << " Qt6::Core Qt6::Widgets Qt6::Gui)\n";
    stream << "\n";
    stream << "# Windows特定设置\n";
    stream << "if(WIN32)\n";
    stream << "    set_target_properties(" << cleanProjectName << " PROPERTIES\n";
    stream << "        WIN32_EXECUTABLE TRUE\n";
    stream << "    )\n";
    stream << "endif()\n";
    
    return cmakeContent;
}

QString SmartPackageConfig::generateSmartMainCpp(const SmartPackageSettings &settings) const
{
    // 清理工程名，移除非字母数字下划线字符，但保留横杠
    // 使用uniqueId生成文件名，避免中文问题
    QString cleanProjectName = settings.uniqueId.isEmpty() ? settings.name : settings.uniqueId;
    cleanProjectName.replace(QRegularExpression("[^a-zA-Z0-9_-]"), "_");
    if (cleanProjectName.isEmpty()) {
        cleanProjectName = "SoftwareProject";
    }
    
    // 检查是否有UI布局文件路径
    if (!settings.uiLayoutPath.isEmpty() && QFile::exists(settings.uiLayoutPath)) {
        // 根据UI布局文件生成main.cpp
        return generateMainCppFromUILayout(settings);
    } else {
        // 生成默认的简单UI
        QString mainContent = 
            "#include <QApplication>\n"
            "#include <QWidget>\n"
            "#include <QVBoxLayout>\n"
            "#include <QLabel>\n"
            "#include <QPushButton>\n"
            "#include <QMessageBox>\n\n"
            "int main(int argc, char *argv[])\n"
            "{\n"
            "    QApplication app(argc, argv);\n\n"
            "    // 创建主窗口\n"
            "    QWidget window;\n"
            "    window.setWindowTitle(\"" + cleanProjectName + "\");\n"
            "    window.resize(400, 300);\n\n"
            "    // 创建布局\n"
            "    QVBoxLayout *layout = new QVBoxLayout(&window);\n\n"
            "    // 添加标题\n"
            "    QLabel *titleLabel = new QLabel(\"" + cleanProjectName + "\");\n"
            "    titleLabel->setAlignment(Qt::AlignCenter);\n"
            "    titleLabel->setStyleSheet(\"font-size: 24px; font-weight: bold;\");\n"
            "    layout->addWidget(titleLabel);\n\n"
            "    // 添加版本信息\n"
            "    QLabel *versionLabel = new QLabel(\"版本: " + settings.version + "\");\n"
            "    versionLabel->setAlignment(Qt::AlignCenter);\n"
            "    layout->addWidget(versionLabel);\n\n"
            "    // 添加开发者信息\n"
            "    QLabel *developerLabel = new QLabel(\"开发者: " + settings.developer + "\");\n"
            "    developerLabel->setAlignment(Qt::AlignCenter);\n"
            "    layout->addWidget(developerLabel);\n\n"
            "    // 添加描述\n"
            "    QLabel *descLabel = new QLabel(\"描述："+ settings.description + "\");\n"
            "    descLabel->setWordWrap(true);\n"
            "    descLabel->setAlignment(Qt::AlignCenter);\n"
            "    layout->addWidget(descLabel);\n"
            "    // 添加按钮\n"
            "    QPushButton *button = new QPushButton(\"关于\");\n"
            "    layout->addWidget(button);\n\n"
            "    // 连接按钮信号\n"
            "    QObject::connect(button, &QPushButton::clicked, [&]() {\n"
            "        QMessageBox::about(&window, \"关于\", \n"
            "            \"软件名称: " + cleanProjectName + "\\n\"\n"
            "            \"版本: " + settings.version + "\\n\"\n"
            "            \"开发者: " + settings.developer + "\\n\"\n"
            "            \"描述: " + settings.description + "\");\n"
            "    });\n\n"
            "    layout->addStretch();\n"
            "    window.show();\n\n"
            "    return app.exec();\n"
            "}\n";
        
        return mainContent;
    }
}

QString SmartPackageConfig::generateMainCppFromUILayout(const SmartPackageSettings &settings) const
{
    // 检查UI布局文件是否存在
    if (settings.uiLayoutPath.isEmpty() || !QFile::exists(settings.uiLayoutPath)) {
        throw QString("UI布局文件不存在: %1").arg(settings.uiLayoutPath);
    }
    
    // 读取UI布局XML文件
    QFile layoutFile(settings.uiLayoutPath);
    if (!layoutFile.open(QIODevice::ReadOnly)) {
        throw QString("无法打开UI布局文件: %1").arg(settings.uiLayoutPath);
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
    codeLines << "#include <QRadioButton>";
    codeLines << "#include <QCheckBox>";
    codeLines << "#include <QComboBox>";
    codeLines << "#include <QSpinBox>";
    codeLines << "#include <QSlider>";
    codeLines << "#include <QProgressBar>";
    codeLines << "#include <QGroupBox>";
    codeLines << "#include <QFrame>";
    codeLines << "#include <QScrollArea>";
    codeLines << "#include <QListWidget>";
    codeLines << "#include <QTreeWidget>";
    codeLines << "#include <QTableWidget>";
    codeLines << "#include <QToolButton>";
    codeLines << "#include <QToolBar>";
    codeLines << "#include <QMenuBar>";
    codeLines << "#include <QStatusBar>";
    codeLines << "#include <QDockWidget>";
    codeLines << "#include <QSplitter>";
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
    codeLines << QString("        setWindowTitle(\"%1\");").arg(settings.name);
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
        } else if (className == "QRadioButton") {
            codeLines << QString("    QRadioButton *%1;").arg(name);
        } else if (className == "QCheckBox") {
            codeLines << QString("    QCheckBox *%1;").arg(name);
        } else if (className == "QComboBox") {
            codeLines << QString("    QComboBox *%1;").arg(name);
        } else if (className == "QSpinBox") {
            codeLines << QString("    QSpinBox *%1;").arg(name);
        } else if (className == "QSlider") {
            codeLines << QString("    QSlider *%1;").arg(name);
        } else if (className == "QProgressBar") {
            codeLines << QString("    QProgressBar *%1;").arg(name);
        } else if (className == "QGroupBox") {
            codeLines << QString("    QGroupBox *%1;").arg(name);
        } else if (className == "QFrame") {
            codeLines << QString("    QFrame *%1;").arg(name);
        } else if (className == "QScrollArea") {
            codeLines << QString("    QScrollArea *%1;").arg(name);
        } else if (className == "QListWidget") {
            codeLines << QString("    QListWidget *%1;").arg(name);
        } else if (className == "QTreeWidget") {
            codeLines << QString("    QTreeWidget *%1;").arg(name);
        } else if (className == "QTableWidget") {
            codeLines << QString("    QTableWidget *%1;").arg(name);
        } else if (className == "QToolButton") {
            codeLines << QString("    QToolButton *%1;").arg(name);
        } else if (className == "QToolBar") {
            codeLines << QString("    QToolBar *%1;").arg(name);
        } else if (className == "QMenuBar") {
            codeLines << QString("    QMenuBar *%1;").arg(name);
        } else if (className == "QStatusBar") {
            codeLines << QString("    QStatusBar *%1;").arg(name);
        } else if (className == "QDockWidget") {
            codeLines << QString("    QDockWidget *%1;").arg(name);
        } else if (className == "QSplitter") {
            codeLines << QString("    QSplitter *%1;").arg(name);
        } else {
            // 对于未知的控件类型，使用QWidget作为基类
            codeLines << QString("    QWidget *%1;").arg(name);
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
                QDomElement tabElement = tabNodes.at(j).toElement();
                if (tabElement.isNull()) continue;
                
                QString tabName = tabElement.attribute("name");
                QString tabText;
                QDomElement tabProperty = tabElement.firstChildElement("property");
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
        } else if (className == "QRadioButton") {
            codeLines << QString("    %1 = new QRadioButton(\"%2\", this);").arg(name, text);
        } else if (className == "QCheckBox") {
            codeLines << QString("    %1 = new QCheckBox(\"%2\", this);").arg(name, text);
        } else if (className == "QComboBox") {
            codeLines << QString("    %1 = new QComboBox(this);").arg(name);
            if (!text.isEmpty()) {
                codeLines << QString("    %1->addItem(\"%2\");").arg(name, text);
            }
        } else if (className == "QSpinBox") {
            codeLines << QString("    %1 = new QSpinBox(this);").arg(name);
        } else if (className == "QSlider") {
            codeLines << QString("    %1 = new QSlider(Qt::Horizontal, this);").arg(name);
        } else if (className == "QProgressBar") {
            codeLines << QString("    %1 = new QProgressBar(this);").arg(name);
        } else if (className == "QGroupBox") {
            codeLines << QString("    %1 = new QGroupBox(\"%2\", this);").arg(name, text);
        } else if (className == "QFrame") {
            codeLines << QString("    %1 = new QFrame(this);").arg(name);
        } else if (className == "QScrollArea") {
            codeLines << QString("    %1 = new QScrollArea(this);").arg(name);
        } else if (className == "QListWidget") {
            codeLines << QString("    %1 = new QListWidget(this);").arg(name);
            if (!text.isEmpty()) {
                codeLines << QString("    %1->addItem(\"%2\");").arg(name, text);
            }
        } else if (className == "QTreeWidget") {
            codeLines << QString("    %1 = new QTreeWidget(this);").arg(name);
        } else if (className == "QTableWidget") {
            codeLines << QString("    %1 = new QTableWidget(this);").arg(name);
        } else if (className == "QToolButton") {
            codeLines << QString("    %1 = new QToolButton(this);").arg(name);
            if (!text.isEmpty()) {
                codeLines << QString("    %1->setText(\"%2\");").arg(name, text);
            }
        } else if (className == "QToolBar") {
            codeLines << QString("    %1 = new QToolBar(this);").arg(name);
        } else if (className == "QMenuBar") {
            codeLines << QString("    %1 = new QMenuBar(this);").arg(name);
        } else if (className == "QStatusBar") {
            codeLines << QString("    %1 = new QStatusBar(this);").arg(name);
        } else if (className == "QDockWidget") {
            codeLines << QString("    %1 = new QDockWidget(\"%2\", this);").arg(name, text);
        } else if (className == "QSplitter") {
            codeLines << QString("    %1 = new QSplitter(Qt::Horizontal, this);").arg(name);
        } else {
            // 对于未知的控件类型，使用QWidget作为基类
            codeLines << QString("    %1 = new QWidget(this);").arg(name);
        }
        
        // 获取控件的几何位置信息
        QDomElement geometryElement = widget.firstChildElement("property");
        int x = 0, y = 0, width = 100, height = 30;
        
        while (!geometryElement.isNull()) {
            if (geometryElement.attribute("name") == "geometry") {
                QDomElement rectElement = geometryElement.firstChildElement("rect");
                if (!rectElement.isNull()) {
                    QDomElement xElement = rectElement.firstChildElement("x");
                    QDomElement yElement = rectElement.firstChildElement("y");
                    QDomElement widthElement = rectElement.firstChildElement("width");
                    QDomElement heightElement = rectElement.firstChildElement("height");
                    
                    if (!xElement.isNull()) x = xElement.text().toInt();
                    if (!yElement.isNull()) y = yElement.text().toInt();
                    if (!widthElement.isNull()) width = widthElement.text().toInt();
                    if (!heightElement.isNull()) height = heightElement.text().toInt();
                }
                break;
            }
            geometryElement = geometryElement.nextSiblingElement("property");
        }
        
        // 设置控件的几何位置
        codeLines << QString("    %1->setGeometry(%2, %3, %4, %5);").arg(name).arg(x).arg(y).arg(width).arg(height);
        codeLines << "";
    }
    
    codeLines << "}";
    codeLines << "";
    
    // main函数
    codeLines << "int main(int argc, char *argv[])";
    codeLines << "{";
    codeLines << "    QApplication app(argc, argv);";
    codeLines << "";
    codeLines << QString("    %1Window window;").arg(settings.name);
    codeLines << "    window.show();";
    codeLines << "";
    codeLines << "    return app.exec();";
    codeLines << "}";
    
    // 添加moc文件包含以处理Q_OBJECT宏
    codeLines << "";
    codeLines << "#include \"main.moc\"";
    
    return codeLines.join("\n");
}

QString SmartPackageConfig::generateBuildBat(const SmartPackageSettings &settings) const
{
    // 清理工程名，移除非字母数字下划线字符，但保留横杠
    // 使用uniqueId生成文件名，避免中文问题
    QString cleanProjectName = settings.uniqueId.isEmpty() ? settings.name : settings.uniqueId;
    cleanProjectName.replace(QRegularExpression("[^a-zA-Z0-9_-]"), "_");
    if (cleanProjectName.isEmpty()) {
        cleanProjectName = "SoftwareProject";
    }
    
    QString buildBatContent = 
        "@echo off\n"
        "setlocal enabledelayedexpansion\n\n"
        "REM 设置Qt安装路径\n"
        "set QT_DIR=" + settings.qtDir + "\n"
        "echo Debug: QT_DIR = %QT_DIR%\n\n"
        "REM 配置MinGW环境\n"
        "set MINGW_DIR=" + settings.mingwDir + "\n"
        "echo Debug: MINGW_DIR = %MINGW_DIR%\n"
        "set PATH=%MINGW_DIR%\\bin;%PATH%\n\n"
        "REM 设置CMake路径（可选）\n"
        "set CMAKEDIR=" + settings.cmakeDir + "\n"
        "echo Debug: CMAKEDIR = %CMAKEDIR%\n\n"
        "REM 创建构建目录\n"
        "set BUILD_DIR=build\n"
        "if not exist \"%BUILD_DIR%\" mkdir \"%BUILD_DIR%\"\n"
        "cd \"%BUILD_DIR%\"\n"
        "echo Debug: Current directory is %CD%\n\n"
        "REM 运行CMake配置项目\n"
        "echo Debug: Running CMake configuration...\n"
        "if \"%CMAKEDIR%\"==\"\" (\n"
        "    cmake -G \"MinGW Makefiles\" -DCMAKE_PREFIX_PATH=\"%QT_DIR%\" -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ ..\n"
        ") else (\n"
        "    \"%CMAKEDIR%\\cmake.exe\" -G \"MinGW Makefiles\" -DCMAKE_PREFIX_PATH=\"%QT_DIR%\" -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ ..\n"
        ")\n"
        "echo Debug: CMake configuration returned %errorlevel%\n\n"
        "if %errorlevel% neq 0 (\n"
        "    echo CMake configuration failed\n"
        "    exit /b 1\n"
        ")\n\n"
        "REM 构建项目\n"
        "echo Debug: Building project...\n"
        "mingw32-make\n\n"
        "if %errorlevel% neq 0 (\n"
        "    echo Build failed\n"
        "    exit /b 1\n"
        ")\n\n"
        "REM 复制生成的可执行文件到上级目录\n"
        "if exist \"" + cleanProjectName + ".exe\" (\n"
        "    copy \"" + cleanProjectName + ".exe\" ..\\\n"
        ")\n"
        "if exist \"Release\\" + cleanProjectName + ".exe\" (\n"
        "    copy \"Release\\" + cleanProjectName + ".exe\" ..\\\n"
        ")\n\n"
        "echo Build succeeded\n"
        "cd ..\n"
        "endlocal\n";
    
    return buildBatContent;
}

bool SmartPackageConfig::executeSmartPackage(const SmartPackageSettings &settings)
{
    // 保存设置到成员变量
    m_settings = settings;
    
    // 不再验证配置，直接生成build.bat文件
    // 即使路径有问题，也允许用户生成build.bat文件，让用户自己去解决构建问题
    logInfo("跳过配置验证，直接生成build.bat文件");
    
    // 创建输出目录
    QString outputDir = settings.outputDir;
    if (outputDir.isEmpty()) {
        // 使用uniqueId生成输出目录，避免中文路径问题，但保留横杠
        QString cleanProjectName = settings.uniqueId.isEmpty() ? settings.name : settings.uniqueId;
        cleanProjectName.replace(QRegularExpression("[^a-zA-Z0-9_-]"), "_");
        if (cleanProjectName.isEmpty()) {
            cleanProjectName = "SoftwareProject";
        }
        outputDir = QDir::currentPath() + "/smart_packages/" + cleanProjectName + "_" + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    }
    
    QDir dir(outputDir);
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            m_lastError = "无法创建输出目录: " + outputDir;
            logError(m_lastError, true);
            return false;
        }
    }
    
    emit progressChanged(10, "正在创建项目文件...");
    
    // 生成CMakeLists.txt
    QString cmakeListsPath = outputDir + "/CMakeLists.txt";
    QFile cmakeFile(cmakeListsPath);
    if (!cmakeFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        m_lastError = "无法创建CMakeLists.txt文件";
        return false;
    }
    
    QString cmakeLists = generateSmartCmakeLists(settings);
    cmakeFile.write(cmakeLists.toUtf8());
    cmakeFile.close();
    
    emit progressChanged(20, "CMakeLists.txt生成完成");
    
    // 复制UI布局文件（如果存在）
    if (!settings.uiLayoutPath.isEmpty() && QFile::exists(settings.uiLayoutPath)) {
        QString uiFileName = QFileInfo(settings.uiLayoutPath).fileName();
        QString uiDestPath = outputDir + "/" + uiFileName;
        
        if (QFile::copy(settings.uiLayoutPath, uiDestPath)) {
            logInfo("UI布局文件复制成功: " + uiDestPath);
            emit progressChanged(25, "UI布局文件复制完成");
        } else {
            logError("UI布局文件复制失败: " + settings.uiLayoutPath + " -> " + uiDestPath, false);
        }
    }
    
    // 生成main.cpp
    QString mainCppPath = outputDir + "/main.cpp";
    QFile mainFile(mainCppPath);
    if (!mainFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        m_lastError = "无法创建main.cpp文件";
        logError(m_lastError, true);
        return false;
    }
    
    QString mainCpp;
    // 根据uiLayoutPath是否存在决定使用哪种方式生成main.cpp
    if (!settings.uiLayoutPath.isEmpty() && QFile::exists(settings.uiLayoutPath)) {
        try {
            mainCpp = generateMainCppFromUILayout(settings);
            logInfo("使用UI布局文件生成main.cpp");
        } catch (const QString &error) {
            logError("UI布局文件生成失败: " + error, false);
            mainCpp = generateSmartMainCpp(settings);
        }
    } else {
        mainCpp = generateSmartMainCpp(settings);
        logInfo("使用默认方式生成main.cpp");
    }
    
    mainFile.write(mainCpp.toUtf8());
    mainFile.close();
    
    emit progressChanged(30, "main.cpp生成完成");
    
    // 生成build.bat
    QString buildBatPath = outputDir + "/build.bat";
    QFile batFile(buildBatPath);
    if (!batFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        m_lastError = "无法创建build.bat文件";
        logError(m_lastError, true);
        return false;
    }
    
    QString buildBat = generateBuildBat(settings);
    batFile.write(buildBat.toUtf8());
    batFile.close();
    
    emit progressChanged(40, "build.bat生成完成");
    
    // 执行编译过程
    emit progressChanged(50, "开始编译项目...");
    
    QProcess buildProcess;
    buildProcess.setWorkingDirectory(outputDir);
    buildProcess.start("cmd", QStringList() << "/c" << "build.bat");
    
    if (!buildProcess.waitForStarted()) {
        m_lastError = "无法启动编译进程";
        logError(m_lastError, true);
        emit progressChanged(0, "编译启动失败");
        return false;
    }
    
    // 等待编译完成
    if (!buildProcess.waitForFinished(300000)) { // 5分钟超时
        m_lastError = "编译过程超时";
        logError(m_lastError, true);
        emit progressChanged(0, "编译超时");
        return false;
    }
    
    int exitCode = buildProcess.exitCode();
    if (exitCode != 0) {
        m_lastError = "编译失败，退出代码: " + QString::number(exitCode);
        logError(m_lastError, true);
        emit progressChanged(0, "编译失败");
        return false;
    }
    
    emit progressChanged(80, "编译完成，检查EXE文件...");
    
    // 检查EXE文件是否生成成功
    // 优先使用uniqueId作为文件名，确保EXE文件名与产品配置一致，但保留横杠
    QString cleanProjectName = settings.uniqueId.isEmpty() ? settings.name : settings.uniqueId;
    cleanProjectName.replace(QRegularExpression("[^a-zA-Z0-9_-]"), "_");
    if (cleanProjectName.isEmpty()) {
        cleanProjectName = "SoftwareProject";
    }
    
    QString exePath = outputDir + "/" + cleanProjectName + ".exe";
    if (!QFile::exists(exePath)) {
        exePath = outputDir + "/build/" + cleanProjectName + ".exe";
    }
    if (!QFile::exists(exePath)) {
        exePath = outputDir + "/build/Release/" + cleanProjectName + ".exe";
    }
    
    if (!QFile::exists(exePath)) {
        m_lastError = "编译成功但未生成EXE文件，请检查编译日志";
        logError(m_lastError, true);
        emit progressChanged(0, "EXE文件未生成");
        return false;
    }
    
    emit progressChanged(90, "EXE文件生成成功");
    logInfo("EXE文件生成成功: " + exePath);
    
    // 复制依赖文件（如果需要）
    if (settings.includeDependencies) {
        emit progressChanged(95, "正在复制依赖文件...");
        
        if (QFile::exists(exePath)) {
            logInfo("找到可执行文件: " + exePath);
            if (!copyDependencies(exePath, outputDir)) {
                logError("依赖复制失败，但继续打包过程", false);
            }
        } else {
            logError("警告：未找到可执行文件，跳过依赖复制", false);
            m_lastError = "警告：构建过程未生成可执行文件，请检查编译日志";
        }
    }
    
    // 设置应用程序图标（如果需要）
    if (!settings.iconPath.isEmpty() && QFile::exists(settings.iconPath)) {
        QString finalExePath = outputDir + "/" + cleanProjectName + ".exe";
        if (QFile::exists(finalExePath)) {
            if (!setApplicationIcon(finalExePath, settings.iconPath)) {
                logError("图标设置失败，但继续打包过程", false);
            }
        }
    }
    
    // 生成NSIS安装包（如果需要）
    QString installerPath = "";
    if (settings.createInstaller) {
        emit progressChanged(97, "正在生成NSIS安装包...");
        
        if (createNsisInstaller(settings, exePath)) {
            // 检查安装包是否生成成功
            installerPath = generateInstallerPath(settings);
            
            if (QFile::exists(installerPath)) {
                // 不再重复输出日志，createNsisInstaller函数中已经输出过
                emit progressChanged(100, "NSIS安装包生成完成");
            } else {
                logError("NSIS安装包生成失败，文件不存在: " + installerPath, true);
                m_lastError = "NSIS安装包生成失败";
                return false;
            }
        } else {
            logError("NSIS安装包生成失败: " + m_lastError, true);
            return false;
        }
    }
    
    // 统一处理打包完成信号
    if (settings.createInstaller) {
        // 用户要求生成安装包
        emit progressChanged(100, "打包完成");
        logInfo("打包完成，安装包路径: " + installerPath);
        // 直接使用installerPath，因为NSIS安装包生成成功时installerPath一定不为空
        emit packageFinished(true, installerPath);
    } else {
        // 用户未要求生成安装包
        logInfo("用户未要求生成安装包，跳过安装包生成步骤");
        emit progressChanged(100, "打包完成（未生成安装包）");
        emit packageFinished(true, exePath);
    }
    
    return true;
}

QString SmartPackageConfig::getLastError() const
{
    return m_lastError;
}

// 手动设置工具路径
void SmartPackageConfig::setQtPath(const QString &path)
{
    m_settings.qtDir = path;
}

void SmartPackageConfig::setMingwPath(const QString &path)
{
    m_settings.mingwDir = path;
}

void SmartPackageConfig::setCmakePath(const QString &path)
{
    m_settings.cmakeDir = path;
}

// 获取当前设置的工具路径
QString SmartPackageConfig::getQtPath() const
{
    return m_settings.qtDir;
}

QString SmartPackageConfig::getMingwPath() const
{
    return m_settings.mingwDir;
}

QString SmartPackageConfig::getCmakePath() const
{
    return m_settings.cmakeDir;
}

// 检测可用的CMake生成器
QStringList SmartPackageConfig::detectAvailableCmakeGenerators() const
{
    QStringList availableGenerators;
    QStringList allGenerators = {
        "MinGW Makefiles",
        "Ninja", 
        "Visual Studio 17 2022",
        "Visual Studio 16 2019",
        "Visual Studio 15 2017",
        "Unix Makefiles",
        "CodeBlocks - MinGW Makefiles",
        "CodeBlocks - Unix Makefiles"
    };
    
    for (const QString &generator : allGenerators) {
        if (testCmakeGenerator(generator)) {
            availableGenerators.append(generator);
        }
    }
    
    return availableGenerators;
}

// 测试CMake生成器是否可用
bool SmartPackageConfig::testCmakeGenerator(const QString &generator) const
{
    QProcess process;
    process.start("cmake", {"--help"});
    
    if (!process.waitForFinished(10000)) {
        return false;
    }
    
    QString output = QString::fromLocal8Bit(process.readAllStandardOutput());
    return output.contains(generator);
}

bool SmartPackageConfig::copyDependencies(const QString &exePath, const QString &outputDir) const
{
    // 使用packagemanager中的实现，避免重复代码
    PackageWorker worker;
    bool success = worker.copyDependencies(exePath, outputDir);
    
    if (success) {
        logInfo("依赖复制完成");
    } else {
        logError("依赖复制失败", false);
    }
    
    return success;
}

bool SmartPackageConfig::setApplicationIcon(const QString &exePath, const QString &iconPath) const
{
    // 使用packagemanager中的完整实现，避免重复代码
    PackageWorker worker;
    worker.setApplicationIcon(exePath, iconPath);
    
    // 由于setApplicationIcon是void函数，我们假设总是成功
    // 在实际使用中，可以通过检查文件修改时间等方式验证
    return true;
}