@echo off
chcp 936 >nul
setlocal enabledelayedexpansion

echo ========================================
echo    Software Editor Simple Monitor
echo ========================================
echo.

echo Starting software editor...
start "" ".\build\software-editor.exe"
echo Program started, PID will be shown below...
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
    echo Process Details:
    echo PID: !pid!
    
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
        
        echo.
        echo Crash Analysis:
        if !threads! gtr 50 echo WARNING: High thread count (!threads!)
        if !memory_mb! gtr 500 echo WARNING: High memory usage (!memory_mb! MB)
        if !handles! gtr 1000 echo WARNING: High handle count (!handles!)
    )
) else (
    echo.
    echo Status: Program has exited
    echo.
    echo Analysis: Program exited cleanly
    pause
    exit
)

echo.
echo Waiting 2 seconds...
timeout /t 2 /nobreak >nul
goto monitor_loop