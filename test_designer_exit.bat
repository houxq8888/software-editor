@echo off
chcp 65001 >nul

echo ===============================================
echo Qt Designer 退出功能测试
echo ===============================================
echo.

echo 步骤1: 清理之前的备份文件
echo -----------------------------------------------
call clean_designer_backup.bat

echo.
echo 步骤2: 启动Qt Designer并测试正常退出
echo -----------------------------------------------
echo 启动Qt Designer...
start /wait .\build\software-editor.exe

echo.
echo 步骤3: 检查进程是否完全退出
echo -----------------------------------------------
timeout /t 3 /nobreak >nul
tasklist /fi "imagename eq software-editor.exe" | find "software-editor.exe"
if %errorlevel% equ 0 (
    echo 错误: Qt Designer进程仍在运行！
    echo 这可能意味着退出逻辑有问题。
) else (
    echo 成功: Qt Designer进程已完全退出。
)

echo.
echo 步骤4: 再次启动Qt Designer检查错误消息
echo -----------------------------------------------
echo 再次启动Qt Designer...
start /wait .\build\software-editor.exe

echo.
echo 步骤5: 检查是否有"session was not terminated"错误
echo -----------------------------------------------
echo 如果程序正常启动且没有显示错误消息，说明修复成功！

echo.
echo ===============================================
echo 测试完成！
echo ===============================================
pause