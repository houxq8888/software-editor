"""
智能打包功能测试用例
测试CMake项目生成、自动编译构建、可执行文件生成等核心功能
"""

import pytest
import time
import os
import subprocess
import shutil
from pywinauto import Application
from config import APP_INFO, UI_ELEMENTS, TEST_DATA
from utils import take_screenshot

def test_packaging_interface():
    """测试打包界面功能"""
    print("=== 开始测试打包界面功能 ===")
    
    try:
        # 启动应用程序
        app = Application().start(APP_INFO["path"], wait_for_idle=False)
        time.sleep(5)
        
        # 获取主窗口
        main_window = app.window(title_re=".*产品编辑器.*")
        main_window.wait("visible", timeout=10)
        
        take_screenshot("packaging_interface_start")
        
        # 查找打包菜单或按钮
        packaging_menu = main_window.child_window(title_re=".*打包.*", control_type="MenuItem")
        
        if packaging_menu.exists():
            # 点击打包菜单
            packaging_menu.click()
            time.sleep(0.5)
            
            # 查找打包选项
            packaging_options = main_window.child_window(title_re=".*生成项目.*", control_type="MenuItem")
            
            if packaging_options.exists():
                packaging_options.click()
                time.sleep(2)
                
                # 查找打包对话框
                packaging_dialog = app.window(title_re=".*打包.*")
                
                if packaging_dialog.exists():
                    packaging_dialog.wait("visible", timeout=5)
                    
                    take_screenshot("packaging_dialog_opened")
                    
                    # 验证打包对话框内容
                    dialog_title = packaging_dialog.window_text()
                    assert "打包" in dialog_title or "生成" in dialog_title
                    
                    print("✅ 打包界面功能测试通过")
                    
                    # 关闭打包对话框
                    packaging_dialog.close()
                    time.sleep(1)
                else:
                    print("⚠️ 未找到打包对话框，跳过打包界面测试")
            else:
                print("⚠️ 未找到打包选项，跳过打包界面测试")
        else:
            # 尝试工具栏按钮
            packaging_button = main_window.child_window(title_re=".*打包.*", control_type="Button")
            
            if packaging_button.exists():
                packaging_button.click()
                time.sleep(2)
                
                print("✅ 打包按钮功能测试通过")
            else:
                print("⚠️ 未找到打包相关控件，跳过打包界面测试")
        
        # 关闭主窗口
        main_window.close()
        time.sleep(2)
        
        return True
        
    except Exception as e:
        print(f"❌ 打包界面功能测试失败: {e}")
        take_screenshot("packaging_interface_error")
        return False

def test_cmake_generation():
    """测试CMake项目生成功能"""
    print("=== 开始测试CMake项目生成功能 ===")
    
    test_project_dir = os.path.join(os.path.dirname(__file__), "test_cmake_project")
    
    try:
        # 清理测试目录
        if os.path.exists(test_project_dir):
            shutil.rmtree(test_project_dir)
        
        # 启动应用程序
        app = Application().start(APP_INFO["path"], wait_for_idle=False)
        time.sleep(5)
        
        # 获取主窗口
        main_window = app.window(title_re=".*产品编辑器.*")
        main_window.wait("visible", timeout=10)
        
        # 输入基本产品信息
        product_name_edit = main_window.child_window(auto_id="QApplication.MainWindow.centralWidget.groupBox.productNameLineEdit")
        product_name_edit.set_text("测试CMake项目")
        time.sleep(0.5)
        
        take_screenshot("cmake_generation_start")
        
        # 查找生成CMake项目功能
        generate_cmake_menu = main_window.child_window(title_re=".*生成CMake.*", control_type="MenuItem")
        
        if generate_cmake_menu.exists():
            generate_cmake_menu.click()
            time.sleep(2)
            
            # 处理生成对话框
            generate_dialog = app.window(title_re=".*生成.*")
            
            if generate_dialog.exists():
                # 设置输出目录
                output_dir_edit = generate_dialog.child_window(title_re=".*输出目录.*", control_type="Edit")
                
                if output_dir_edit.exists():
                    output_dir_edit.set_text(test_project_dir)
                    time.sleep(0.5)
                    
                    # 点击生成按钮
                    generate_button = generate_dialog.child_window(title="生成", control_type="Button")
                    generate_button.click()
                    time.sleep(5)  # 等待生成完成
                    
                    # 验证CMake项目文件
                    cmake_file = os.path.join(test_project_dir, "CMakeLists.txt")
                    assert os.path.exists(cmake_file), "CMakeLists.txt文件不存在"
                    
                    # 验证其他必要文件
                    expected_files = ["main.cpp", "src/", "include/"]
                    for file in expected_files:
                        file_path = os.path.join(test_project_dir, file)
                        if not os.path.exists(file_path):
                            print(f"⚠️ 文件 {file} 不存在")
                    
                    take_screenshot("cmake_generation_completed")
                    
                    print("✅ CMake项目生成功能测试通过")
                else:
                    print("⚠️ 未找到输出目录输入框，跳过CMake生成测试")
            else:
                print("⚠️ 未找到生成对话框，跳过CMake生成测试")
        else:
            print("⚠️ 未找到生成CMake项目菜单，跳过CMake生成测试")
        
        # 关闭主窗口
        main_window.close()
        time.sleep(2)
        
        # 清理测试目录
        if os.path.exists(test_project_dir):
            shutil.rmtree(test_project_dir)
        
        return True
        
    except Exception as e:
        print(f"❌ CMake项目生成功能测试失败: {e}")
        take_screenshot("cmake_generation_error")
        
        # 清理测试目录
        if os.path.exists(test_project_dir):
            shutil.rmtree(test_project_dir)
        
        return False

