@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

echo ========================================
echo   高级依赖复制脚本 (使用windeployqt)
echo ========================================
echo.

if "%1"=="" (
    echo 用法: advanced_dependency_copy.bat <exe文件路径> <目标目录>
    echo 示例: advanced_dependency_copy.bat build\software-editor.exe output
    pause
    exit /b 1
)

if "%2"=="" (
    echo 错误: 请指定目标目录
    pause
    exit /b 1
)

set "EXE_PATH=%~1"
set "TARGET_DIR=%~2"

if not exist "%EXE_PATH%" (
    echo 错误: EXE文件不存在 - %EXE_PATH%
    pause
    exit /b 1
)

echo 正在处理: %EXE_PATH%
echo 目标目录: %TARGET_DIR%
echo.

:: 检测Qt安装路径
set "QT_DIR="
for %%P in (
    "D:\Qt\6.9.1\mingw_64"
    "C:\Qt\6.9.1\mingw_64" 
    "%QT_DIR%"
) do (
    if exist "%%P\bin\windeployqt.exe" (
        set "QT_DIR=%%P"
        goto :qt_found
    )
)

:qt_found
if "%QT_DIR%"=="" (
    echo 错误: 未检测到Qt安装路径或windeployqt工具
    echo 请确保已安装Qt 6.9.1或更高版本
    pause
    exit /b 1
)

echo 检测到Qt路径: %QT_DIR%

:: 创建目标目录
if not exist "%TARGET_DIR%" mkdir "%TARGET_DIR%"

:: 复制主程序
echo.
echo [1/4] 复制主程序...
copy "%EXE_PATH%" "%TARGET_DIR%" >nul 2>&1
if errorlevel 1 (
    echo 错误: 无法复制主程序
    pause
    exit /b 1
)
echo   已复制: %~nx1

:: 使用windeployqt自动复制依赖
echo.
echo [2/4] 使用windeployqt自动复制依赖...
set "WIN_DEPLOY_QT=%QT_DIR%\bin\windeployqt.exe"

if not exist "%WIN_DEPLOY_QT%" (
    echo 错误: windeployqt.exe不存在
    pause
    exit /b 1
)

echo 执行: %WIN_DEPLOY_QT% "%TARGET_DIR%\%~nx1"
"%WIN_DEPLOY_QT%" "%TARGET_DIR%\%~nx1"

if errorlevel 1 (
    echo 警告: windeployqt执行过程中可能出现问题
    echo 将继续手动复制依赖...
)

:: 手动补充可能遗漏的依赖
echo.
echo [3/4] 手动补充可能遗漏的依赖...

:: 复制MinGW运行时DLL
set "MINGW_DIR=%QT_DIR%\..\..\Tools\mingw1310_64"
if exist "%MINGW_DIR%" (
    echo 检测到MinGW路径: %MINGW_DIR%
    
    if not exist "%TARGET_DIR%\libgcc_s_seh-1.dll" (
        if exist "%MINGW_DIR%\bin\libgcc_s_seh-1.dll" (
            copy "%MINGW_DIR%\bin\libgcc_s_seh-1.dll" "%TARGET_DIR%" >nul 2>&1
            echo   已复制: libgcc_s_seh-1.dll
        )
    )
    
    if not exist "%TARGET_DIR%\libstdc++-6.dll" (
        if exist "%MINGW_DIR%\bin\libstdc++-6.dll" (
            copy "%MINGW_DIR%\bin\libstdc++-6.dll" "%TARGET_DIR%" >nul 2>&1
            echo   已复制: libstdc++-6.dll
        )
    )
    
    if not exist "%TARGET_DIR%\libwinpthread-1.dll" (
        if exist "%MINGW_DIR%\bin\libwinpthread-1.dll" (
            copy "%MINGW_DIR%\bin\libwinpthread-1.dll" "%TARGET_DIR%" >nul 2>&1
            echo   已复制: libwinpthread-1.dll
        )
    )
)

:: 复制系统运行时DLL
if not exist "%TARGET_DIR%\vcruntime140.dll" (
    if exist "C:\Windows\System32\vcruntime140.dll" (
        copy "C:\Windows\System32\vcruntime140.dll" "%TARGET_DIR%" >nul 2>&1
        echo   已复制: vcruntime140.dll
    )
)

if not exist "%TARGET_DIR%\vcruntime140_1.dll" (
    if exist "C:\Windows\System32\vcruntime140_1.dll" (
        copy "C:\Windows\System32\vcruntime140_1.dll" "%TARGET_DIR%" >nul 2>&1
        echo   已复制: vcruntime140_1.dll
    )
)

if not exist "%TARGET_DIR%\msvcp140.dll" (
    if exist "C:\Windows\System32\msvcp140.dll" (
        copy "C:\Windows\System32\msvcp140.dll" "%TARGET_DIR%" >nul 2>&1
        echo   已复制: msvcp140.dll
    )
)

:: 生成依赖报告
echo.
echo [4/4] 生成依赖报告...
(
    echo 高级依赖复制报告
    echo =================
    echo.
    echo 生成时间: %date% %time%
    echo 目标文件: %EXE_PATH%
    echo 目标目录: %TARGET_DIR%
    echo.
    echo 使用的工具:
    echo - windeployqt.exe: %WIN_DEPLOY_QT%
    echo.
    echo 已复制的文件:
    echo =============
    for /f "delims=" %%f in ('dir /b "%TARGET_DIR%" 2^>nul') do echo - %%f
    echo.
    echo 插件目录结构:
    echo =============
    if exist "%TARGET_DIR%\plugins" (
        for /f "delims=" %%d in ('dir /b /s "%TARGET_DIR%\plugins" 2^>nul') do echo - %%d
    ) else (
        echo - 无插件目录
    )
    echo.
    echo 总文件数:
    set /a file_count=0
    for /f %%f in ('dir /b "%TARGET_DIR%" ^| find /c /v ""') do set /a file_count=%%f
    echo - 主目录: !file_count! 个文件
    
    if exist "%TARGET_DIR%\plugins" (
        set /a plugin_count=0
        for /f %%p in ('dir /b /s "%TARGET_DIR%\plugins" ^| find /c /v ""') do set /a plugin_count=%%p
        echo - 插件目录: !plugin_count! 个文件
        set /a total_count=file_count+plugin_count
        echo - 总计: !total_count! 个文件
    )
    echo.
    echo 注意事项:
    echo - 请确保目标目录包含所有必要的依赖文件后再分发
    echo - 建议在目标系统上测试程序运行情况
    echo - 如有问题，请检查依赖报告中的文件列表
) > "%TARGET_DIR%\advanced_dependency_report.txt"

echo   依赖报告已生成: %TARGET_DIR%\advanced_dependency_report.txt

echo.
echo ========================================
echo   高级依赖复制完成
echo ========================================
echo.
echo 总结:
echo - 使用windeployqt自动检测和复制Qt依赖
echo - 手动补充MinGW和系统运行时依赖
echo - 生成详细的依赖报告
echo.
echo 建议:
echo - 在目标系统上测试程序运行情况
echo - 如有缺失依赖，请参考依赖报告进行补充
echo.

pause