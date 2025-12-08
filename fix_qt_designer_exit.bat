@echo off
chcp 936 >nul
setlocal enabledelayedexpansion

echo ===============================================
echo Qt Designer 正常退出修复工具
echo ===============================================
echo.

echo 步骤1: 清理残留的Qt Designer进程
echo -----------------------------------------------
tasklist /fi "imagename eq designer.exe" /fo table
if %errorlevel% equ 0 (
    echo 发现运行的Qt Designer进程，正在终止...
    taskkill /f /im designer.exe 2>nul
    echo 残留进程已清理
) else (
    echo 未发现运行的Qt Designer进程
)

echo.
echo 步骤2: 清理备份文件设置
echo -----------------------------------------------
reg query "HKEY_CURRENT_USER\Software\QtProject" /s 2>nul | findstr /i "backup"
if %errorlevel% equ 0 (
    echo 发现备份文件设置，正在清理...
    reg delete "HKEY_CURRENT_USER\Software\QtProject\Qt Designer" /v "backup/fileListOrg" /f 2>nul
    reg delete "HKEY_CURRENT_USER\Software\QtProject\Qt Designer" /v "backup/fileListBak" /f 2>nul
    echo 备份文件设置已清理
) else (
    echo 未找到备份文件设置
)

echo.
echo 步骤3: 清理临时备份文件
echo -----------------------------------------------
echo 检查临时目录中的备份文件...
for /f "delims=" %%f in ('dir /s /b "%TEMP%\*designer*" "%TEMP%\*backup*" 2^>nul') do (
    echo 删除: %%f
    del /q "%%f" 2>nul
)

echo.
echo 步骤4: 清理用户配置目录
echo -----------------------------------------------
if exist "%APPDATA%\QtProject\Qt Designer" (
    echo 清理Qt Designer配置目录...
    del /q "%APPDATA%\QtProject\Qt Designer\*backup*" 2>nul
    del /q "%APPDATA%\QtProject\Qt Designer\*designer*" 2>nul
    echo 配置目录已清理
)

echo.
echo ===============================================
echo 程序代码修复建议：
echo ===============================================
echo.
echo 1. 确保在closeEvent中调用QApplication::quit()
echo    - 在ProductMainWindow::closeEvent()中添加
echo    - 在UILayoutWindow::closeEvent()中添加
echo.
echo 2. 添加异常处理机制
echo    - 在main()函数中添加try-catch块
echo    - 捕获未处理的异常并执行清理
echo.
echo 3. 改进关闭流程
echo    - 确保所有窗口都正确关闭
echo    - 添加关闭确认对话框
echo.
echo ===============================================
echo 修复完成！现在可以正常启动Qt Designer了。
echo ===============================================

pause