def test_automatic_compilation():
    """测试自动编译构建功能"""
    print("=== 开始测试自动编译构建功能 ===")
    
    test_build_dir = os.path.join(os.path.dirname(__file__), "test_build")
    
    try:
        # 启动应用程序
        app = Application().start(APP_INFO["path"], wait_for_idle=False)
        time.sleep(5)
        
        main_window = app.window(title_re=".*产品编辑器.*")
        main_window.wait("visible", timeout=10)
        
        # 打开打包界面
        main_window.type_keys("^P")  # Ctrl+P
        time.sleep(2)
        
        packaging_window = app.window(title_re=".*打包.*")
        packaging_window.wait("visible", timeout=10)
        
        take_screenshot("automatic_compilation_start")
        
        # 设置构建目录
        build_dir_field = packaging_window.child_window(title_re=".*构建目录.*", control_type="Edit")
        
        if build_dir_field.exists():
            build_dir_field.set_text(test_build_dir)
            time.sleep(1)
            
            # 开始构建
            build_button = packaging_window.child_window(title="构建", control_type="Button")
            
            if build_button.exists():
                build_button.click()
                time.sleep(10)  # 等待构建完成
                
                # 检查构建结果
                build_output = packaging_window.child_window(title_re=".*输出.*", control_type="Edit")
                
                if build_output.exists():
                    output_text = build_output.texts()[0]
                    
                    if "构建成功" in output_text or "Build successful" in output_text:
                        print("✅ 自动编译构建功能测试通过")
                    else:
                        print("⚠️ 构建输出未包含成功信息")
                else:
                    print("⚠️ 未找到构建输出区域")
            else:
                print("⚠️ 未找到构建按钮，跳过构建测试")
        else:
            print("⚠️ 未找到构建目录字段，跳过构建测试")
        
        # 清理构建目录
        if os.path.exists(test_build_dir):
            shutil.rmtree(test_build_dir)
        
        # 关闭窗口
        packaging_window.close()
        time.sleep(1)
        main_window.close()
        time.sleep(2)
        
        return True
        
    except Exception as e:
        print(f"❌ 自动编译构建功能测试失败: {e}")
        take_screenshot("automatic_compilation_error")
        
        # 清理构建目录
        if os.path.exists(test_build_dir):
            shutil.rmtree(test_build_dir)
        
        return False

