"""
产品编辑器边界值测试用例
测试空值、超长文本、特殊字符等边界情况
"""

import os
import time
import json
import pyautogui
import pyperclip
import psutil
from datetime import datetime
from pywinauto import Application
from pywinauto.findwindows import ElementNotFoundError

def paste_text_with_clipboard(text):
    """使用剪贴板粘贴文本的辅助函数"""
    try:
        # 保存原始剪贴板内容
        original_clipboard = pyperclip.paste()
        
        # 复制文本到剪贴板
        pyperclip.copy(text)
        time.sleep(0.5)
        
        # 使用Ctrl+V粘贴
        pyautogui.hotkey('ctrl', 'v')
        time.sleep(0.5)
        
        # 恢复原始剪贴板内容
        pyperclip.copy(original_clipboard)
        
        print(f"[INFO] 使用剪贴板粘贴文本: {text}")
    except Exception as e:
        print(f"[ERROR] 剪贴板粘贴失败: {e}")
        # 如果剪贴板失败，回退到直接输入
        pyautogui.write(text)

def get_test_results_dir():
    """获取测试结果目录"""
    today = datetime.now().strftime("%Y-%m-%d")
    results_dir = os.path.join(os.path.dirname(__file__), "test_results", today)
    os.makedirs(results_dir, exist_ok=True)
    return results_dir

def check_application_status(process, app_path):
    """检查应用程序状态"""
    try:
        # 检查进程是否仍在运行
        if process.poll() is not None:
            print(f"[ERROR] 应用程序进程已退出，退出码: {process.returncode}")
            return False
        
        # 检查应用程序窗口是否存在
        try:
            app = Application().connect(path=app_path)
            return True
        except:
            # 尝试通过窗口标题连接
            try:
                app = Application().connect(title_re=".*产品编辑器.*")
                return True
            except:
                print("[ERROR] 应用程序窗口已消失")
                return False
                
    except Exception as e:
        print(f"[ERROR] 检查应用程序状态失败: {e}")
        return False

def safe_operation(process, app_path, operation_func, operation_name, max_retries=3):
    """安全执行操作，包含重试机制"""
    for attempt in range(max_retries):
        try:
            # 检查应用程序状态
            if not check_application_status(process, app_path):
                print(f"[ERROR] 应用程序异常，无法执行操作: {operation_name}")
                return False
            
            # 执行操作
            result = operation_func()
            return result
            
        except Exception as e:
            print(f"[WARNING] {operation_name} 第{attempt+1}次尝试失败: {e}")
            time.sleep(2)  # 等待后重试
            
            # 如果是最后一次尝试，检查应用程序状态
            if attempt == max_retries - 1:
                if not check_application_status(process, app_path):
                    print(f"[ERROR] 应用程序异常，操作失败: {operation_name}")
                    return False
                else:
                    print(f"[ERROR] 操作失败但应用程序正常: {operation_name}")
                    return False
    
    return False

