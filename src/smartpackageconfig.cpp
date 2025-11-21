#include "smartpackageconfig.h"
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QStandardPaths>
#include <QDateTime>
#include <QTextStream>
#include <QCoreApplication>
#include <QRegularExpression>

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

QString SmartPackageConfig::generateSmartCmakeLists(const SmartPackageSettings &settings) const
{
    QString cmakeContent;
    QTextStream stream(&cmakeContent);
    
    // 清理工程名，移除中文字符
    QString cleanProjectName = settings.name;
    cleanProjectName.replace(QRegularExpression("[^a-zA-Z0-9_]"), "_");
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
    // 清理工程名，移除中文字符
    QString cleanProjectName = settings.name;
    cleanProjectName.replace(QRegularExpression("[^a-zA-Z0-9_]"), "_");
    if (cleanProjectName.isEmpty()) {
        cleanProjectName = "SoftwareProject";
    }
    
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
        "    if (!settings.description.isEmpty()) {\n"
        "        QLabel *descLabel = new QLabel(settings.description);\n"
        "        descLabel->setWordWrap(true);\n"
        "        descLabel->setAlignment(Qt::AlignCenter);\n"
        "        layout->addWidget(descLabel);\n"
        "    }\n\n"
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

QString SmartPackageConfig::generateBuildBat(const SmartPackageSettings &settings) const
{
    // 清理工程名，移除中文字符
    QString cleanProjectName = settings.name;
    cleanProjectName.replace(QRegularExpression("[^a-zA-Z0-9_]"), "_");
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
    qDebug()<<"跳过配置验证，直接生成build.bat文件";
    
    // 创建输出目录
    QString outputDir = settings.outputDir;
    if (outputDir.isEmpty()) {
        // 清理工程名，移除非字母数字下划线字符
        QString cleanProjectName = settings.name;
        cleanProjectName.replace(QRegularExpression("[^a-zA-Z0-9_]"), "_");
        if (cleanProjectName.isEmpty()) {
            cleanProjectName = "SoftwareProject";
        }
        outputDir = QDir::currentPath() + "/smart_packages/" + cleanProjectName + "_" + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    }
    
    QDir dir(outputDir);
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            m_lastError = "无法创建输出目录: " + outputDir;
            return false;
        }
    }
    
    // 生成CMakeLists.txt
    QString cmakeListsPath = outputDir + "/CMakeLists.txt";
    QFile cmakeFile(cmakeListsPath);
    if (!cmakeFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        m_lastError = "无法创建CMakeLists.txt文件";
        return false;
    }
    
    QTextStream cmakeStream(&cmakeFile);
    cmakeStream << generateSmartCmakeLists(settings);
    cmakeFile.close();
    
    // 生成main.cpp
    QString mainCppPath = outputDir + "/main.cpp";
    QFile mainFile(mainCppPath);
    if (!mainFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        m_lastError = "无法创建main.cpp文件";
        return false;
    }
    
    QTextStream mainStream(&mainFile);
    mainStream << generateSmartMainCpp(settings);
    mainFile.close();
    
    // 生成build.bat
    QString buildBatPath = outputDir + "/build.bat";
    QFile buildFile(buildBatPath);
    if (!buildFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        m_lastError = "无法创建build.bat文件";
        return false;
    }
    
    QTextStream buildStream(&buildFile);
    buildStream << generateBuildBat(settings);
    buildFile.close();
    
    // 设置环境变量
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("QT_DIR", settings.qtDir);
    env.insert("MINGW_DIR", settings.mingwDir);
    
    // 执行构建
    QProcess buildProcess;
    buildProcess.setProcessEnvironment(env);
    buildProcess.setWorkingDirectory(outputDir);
    
    // 在Windows上使用cmd执行build.bat
    buildProcess.start("cmd", QStringList() << "/c" << "build.bat");
    
    if (!buildProcess.waitForStarted()) {
        m_lastError = "无法启动构建进程";
        return false;
    }
    
    // 等待构建完成（设置超时时间）
    if (!buildProcess.waitForFinished(300000)) { // 5分钟超时
        buildProcess.kill();
        m_lastError = "构建超时";
        return false;
    }
    
    // 检查构建结果
    if (buildProcess.exitCode() != 0) {
        m_lastError = "构建失败，退出码: " + QString::number(buildProcess.exitCode()) + 
                     "\n错误输出: " + QString::fromLocal8Bit(buildProcess.readAllStandardError());
        return false;
    }
    
    // 检查生成的可执行文件
    QString cleanProjectName = settings.name;
    cleanProjectName.replace(QRegularExpression("[^a-zA-Z0-9_]"), "_");
    if (cleanProjectName.isEmpty()) {
        cleanProjectName = "SoftwareProject";
    }
    
    QString exePath = outputDir + "/" + cleanProjectName + ".exe";
    if (!QFile::exists(exePath)) {
        // 尝试在build子目录中查找
        exePath = outputDir + "/build/" + cleanProjectName + ".exe";
        if (!QFile::exists(exePath)) {
            // 尝试在Release子目录中查找
            exePath = outputDir + "/build/Release/" + cleanProjectName + ".exe";
            if (!QFile::exists(exePath)) {
                m_lastError = "未找到生成的可执行文件";
                return false;
            }
        }
    }
    
    // 复制可执行文件到输出目录根目录
    QString finalExePath = outputDir + "/" + cleanProjectName + ".exe";
    if (exePath != finalExePath) {
        if (QFile::exists(finalExePath)) {
            QFile::remove(finalExePath);
        }
        if (!QFile::copy(exePath, finalExePath)) {
            m_lastError = "无法复制可执行文件到目标位置";
            return false;
        }
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
    // 简化的依赖复制逻辑
    QFileInfo exeInfo(exePath);
    QString exeDir = exeInfo.absolutePath();
    
    // 复制Qt DLL文件
    QStringList qtDlls = {"Qt6Core.dll", "Qt6Gui.dll", "Qt6Widgets.dll"};
    
    for (const QString &dllName : qtDlls) {
        QString srcPath = m_settings.qtDir + "/bin/" + dllName;
        QString dstPath = outputDir + "/" + dllName;
        
        if (QFile::exists(srcPath)) {
            QFile::copy(srcPath, dstPath);
        }
    }
    
    return true;
}

bool SmartPackageConfig::setApplicationIcon(const QString &exePath, const QString &iconPath) const
{
    // 简化的图标设置逻辑
    if (iconPath.isEmpty() || !QFile::exists(iconPath)) {
        return false;
    }
    
    // 在实际实现中，这里需要使用资源编辑器工具来设置图标
    // 这里返回true表示功能已预留
    return true;
}