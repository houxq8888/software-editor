@echo off
echo ===============================================
echo Test Suppress Flag Debug Script
echo ===============================================
echo.

REM Clean old log file
if exist "build\debug.log" (
    echo Delete old debug.log file
    del "build\debug.log"
)

echo.
echo Scenario 1: Start software editor directly (no parameters)
echo Expected: Should call showNewForm()
echo -----------------------------------------------
start /wait "" "build\software-editor.exe"
if exist "build\debug.log" (
    echo Check suppress flag info in debug.log:
    findstr /C:"设置抑制标志" "build\debug.log"
    findstr /C:"workbench抑制标志" "build\debug.log"
    findstr /C:"检查showNewForm条件" "build\debug.log"
    findstr /C:"showNewForm" "build\debug.log"
    echo.
    echo Full debug.log content:
    type "build\debug.log"
) else (
    echo Error: debug.log file does not exist
)

echo.
echo ===============================================
echo.

REM Clean log file
if exist "build\debug.log" (
    echo Delete debug.log file
    del "build\debug.log"
)

echo.
echo Scenario 2: Start with UI file parameter (with .ui file)
echo Expected: Should NOT call showNewForm()
echo -----------------------------------------------
start /wait "" "build\software-editor.exe" "43214.ui"
if exist "build\debug.log" (
    echo Check suppress flag info in debug.log:
    findstr /C:"设置抑制标志" "build\debug.log"
    findstr /C:"workbench抑制标志" "build\debug.log"
    findstr /C:"检查showNewForm条件" "build\debug.log"
    findstr /C:"showNewForm" "build\debug.log"
    echo.
    echo Full debug.log content:
    type "build\debug.log"
) else (
    echo Error: debug.log file does not exist
)

echo.
echo ===============================================
echo Test completed
echo ===============================================
pause