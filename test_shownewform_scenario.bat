@echo off
echo ============================================
echo 测试showNewForm()调用场景 - 修复后验证
echo ============================================
echo.

echo 场景1：直接启动软件编辑器（无参数）
echo    预期结果：应该调用showNewForm()，debug.log中会有相关日志
echo.
if exist "build\debug.log" del "build\debug.log"
echo 启动软件编辑器...
start /wait "" "build\software-editor.exe"
echo.
echo 检查debug.log文件内容：
type "build\debug.log" 2>nul
echo.

echo 场景2：通过产品配置文件启动（带UI文件参数）
echo    预期结果：不应该调用showNewForm()，debug.log中不会有相关日志
echo.
if exist "build\debug.log" del "build\debug.log"
echo 启动软件编辑器并打开产品配置文件...
start /wait "" "build\software-editor.exe" "test_product.json"
echo.
echo 检查debug.log文件内容：
type "build\debug.log" 2>nul
echo.

echo 场景3：直接打开UI文件（带.ui文件参数）
echo    预期结果：不应该调用showNewForm()，debug.log中不会有相关日志
echo.
if exist "build\debug.log" del "build\debug.log"
echo 启动软件编辑器并直接打开UI文件...
start /wait "" "build\software-editor.exe" "43214.ui"
echo.
echo 检查debug.log文件内容：
type "build\debug.log" 2>nul
echo.

pause