@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

echo ========================================
echo   依赖复制功能测试脚本
echo ========================================
echo.

:: 设置测试参数
set "TEST_EXE=build\software-editor.exe"
set "TEST_OUTPUT=test_output"
set "TEST_REPORT=test_dependency_report.txt"

echo 测试配置:
echo - 测试EXE: %TEST_EXE%
echo - 输出目录: %TEST_OUTPUT%
echo - 报告文件: %TEST_REPORT%
echo.

:: 检查测试EXE是否存在
if not exist "%TEST_EXE%" (
    echo 错误: 测试EXE文件不存在
    echo 请先编译项目: .\build.bat
    pause
    exit /b 1
)

echo 开始依赖复制功能测试...
echo.

:: 清理之前的测试输出
if exist "%TEST_OUTPUT%" (
    echo 清理之前的测试输出...
    rmdir /s /q "%TEST_OUTPUT%" >nul 2>&1
)

:: 测试1: 使用基础依赖复制脚本
echo [测试1] 基础依赖复制脚本
echo ===========================
call scripts\copy_dependencies.bat "%TEST_EXE%" "%TEST_OUTPUT%_basic"

if errorlevel 1 (
    echo ❌ 基础依赖复制测试失败
) else (
    echo ✅ 基础依赖复制测试成功
    
    :: 检查复制的文件
    echo.
    echo 基础测试结果:
    if exist "%TEST_OUTPUT%_basic\software-editor.exe" (
        echo - 主程序: ✅ 存在
    ) else (
        echo - 主程序: ❌ 缺失
    )
    
    if exist "%TEST_OUTPUT%_basic\Qt6Core.dll" (
        echo - Qt运行时: ✅ 存在
    ) else (
        echo - Qt运行时: ❌ 缺失
    )
    
    if exist "%TEST_OUTPUT%_basic\plugins\platforms\qwindows.dll" (
        echo - Qt插件: ✅ 存在
    ) else (
        echo - Qt插件: ❌ 缺失
    )
    
    if exist "%TEST_OUTPUT%_basic\dependency_report.txt" (
        echo - 依赖报告: ✅ 存在
    ) else (
        echo - 依赖报告: ❌ 缺失
    )
)

echo.

:: 测试2: 使用高级依赖复制脚本
echo [测试2] 高级依赖复制脚本
echo ===========================
call scripts\advanced_dependency_copy.bat "%TEST_EXE%" "%TEST_OUTPUT%_advanced"

if errorlevel 1 (
    echo ❌ 高级依赖复制测试失败
) else (
    echo ✅ 高级依赖复制测试成功
    
    :: 检查复制的文件
    echo.
    echo 高级测试结果:
    if exist "%TEST_OUTPUT%_advanced\software-editor.exe" (
        echo - 主程序: ✅ 存在
    ) else (
        echo - 主程序: ❌ 缺失
    )
    
    if exist "%TEST_OUTPUT%_advanced\Qt6Core.dll" (
        echo - Qt运行时: ✅ 存在
    ) else (
        echo - Qt运行时: ❌ 缺失
    )
    
    if exist "%TEST_OUTPUT%_advanced\plugins" (
        echo - Qt插件目录: ✅ 存在
    ) else (
        echo - Qt插件目录: ❌ 缺失
    )
    
    if exist "%TEST_OUTPUT%_advanced\advanced_dependency_report.txt" (
        echo - 高级依赖报告: ✅ 存在
    ) else (
        echo - 高级依赖报告: ❌ 缺失
    )
)

echo.

:: 测试3: 集成测试（通过智能打包功能）
echo [测试3] 智能打包集成测试
echo ===========================
echo 注意: 此测试需要运行软件编辑器并执行智能打包功能
echo 请手动测试以下步骤:
echo 1. 运行 .\build\software-editor.exe
echo 2. 创建或打开一个项目
echo 3. 执行智能打包功能
echo 4. 检查输出目录是否包含完整的依赖文件
echo.

:: 生成测试报告
echo 生成测试报告...
(
    echo 依赖复制功能测试报告
    echo =====================
    echo.
    echo 测试时间: %date% %time%
    echo 测试环境: Windows
    echo.
    echo 测试结果:
    echo.
    echo 基础依赖复制测试:
    if exist "%TEST_OUTPUT%_basic\software-editor.exe" (
        echo - 状态: 通过
        echo - 文件数量: 
        set /a basic_count=0
        for /f %%f in ('dir /b "%TEST_OUTPUT%_basic" ^| find /c /v ""') do set /a basic_count=%%f
        echo   !basic_count! 个文件
    ) else (
        echo - 状态: 失败
    )
    echo.
    echo 高级依赖复制测试:
    if exist "%TEST_OUTPUT%_advanced\software-editor.exe" (
        echo - 状态: 通过
        echo - 文件数量: 
        set /a advanced_count=0
        for /f %%f in ('dir /b "%TEST_OUTPUT%_advanced" ^| find /c /v ""') do set /a advanced_count=%%f
        echo   !advanced_count! 个文件
    ) else (
        echo - 状态: 失败
    )
    echo.
    echo 建议:
    echo - 基础脚本适合简单的依赖复制需求
    echo - 高级脚本使用windeployqt，更适合复杂的Qt应用程序
    echo - 建议在实际目标系统上测试程序运行情况
    echo.
    echo 文件位置:
    echo - 基础测试输出: %TEST_OUTPUT%_basic
    echo - 高级测试输出: %TEST_OUTPUT%_advanced
) > "%TEST_REPORT%"

echo 测试报告已生成: %TEST_REPORT%
echo.

:: 显示测试结果摘要
echo ========================================
echo   测试结果摘要
echo ========================================
echo.

type "%TEST_REPORT%"

echo.
echo ========================================
echo   测试完成
echo ========================================
echo.
echo 下一步:
echo 1. 检查测试输出目录中的文件是否完整
echo 2. 在实际系统上测试复制的程序能否正常运行
echo 3. 如有问题，参考依赖报告进行调试
echo.

pause