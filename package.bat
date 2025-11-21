@echo off
setlocal enabledelayedexpansion

echo ========================================
echo 软件打包脚本
echo ========================================

REM 设置Qt和MinGW路径
set "QT_DIR=D:\Qt\6.9.1\mingw_64"
set "MINGW_DIR=D:\Qt\Tools\mingw1310_64"
set "CMAKE_DIR=D:\Qt\Tools\CMake_64"

REM 检查必要的工具是否存在
if not exist "%QT_DIR%" (
    echo 错误: Qt目录不存在: %QT_DIR%
    echo 请检查Qt安装路径
    pause
    exit /b 1
)

if not exist "%MINGW_DIR%" (
    echo 错误: MinGW目录不存在: %MINGW_DIR%
    echo 请检查MinGW安装路径
    pause
    exit /b 1
)

if not exist "%CMAKE_DIR%" (
    echo 错误: CMake目录不存在: %CMAKE_DIR%
    echo 请检查CMake安装路径
    pause
    exit /b 1
)

REM 设置环境变量
set "PATH=%MINGW_DIR%\bin;%CMAKE_DIR%\bin;%PATH%"

REM 设置打包输出目录
set "PACKAGE_DIR=packages"
if not exist "%PACKAGE_DIR%" mkdir "%PACKAGE_DIR%"

REM 生成时间戳用于唯一标识
for /f "tokens=1-3 delims=: " %%a in ('echo %date% %time%') do (
    set "TIMESTAMP=%%a%%b%%c_%%d%%e%%f"
)
set "TIMESTAMP=%TIMESTAMP:/=%"
set "TIMESTAMP=%TIMESTAMP: =_%"

set "OUTPUT_DIR=%PACKAGE_DIR%\打包产品_%TIMESTAMP%"

echo 创建输出目录: %OUTPUT_DIR%
if not exist "%OUTPUT_DIR%" mkdir "%OUTPUT_DIR%"

REM 复制必要的文件到打包目录
echo 复制项目文件...
copy "CMakeLists.txt" "%OUTPUT_DIR%" >nul 2>&1

REM 创建简化的CMakeLists.txt用于打包
(
echo cmake_minimum_required(VERSION 3.16)
echo.
echo project(打包产品_%TIMESTAMP%)
echo.
echo set(CMAKE_CXX_STANDARD 17)
echo set(CMAKE_CXX_STANDARD_REQUIRED ON)
echo.
echo # 查找Qt6
echo find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
echo.
echo # 设置自动包含moc文件
echo set(CMAKE_AUTOMOC ON)
echo set(CMAKE_AUTORCC ON)
echo set(CMAKE_AUTOUIC ON)
echo.
echo # 添加可执行文件
echo add_executable(打包产品_%TIMESTAMP% main.cpp)
echo.
echo # 链接Qt库
echo target_link_libraries(打包产品_%TIMESTAMP% Qt6::Core Qt6::Widgets)
echo.
echo # Windows特定设置
echo if(WIN32)
echo     # 设置子系统为Windows
echo     set_target_properties(打包产品_%TIMESTAMP% PROPERTIES WIN32_EXECUTABLE TRUE)
echo endif()
) > "%OUTPUT_DIR%\CMakeLists.txt"

REM 创建简单的main.cpp文件
(
echo #include <QApplication>
echo #include <QMainWindow>
echo #include <QLabel>
echo #include <QVBoxLayout>
echo #include <QWidget>
echo.
echo int main(int argc, char *argv[])
echo {
echo     QApplication app(argc, argv);
echo.
echo     QMainWindow window;
echo     QWidget *centralWidget = new QWidget(&window);
echo     QVBoxLayout *layout = new QVBoxLayout(centralWidget);
echo.
echo     QLabel *label = new QLabel("软件打包产品 - %TIMESTAMP%", centralWidget);
echo     label->setAlignment(Qt::AlignCenter);
echo     layout->addWidget(label);
echo.
echo     window.setCentralWidget(centralWidget);
echo     window.setWindowTitle("打包产品");
echo     window.resize(400, 300);
echo     window.show();
echo.
echo     return app.exec();
echo }
) > "%OUTPUT_DIR%\main.cpp"

REM 进入输出目录并执行CMake配置
cd "%OUTPUT_DIR%"

echo.
echo 执行CMake配置...
"%CMAKE_DIR%\bin\cmake.exe" -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="%QT_DIR%" -DCMAKE_BUILD_TYPE=Release ..

if %errorlevel% neq 0 (
    echo CMake配置失败
    echo 尝试使用备用配置...
    
    REM 尝试备用配置
    "%CMAKE_DIR%\bin\cmake.exe" -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="%QT_DIR%" -DCMAKE_BUILD_TYPE=Release .
    
    if %errorlevel% neq 0 (
        echo CMake配置仍然失败，请检查环境配置
        cd ..
        pause
        exit /b 1
    )
)

echo CMake配置成功

REM 执行构建
echo.
echo 执行构建过程...
mingw32-make

if %errorlevel% neq 0 (
    echo 构建失败
    cd ..
    pause
    exit /b 1
)

echo.
echo ========================================
echo 打包成功完成！
echo 输出目录: %OUTPUT_DIR%
echo 可执行文件: %OUTPUT_DIR%\打包产品_%TIMESTAMP%.exe
echo ========================================

REM 返回上级目录
cd ..

REM 显示打包结果
dir "%OUTPUT_DIR%"

echo.
echo 打包过程已完成。按任意键退出...
pause >nul