@echo off
chcp 65001 >nul
echo 开始打包软件...

REM 设置变量
set PROJECT_NAME=软件编辑器
set BUILD_DIR=build
set DIST_DIR=dist
set EXE_NAME=%PROJECT_NAME%.exe

REM 创建构建目录
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
if not exist "%DIST_DIR%" mkdir "%DIST_DIR%"

REM 检查CMake
cmake --version >nul 2>&1
if errorlevel 1 (
    echo 错误: CMake未安装或未在PATH中
    pause
    exit /b 1
)

REM 配置项目
echo 配置项目...
cd "%BUILD_DIR%"
cmake ..
if errorlevel 1 (
    echo 错误: CMake配置失败
    pause
    exit /b 1
)

REM 编译项目
echo 编译项目...
cmake --build . --config Release
if errorlevel 1 (
    echo 错误: 编译失败
    pause
    exit /b 1
)

REM 复制可执行文件
echo 复制可执行文件...
cd ..
if exist "%BUILD_DIR%\Release\software-editor.exe" (
    copy "%BUILD_DIR%\Release\software-editor.exe" "%DIST_DIR%\%EXE_NAME%"
    echo 可执行文件复制完成
) else (
    echo 错误: 可执行文件未找到
    pause
    exit /b 1
)

REM 复制依赖文件
echo 复制依赖文件...
if exist "%BUILD_DIR%\*.dll" (
    copy "%BUILD_DIR%\*.dll" "%DIST_DIR%\"
)

REM 复制插件目录
if exist "%BUILD_DIR%\platforms" (
    if not exist "%DIST_DIR%\platforms" mkdir "%DIST_DIR%\platforms"
    copy "%BUILD_DIR%\platforms\*.dll" "%DIST_DIR%\platforms\"
)

if exist "%BUILD_DIR%\imageformats" (
    if not exist "%DIST_DIR%\imageformats" mkdir "%DIST_DIR%\imageformats"
    copy "%BUILD_DIR%\imageformats\*.dll" "%DIST_DIR%\imageformats\"
)

echo.
echo ============================================
echo 打包完成！
echo 可执行文件: %DIST_DIR%\%EXE_NAME%
echo 依赖文件: %DIST_DIR%\
echo ============================================

pause