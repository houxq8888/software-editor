@echo off
chcp 936 >nul
setlocal enabledelayedexpansion

echo ========================================
echo    Software Editor Crash Debug Tool
echo ========================================
echo.

:menu
echo Options:
echo 1. Start program with detailed monitoring
echo 2. Check current process state
echo 3. Analyze crash dump (if available)
echo 4. Exit
echo.
set /p choice=Enter choice (1-4): 

if "!choice!"=="1" goto start_detailed
if "!choice!"=="2" goto check_state
if "!choice!"=="3" goto analyze_dump
if "!choice!"=="4" goto exit

echo Invalid choice, please try again
echo.
goto menu

:start_detailed
echo Starting software editor with detailed monitoring...
start "" ".\build\software-editor.exe"
echo Program started, waiting for initialization...
timeout /t 5 /nobreak >nul

echo.
echo Starting crash detection monitoring...
echo Press Ctrl+C to stop monitoring
echo ====================

set crash_detected=0
set monitor_count=0

:monitor_loop
set /a monitor_count+=1
cls
echo Monitoring Cycle: !monitor_count!
echo Time: %date% %time%
echo ====================

tasklist /fi "imagename eq software-editor.exe" /fo table

set pid=
for /f "tokens=2" %%i in ('tasklist /fi "imagename eq software-editor.exe" /fo csv ^| findstr software-editor') do set pid=%%i

if defined pid (
    echo.
    echo Process Details:
    echo PID: !pid!
    
    echo.
    echo System Resource Analysis:
    
    set threads=
    set memory=
    set handles=
    set cpu_time=
    
    for /f "tokens=2 delims==" %%j in ('wmic process where "processid=!pid!" get threadcount /value ^| findstr ThreadCount') do set threads=%%j
    for /f "tokens=2 delims==" %%j in ('wmic process where "processid=!pid!" get workingsetsize /value ^| findstr WorkingSetSize') do set memory=%%j
    for /f "tokens=2 delims==" %%j in ('wmic process where "processid=!pid!" get handlecount /value ^| findstr HandleCount') do set handles=%%j
    for /f "tokens=2 delims==" %%j in ('wmic process where "processid=!pid!" get kernelmodetime /value ^| findstr KernelModeTime') do set cpu_time=%%j
    
    if defined memory (
        set /a memory_mb=!memory!/1048576
        echo Threads: !threads!
        echo Memory: !memory_mb! MB
        echo Handles: !handles!
        if defined cpu_time (
            set /a cpu_seconds=!cpu_time!/10000000
            echo CPU Time: !cpu_seconds! seconds
        )
        echo.
        echo Status: Running
        
        echo.
        echo Crash Detection Analysis:
        if !threads! gtr 50 (
            echo WARNING: High thread count (!threads!) - possible thread leak
            set crash_detected=1
        )
        if !memory_mb! gtr 500 (
            echo WARNING: High memory usage (!memory_mb! MB) - possible memory leak
            set crash_detected=1
        )
        if !handles! gtr 1000 (
            echo WARNING: High handle count (!handles!) - possible resource leak
            set crash_detected=1
        )
        
        if !crash_detected!==1 (
            echo.
            echo CRASH DETECTED: Potential resource issues identified
            echo.
            echo Debug Information:
            echo Process may be stuck in resource cleanup
            echo Check closeEvent and destructor logic
            echo.
            set /p continue=Press any key to continue monitoring...
        )
    )
) else (
    echo.
    echo Status: Program has exited normally
    echo Total monitoring cycles: !monitor_count!
    if !crash_detected!==1 (
        echo Program exited after crash detection
    ) else (
        echo Program exited cleanly
    )
    goto menu
)

echo Waiting 3 seconds for next check...
timeout /t 3 /nobreak >nul
goto monitor_loop

:check_state
echo.
echo Checking current process state...
echo ====================

tasklist /fi "imagename eq software-editor.exe" /fo table

set pid=
for /f "tokens=2" %%i in ('tasklist /fi "imagename eq software-editor.exe" /fo csv ^| findstr software-editor') do set pid=%%i

if defined pid (
    echo.
    echo Process is running with PID: !pid!
    echo.
    echo Detailed Analysis:
    
    for /f "tokens=2 delims==" %%j in ('wmic process where "processid=!pid!" get threadcount /value ^| findstr ThreadCount') do echo Threads: %%j
    for /f "tokens=2 delims==" %%j in ('wmic process where "processid=!pid!" get workingsetsize /value ^| findstr WorkingSetSize') do (
        set memory=%%j
        set /a memory_mb=!memory!/1048576
        echo Memory: !memory_mb! MB
    )
    for /f "tokens=2 delims==" %%j in ('wmic process where "processid=!pid!" get handlecount /value ^| findstr HandleCount') do echo Handles: %%j
    
    echo.
    echo If program is stuck, check:
    echo 1. closeEvent function in product_mainwindow.cpp
    echo 2. Destructor logic
    echo 3. QApplication::quit() call
) else (
    echo Program is not running
)

echo.
set /p continue=Press any key to return to menu...
goto menu

:analyze_dump
echo.
echo Crash dump analysis not available in this version
echo Check Windows Event Viewer for crash details
echo.
set /p continue=Press any key to return to menu...
goto menu

:exit
echo Exiting debug tool
exit