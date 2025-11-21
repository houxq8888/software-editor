@echo off
setlocal enabledelayedexpansion

echo ========================================
echo 智能软件打包脚本
echo ========================================

REM 自动检测Qt安装路径
set "QT_FOUND=0"
set "QT_DIR="

REM 尝试常见的Qt安装路径
for %%Q in (
    "D:\Qt\6.9.1\mingw_64"
    "D:\Qt\6.8.3\mingw_64"
    "D:\Qt\6.7.0\mingw_64"
    "C:\Qt\6.9.1\mingw_64"
    "C:\Qt\6.8.3\mingw_64"
    "C:\Qt\6.7.0\mingw_64"
) do (
    if exist "%%~Q" (
        set "QT_DIR=%%~Q"
        set "QT_FOUND=1"
        echo 找到Qt安装目录: !QT_DIR!
        goto :QT_FOUND
    )
)

:QT_FOUND
if "!QT_FOUND!"=="0" (
    echo 错误: 未找到Qt安装目录
    echo 请手动设置QT_DIR环境变量
    pause
    exit /b 1
)

REM 自动检测MinGW路径
set "MINGW_DIR="
if exist "!QT_DIR!\..\..\Tools\mingw1310_64" (
    set "MINGW_DIR=!QT_DIR!\..\..\Tools\mingw1310_64"
) else if exist "!QT_DIR!\..\..\Tools\mingw1120_64" (
    set "MINGW_DIR=!QT_DIR!\..\..\Tools\mingw1120_64"
) else if exist "!QT_DIR!\..\..\Tools\mingw" (
    set "MINGW_DIR=!QT_DIR!\..\..\Tools\mingw"
)

if "!MINGW_DIR!"=="" (
    echo 警告: 未找到MinGW目录，尝试使用系统PATH中的MinGW
)

REM 自动检测CMake路径
set "CMAKE_DIR="
if exist "!QT_DIR!\..\..\Tools\CMake_64" (
    set "CMAKE_DIR=!QT_DIR!\..\..\Tools\CMake_64"
) else if exist "C:\Program Files\CMake" (
    set "CMAKE_DIR=C:\Program Files\CMake"
) else if exist "C:\Program Files (x86)\CMake" (
    set "CMAKE_DIR=C:\Program Files (x86)\CMake"
)

if "!CMAKE_DIR!"=="" (
    echo 警告: 未找到CMake目录，尝试使用系统PATH中的CMake
)

REM 设置环境变量
if not "!MINGW_DIR!"=="" (
    set "PATH=!MINGW_DIR!\bin;!PATH!"
)

if not "!CMAKE_DIR!"=="" (
    set "PATH=!CMAKE_DIR!\bin;!PATH!"
)

REM 检查必要的工具
where cmake >nul 2>&1
if %errorlevel% neq 0 (
    echo 错误: 系统中未找到CMake
    pause
    exit /b 1
)

where gcc >nul 2>&1
if %errorlevel% neq 0 (
    echo 错误: 系统中未找到GCC编译器
    pause
    exit /b 1
)

REM 设置打包输出目录
set "PACKAGE_DIR=packages"
if not exist "%PACKAGE_DIR%" mkdir "%PACKAGE_DIR%"

REM 生成时间戳
for /f "tokens=1-6 delims=: /- " %%a in ('echo %date% %time%') do (
    set "YEAR=%%a"
    set "MONTH=%%b"
    set "DAY=%%c"
    set "HOUR=%%d"
    set "MINUTE=%%e"
    set "SECOND=%%f"
)

set "TIMESTAMP=!YEAR!!MONTH!!DAY!_!HOUR!!MINUTE!!SECOND!"
set "TIMESTAMP=!TIMESTAMP:/=!"
set "TIMESTAMP=!TIMESTAMP: =0!"

set "OUTPUT_DIR=%PACKAGE_DIR%\product_!TIMESTAMP!"

echo 创建输出目录: !OUTPUT_DIR!
if not exist "!OUTPUT_DIR!" mkdir "!OUTPUT_DIR!"

REM 复制项目文件
echo 准备打包文件...

REM 创建简化的CMakeLists.txt
(
echo cmake_minimum_required(VERSION 3.16)
echo.
echo project(product_!TIMESTAMP!)
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
echo add_executable(product_!TIMESTAMP! main.cpp)
echo.
echo # 链接Qt库
echo target_link_libraries(product_!TIMESTAMP! Qt6::Core Qt6::Widgets)
echo.
echo # Windows特定设置
echo if(WIN32)
echo     # 设置子系统为Windows
echo     set_target_properties(product_!TIMESTAMP! PROPERTIES WIN32_EXECUTABLE TRUE)
echo     # 设置入口点为main
echo     set_target_properties(product_!TIMESTAMP! PROPERTIES LINK_FLAGS "-mwindows")
echo endif()
) > "!OUTPUT_DIR!\CMakeLists.txt"

