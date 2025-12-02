@echo off
setlocal enabledelayedexpansion

REM 自动化打包脚本 - 一键构建、打包和创建安装包

echo ========================================
echo     软件编辑器 - 自动化打包工具
echo ========================================
echo.

REM 设置项目根目录
set PROJECT_ROOT=%~dp0
set PROJECT_ROOT=%PROJECT_ROOT:~0,-1%

REM 检查Python是否可用
python --version >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] Python未安装或不在PATH中
    echo 请安装Python 3.8+ 并确保在PATH中可用
    pause
    exit /b 1
)

REM 检查必要的工具
set MISSING_TOOLS=0

REM 检查CMake
cmake --version >nul 2>&1
if %errorlevel% neq 0 (
    echo [WARNING] CMake未安装或不在PATH中
    set /a MISSING_TOOLS+=1
)

REM 检查MinGW
mingw32-make --version >nul 2>&1
if %errorlevel% neq 0 (
    echo [WARNING] MinGW未安装或不在PATH中
    set /a MISSING_TOOLS+=1
)

REM 检查Qt路径
set QT_DIR=D:\Qt\6.9.1\mingw_64
if not exist "%QT_DIR%" (
    echo [WARNING] Qt目录不存在: %QT_DIR%
    echo 请修改脚本中的QT_DIR变量为您的实际Qt安装路径
    set /a MISSING_TOOLS+=1
)

if %MISSING_TOOLS% gtr 0 (
    echo.
    echo [WARNING] 检测到 %MISSING_TOOLS% 个工具缺失或路径不正确
    echo 打包过程可能会失败
    echo.
    set /p CONTINUE="是否继续? (y/n): "
    if /i not "%CONTINUE%"=="y" (
        echo 用户取消操作
        pause
        exit /b 0
    )
)

echo.
echo 开始自动化打包流程...
echo.

REM 运行Python打包脚本
python scripts\auto_package.py --project-root "%PROJECT_ROOT%"

if %errorlevel% equ 0 (
    echo.
    echo ✅ 打包成功完成！
    echo.
    echo 📦 打包文件位于: %PROJECT_ROOT%\packages\
    echo 🔧 安装包位于: %PROJECT_ROOT%\packages\SoftwareEditor_setup.exe
    echo.
    echo 您可以:
    echo 1. 直接运行 portable 版本
    echo 2. 使用安装包进行安装
    echo 3. 分发打包好的软件
) else (
    echo.
    echo ❌ 打包失败
    echo 请检查错误信息并确保所有依赖工具正确安装
)

echo.
pause