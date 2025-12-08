@echo off
chcp 936 >nul
setlocal enabledelayedexpansion

echo ========================================
echo    Software Editor Process Monitor
echo ========================================
echo.

:menu
echo Options:
echo 1. Check current process status
echo 2. Start program and monitor
echo 3. Real-time monitoring
echo 4. Exit
echo.
set /p choice=Enter choice (1-4): 

if "!choice!"=="1" goto check_status
if "!choice!"=="2" goto start_and_monitor
if "!choice!"=="3" goto realtime_monitor
if "!choice!"=="4" goto exit

echo Invalid choice, please try again
echo.
goto menu

:start_and_monitor
echo Starting software editor...
start "" ".\build\software-editor.exe"
echo Waiting for program initialization...
timeout /t 3 /nobreak >nul
goto check_status

:check_status
echo.
echo Checking process status...
echo ====================
tasklist /fi "imagename eq software-editor.exe" /fo table

set pid=
for /f "tokens=2" %%i in ('tasklist /fi "imagename eq software-editor.exe" /fo csv ^| findstr software-editor') do set pid=%%i

if defined pid (
    echo.
    echo Process Details:
    echo Process ID: !pid!
    
    echo.
    echo Thread Information:
    wmic process where "processid=!pid!" get threadcount /value
    
    echo.
    echo Memory Usage:
    for /f "tokens=2 delims==" %%j in ('wmic process where "processid=!pid!" get workingsetsize /value ^| findstr WorkingSetSize') do set memory=%%j
    set /a memory_mb=!memory!/1048576
    echo Working Set: !memory_mb! MB
    
    echo.
    echo Handle Information:
    wmic process where "processid=!pid!" get handlecount /value
) else (
    echo Program is not running
)

echo.
set /p continue=Press any key to return to menu...
goto menu

:realtime_monitor
echo.
echo Starting real-time monitoring...
echo Press Ctrl+C to stop monitoring
echo ====================

:monitor_loop
cls
echo Monitoring Time: %date% %time%
echo ====================

tasklist /fi "imagename eq software-editor.exe" /fo table

set pid=
for /f "tokens=2" %%i in ('tasklist /fi "imagename eq software-editor.exe" /fo csv ^| findstr software-editor') do set pid=%%i

if defined pid (
    echo.
    echo Process ID: !pid!
    
    set threads=
    set memory=
    set handles=
    
    for /f "tokens=2 delims==" %%j in ('wmic process where "processid=!pid!" get threadcount /value ^| findstr ThreadCount') do set threads=%%j
    for /f "tokens=2 delims==" %%j in ('wmic process where "processid=!pid!" get workingsetsize /value ^| findstr WorkingSetSize') do set memory=%%j
    for /f "tokens=2 delims==" %%j in ('wmic process where "processid=!pid!" get handlecount /value ^| findstr HandleCount') do set handles=%%j
    
    if defined memory (
        set /a memory_mb=!memory!/1048576
        echo Threads: !threads!
        echo Memory: !memory_mb! MB
        echo Handles: !handles!
        echo.
        echo Status: Running
    ) else (
        echo Status: Process details not available
    )
) else (
    echo.
    echo Status: Program has exited
    goto menu
)

echo Waiting 2 seconds for refresh...
timeout /t 2 /nobreak >nul
goto monitor_loop

:exit
echo Exiting monitor tool
exit