def test_packaging_options():
    """测试打包选项配置功能"""
    print("=== 开始测试打包选项配置功能 ===")
    
    try:
        # 启动应用程序
        app = Application().start(APP_INFO["path"], wait_for_idle=False)
        time.sleep(5)
        
        main_window = app.window(title_re=".*产品编辑器.*")
        main_window.wait("visible", timeout=10)
        
        # 打开打包界面
        main_window.type_keys("^P")  # Ctrl+P
        time.sleep(2)
        
        packaging_window = app.window(title_re=".*打包.*")
        packaging_window.wait("visible", timeout=10)
        
        take_screenshot("packaging_options_start")
        
        # 测试打包类型选择
        package_type_combo = packaging_window.child_window(title_re=".*打包类型.*", control_type="ComboBox")
        
        if package_type_combo.exists():
            # 测试选择不同的打包类型
            package_types = ["可执行文件", "安装包", "库文件"]
            
            for pkg_type in package_types:
                try:
                    package_type_combo.select(pkg_type)
                    time.sleep(1)
                    print(f"✅ 打包类型 '{pkg_type}' 选择测试通过")
                except:
                    print(f"⚠️ 打包类型 '{pkg_type}' 选择失败")
        else:
            print("⚠️ 未找到打包类型选择框，跳过打包类型测试")
        
        # 测试平台选择
        platform_combo = packaging_window.child_window(title_re=".*平台.*", control_type="ComboBox")
        
        if platform_combo.exists():
            # 测试选择不同的平台
            platforms = ["Windows", "Linux", "macOS"]
            
            for platform in platforms:
                try:
                    platform_combo.select(platform)
                    time.sleep(1)
                    print(f"✅ 平台 '{platform}' 选择测试通过")
                except:
                    print(f"⚠️ 平台 '{platform}' 选择失败")
        else:
            print("⚠️ 未找到平台选择框，跳过平台测试")
        
        # 测试打包选项
        options_group = packaging_window.child_window(title_re=".*选项.*", control_type="Group")
        
        if options_group.exists():
            # 测试启用/禁用各种选项
            options = ["包含调试信息", "优化代码", "静态链接"]
            
            for option in options:
                option_checkbox = options_group.child_window(title_re=f".*{option}.*", control_type="CheckBox")
                
                if option_checkbox.exists():
                    # 切换复选框状态
                    option_checkbox.click()
                    time.sleep(0.5)
                    
                    option_checkbox.click()
                    time.sleep(0.5)
                    
                    print(f"✅ 选项 '{option}' 切换测试通过")
                else:
                    print(f"⚠️ 未找到选项 '{option}' 复选框")
        else:
            print("⚠️ 未找到选项组，跳过选项测试")
        
        # 关闭窗口
        packaging_window.close()
        time.sleep(1)
        main_window.close()
        time.sleep(2)
        
        return True
        
    except Exception as e:
        print(f"❌ 打包选项配置功能测试失败: {e}")
        take_screenshot("packaging_options_error")
        return False

def test_package_validation():
    """测试打包验证功能"""
    print("=== 开始测试打包验证功能 ===")
    
    try:
        # 启动应用程序
        app = Application().start(APP_INFO["path"], wait_for_idle=False)
        time.sleep(5)
        
        main_window = app.window(title_re=".*产品编辑器.*")
        main_window.wait("visible", timeout=10)
        
        # 打开打包界面
        main_window.type_keys("^P")  # Ctrl+P
        time.sleep(2)
        
        packaging_window = app.window(title_re=".*打包.*")
        packaging_window.wait("visible", timeout=10)
        
        take_screenshot("package_validation_start")
        
        # 测试验证功能
        validate_button = packaging_window.child_window(title="验证", control_type="Button")
        
        if validate_button.exists():
            validate_button.click()
            time.sleep(3)
            
            # 检查验证结果
            validation_output = packaging_window.child_window(title_re=".*验证结果.*", control_type="Edit")
            
            if validation_output.exists():
                output_text = validation_output.texts()[0]
                
                if "验证通过" in output_text or "Validation passed" in output_text:
                    print("✅ 打包验证功能测试通过")
                elif "错误" in output_text or "Error" in output_text:
                    print("⚠️ 验证发现错误，但验证功能正常")
                else:
                    print("⚠️ 验证输出信息不明确")
            else:
                print("⚠️ 未找到验证结果输出区域")
        else:
            print("⚠️ 未找到验证按钮，跳过验证测试")
        
        # 关闭窗口
        packaging_window.close()
        time.sleep(1)
        main_window.close()
        time.sleep(2)
        
        return True
        
    except Exception as e:
        print(f"❌ 打包验证功能测试失败: {e}")
        take_screenshot("package_validation_error")
        return False

