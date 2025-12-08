@echo off
echo ============================================
echo 测试showNewForm()日志输出
echo ============================================
echo.

echo 1. 启动软件编辑器（不加载任何文件）
echo    预期结果：debug.log文件中应该包含showNewForm相关的日志
echo.
."build\software-editor.exe"

echo.
echo 2. 检查debug.log文件内容
type "build\debug.log" 2>nul
echo.

echo 3. 如果debug.log文件不存在，说明日志配置有问题
echo.
pause