REM 创建简单的main.cpp文件
(
echo #include <QApplication>
echo #include <QMainWindow>
echo #include <QLabel>
echo #include <QVBoxLayout>
echo #include <QWidget>
echo #include <QPushButton>
echo.
echo int main(int argc, char *argv[])
echo {
echo     QApplication app(argc, argv);
echo.
echo     QMainWindow *window = new QMainWindow();
echo     QWidget *centralWidget = new QWidget(window);
echo     QVBoxLayout *layout = new QVBoxLayout(centralWidget);
echo.
echo     QLabel *label = new QLabel("软件打包产品 - !TIMESTAMP!", centralWidget);
echo     label->setAlignment(Qt::AlignCenter);
echo     layout->addWidget(label);
echo.
echo     QPushButton *button = new QPushButton("点击我", centralWidget);
echo     layout->addWidget(button);
echo.
echo     QObject::connect(button, &QPushButton::clicked, [label]() {
echo         label->setText("按钮被点击了！");
echo     });
echo.
echo     window->setCentralWidget(centralWidget);
echo     window->setWindowTitle("打包产品 - !TIMESTAMP!");
echo     window->resize(400, 300);
echo     window->show();
echo.
echo     return app.exec();
echo }
) > "!OUTPUT_DIR!\main.cpp"

REM 进入输出目录
cd "!OUTPUT_DIR!"

echo.
echo 开始CMake配置过程...

REM 尝试不同的CMake配置方案
set "CONFIG_SUCCESS=0"

REM 方案1: 标准配置
echo 尝试标准配置...
cmake -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="!QT_DIR!" -DCMAKE_BUILD_TYPE=Release .
if %errorlevel% equ 0 (
    set "CONFIG_SUCCESS=1"
    goto :CONFIG_DONE
)

REM 方案2: 简化配置
echo 标准配置失败，尝试简化配置...
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release .
if %errorlevel% equ 0 (
    set "CONFIG_SUCCESS=1"
    goto :CONFIG_DONE
)

REM 方案3: 使用Unix Makefiles
echo 简化配置失败，尝试Unix Makefiles...
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release .
if %errorlevel% equ 0 (
    set "CONFIG_SUCCESS=1"
    goto :CONFIG_DONE
)

REM 方案4: 不使用生成器
echo Unix Makefiles失败，尝试默认生成器...
cmake -DCMAKE_BUILD_TYPE=Release .
if %errorlevel% equ 0 (
    set "CONFIG_SUCCESS=1"
    goto :CONFIG_DONE
)

:CONFIG_DONE
if "!CONFIG_SUCCESS!"=="0" (
    echo.
    echo CMake配置失败，详细信息如下：
    echo 1. 请检查Qt安装路径: !QT_DIR!
    echo 2. 请检查MinGW安装
    echo 3. 请检查CMake版本
    echo.
    echo 建议解决方案：
    echo - 确保Qt、MinGW、CMake已正确安装
    echo - 检查环境变量设置
    echo - 尝试手动运行CMake命令进行调试
    cd ..
    pause
    exit /b 1
)

echo CMake配置成功！

REM 执行构建
echo.
echo 开始构建过程...
mingw32-make

if %errorlevel% neq 0 (
    echo.
    echo 构建失败，尝试使用make命令...
    make
    
    if %errorlevel% neq 0 (
        echo.
        echo 构建仍然失败
        echo 请检查编译错误信息
        cd ..
        pause
        exit /b 1
    )
)

echo.
echo ========================================
echo 打包成功完成！
echo ========================================
echo 输出目录: !OUTPUT_DIR!
echo 可执行文件: !OUTPUT_DIR!\product_!TIMESTAMP!.exe
echo.

REM 检查生成的可执行文件
if exist "product_!TIMESTAMP!.exe" (
    echo 可执行文件生成成功
    echo 文件大小: %%~zI bytes
) else (
    echo 警告: 未找到生成的可执行文件
)

REM 返回上级目录
cd ..

REM 显示打包结果
echo.
echo 打包目录内容：
dir "!OUTPUT_DIR!"

echo.
echo 打包过程已完成。按任意键退出...
pause >nul