@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

echo ========================================
echo   智能依赖复制脚本
echo ========================================
echo.

if "%1"=="" (
    echo 用法: copy_dependencies.bat <exe文件路径> <目标目录>
    echo 示例: copy_dependencies.bat build\software-editor.exe output
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

:: 创建目标目录
if not exist "%TARGET_DIR%" mkdir "%TARGET_DIR%"

:: 检测Qt安装路径
set "QT_DIR="
for %%P in (
    "D:\Qt\6.9.1\mingw_64"
    "C:\Qt\6.9.1\mingw_64" 
    "%QT_DIR%"
) do (
    if exist "%%P\bin\Qt6Core.dll" (
        set "QT_DIR=%%P"
        goto :qt_found
    )
)

:qt_found
if "%QT_DIR%"=="" (
    echo 警告: 未检测到Qt安装路径
) else (
    echo 检测到Qt路径: %QT_DIR%
)

:: 检测MinGW路径
set "MINGW_DIR="
for %%P in (
    "D:\Qt\Tools\mingw1310_64"
    "C:\Qt\Tools\mingw1310_64"
    "%MINGW_DIR%"
) do (
    if exist "%%P\bin\gcc.exe" (
        set "MINGW_DIR=%%P"
        goto :mingw_found
    )
)

:mingw_found
if "%MINGW_DIR%"=="" (
    echo 警告: 未检测到MinGW安装路径
) else (
    echo 检测到MinGW路径: %MINGW_DIR%
)

echo.
echo 开始复制依赖文件...
echo.

:: 1. 复制基本的Qt DLL文件
echo [1/6] 复制Qt运行时DLL...
if not "%QT_DIR%"=="" (
    copy "%QT_DIR%\bin\Qt6Core.dll" "%TARGET_DIR%" >nul 2>&1
    copy "%QT_DIR%\bin\Qt6Gui.dll" "%TARGET_DIR%" >nul 2>&1
    copy "%QT_DIR%\bin\Qt6Widgets.dll" "%TARGET_DIR%" >nul 2>&1
    echo   已复制: Qt6Core.dll, Qt6Gui.dll, Qt6Widgets.dll
)

:: 2. 复制MinGW运行时DLL
echo [2/6] 复制MinGW运行时DLL...
if not "%MINGW_DIR%"=="" (
    copy "%MINGW_DIR%\bin\libgcc_s_seh-1.dll" "%TARGET_DIR%" >nul 2>&1
    copy "%MINGW_DIR%\bin\libstdc++-6.dll" "%TARGET_DIR%" >nul 2>&1
    copy "%MINGW_DIR%\bin\libwinpthread-1.dll" "%TARGET_DIR%" >nul 2>&1
    echo   已复制: libgcc_s_seh-1.dll, libstdc++-6.dll, libwinpthread-1.dll
)

:: 3. 复制Qt插件
echo [3/6] 复制Qt插件...
if not "%QT_DIR%"=="" (
    if not exist "%TARGET_DIR%\plugins" mkdir "%TARGET_DIR%\plugins"
    if not exist "%TARGET_DIR%\plugins\platforms" mkdir "%TARGET_DIR%\plugins\platforms"
    if not exist "%TARGET_DIR%\plugins\imageformats" mkdir "%TARGET_DIR%\plugins\imageformats"
    if not exist "%TARGET_DIR%\plugins\styles" mkdir "%TARGET_DIR%\plugins\styles"
    
    copy "%QT_DIR%\plugins\platforms\qwindows.dll" "%TARGET_DIR%\plugins\platforms\" >nul 2>&1
    copy "%QT_DIR%\plugins\imageformats\qjpeg.dll" "%TARGET_DIR%\plugins\imageformats\" >nul 2>&1
    copy "%QT_DIR%\plugins\imageformats\qgif.dll" "%TARGET_DIR%\plugins\imageformats\" >nul 2>&1
    copy "%QT_DIR%\plugins\imageformats\qpng.dll" "%TARGET_DIR%\plugins\imageformats\" >nul 2>&1
    copy "%QT_DIR%\plugins\imageformats\qsvg.dll" "%TARGET_DIR%\plugins\imageformats\" >nul 2>&1
    copy "%QT_DIR%\plugins\imageformats\qico.dll" "%TARGET_DIR%\plugins\imageformats\" >nul 2>&1
    copy "%QT_DIR%\plugins\styles\qwindowsvistastyle.dll" "%TARGET_DIR%\plugins\styles\" >nul 2>&1
    
    echo   已复制: 平台插件、图像格式插件、样式插件
)

:: 4. 复制额外的Qt组件
echo [4/6] 复制额外的Qt组件...
if not "%QT_DIR%"=="" (
    copy "%QT_DIR%\bin\Qt6Network.dll" "%TARGET_DIR%" >nul 2>&1
    copy "%QT_DIR%\bin\Qt6Sql.dll" "%TARGET_DIR%" >nul 2>&1
    copy "%QT_DIR%\bin\Qt6Xml.dll" "%TARGET_DIR%" >nul 2>&1
    copy "%QT_DIR%\bin\Qt6Svg.dll" "%TARGET_DIR%" >nul 2>&1
    echo   已复制: Qt6Network.dll, Qt6Sql.dll, Qt6Xml.dll, Qt6Svg.dll
)

:: 5. 复制系统依赖
echo [5/6] 复制系统依赖...
copy "C:\Windows\System32\vcruntime140.dll" "%TARGET_DIR%" >nul 2>&1
copy "C:\Windows\System32\vcruntime140_1.dll" "%TARGET_DIR%" >nul 2>&1
copy "C:\Windows\System32\msvcp140.dll" "%TARGET_DIR%" >nul 2>&1
copy "C:\Windows\System32\ucrtbase.dll" "%TARGET_DIR%" >nul 2>&1
echo   已复制: VC++运行时库

:: 6. 复制主程序
echo [6/6] 复制主程序...
copy "%EXE_PATH%" "%TARGET_DIR%" >nul 2>&1
echo   已复制: %~nx1

echo.
echo ========================================
echo   依赖复制完成
echo ========================================
echo.
echo 已复制的文件:
dir /b "%TARGET_DIR%"

if exist "%TARGET_DIR%\plugins" (
    echo.
    echo 已复制的插件:
    dir /b /s "%TARGET_DIR%\plugins"
)

echo.
echo 总文件数:
dir /b "%TARGET_DIR%" | find /c /v ""

echo.
echo 注意: 请确保目标目录包含所有必要的依赖文件后再分发。
echo 建议使用windeployqt工具进行更完整的依赖检测。

pause