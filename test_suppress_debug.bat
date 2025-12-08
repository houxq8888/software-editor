@echo off
echo ===============================================
echo 测试抑制标志调试脚本
echo ===============================================
echo.

REM 清理旧的日志文件
if exist "build\debug.log" (
    echo 删除旧的debug.log文件
    del "build\debug.log"
)

echo.
echo 场景1: 直接启动软件编辑器（无参数）
echo 预期: 应该调用showNewForm()
echo -----------------------------------------------
start /wait "" "build\software-editor.exe"
if exist "build\debug.log" (
    echo 检查debug.log中的抑制标志信息:
    findstr /C:"设置抑制标志" "build\debug.log"
    findstr /C:"workbench抑制标志" "build\debug.log"
    findstr /C:"检查showNewForm条件" "build\debug.log"
    findstr /C:"showNewForm" "build\debug.log"
    echo.
    echo 完整的debug.log内容:
    type "build\debug.log"
) else (
    echo 错误: debug.log文件不存在
)

echo.
echo ===============================================
echo.

REM 清理日志文件
if exist "build\debug.log" (
    echo 删除debug.log文件
    del "build\debug.log"
)

echo.
echo 场景2: 通过UI文件启动（带.ui文件参数）
echo 预期: 不应该调用showNewForm()
echo -----------------------------------------------
start /wait "" "build\software-editor.exe" "43214.ui"
if exist "build\debug.log" (
    echo 检查debug.log中的抑制标志信息:
    findstr /C:"设置抑制标志" "build\debug.log"
    findstr /C:"workbench抑制标志" "build\debug.log"
    findstr /C:"检查showNewForm条件" "build\debug.log"
    findstr /C:"showNewForm" "build\debug.log"
    echo.
    echo 完整的debug.log内容:
    type "build\debug.log"
) else (
    echo 错误: debug.log文件不存在
)

echo.
echo ===============================================
echo 测试完成
echo ===============================================
pause