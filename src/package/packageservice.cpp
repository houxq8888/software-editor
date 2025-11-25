#include "packageservice.h"
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QDateTime>
#include <QTextStream>
#include <QCoreApplication>
#include <QRegularExpression>
#include <QDomDocument>
#include <QDirIterator>

PackageService::PackageService(QObject *parent)
    : IPackageService(parent)
{
    // 使用默认配置
    m_config = PackageConfig::defaultConfig();
    logInfo("PackageService initialized with default configuration");
}

PackageService::~PackageService()
{
    logInfo("PackageService destroyed");
}

bool PackageService::detectDevelopmentTools()
{
    emit progressChanged(10, "开始检测开发工具路径...");
    
    // 检测Qt路径
    QString qtPath = detectQtPath();
    if (qtPath.isEmpty()) {
        emit progressChanged(20, "警告: 未检测到Qt安装路径，请手动设置");
    } else {
        m_config.qtDir = qtPath;
        emit progressChanged(30, "Qt路径检测完成: " + qtPath);
    }
    
    // 检测MinGW路径
    QString mingwPath = detectMingwPath();
    if (mingwPath.isEmpty()) {
        emit progressChanged(40, "警告: 未检测到MinGW安装路径，请手动设置");
    } else {
        m_config.mingwDir = mingwPath;
        emit progressChanged(50, "MinGW路径检测完成: " + mingwPath);
    }
    
    // 检测CMake路径
    QString cmakePath = detectCmakePath();
    if (cmakePath.isEmpty()) {
        emit progressChanged(60, "警告: 未检测到CMake安装路径，请手动设置");
    } else {
        m_config.cmakeDir = cmakePath;
        emit progressChanged(70, "CMake路径检测完成: " + cmakePath);
    }
    
    emit progressChanged(100, "开发工具检测完成");
    return true;
}

QString PackageService::detectQtPath() const
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

