@echo off
chcp 65001 >nul

echo ===============================================
echo showNewForm()调用抑制修复测试
echo ===============================================
echo.

echo 测试1: 传入UI文件，检查是否显示新建表单对话框
echo -----------------------------------------------
echo 启动Qt Designer并传入UI文件...
start /wait .\build\software-editor.exe .\test\data\mainwindow.ui

echo.
echo 测试2: 传入不存在的UI文件，检查是否显示新建表单对话框
echo -----------------------------------------------
echo 启动Qt Designer并传入不存在的UI文件...
start /wait .\build\software-editor.exe .\test\data\nonexistent.ui

echo.
echo 测试3: 传入JSON文件（产品配置），检查是否显示新建表单对话框
echo -----------------------------------------------
echo 启动Qt Designer并传入JSON文件...
start /wait .\build\software-editor.exe .\test_product.json

echo.
echo 测试4: 不传入任何文件，检查是否显示新建表单对话框
echo -----------------------------------------------
echo 启动Qt Designer，不传入任何文件...
start /wait .\build\software-editor.exe

echo.
echo 测试5: 传入多个文件，检查是否显示新建表单对话框
echo -----------------------------------------------
echo 启动Qt Designer并传入多个文件...
start /wait .\build\software-editor.exe .\test\data\mainwindow.ui .\test_product.json

echo.
echo ===============================================
echo 测试完成！
echo ===============================================
echo.
echo 预期结果：
echo - 测试1: 不应显示新建表单对话框（有UI文件传入）
echo - 测试2: 不应显示新建表单对话框（有文件传入，即使不存在）
echo - 测试3: 可能显示新建表单对话框（JSON文件不抑制）
echo - 测试4: 应显示新建表单对话框（无文件传入）
echo - 测试5: 不应显示新建表单对话框（有文件传入）
echo.
echo 注意：测试3中JSON文件可能不会抑制新建表单对话框，这是正常行为。
pause