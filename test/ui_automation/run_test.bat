@echo off
echo ========================================
echo 状态机解耦测试脚本
echo ========================================

REM 检查是否在正确的目录
if not exist "..\..\build.bat" (
    echo 错误：请确保在项目根目录下运行此脚本
    echo 当前目录：%CD%
    pause
    exit /b 1
)

REM 切换到项目根目录
cd ..\..

REM 编译项目
echo.
echo 正在编译项目...
call build.bat

if %ERRORLEVEL% neq 0 (
    echo 编译失败！
    pause
    exit /b 1
)

echo 编译成功！

REM 检查可执行文件是否存在
if not exist "build\software-editor.exe" (
    echo 错误：可执行文件不存在
    pause
    exit /b 1
)

REM 运行测试程序
echo.
echo 正在启动状态机测试程序...
start "" "build\software-editor.exe" --test-statemachine

echo.
echo 测试程序已启动，请查看应用程序界面进行测试。
echo.
pause