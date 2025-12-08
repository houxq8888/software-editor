@echo off
chcp 936 >nul
setlocal enabledelayedexpansion

echo ========================================
echo    Software Editor Startup Diagnosis
echo ========================================
echo.

echo Step 1: Checking dependencies...
echo.

if not exist ".\build\software-editor.exe" (
    echo ERROR: Executable not found in build directory
    echo Please run .\build.bat first
    pause
    exit /b 1
)

echo Executable found: .\build\software-editor.exe
echo.

echo Step 2: Checking required DLL files...
echo.

set missing_dlls=0
if not exist "D:\Qt\6.9.1\mingw_64\bin\Qt6Core.dll" (
    echo WARNING: Qt6Core.dll not found
    set /a missing_dlls+=1
) else (
    echo Qt6Core.dll: OK
)

if not exist "D:\Qt\6.9.1\mingw_64\bin\Qt6Gui.dll" (
    echo WARNING: Qt6Gui.dll not found
    set /a missing_dlls+=1
) else (
    echo Qt6Gui.dll: OK
)

if not exist "D:\Qt\6.9.1\mingw_64\bin\Qt6Widgets.dll" (
    echo WARNING: Qt6Widgets.dll not found
    set /a missing_dlls+=1
) else (
    echo Qt6Widgets.dll: OK
)

echo.
echo Step 3: Starting program with detailed output...
echo.

echo Starting software-editor.exe...
echo This will capture any startup error messages...
echo.

cd build

echo === Program Output Start ===
software-editor.exe
echo === Program Output End ===
echo.

echo Program Exit Code: %errorlevel%
echo.

cd ..

if %errorlevel% equ 0 (
    echo SUCCESS: Program started and exited normally
) else (
    echo ERROR: Program exited with code %errorlevel%
    echo.
    echo Possible causes:
    echo - Missing dependencies
    echo - Configuration issues
    echo - Runtime errors
)

echo.
pause