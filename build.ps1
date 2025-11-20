# 构建脚本

# 设置Qt和MinGW路径
$QT_DIR = "D:\Qt\6.9.1\mingw_64"
$MINGW_DIR = "D:\Qt\Tools\mingw1310_64"
$CMAKE_DIR = "D:\Qt\Tools\CMake_64\bin"

# 设置环境变量
$env:PATH = "$MINGW_DIR\bin;$CMAKE_DIR;$env:PATH"

# 进入项目目录
Set-Location "d:\virtualMachine\github\software-editor"

# 创建build目录
if (-not (Test-Path "build")) {
    New-Item -ItemType Directory -Name "build" | Out-Null
}
Set-Location "build"

# 运行CMake
Write-Host "Running CMake..."
& "$CMAKE_DIR\cmake.exe" -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="$QT_DIR" -DCMAKE_BUILD_TYPE=Release ..
if ($LASTEXITCODE -ne 0) {
    Write-Host "CMake configuration failed" -ForegroundColor Red
    exit 1
}

# 编译
Write-Host "Building project..."
& "$MINGW_DIR\bin\mingw32-make.exe"
if ($LASTEXITCODE -ne 0) {
    Write-Host "Compilation failed" -ForegroundColor Red
    exit 1
}

Write-Host "Build succeeded" -ForegroundColor Green