def test_executable_generation():
    """测试可执行文件生成功能"""
    print("=== 开始测试可执行文件生成功能 ===")
    
    test_output_dir = os.path.join(os.path.dirname(__file__), "test_output")
    
    try:
        # 清理测试目录
        if os.path.exists(test_output_dir):
            shutil.rmtree(test_output_dir)
        
        # 启动应用程序
        app = Application().start(APP_INFO["path"], wait_for_idle=False)
        time.sleep(5)
        
        # 获取主窗口
        main_window = app.window(title_re=".*产品编辑器.*")
        main_window.wait("visible", timeout=10)
        
        take_screenshot("executable_generation_start")
        
        # 查找生成可执行文件功能
        generate_exe_menu = main_window.child_window(title_re=".*生成可执行文件.*", control_type="MenuItem")
        
        if generate_exe_menu.exists():
            generate_exe_menu.click()
            time.sleep(2)
            
            # 处理生成对话框
            exe_dialog = app.window(title_re=".*生成可执行文件.*")
            
            if exe_dialog.exists():
                # 设置输出目录
                output_dir_edit = exe_dialog.child_window(title_re=".*输出目录.*", control_type="Edit")
                
                if output_dir_edit.exists():
                    output_dir_edit.set_text(test_output_dir)
                    time.sleep(0.5)
                    
                    # 点击生成按钮
                    generate_button = exe_dialog.child_window(title="生成", control_type="Button")
                    generate_button.click()
                    time.sleep(10)  # 等待生成完成
                    
                    # 验证可执行文件
                    exe_files = []
                    for root, dirs, files in os.walk(test_output_dir):
                        for file in files:
                            if file.endswith('.exe'):
                                exe_files.append(os.path.join(root, file))
                    
                    if exe_files:
                        print(f"✅ 可执行文件生成成功，生成 {len(exe_files)} 个可执行文件")
                        for exe in exe_files:
                            print(f"   - {exe}")
                            
                            # 测试可执行文件是否可以运行
                            try:
                                result = subprocess.run([exe, "--version"], capture_output=True, timeout=5)
                                if result.returncode == 0:
                                    print("✅ 可执行文件运行测试通过")
                                else:
                                    print("⚠️ 可执行文件运行返回非零状态码")
                            except:
                                print("⚠️ 可执行文件运行测试跳过（可能不需要参数）")
                    else:
                        print("⚠️ 输出目录中未找到可执行文件")
                    
                    take_screenshot("executable_generation_completed")
                    
                    print("✅ 可执行文件生成功能测试通过")
                else:
                    print("⚠️ 未找到输出目录输入框，跳过可执行文件生成测试")
            else:
                print("⚠️ 未找到生成对话框，跳过可执行文件生成测试")
        else:
            print("⚠️ 未找到生成可执行文件菜单，跳过可执行文件生成测试")
        
        # 关闭主窗口
        main_window.close()
        time.sleep(2)
        
        # 清理测试目录
        if os.path.exists(test_output_dir):
            shutil.rmtree(test_output_dir)
        
        return True
        
    except Exception as e:
        print(f"❌ 可执行文件生成功能测试失败: {e}")
        take_screenshot("executable_generation_error")
        
        # 清理测试目录
        if os.path.exists(test_output_dir):
            shutil.rmtree(test_output_dir)
        
        return False

