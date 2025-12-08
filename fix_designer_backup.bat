@echo off
chcp 936 >nul
setlocal enabledelayedexpansion

echo ===============================================
echo Qt Designer 备份文件清理工具
echo ===============================================
echo.

echo 正在检查Qt Designer备份文件...
echo.

:: 检查注册表中的备份文件设置
reg query "HKEY_CURRENT_USER\Software\QtProject" /s 2>nul | findstr /i "backup"
if %errorlevel% equ 0 (
    echo 发现Qt Designer备份文件设置，正在清理...
    reg delete "HKEY_CURRENT_USER\Software\QtProject\Qt Designer" /v backupOrgListKey /f 2>nul
    reg delete "HKEY_CURRENT_USER\Software\QtProject\Qt Designer" /v backupBakListKey /f 2>nul
    echo 备份文件设置已清理
) else (
    echo 未找到备份文件设置
)

echo.
echo 检查临时备份文件...
:: 检查临时目录中的备份文件
dir /s /b %TEMP%\*designer* 2>nul
dir /s /b %TEMP%\*backup* 2>nul

echo.
echo 清理用户配置目录中的备份文件...
:: 清理用户配置目录
if exist "%APPDATA%\QtProject\Qt Designer" (
    echo 发现Qt Designer配置目录，正在清理备份文件...
    del /q "%APPDATA%\QtProject\Qt Designer\*backup*" 2>nul
    del /q "%APPDATA%\QtProject\Qt Designer\*designer*" 2>nul
    echo 配置目录备份文件已清理
)

echo.
echo ===============================================
echo 修复建议：
echo 1. 确保程序通过正常的关闭流程退出
echo 2. 在closeEvent中调用QApplication::quit()
echo 3. 避免使用任务管理器强制结束程序
echo 4. 检查程序是否有未处理的异常或崩溃
echo ===============================================

pause