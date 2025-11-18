@echo off
REM Qt Tools Build Script for Windows
REM Dependencies: Qt 6.8.3, CMake 3.16+, Ninja, Visual Studio 2022

echo Debug: Starting build script...

setlocal enabledelayedexpansion

REM Set Qt installation path (modify according to your actual installation)
set QT_DIR=D:\Qt\6.9.1\mingw_64
echo Debug: QT_DIR = %QT_DIR%

REM Configure MinGW environment
set MINGW_DIR=D:\Qt\Tools\mingw1310_64
echo Debug: MINGW_DIR = %MINGW_DIR%
set PATH=%MINGW_DIR%\bin;%PATH%

REM Create build directory
set BUILD_DIR=d:\virtualMachine\github\software-editor\build
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
cd "%BUILD_DIR%"
echo Debug: Current directory is %CD%

REM Run CMake to configure the project
echo Debug: Running CMake configuration...
"D:\Qt\Tools\CMake_64\bin\cmake.exe" -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="%QT_DIR%" -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -DFEATURE_clang=OFF ..
echo Debug: CMake configuration returned %errorlevel%

if %errorlevel% neq 0 (
    echo CMake configuration failed
    exit /b 1
)

REM Build the project
echo Debug: Building project...
mingw32-make

if %errorlevel% neq 0 (
    echo Build failed
    exit /b 1
)

echo Build succeeded
cd ..
endlocal