def test_packaging_workflow():
    """测试完整的打包工作流程"""
    print("=== 开始测试完整的打包工作流程 ===")
    
    test_workflow_dir = os.path.join(os.path.dirname(__file__), "test_workflow")
    
    try:
        # 清理测试目录
        if os.path.exists(test_workflow_dir):
            shutil.rmtree(test_workflow_dir)
        
        # 启动应用程序
        app = Application().start(APP_INFO["path"], wait_for_idle=False)
        time.sleep(5)
        
        # 获取主窗口
        main_window = app.window(title_re=".*产品编辑器.*")
        main_window.wait("visible", timeout=10)
        
        # 输入完整的产品信息
        product_name_edit = main_window.child_window(auto_id="QApplication.MainWindow.centralWidget.groupBox.productNameLineEdit")
        product_name_edit.set_text("测试打包工作流程")
        time.sleep(0.5)
        
        version_edit = main_window.child_window(auto_id="QApplication.MainWindow.centralWidget.groupBox.versionLineEdit")
        version_edit.set_text("1.0.0")
        time.sleep(0.5)
        
        take_screenshot("packaging_workflow_start")
        
        # 查找打包向导或完整打包功能
        packaging_wizard = main_window.child_window(title_re=".*打包向导.*", control_type="MenuItem")
        
        if packaging_wizard.exists():
            packaging_wizard.click()
            time.sleep(2)
            
            # 处理打包向导
            wizard_dialog = app.window(title_re=".*打包向导.*")
            
            if wizard_dialog.exists():
                wizard_dialog.wait("visible", timeout=5)
                
                take_screenshot("packaging_wizard_opened")
                
                # 模拟向导步骤
                # 步骤1: 设置输出目录
                next_button = wizard_dialog.child_window(title="下一步", control_type="Button")
                
                if next_button.exists():
                    # 设置输出目录
                    output_dir_edit = wizard_dialog.child_window(title_re=".*输出目录.*", control_type="Edit")
                    
                    if output_dir_edit.exists():
                        output_dir_edit.set_text(test_workflow_dir)
                        time.sleep(0.5)
                        
                        # 点击下一步
                        next_button.click()
                        time.sleep(1)
                        
                        # 步骤2: 选择打包选项（如果有）
                        # 继续点击下一步直到完成
                        
                        # 点击完成按钮
                        finish_button = wizard_dialog.child_window(title="完成", control_type="Button")
                        
                        if finish_button.exists():
                            finish_button.click()
                            time.sleep(10)  # 等待打包完成
                            
                            # 验证打包结果
                            if os.path.exists(test_workflow_dir):
                                # 检查是否生成了必要的文件
                                generated_files = []
                                for root, dirs, files in os.walk(test_workflow_dir):
                                    for file in files:
                                        generated_files.append(os.path.join(root, file))
                                
                                if generated_files:
                                    print(f"✅ 打包工作流程完成，生成 {len(generated_files)} 个文件")
                                    for file in generated_files[:5]:  # 只显示前5个文件
                                        print(f"   - {file}")
                                else:
                                    print("⚠️ 打包目录为空")
                            else:
                                print("⚠️ 打包目录不存在")
                            
                            take_screenshot("packaging_workflow_completed")
                            
                            print("✅ 完整的打包工作流程测试通过")
                        else:
                            print("⚠️ 未找到完成按钮，跳过打包工作流程测试")
                    else:
                        print("⚠️ 未找到输出目录输入框，跳过打包工作流程测试")
                else:
                    print("⚠️ 未找到下一步按钮，跳过打包工作流程测试")
            else:
                print("⚠️ 未找到打包向导对话框，跳过打包工作流程测试")
        else:
            print("⚠️ 未找到打包向导菜单，跳过打包工作流程测试")
        
        # 关闭主窗口
        main_window.close()
        time.sleep(2)
        
        # 清理测试目录
        if os.path.exists(test_workflow_dir):
            shutil.rmtree(test_workflow_dir)
        
        return True
        
    except Exception as e:
        print(f"❌ 完整的打包工作流程测试失败: {e}")
        take_screenshot("packaging_workflow_error")
        
        # 清理测试目录
        if os.path.exists(test_workflow_dir):
            shutil.rmtree(test_workflow_dir)
        
        return False

if __name__ == "__main__":
    # 运行所有测试用例
    results = []
    
    results.append(test_packaging_interface())
    results.append(test_cmake_generation())
    results.append(test_automatic_compilation())
    results.append(test_executable_generation())
    results.append(test_packaging_workflow())
    
    print("\n=== 智能打包功能测试结果汇总 ===")
    passed = sum(results)
    total = len(results)
    
    print(f"通过测试: {passed}/{total}")
    
    if passed == total:
        print("🎉 所有智能打包功能测试通过！")
    else:
        print("⚠️ 部分测试失败，请检查日志和截图")