def test_boundary_values():
    """测试边界值情况"""
    
    # 边界值测试数据
    boundary_test_cases = [
        # 空值测试
        {"name": "", "version": "", "category": "", "developer": "", "description": ""},
        
        # 超长文本测试
        {"name": "A" * 500, "version": "1.0.0", "category": "工具软件", "developer": "测试开发者", "description": "这是一个超长描述" * 100},
        
        # 特殊字符测试
        {"name": "产品@#$%^&*()", "version": "1.0.0-beta+test", "category": "工具/软件", "developer": "开发者<test>", "description": "描述包含\n换行\t制表符"},
        
        # 中英文混合测试
        {"name": "产品Product测试Test", "version": "v1.0.0", "category": "工具Tools软件Software", "developer": "开发者Developer", "description": "中文Chinese混合Mixed描述Description"},
        
        # 数字边界测试
        {"name": "产品123", "version": "999.999.999", "category": "工具软件", "developer": "开发者123", "description": "描述1234567890"},
        
        # 空格测试
        {"name": "  产品  ", "version": "  1.0.0  ", "category": "  工具软件  ", "developer": "  开发者  ", "description": "  描述  "},
    ]
    
    # 应用程序路径
    app_path = os.path.join(os.path.dirname(os.path.dirname(os.path.dirname(__file__))), "build", "software-editor.exe")
    
    if not os.path.exists(app_path):
        print(f"[ERROR] 应用程序不存在: {app_path}")
        return False
    
    print("=== 启动软件编辑器进行边界值测试 ===")
    
    # 使用subprocess启动应用程序
    import subprocess
    process = subprocess.Popen([app_path])
    time.sleep(8)
    
    # 安全连接应用程序
    def connect_app():
        try:
            app = Application().connect(title_re=".*产品编辑器.*")
            time.sleep(2)
            return app
        except Exception as e:
            print(f"[WARNING] 连接应用程序失败: {e}")
            app = Application().connect(path=app_path)
            time.sleep(2)
            return app
    
    # 安全获取主窗口
    def get_main_window():
        app = connect_app()
        dlg = app.window(title_re=".*产品编辑器.*")
        dlg.wait("visible", timeout=20)
        return dlg
    
    # 使用安全操作机制连接应用程序
    app = safe_operation(process, app_path, connect_app, "连接应用程序")
    if not app:
        print("[ERROR] 应用程序连接失败")
        process.terminate()
        return False
    
    # 使用安全操作机制获取主窗口
    dlg = safe_operation(process, app_path, get_main_window, "获取主窗口")
    if not dlg:
        print("[ERROR] 无法获取主窗口")
        process.terminate()
        return False
    
    print("[SUCCESS] 软件编辑器启动成功")
    
    test_results = []
    
    try:
        for i, test_case in enumerate(boundary_test_cases):
            print(f"\n=== 测试用例 {i+1}: {test_case['name'][:50] if test_case['name'] else '空值测试'} ===")
            
            # 清空现有数据
            def clear_data():
                try:
                    # 尝试清空产品名称
                    name_edit = dlg.child_window(auto_id="QApplication.MainWindow.centralWidget.tabWidget.qt_tabwidget_stackedwidget.basicInfoTab.groupBox.productNameLineEdit", control_type="Unknown")
                    name_edit.click_input()
                    name_edit.set_text("")
                    time.sleep(0.5)
                    return True
                except Exception as e:
                    print(f"[INFO] 清空产品名称失败: {e}")
                    return False
            
            # 使用安全操作清空数据
            if not safe_operation(process, app_path, clear_data, "清空现有数据"):
                print("[WARNING] 清空数据过程中应用程序异常")
            
            # 输入测试数据
            test_success = True
            
            # 安全输入产品名称
            def input_product_name():
                try:
                    name_edit = dlg.child_window(auto_id="QApplication.MainWindow.centralWidget.tabWidget.qt_tabwidget_stackedwidget.basicInfoTab.groupBox.productNameLineEdit", control_type="Unknown")
                    name_edit.click_input()
                    name_edit.set_text(test_case['name'])
                    time.sleep(0.5)
                    print(f"[SUCCESS] 输入产品名称: {test_case['name'][:50] if test_case['name'] else '空值'}")
                    return True
                except Exception as e:
                    print(f"[WARNING] 自动化输入产品名称失败: {e}")
                    try:
                        # 获取控件位置并点击中心
                        name_edit = dlg.child_window(auto_id="QApplication.MainWindow.centralWidget.tabWidget.qt_tabwidget_stackedwidget.basicInfoTab.groupBox.productNameLineEdit", control_type="Unknown")
                        rect = name_edit.rectangle()
                        center_x = rect.left + (rect.width() // 2)
                        center_y = rect.top + (rect.height() // 2)
                        pyautogui.click(center_x, center_y)
                        pyautogui.hotkey('ctrl', 'a')
                        paste_text_with_clipboard(test_case['name'])
                        time.sleep(0.5)
                        print(f"[INFO] 使用剪贴板输入产品名称")
                        return True
                    except Exception as e2:
                        print(f"[ERROR] 输入产品名称失败: {e2}")
                        return False
        
        # 使用安全操作输入产品名称
        if not safe_operation(process, app_path, input_product_name, "输入产品名称"):
            test_success = False
        
        # 安全输入版本号
        def input_version():
            try:
                version_edit = dlg.child_window(auto_id="QApplication.MainWindow.centralWidget.tabWidget.qt_tabwidget_stackedwidget.basicInfoTab.groupBox.versionLineEdit", control_type="Unknown")
                version_edit.click_input()
                version_edit.set_text(test_case['version'])
                time.sleep(0.5)
                print(f"[SUCCESS] 输入版本号: {test_case['version'][:50] if test_case['version'] else '空值'}")
                return True
            except Exception as e:
                print(f"[WARNING] 自动化输入版本号失败: {e}")
                try:
                    # 获取控件位置并点击中心
                    version_edit = dlg.child_window(auto_id="QApplication.MainWindow.centralWidget.tabWidget.qt_tabwidget_stackedwidget.basicInfoTab.groupBox.versionLineEdit", control_type="Unknown")
                    rect = version_edit.rectangle()
                    center_x = rect.left + (rect.width() // 2)
                    center_y = rect.top + (rect.height() // 2)
                    pyautogui.click(center_x, center_y)
                    pyautogui.hotkey('ctrl', 'a')
                    paste_text_with_clipboard(test_case['version'])
                    time.sleep(0.5)
                    print(f"[INFO] 使用剪贴板输入版本号")
                    return True
                except Exception as e2:
                    print(f"[ERROR] 输入版本号失败: {e2}")
                    return False
        
        # 使用安全操作输入版本号
        if not safe_operation(process, app_path, input_version, "输入版本号"):
            test_success = False
        
        # 安全输入分类
        def input_category():
            try:
                category_edit = dlg.child_window(auto_id="QApplication.MainWindow.centralWidget.tabWidget.qt_tabwidget_stackedwidget.basicInfoTab.groupBox.categoryLineEdit", control_type="Unknown")
                category_edit.click_input()
                category_edit.set_text(test_case['category'])
                time.sleep(0.5)
                print(f"[SUCCESS] 输入分类: {test_case['category'][:50] if test_case['category'] else '空值'}")
                return True
            except Exception as e:
                print(f"[WARNING] 自动化输入分类失败: {e}")
                try:
                    # 获取控件位置并点击中心
                    category_edit = dlg.child_window(auto_id="QApplication.MainWindow.centralWidget.tabWidget.qt_tabwidget_stackedwidget.basicInfoTab.groupBox.categoryLineEdit", control_type="Unknown")
                    rect = category_edit.rectangle()
                    center_x = rect.left + (rect.width() // 2)
                    center_y = rect.top + (rect.height() // 2)
                    pyautogui.click(center_x, center_y)
                    pyautogui.hotkey('ctrl', 'a')
                    paste_text_with_clipboard(test_case['category'])
                    time.sleep(0.5)
                    print(f"[INFO] 使用剪贴板输入分类")
                    return True
                except Exception as e2:
                    print(f"[ERROR] 输入分类失败: {e2}")
                    return False
        
        # 使用安全操作输入分类
        if not safe_operation(process, app_path, input_category, "输入分类"):
            test_success = False
        
        # 安全输入开发者
        def input_developer():
            try:
                developer_edit = dlg.child_window(auto_id="QApplication.MainWindow.centralWidget.tabWidget.qt_tabwidget_stackedwidget.basicInfoTab.groupBox.developerLineEdit", control_type="Unknown")
                developer_edit.click_input()
                developer_edit.set_text(test_case['developer'])
                time.sleep(0.5)
                print(f"[SUCCESS] 输入开发者: {test_case['developer'][:50] if test_case['developer'] else '空值'}")
                return True
            except Exception as e:
                print(f"[WARNING] 自动化输入开发者失败: {e}")
                try:
                    # 获取控件位置并点击中心
                    developer_edit = dlg.child_window(auto_id="QApplication.MainWindow.centralWidget.tabWidget.qt_tabwidget_stackedwidget.basicInfoTab.groupBox.developerLineEdit", control_type="Unknown")
                    rect = developer_edit.rectangle()
                    center_x = rect.left + (rect.width() // 2)
                    center_y = rect.top + (rect.height() // 2)
                    pyautogui.click(center_x, center_y)
                    pyautogui.hotkey('ctrl', 'a')
                    paste_text_with_clipboard(test_case['developer'])
                    time.sleep(0.5)
                    print(f"[INFO] 使用剪贴板输入开发者")
                    return True
                except Exception as e2:
                    print(f"[ERROR] 输入开发者失败: {e2}")
                    return False
        
        # 使用安全操作输入开发者
        if not safe_operation(process, app_path, input_developer, "输入开发者"):
            test_success = False
        
        # 安全输入描述
        def input_description():
            try:
                desc_edit = dlg.child_window(auto_id="QApplication.MainWindow.centralWidget.tabWidget.qt_tabwidget_stackedwidget.basicInfoTab.groupBox_2.descriptionTextEdit", control_type="Unknown")
                desc_edit.click_input()
                desc_edit.set_text(test_case['description'])
                time.sleep(0.5)
                print(f"[SUCCESS] 输入描述: {test_case['description'][:100] if test_case['description'] else '空值'}")
                return True
            except Exception as e:
                print(f"[WARNING] 自动化输入描述失败: {e}")
                try:
                    # 获取控件位置并点击中心
                    desc_edit = dlg.child_window(auto_id="QApplication.MainWindow.centralWidget.tabWidget.qt_tabwidget_stackedwidget.basicInfoTab.groupBox_2.descriptionTextEdit", control_type="Unknown")
                    rect = desc_edit.rectangle()
                    center_x = rect.left + (rect.width() // 2)
                    center_y = rect.top + (rect.height() // 2)
                    pyautogui.click(center_x, center_y)
                    pyautogui.hotkey('ctrl', 'a')
                    paste_text_with_clipboard(test_case['description'])
                    time.sleep(0.5)
                    print(f"[INFO] 使用剪贴板输入描述")
                    return True
                except Exception as e2:
                    print(f"[ERROR] 输入描述失败: {e2}")
                    return False
        
        # 使用安全操作输入描述
        if not safe_operation(process, app_path, input_description, "输入描述"):
            test_success = False
        
        # 保存测试结果
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        filename = f"test_boundary_{i+1}_{timestamp}.json"
        results_dir = get_test_results_dir()
        filepath = os.path.join(results_dir, filename)
        
        # 安全保存测试结果
        def save_data():
            try:
                save_button = dlg.child_window(auto_id="QApplication.MainWindow.centralWidget.groupBox_5.saveButton", control_type="Unknown")
                save_button.click_input()
                time.sleep(2)
                
                # 处理保存对话框
                save_dlg = app.window(title_re=".*保存.*")
                if save_dlg.exists():
                    filename_edit = save_dlg.child_window(auto_id="FileNameControlHost", control_type="Edit")
                    filename_edit.set_text(filepath)
                    time.sleep(1)
                    
                    save_confirm_button = save_dlg.child_window(title="保存", control_type="Button")
                    save_confirm_button.click_input()
                    time.sleep(2)
                    
                    if os.path.exists(filepath):
                        print(f"[SUCCESS] 保存成功: {filepath}")
                        return True
                    else:
                        print("[WARNING] 保存文件不存在")
                        return False
                else:
                    print("[INFO] 未检测到保存对话框")
                    return False
            except Exception as e:
                print(f"[ERROR] 保存操作失败: {e}")
                return False
        
        # 使用安全操作保存数据
        save_success = safe_operation(process, app_path, save_data, "保存数据")
        
        # 记录测试结果
        test_result = {
            "test_case": i + 1,
            "description": f"边界值测试 - {test_case['name'][:50] if test_case['name'] else '空值测试'}",
            "input_data": test_case,
            "input_success": test_success,
            "save_success": save_success,
            "file_path": filepath if save_success else None,
            "timestamp": timestamp
        }
        test_results.append(test_result)
        
        # 验证保存的文件内容
        if save_success and os.path.exists(filepath):
            try:
                with open(filepath, 'r', encoding='utf-8') as f:
                    saved_data = json.load(f)
                
                # 检查数据一致性
                data_consistent = True
                issues = []
                
                if saved_data.get('name') != test_case['name']:
                    data_consistent = False
                    issues.append(f"名称不一致: 输入='{test_case['name']}', 保存='{saved_data.get('name')}'")
                
                if saved_data.get('version') != test_case['version']:
                    data_consistent = False
                    issues.append(f"版本不一致: 输入='{test_case['version']}', 保存='{saved_data.get('version')}'")
                
                if saved_data.get('category') != test_case['category']:
                    data_consistent = False
                    issues.append(f"分类不一致: 输入='{test_case['category']}', 保存='{saved_data.get('category')}'")
                
                if saved_data.get('developer') != test_case['developer']:
                    data_consistent = False
                    issues.append(f"开发者不一致: 输入='{test_case['developer']}', 保存='{saved_data.get('developer')}'")
                
                if saved_data.get('description') != test_case['description']:
                    data_consistent = False
                    issues.append(f"描述不一致: 输入长度={len(test_case['description'])}, 保存长度={len(saved_data.get('description', ''))}")
                
                test_result["data_consistent"] = data_consistent
                test_result["issues"] = issues
                
                if data_consistent:
                    print("[SUCCESS] 数据一致性验证通过")
                else:
                    print(f"[WARNING] 数据一致性验证失败: {issues}")
                    
            except Exception as e:
                print(f"[ERROR] 验证文件内容失败: {e}")
                test_result["data_consistent"] = False
                test_result["issues"] = [f"文件读取失败: {e}"]
        
        # 关闭应用程序
        print("\n=== 关闭软件编辑器 ===")
        dlg.close()
        time.sleep(2)
        
        # 生成测试报告
        report_path = os.path.join(get_test_results_dir(), f"boundary_test_report_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json")
        with open(report_path, 'w', encoding='utf-8') as f:
            json.dump(test_results, f, ensure_ascii=False, indent=2)
        
        print(f"\n=== 边界值测试完成 ===")
        print(f"测试报告已保存: {report_path}")
        
        # 统计测试结果
        total_cases = len(test_results)
        input_success = sum(1 for r in test_results if r["input_success"])
        save_success = sum(1 for r in test_results if r["save_success"])
        data_consistent = sum(1 for r in test_results if r.get("data_consistent", False))
        
        print(f"总测试用例: {total_cases}")
        print(f"输入成功: {input_success}/{total_cases}")
        print(f"保存成功: {save_success}/{total_cases}")
        print(f"数据一致: {data_consistent}/{save_success if save_success > 0 else 'N/A'}")
        
        return True
    
    except Exception as e:
        print(f"[ERROR] 测试过程中出现异常: {e}")
        return False

if __name__ == "__main__":
    success = test_boundary_values()
    if success:
        print("\n=== 边界值测试执行完成 ===")
    else:
        print("\n=== 边界值测试执行失败 ===")
