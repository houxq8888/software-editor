@echo off
setlocal enabledelayedexpansion

echo ========================================
echo   Dependency Copy Script
echo ========================================
echo.

if "%1"=="" (
    echo Usage: copy_dependencies_en.bat ^<exe_path^> ^<target_dir^>
    echo Example: copy_dependencies_en.bat build\software-editor.exe build
    pause
    exit /b 1
)

if "%2"=="" (
    echo Error: Please specify target directory
    pause
    exit /b 1
)

set "EXE_PATH=%~1"
set "TARGET_DIR=%~2"

if not exist "%EXE_PATH%" (
    echo Error: EXE file not found - %EXE_PATH%
    pause
    exit /b 1
)

echo Processing: %EXE_PATH%
echo Target Directory: %TARGET_DIR%
echo.

:: Create target directory
if not exist "%TARGET_DIR%" mkdir "%TARGET_DIR%"

:: Detect Qt installation path
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
    echo Warning: Qt installation path not found
) else (
    echo Qt Path Detected: %QT_DIR%
)

:: Detect MinGW path
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
    echo Warning: MinGW path not found
) else (
    echo MinGW Path Detected: %MINGW_DIR%
)

echo.
echo Starting dependency copy...
echo.

:: 1. Copy basic Qt DLLs
echo [1/6] Copying Qt runtime DLLs...
if not "%QT_DIR%"=="" (
    xcopy "%QT_DIR%\bin\Qt6Core.dll" "%TARGET_DIR%" /Y /Q
    xcopy "%QT_DIR%\bin\Qt6Gui.dll" "%TARGET_DIR%" /Y /Q
    xcopy "%QT_DIR%\bin\Qt6Widgets.dll" "%TARGET_DIR%" /Y /Q
    xcopy "%QT_DIR%\bin\Qt6Designer.dll" "%TARGET_DIR%" /Y /Q
    xcopy "%QT_DIR%\bin\Qt6Network.dll" "%TARGET_DIR%" /Y /Q
    xcopy "%QT_DIR%\bin\Qt6Xml.dll" "%TARGET_DIR%" /Y /Q
    xcopy "%QT_DIR%\bin\Qt6Svg.dll" "%TARGET_DIR%" /Y /Q
    xcopy "%QT_DIR%\bin\Qt6PrintSupport.dll" "%TARGET_DIR%" /Y /Q
    xcopy "%QT_DIR%\bin\Qt6OpenGL.dll" "%TARGET_DIR%" /Y /Q
    xcopy "%QT_DIR%\bin\Qt6OpenGLWidgets.dll" "%TARGET_DIR%" /Y /Q
    xcopy "%QT_DIR%\bin\Qt6Compat.dll" "%TARGET_DIR%" /Y /Q
    xcopy "%QT_DIR%\bin\Qt6Dbus.dll" "%TARGET_DIR%" /Y /Q
    echo   Copied: Qt6Core.dll, Qt6Gui.dll, Qt6Widgets.dll, Qt6Designer.dll, Qt6Network.dll, Qt6Xml.dll, Qt6Svg.dll, Qt6PrintSupport.dll, Qt6OpenGL.dll, Qt6OpenGLWidgets.dll, Qt6Compat.dll, Qt6Dbus.dll
)

:: 2. Copy MinGW runtime DLLs
echo [2/6] Copying MinGW runtime DLLs...
if not "%MINGW_DIR%"=="" (
    xcopy "%MINGW_DIR%\bin\libgcc_s_seh-1.dll" "%TARGET_DIR%" /Y /Q
    xcopy "%MINGW_DIR%\bin\libstdc++-6.dll" "%TARGET_DIR%" /Y /Q
    xcopy "%MINGW_DIR%\bin\libwinpthread-1.dll" "%TARGET_DIR%" /Y /Q
    echo   Copied: libgcc_s_seh-1.dll, libstdc++-6.dll, libwinpthread-1.dll
)

:: 3. Copy Qt platform plugins
echo [3/6] Copying Qt platform plugins...
if not "%QT_DIR%"=="" (
    if not exist "%TARGET_DIR%\platforms" mkdir "%TARGET_DIR%\platforms"
    xcopy "%QT_DIR%\plugins\platforms\qwindows.dll" "%TARGET_DIR%\platforms" /Y /Q
    echo   Copied: platforms/qwindows.dll
)

echo.
echo Dependency copy completed!
echo.