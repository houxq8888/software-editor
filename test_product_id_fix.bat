@echo off
echo ============================================
echo 测试产品ID状态栏显示修复
echo ============================================
echo.

echo 1. 启动软件编辑器（不加载任何产品文件）
echo    预期结果：状态栏不应显示产品ID
echo.
."build\software-editor.exe"

echo.
echo 2. 测试完成
echo.
pause