QString PackageService::detectMingwPath() const
{
    // 优先查找Qt Tools目录下的MinGW工具路径
    QStringList possiblePaths;
    
    // 从Qt安装路径推断Tools目录
    if (!m_config.qtDir.isEmpty()) {
        QFileInfo qtInfo(m_config.qtDir);
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

QString PackageService::detectCmakePath() const
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

PackageConfig PackageService::getCurrentConfig() const
{
    return m_config;
}

void PackageService::updateConfig(const PackageConfig &config)
{
    m_config = config;
    logInfo("Configuration updated");
}

bool PackageService::loadConfig(const QString &configName)
{
    // 配置管理功能暂未实现，直接返回成功
    logInfo("Configuration loading not implemented, using default configuration");
    return true;
}

bool PackageService::saveConfig(const QString &configName)
{
    // 配置管理功能暂未实现，直接返回成功
    logInfo("Configuration saving not implemented");
    return true;
}

QStringList PackageService::getConfigList() const
{
    // 配置管理功能暂未实现，返回空列表
    return QStringList();
}

bool PackageService::deleteConfig(const QString &configName)
{
    // 配置管理功能暂未实现，直接返回成功
    logInfo("Configuration deletion not implemented");
    return true;
}

bool PackageService::validateConfiguration()
{
    if (m_config.qtDir.isEmpty()) {
        m_lastError = "Qt路径未设置";
        return false;
    }
    
    if (m_config.mingwDir.isEmpty()) {
        m_lastError = "MinGW路径未设置";
        return false;
    }
    
    if (m_config.cmakeDir.isEmpty()) {
        m_lastError = "CMake路径未设置";
        return false;
    }
    
    if (m_config.outputDir.isEmpty()) {
        m_lastError = "输出目录未设置";
        return false;
    }
    
    return true;
}

QString PackageService::generateCmakeLists(const PackageConfig &config) const
{
    QString cmakeContent;
    QTextStream stream(&cmakeContent);
    
    // 清理工程名，移除非字母数字下划线字符，但保留横杠
    // 使用uniqueId生成文件名，避免中文问题
    const_cast<PackageService*>(this)->logInfo("generateSmartCmakeLists uniqueId: " + config.uniqueId);
    QString cleanProjectName = config.uniqueId.isEmpty() ? config.name : config.uniqueId;
    cleanProjectName.replace(QRegularExpression("[^a-zA-Z0-9_-]"), "_");
    if (cleanProjectName.isEmpty()) {
        cleanProjectName = "SoftwareProject";
    }
    
    stream << "cmake_minimum_required(VERSION 3.16)\n";
    stream << "\n";
    stream << "project(" << cleanProjectName << " VERSION " << config.version << ")\n";
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

QString PackageService::generateMainCpp(const PackageConfig &config) const
{
    QString mainCpp;
    
    mainCpp += "#include <QApplication>\n";
    mainCpp += "#include <QWidget>\n";
    mainCpp += "#include <QLabel>\n";
    mainCpp += "#include <QVBoxLayout>\n\n";
    
    mainCpp += "int main(int argc, char *argv[])\n";
    mainCpp += "{\n";
    mainCpp += "    QApplication app(argc, argv);\n\n";
    
    mainCpp += "    // 创建主窗口\n";
    mainCpp += "    QWidget window;\n";
    mainCpp += "    window.setWindowTitle(\"" + config.name + "\");\n";
    mainCpp += "    window.resize(400, 300);\n\n";
    
    mainCpp += "    // 创建布局\n";
    mainCpp += "    QVBoxLayout *layout = new QVBoxLayout(&window);\n\n";
    
    mainCpp += "    // 添加标签\n";
    mainCpp += "    QLabel *label = new QLabel(\"欢迎使用 " + config.name + " - " + config.version + "\");\n";
    mainCpp += "    label->setAlignment(Qt::AlignCenter);\n";
    mainCpp += "    layout->addWidget(label);\n\n";
    
    mainCpp += "    // 显示窗口\n";
    mainCpp += "    window.show();\n\n";
    
    mainCpp += "    return app.exec();\n";
    mainCpp += "}\n";
    
    return mainCpp;
}

QString PackageService::generateMainCppFromUILayout(const PackageConfig &config) const
{
    // 检查UI布局文件是否存在
    if (config.uiLayoutPath.isEmpty() || !QFile::exists(config.uiLayoutPath)) {
        return generateMainCpp(config);
    }
    
    // 读取UI布局文件
    QFile layoutFile(config.uiLayoutPath);
    if (!layoutFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // 在const方法中调用const版本的logError
        const_cast<PackageService*>(this)->logError("无法打开UI布局文件: " + config.uiLayoutPath, false);
        return generateMainCpp(config);
    }
    
    QDomDocument doc;
    QString errorMsg;
    int errorLine, errorColumn;
    
    // 使用新的setContent方法调用方式（Qt 6兼容）
    QString xmlContent = layoutFile.readAll();
    layoutFile.close();
    
    // 使用Qt 6推荐的setContent方法（避免弃用警告）
    QDomDocument::ParseResult result = doc.setContent(xmlContent);
    if (!result) {
        const_cast<PackageService*>(this)->logError(QString("UI布局文件XML格式错误: %1 (行%2, 列%3)").arg(result.errorMessage).arg(result.errorLine).arg(result.errorColumn), false);
        return generateMainCpp(config);
    }
    
    // 获取根元素
    QDomElement root = doc.documentElement();
    if (root.isNull()) {
        const_cast<PackageService*>(this)->logError("UI布局文件格式错误: 缺少根元素", false);
        return generateMainCpp(config);
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
    codeLines << QString("class %1Window : public QMainWindow").arg(config.name);
    codeLines << "{";
    codeLines << "    Q_OBJECT";
    codeLines << "";
    codeLines << "public:";
    codeLines << QString("    %1Window(QWidget *parent = nullptr) : QMainWindow(parent)").arg(config.name);
    codeLines << "    {";
    codeLines << "        setupUI();";
    codeLines << QString("        setWindowTitle(\"%1\");").arg(config.name);
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
    codeLines << QString("void %1Window::setupUI()").arg(config.name);
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
    codeLines << QString("    %1Window window;").arg(config.name);
    codeLines << "    window.show();";
    codeLines << "";
    codeLines << "    return app.exec();";
    codeLines << "}";
    
    // 添加moc文件包含以处理Q_OBJECT宏
    codeLines << "";
    codeLines << "#include \"main.moc\"";

    return codeLines.join("\n");
}

bool PackageService::executePackage(const PackageConfig &config)
{
    m_config = config;
    
    // 跳过配置验证，直接生成build.bat文件（对标smartpackageconfig逻辑）
    logInfo("跳过配置验证，直接生成build.bat文件");
    
    // 创建输出目录（使用uniqueId避免中文路径问题）
    QString outputDir = config.outputDir;
    if (outputDir.isEmpty()) {
        // 使用uniqueId生成输出目录，避免中文路径问题，但保留横杠
        QString cleanProjectName = config.uniqueId.isEmpty() ? config.name : config.uniqueId;
        cleanProjectName.replace(QRegularExpression("[^a-zA-Z0-9_-]"), "_");
        if (cleanProjectName.isEmpty()) {
            cleanProjectName = "SoftwareProject";
        }
        outputDir = QDir::currentPath() + "/packages/" + cleanProjectName + "_" + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    }
    
    QDir dir(outputDir);
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            m_lastError = "无法创建输出目录: " + outputDir;
            logError(m_lastError, true);
            emit errorOccurred(m_lastError);
            return false;
        }
    }
    
    emit progressChanged(10, "正在创建项目文件...");
    
    // 生成CMakeLists.txt文件
    QString cmakeFilePath = outputDir + "/CMakeLists.txt";
    QString cmakeContent = generateCmakeLists(config);
    
    QFile cmakeFile(cmakeFilePath);
    if (!cmakeFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        m_lastError = "无法创建CMakeLists.txt文件";
        logError(m_lastError, true);
        emit errorOccurred(m_lastError);
        return false;
    }
    
    QTextStream cmakeStream(&cmakeFile);
    cmakeStream << cmakeContent;
    cmakeFile.close();
    
    emit progressChanged(20, "CMakeLists.txt生成完成");
    
    // 生成main.cpp文件
    QString mainCppFilePath = outputDir + "/main.cpp";
    QString mainCppContent = generateMainCppFromUILayout(config);
    
    QFile mainCppFile(mainCppFilePath);
    if (!mainCppFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        m_lastError = "无法创建main.cpp文件";
        logError(m_lastError, true);
        emit errorOccurred(m_lastError);
        return false;
    }
    
    QTextStream mainCppStream(&mainCppFile);
    mainCppStream << mainCppContent;
    mainCppFile.close();
    
    emit progressChanged(30, "main.cpp生成完成");
    
    // 生成build.bat文件（对标smartpackageconfig逻辑）
    QString buildBatPath = outputDir + "/build.bat";
    QFile batFile(buildBatPath);
    if (!batFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        m_lastError = "无法创建build.bat文件";
        logError(m_lastError, true);
        emit errorOccurred(m_lastError);
        return false;
    }
    
    QString buildBat = generateBuildBat(config);
    batFile.write(buildBat.toUtf8());
    batFile.close();
    
    emit progressChanged(40, "build.bat生成完成");
    
    // 执行编译过程（对标smartpackageconfig逻辑）
    emit progressChanged(50, "开始编译项目...");
    
    QProcess buildProcess;
    buildProcess.setWorkingDirectory(outputDir);
    buildProcess.start("cmd", QStringList() << "/c" << "build.bat");
    
    if (!buildProcess.waitForStarted()) {
        m_lastError = "无法启动编译进程";
        logError(m_lastError, true);
        emit progressChanged(0, "编译启动失败");
        emit errorOccurred(m_lastError);
        return false;
    }
    
    // 等待编译完成（5分钟超时）
    if (!buildProcess.waitForFinished(300000)) {
        m_lastError = "编译过程超时";
        logError(m_lastError, true);
        emit progressChanged(0, "编译超时");
        emit errorOccurred(m_lastError);
        return false;
    }
    
    int exitCode = buildProcess.exitCode();
    if (exitCode != 0) {
        m_lastError = "编译失败，退出代码: " + QString::number(exitCode);
        logError(m_lastError, true);
        emit progressChanged(0, "编译失败");
        emit errorOccurred(m_lastError);
        return false;
    }
    
    emit progressChanged(80, "编译完成，检查EXE文件...");
    
    // 检查EXE文件是否生成成功（对标smartpackageconfig逻辑）
    QString cleanProjectName = config.uniqueId.isEmpty() ? config.name : config.uniqueId;
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
        emit errorOccurred(m_lastError);
        return false;
    }
    
    emit progressChanged(90, "EXE文件生成成功");
    logInfo("EXE文件生成成功: " + exePath);
    
    // 复制依赖文件（如果需要）
    if (config.includeDependencies) {
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
    if (!config.iconPath.isEmpty() && QFile::exists(config.iconPath)) {
        QString finalExePath = outputDir + "/" + cleanProjectName + ".exe";
        if (QFile::exists(finalExePath)) {
            if (!setApplicationIcon(finalExePath, config.iconPath)) {
                logError("图标设置失败，但继续打包过程", false);
            }
        }
    }
    
    // 生成NSIS安装包（如果需要）
    QString installerPath = "";
    if (config.createInstaller) {
        emit progressChanged(97, "正在生成NSIS安装包...");
        
        if (createNsisInstaller(config, exePath)) {
            // 检查安装包是否生成成功
            installerPath = generateInstallerPath(config);
            
            if (QFile::exists(installerPath)) {
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
    
    // 统一处理打包完成信号（对标smartpackageconfig逻辑）
    if (config.createInstaller) {
        // 用户要求生成安装包
        emit progressChanged(100, "打包完成");
        logInfo("打包完成，安装包路径: " + installerPath);
        emit packageFinished(true, installerPath);
    } else {
        // 用户未要求生成安装包
        logInfo("用户未要求生成安装包，跳过安装包生成步骤");
        emit progressChanged(100, "打包完成");
        emit packageFinished(true, exePath);
    }
    
    return true;
}

bool PackageService::createNsisInstaller(const PackageConfig &config, const QString &exePath) const
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
    QString nsisScript = createNsisScript(config, exePath);
    
    // 保存NSIS脚本文件
    QDir outputDir(config.outputDir);
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
    QString installerPath = generateInstallerPath(config);
    qDebug()<<"installPath:"<<installerPath;
    if (!QFile::exists(installerPath)) {
        logError("NSIS安装包文件未生成: " + installerPath, true);
        return false;
    }
    
    logInfo("NSIS安装包生成成功: " + installerPath);
    return true;
}

bool PackageService::copyDependencies(const QString &exePath, const QString &outputDir)
{
        if (!QFile::exists(exePath)) {
        logError("可执行文件不存在: " + exePath, true);
        return false;
    }
    
    QDir outputDirObj(outputDir);
    if (!outputDirObj.exists()) {
        if (!outputDirObj.mkpath(".")) {
            logError("无法创建输出目录: " + outputDir, true);
            return false;
        }
    }
    
    bool success = true;
    
    // 1. 使用windeployqt工具复制Qt依赖
    logInfo("开始复制Qt依赖...");
    QProcess deployProcess;
    
    // 使用正确的windeployqt路径
    QString windeployqtPath = "D:/Qt/6.9.1/mingw_64/bin/windeployqt.exe";
    
    // 检查windeployqt是否可用
    deployProcess.start(windeployqtPath, {"--version"});
    if (!deployProcess.waitForFinished(5000)) {
        logError("windeployqt工具不可用，将跳过Qt依赖复制", false);
        success = false;
    } else {
        // 重新执行windeployqt复制依赖
        deployProcess.start(windeployqtPath, {"--release", "--no-compiler-runtime", "--dir", outputDir, exePath});
        
        if (!deployProcess.waitForFinished(180000)) { // 3分钟超时
            logError("复制Qt依赖文件超时", false);
            success = false;
        } else if (deployProcess.exitCode() != 0) {
            QString errorOutput = QString::fromLocal8Bit(deployProcess.readAllStandardError());
            logError("复制Qt依赖文件失败: " + errorOutput, false);
            success = false;
        } else {
            logInfo("Qt依赖复制完成");
        }
    }
    
    // 2. 复制MinGW运行时依赖
    logInfo("开始复制MinGW运行时依赖...");
    QStringList mingwDlls = {
        "libgcc_s_seh-1.dll",
        "libstdc++-6.dll", 
        "libwinpthread-1.dll"
    };
    
    // 自动检测MinGW路径
    QString mingwBinDir = findMingwBinDir();
    
    if (mingwBinDir.isEmpty()) {
        logError("未找到MinGW安装路径，将跳过MinGW依赖复制", false);
        success = false;
    } else {
        logInfo("使用MinGW路径: " + mingwBinDir);
        
        for (const QString &dllName : mingwDlls) {
            QString sourcePath = mingwBinDir + "/" + dllName;
            QString destPath = outputDir + "/" + dllName;
            
            if (QFile::exists(sourcePath) && !QFile::exists(destPath)) {
                if (QFile::copy(sourcePath, destPath)) {
                    logInfo("复制MinGW DLL: " + dllName);
                } else {
                    logError("无法复制MinGW DLL: " + dllName, false);
                    success = false;
                }
            }
        }
    }
    
    // 3. 复制系统依赖（如果缺少）
    logInfo("检查系统依赖...");
    QStringList systemDlls = {
        "VCRUNTIME140.dll",
        "VCRUNTIME140_1.dll", 
        "MSVCP140.dll",
        "ucrtbase.dll"
    };
    
    for (const QString &dllName : systemDlls) {
        QString destPath = outputDir + "/" + dllName;
        if (!QFile::exists(destPath)) {
            logError("缺少系统DLL: " + dllName + "（需要安装Visual C++ Redistributable）", false);
        }
    }
    
    // 4. 复制Qt插件
    logInfo("开始复制Qt插件...");
    QStringList qtPlugins = {
        "platforms",
        "styles", 
        "imageformats",
        "iconengines"
    };
    
    // 自动检测Qt插件路径
    QString qtPluginsDir = findQtPluginsDir();
    
    if (qtPluginsDir.isEmpty()) {
        logError("未找到Qt插件路径，将跳过Qt插件复制", false);
        success = false;
    } else {
        logInfo("使用Qt插件路径: " + qtPluginsDir);
        
        for (const QString &pluginDir : qtPlugins) {
            QString sourceDir = qtPluginsDir + "/" + pluginDir;
            QString destDir = outputDir + "/" + pluginDir;
            
            if (QDir(sourceDir).exists() && !QDir(destDir).exists()) {
                if (copyDirectory(sourceDir, destDir)) {
                    logInfo("复制Qt插件目录: " + pluginDir);
                } else {
                    logError("无法复制Qt插件目录: " + pluginDir, false);
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
            logInfo("复制主程序到输出目录");
        } else {
            logError("无法复制主程序到输出目录", false);
            success = false;
        }
    }
    
    // 6. 生成依赖报告
    logInfo("生成依赖报告...");
    generateDependencyReport(exePath, outputDir);
    
    if (success) {
        logInfo("依赖复制完成");
    } else {
        logError("依赖复制过程中出现错误", false);
    }
    
    return success;
}

bool PackageService::setApplicationIcon(const QString &exePath, const QString &iconPath)
{
    // 这里实现图标设置逻辑
    logInfo("设置应用程序图标...");
    return true;
}

QString PackageService::getLastError() const
{
    return m_lastError;
}

// 内部工具函数实现
bool PackageService::checkPathExists(const QString &path) const
{
    return QFileInfo::exists(path);
}

QString PackageService::executeCommand(const QString &command, const QStringList &args) const
{
    QProcess process;
    process.start(command, args);
    if (!process.waitForFinished(5000)) {
        return QString();
    }
    return QString::fromLocal8Bit(process.readAllStandardOutput());
}

bool PackageService::createOutputDirectory(const QString &path) const
{
    QDir dir(path);
    if (!dir.exists()) {
        return dir.mkpath(".");
    }
    return true;
}

bool PackageService::copyDirectory(const QString &sourceDir, const QString &destinationDir)
{
    QDir source(sourceDir);
    if (!source.exists()) {
        return false;
    }
    
    QDir destination(destinationDir);
    if (!destination.exists()) {
        destination.mkpath(".");
    }
    
    // 这里实现目录复制逻辑
    return true;
}

QString PackageService::createNsisScript(const PackageConfig &config, const QString &exePath) const
{
    QString script;
    
    // NSIS脚本头部定义
    script += "!include \"MUI2.nsh\"\n";
    script += "!include \"FileFunc.nsh\"\n";
    script += "!include \"WordFunc.nsh\"\n";
    script += "\n";
    
    // 基本设置 - 统一使用uniqueId进行匹配
    QString identifier = config.uniqueId.isEmpty() ? config.name : config.uniqueId;
    script += "Name \"" + identifier + "\"\n";
    script += "OutFile \"" + identifier + "_" + config.version + "_Setup.exe\"\n";
    script += "InstallDir \"$PROGRAMFILES\\" + identifier + "\"\n";
    script += "RequestExecutionLevel admin\n";
    script += "\n";
    
    // 定义变量
    script += "Var StartMenuFolder\n";
    script += "\n";
    
    // MUI界面设置
    script += "!define MUI_ABORTWARNING\n";
    script += "!define MUI_ICON \"${NSISDIR}\\Contrib\\Graphics\\Icons\\modern-install.ico\"\n";
    script += "!define MUI_UNICON \"${NSISDIR}\\Contrib\\Graphics\\Icons\\modern-uninstall.ico\"\n";
    script += "\n";
    
    // 欢迎页面
    script += "!insertmacro MUI_PAGE_WELCOME\n";
    
    // 许可证页面（如果提供了许可证文件）
    if (!config.licenseFile.isEmpty() && QFile::exists(config.licenseFile)) {
        script += "!insertmacro MUI_PAGE_LICENSE \"" + config.licenseFile + "\"\n";
    }
    
    // 目录选择页面
    script += "!insertmacro MUI_PAGE_DIRECTORY\n";
    
    // 开始菜单文件夹选择
    script += "!define MUI_STARTMENUPAGE_REGISTRY_ROOT \"HKLM\"\n";
    script += "!define MUI_STARTMENUPAGE_REGISTRY_KEY \"Software\\" + identifier + "\"\n";
    script += "!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME \"Start Menu Folder\"\n";
    script += "!insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder\n";
    
    // 安装文件页面
    script += "!insertmacro MUI_PAGE_INSTFILES\n";
    
    // 完成页面
    script += "!define MUI_FINISHPAGE_RUN \"$INSTDIR\\" + QFileInfo(exePath).fileName() + "\"\n";
    script += "!define MUI_FINISHPAGE_RUN_TEXT \"运行" + config.name + "\"\n";
    script += "!insertmacro MUI_PAGE_FINISH\n";
    
    // 卸载页面
    script += "!insertmacro MUI_UNPAGE_CONFIRM\n";
    script += "!insertmacro MUI_UNPAGE_INSTFILES\n";
    
    // 语言设置
    script += "!insertmacro MUI_LANGUAGE \"SimpChinese\"\n";
    script += "\n";
    
    // 安装节
    script += "Section \"Main\" SEC01\n";
    script += "    SetOutPath \"$INSTDIR\"\n";
    script += "    \n";
    
    // 复制主程序文件
    script += "    File \"" + QFileInfo(exePath).fileName() + "\"\n";
    
    // 复制根目录下的所有DLL文件（Qt核心依赖）
    QDir outputDir(config.outputDir);
    QStringList dllFiles = outputDir.entryList(QStringList("*.dll"), QDir::Files);
    for (const QString &dllFile : dllFiles) {
        if (dllFile != QFileInfo(exePath).fileName()) { // 排除主程序文件
            script += "    File \"" + dllFile + "\"\n";
        }
    }
    
    // 复制依赖文件（如果存在依赖目录）
    QString dependenciesDir = outputDir.absoluteFilePath("dependencies");
    if (QDir(dependenciesDir).exists()) {
        script += "    SetOutPath \"$INSTDIR\\dependencies\"\n";
        script += "    File /r \"dependencies\\*.*\"\n";
    }
    
    // 复制Qt插件目录（platforms、styles、imageformats等）
    QStringList qtPluginDirs = {"platforms", "styles", "imageformats", "iconengines"};
    for (const QString &pluginDir : qtPluginDirs) {
        QString pluginPath = outputDir.absoluteFilePath(pluginDir);
        if (QDir(pluginPath).exists()) {
            script += "    SetOutPath \"$INSTDIR\\" + pluginDir + "\"\n";
            script += "    File /r \"" + pluginDir + "\\*.*\"\n";
        }
    }
    
    // 创建开始菜单快捷方式
    script += "    !insertmacro MUI_STARTMENU_WRITE_BEGIN Application\n";
    script += "    CreateDirectory \"$SMPROGRAMS\\$StartMenuFolder\"\n";
    script += "    CreateShortcut \"$SMPROGRAMS\\$StartMenuFolder\\" + identifier + ".lnk\" \"$INSTDIR\\" + QFileInfo(exePath).fileName() + "\"\n";
    script += "    CreateShortcut \"$SMPROGRAMS\\$StartMenuFolder\\卸载" + identifier + ".lnk\" \"$INSTDIR\\uninstall.exe\"\n";
    script += "    !insertmacro MUI_STARTMENU_WRITE_END\n";
    
    // 创建桌面快捷方式
    script += "    CreateShortcut \"$DESKTOP\\" + identifier + ".lnk\" \"$INSTDIR\\" + QFileInfo(exePath).fileName() + "\"\n";
    
    // 写入卸载信息
    script += "    WriteUninstaller \"$INSTDIR\\uninstall.exe\"\n";
    script += "    WriteRegStr HKLM \"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\" + identifier + "\" \"DisplayName\" \"" + config.name + "\"\n";
    script += "    WriteRegStr HKLM \"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\" + identifier + "\" \"UninstallString\" \"$INSTDIR\\uninstall.exe\"\n";
    script += "    WriteRegStr HKLM \"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\" + identifier + "\" \"DisplayIcon\" \"$INSTDIR\\" + QFileInfo(exePath).fileName() + "\"\n";
    script += "    WriteRegStr HKLM \"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\" + identifier + "\" \"Publisher\" \"" + config.developer + "\"\n";
    script += "    WriteRegDWORD HKLM \"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\" + identifier + "\" \"NoModify\" 1\n";
    script += "    WriteRegDWORD HKLM \"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\" + identifier + "\" \"NoRepair\" 1\n";
    
    script += "SectionEnd\n";
    script += "\n";
    
    // 卸载节
    script += "Section \"Uninstall\"\n";
    script += "    !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder\n";
    script += "    \n";
    
    // 删除文件
    script += "    Delete \"$INSTDIR\\" + QFileInfo(exePath).fileName() + "\"\n";
    script += "    Delete \"$INSTDIR\\uninstall.exe\"\n";
    
    // 删除根目录下的所有DLL文件（Qt核心依赖）
    QStringList uninstallDllFiles = outputDir.entryList(QStringList("*.dll"), QDir::Files);
    for (const QString &dllFile : uninstallDllFiles) {
        if (dllFile != QFileInfo(exePath).fileName()) { // 排除主程序文件
            script += "    Delete \"$INSTDIR\\" + dllFile + "\"\n";
        }
    }
    
    // 删除依赖目录
    script += "    RMDir /r \"$INSTDIR\\dependencies\"\n";
    
    // 删除Qt插件目录
    for (const QString &pluginDir : qtPluginDirs) {
        script += "    RMDir /r \"$INSTDIR\\" + pluginDir + "\"\n";
    }
    
    // 删除快捷方式
    script += "    Delete \"$DESKTOP\\" + identifier + ".lnk\"\n";
    script += "    Delete \"$SMPROGRAMS\\$StartMenuFolder\\" + identifier + ".lnk\"\n";
    script += "    Delete \"$SMPROGRAMS\\$StartMenuFolder\\卸载" + identifier + ".lnk\"\n";
    script += "    RMDir \"$SMPROGRAMS\\$StartMenuFolder\"\n";
    
    // 删除安装目录
    script += "    RMDir \"$INSTDIR\"\n";
    
    // 删除注册表信息
    script += "    DeleteRegKey HKLM \"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\" + identifier + "\"\n";
    script += "    DeleteRegKey HKLM \"Software\\" + identifier + "\"\n";
    
    script += "SectionEnd\n";
    
    return script;
}

QString PackageService::findNsisTool() const
{
    // 检查常见NSIS安装路径
    QStringList possiblePaths;
    
    // 从环境变量获取
    QString nsisDir = qEnvironmentVariable("NSISDIR");
    if (!nsisDir.isEmpty()) {
        possiblePaths << nsisDir + "/makensis.exe";
        possiblePaths << nsisDir + "/makensis";
    }
    
    // 常见安装路径
    possiblePaths << "C:/Program Files (x86)/NSIS/makensis.exe";
    possiblePaths << "C:/Program Files (x86)/NSIS/makensis";
    possiblePaths << "C:/Program Files/NSIS/makensis.exe";
    possiblePaths << "C:/Program Files/NSIS/makensis";
    possiblePaths << "D:/Program Files (x86)/NSIS/makensis.exe";
    possiblePaths << "D:/Program Files (x86)/NSIS/makensis";
    possiblePaths << "D:/Program Files/NSIS/makensis.exe";
    possiblePaths << "D:/Program Files/NSIS/makensis";
    
    // 检查PATH环境变量
    QString pathEnv = qEnvironmentVariable("PATH");
    QStringList pathDirs = pathEnv.split(";");
    for (const QString &dir : pathDirs) {
        if (dir.contains("NSIS", Qt::CaseInsensitive)) {
            possiblePaths << dir + "/makensis.exe";
            possiblePaths << dir + "/makensis";
        }
    }
    
    // 检查当前目录和父目录
    QDir currentDir(QDir::currentPath());
    possiblePaths << currentDir.absoluteFilePath("makensis.exe");
    possiblePaths << currentDir.absoluteFilePath("makensis");
    
    QDir parentDir(currentDir.absolutePath() + "/..");
    possiblePaths << parentDir.absoluteFilePath("makensis.exe");
    possiblePaths << parentDir.absoluteFilePath("makensis");
    
    // 检查系统路径
    possiblePaths << "makensis.exe";
    possiblePaths << "makensis";
    
    for (const QString &path : possiblePaths) {
        if (QFile::exists(path)) {
            logInfo("找到NSIS工具: " + path);
            return path;
        }
    }
    
    logError("未找到NSIS工具，请确保NSIS已正确安装并配置环境变量");
    return QString();
}

QString PackageService::generateInstallerPath(const PackageConfig &config) const
{
    QDir outputDir(config.outputDir);
    
    // 生成安装包文件名，格式：uniqueId_版本号_Setup.exe
    // 优先使用uniqueId，如果为空则使用name
    QString identifier = config.uniqueId.isEmpty() ? config.name : config.uniqueId;
    QString installerName = identifier + "_" + config.version + "_Setup.exe";
    
    // 清理文件名中的非法字符
    installerName.replace("/", "-");
    installerName.replace("\\", "-");
    installerName.replace(":", "-");
    installerName.replace("*", "-");
    installerName.replace("?", "-");
    installerName.replace("\"", "-");
    installerName.replace("<", "-");
    installerName.replace(">", "-");
    installerName.replace("|", "-");
    
    return outputDir.absoluteFilePath(installerName);
}

QString PackageService::generateDependencyReport(const QString &exePath, const QString &outputDir)
{
    // 这里实现依赖报告生成逻辑
    return QString();
}

QString PackageService::findMingwBinDir()
{
    return m_config.mingwDir + "/bin";
}

QString PackageService::findQtPluginsDir()
{
    return m_config.qtDir + "/plugins";
}

void PackageService::logError(const QString &message, bool isCritical) const
{
    const_cast<PackageService*>(this)->m_lastError = message;
    if (isCritical) {
        emit const_cast<PackageService*>(this)->errorOccurred(message);
    }
}

void PackageService::logInfo(const QString &message) const
{
    emit const_cast<PackageService*>(this)->progressChanged(-1, message);
}

void PackageService::logProgress(const QString &message) const
{
    emit const_cast<PackageService*>(this)->progressChanged(-1, message);
}

bool PackageService::executeSmartPackageLogic(const PackageConfig &config, const QString &uiLayoutPath)
{
    // 这里实现智能打包逻辑
    logInfo("执行智能打包逻辑...");
    return true;
}

QString PackageService::generateCppCodeFromUILayout(const QString &uiLayoutPath, const PackageConfig &config)
{
    // 这里实现从UI布局生成C++代码的逻辑
    return generateMainCpp(config);
}

void PackageService::generateCppProjectFiles(const PackageConfig &config, const QString &cppCode, const QString &outputDir)
{
    // 这里实现C++项目文件生成逻辑
    logInfo("生成C++项目文件...");
}

void PackageService::cancelPackage()
{
    // 这里实现取消打包的逻辑
    logInfo("取消打包操作...");
    emit progressChanged(0, "打包已取消");
    emit packageFinished(false, "打包被用户取消");
}

QString PackageService::generateBuildBat(const PackageConfig &config) const
{
    // 清理工程名，移除非字母数字下划线字符，但保留横杠
    // 使用uniqueId生成文件名，避免中文问题
    QString cleanProjectName = config.uniqueId.isEmpty() ? config.name : config.uniqueId;
    cleanProjectName.replace(QRegularExpression("[^a-zA-Z0-9_-]"), "_");
    if (cleanProjectName.isEmpty()) {
        cleanProjectName = "SoftwareProject";
    }
    
    QString buildBatContent = 
        "@echo off\n"
        "setlocal enabledelayedexpansion\n\n"
        "REM 设置Qt安装路径\n"
        "set QT_DIR=" + config.qtDir + "\n"
        "echo Debug: QT_DIR = %QT_DIR%\n\n"
        "REM 配置MinGW环境\n"
        "set MINGW_DIR=" + config.mingwDir + "\n"
        "echo Debug: MINGW_DIR = %MINGW_DIR%\n"
        "set PATH=%MINGW_DIR%\\bin;%PATH%\n\n"
        "REM 设置CMake路径（可选）\n"
        "set CMAKEDIR=" + config.cmakeDir + "\n"
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