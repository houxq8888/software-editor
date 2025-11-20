@echo off
setlocal

:: 设置Qt和MinGW路径
set QT_DIR=D:\Qt\6.9.1\mingw_64
set MINGW_DIR=D:\Qt\Tools\mingw1310_64
set CMAKE_DIR=D:\Qt\Tools\CMake_64\bin

:: 设置环境变量
set PATH=%MINGW_DIR%\bin;%CMAKE_DIR%;%PATH%

:: 进入项目目录
cd /d "d:\virtualMachine\github\software-editor"

:: 创建build目录
if not exist "build" mkdir "build"
cd "build"

:: 运行CMake
cmake.exe -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="%QT_DIR%" -DCMAKE_BUILD_TYPE=Release ..
if %ERRORLEVEL% neq 0 (
    echo CMake配置失败
    exit /b 1
)

:: 编译
mingw32-make.exe
if %ERRORLEVEL% neq 0 (
    echo 编译失败
    exit /b 1
)

echo 构建成功
endlocal