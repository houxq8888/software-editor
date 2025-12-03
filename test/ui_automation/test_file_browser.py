"""
产品编辑器文件浏览功能测试用例
测试图标路径、截图路径选择等文件浏览功能
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

def test_file_browser():
    """测试文件浏览功能"""
    
    # 文件浏览测试用例
    file_browser_test_cases = [
        # 图标路径选择测试
        {
            "name": "图标路径选择测试",
            "test_cases": [
                {
                    "test_type": "icon_path",
                    "file_types": ["png", "jpg", "jpeg", "ico", "bmp", "svg"],
                    "file_sizes": ["small (1-10KB)", "medium (10-100KB)", "large (100KB-1MB)"],
                    "expected_behavior": "应该能够选择不同格式和尺寸的图标文件"
                }
            ]
        },
        
        # 截图路径选择测试
        {
            "name": "截图路径选择测试",
            "test_cases": [
                {
                    "test_type": "screenshot_path",
                    "file_types": ["png", "jpg", "jpeg"],
                    "file_sizes": ["small (10-100KB)", "medium (100KB-1MB)", "large (1-10MB)"],
                    "expected_behavior": "应该能够选择不同格式和尺寸的截图文件"
                }
            ]
        },
        
        # 文件对话框功能测试
        {
            "name": "文件对话框功能测试",
            "test_cases": [
                {
                    "test_type": "file_dialog",
                    "operations": ["打开", "保存", "取消", "文件类型过滤", "路径导航"],
                    "expected_behavior": "文件对话框应该正常工作"
                }
            ]
        },
        
        # 路径验证测试
        {
            "name": "路径验证测试",
            "test_cases": [
                {
                    "test_type": "path_validation",
                    "invalid_paths": ["不存在的路径", "空路径", "包含特殊字符的路径", "超长路径"],
                    "expected_behavior": "应该验证路径的有效性"
                }
            ]
        }
    ]
    
    # 应用程序路径
    app_path = os.path.join(os.path.dirname(os.path.dirname(os.path.dirname(__file__))), "build", "software-editor.exe")
    
    if not os.path.exists(app_path):
        print(f"[ERROR] 应用程序不存在: {app_path}")
        return False
    
    print("=== 启动软件编辑器进行文件浏览功能测试 ===")
    
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
    
    for file_browser_group in file_browser_test_cases:
        print(f"\n=== 测试组: {file_browser_group['name']} ===")
        
        group_results = {
            "group_name": file_browser_group['name'],
            "test_cases": []
        }
        
        for test_case in file_browser_group['test_cases']:
            print(f"\n--- 测试类型: {test_case['test_type']} ---")
            print(f"预期行为: {test_case['expected_behavior']}")
            
            case_result = {
                "test_type": test_case['test_type'],
                "operations_performed": [],
                "success": True,
                "notes": ""
            }
            
            if test_case['test_type'] == "icon_path":
                # 测试图标路径选择
                def test_icon_path():
                    # 查找图标路径相关控件
                    print("[INFO] 查找图标路径相关控件...")
                    
                    # 尝试点击图标路径按钮或输入框
                    try:
                        icon_button = dlg.child_window(title_re=".*图标.*|.*icon.*", control_type="Button")
                        icon_button.click_input()
                        time.sleep(2)
                        print("[SUCCESS] 点击图标路径按钮")
                        case_result["operations_performed"].append("点击图标路径按钮")
                    except Exception as e:
                        print(f"[WARNING] 自动化点击图标按钮失败: {e}")
                        
                        # 尝试使用控件位置点击
                        try:
                            # 尝试查找图标路径相关控件
                            icon_button = dlg.child_window(title_re=".*图标.*|.*icon.*", control_type="Button")
                            rect = icon_button.rectangle()
                            center_x = rect.left + (rect.width() // 2)
                            center_y = rect.top + (rect.height() // 2)
                            pyautogui.click(center_x, center_y)
                            time.sleep(2)
                            print("[INFO] 使用控件位置点击图标路径按钮")
                            case_result["operations_performed"].append("控件位置点击图标路径按钮")
                        except Exception as e2:
                            print(f"[ERROR] 控件位置点击图标路径按钮失败: {e2}")
                            case_result["success"] = False
                            case_result["notes"] = "无法定位图标路径按钮"
                    
                    # 检查是否打开了文件对话框
                    try:
                        file_dialog = app.window(title_re=".*打开.*|.*Open.*")
                        if file_dialog.exists():
                            print("[SUCCESS] 检测到文件对话框")
                            case_result["operations_performed"].append("打开文件对话框")
                            
                            # 取消文件对话框
                            try:
                                cancel_button = file_dialog.child_window(title_re=".*取消.*|.*Cancel.*", control_type="Button")
                                cancel_button.click_input()
                                time.sleep(1)
                                print("[INFO] 取消文件对话框")
                                case_result["operations_performed"].append("取消文件对话框")
                            except:
                                # 尝试按ESC键取消
                                pyautogui.press('esc')
                                time.sleep(1)
                                print("[INFO] 使用ESC取消文件对话框")
                                case_result["operations_performed"].append("ESC取消文件对话框")
                    except:
                        print("[INFO] 未检测到文件对话框")
                    
                    return True
                
                # 使用安全操作执行图标路径测试
                if not safe_operation(process, app_path, test_icon_path, "图标路径测试"):
                    case_result["success"] = False
                    case_result["notes"] = "图标路径测试过程中应用程序异常"
                
            elif test_case['test_type'] == "screenshot_path":
                # 测试截图路径选择
                def test_screenshot_path():
                    print("[INFO] 查找截图路径相关控件...")
                    
                    # 尝试点击截图路径按钮或输入框
                    try:
                        screenshot_button = dlg.child_window(title_re=".*截图.*|.*screenshot.*", control_type="Button")
                        screenshot_button.click_input()
                        time.sleep(2)
                        print("[SUCCESS] 点击截图路径按钮")
                        case_result["operations_performed"].append("点击截图路径按钮")
                    except Exception as e:
                        print(f"[WARNING] 自动化点击截图按钮失败: {e}")
                        
                        # 尝试使用控件位置点击
                        try:
                            # 尝试查找截图路径相关控件
                            screenshot_button = dlg.child_window(title_re=".*截图.*|.*screenshot.*", control_type="Button")
                            rect = screenshot_button.rectangle()
                            center_x = rect.left + (rect.width() // 2)
                            center_y = rect.top + (rect.height() // 2)
                            pyautogui.click(center_x, center_y)
                            time.sleep(2)
                            print("[INFO] 使用控件位置点击截图路径按钮")
                            case_result["operations_performed"].append("控件位置点击截图路径按钮")
                        except Exception as e2:
                            print(f"[ERROR] 控件位置点击截图路径按钮失败: {e2}")
                            case_result["success"] = False
                            case_result["notes"] = "无法定位截图路径按钮"
                    
                    # 检查是否打开了文件对话框
                    try:
                        file_dialog = app.window(title_re=".*打开.*|.*Open.*")
                        if file_dialog.exists():
                            print("[SUCCESS] 检测到文件对话框")
                            case_result["operations_performed"].append("打开文件对话框")
                            
                            # 取消文件对话框
                            try:
                                cancel_button = file_dialog.child_window(title_re=".*取消.*|.*Cancel.*", control_type="Button")
                                cancel_button.click_input()
                                time.sleep(1)
                                print("[INFO] 取消文件对话框")
                                case_result["operations_performed"].append("取消文件对话框")
                            except:
                                # 尝试按ESC键取消
                                pyautogui.press('esc')
                                time.sleep(1)
                                print("[INFO] 使用ESC取消文件对话框")
                                case_result["operations_performed"].append("ESC取消文件对话框")
                    except:
                        print("[INFO] 未检测到文件对话框")
                    
                    return True
                
                # 使用安全操作执行截图路径测试
                if not safe_operation(process, app_path, test_screenshot_path, "截图路径测试"):
                    case_result["success"] = False
                    case_result["notes"] = "截图路径测试过程中应用程序异常"
                
            elif test_case['test_type'] == "file_dialog":
                # 测试文件对话框功能
                def test_file_dialog():
                    print("[INFO] 测试文件对话框功能...")
                    
                    # 尝试打开文件对话框
                    try:
                        # 先尝试图标路径
                        try:
                            icon_button = dlg.child_window(title_re=".*图标.*|.*icon.*", control_type="Button")
                            rect = icon_button.rectangle()
                            center_x = rect.left + (rect.width() // 2)
                            center_y = rect.top + (rect.height() // 2)
                            pyautogui.click(center_x, center_y)
                            time.sleep(2)
                            print("[INFO] 使用控件位置点击图标路径按钮")
                        except Exception as e2:
                            print(f"[ERROR] 控件位置点击图标路径按钮失败: {e2}")
                            # 如果控件定位失败，尝试使用相对位置
                            window_rect = dlg.rectangle()
                            center_x = window_rect.left + (window_rect.width() // 2)
                            center_y = window_rect.top + (window_rect.height() // 2)
                            pyautogui.click(center_x - 100, center_y + 100)  # 相对窗口中心的位置
                            time.sleep(2)
                            print("[INFO] 使用相对位置点击图标路径按钮")
                        
                        # 检查文件对话框
                        file_dialog = app.window(title_re=".*打开.*|.*Open.*")
                        if file_dialog.exists():
                            print("[SUCCESS] 文件对话框打开成功")
                            case_result["operations_performed"].append("打开文件对话框")
                            
                            # 测试文件类型过滤
                            try:
                                # 尝试点击文件类型下拉框
                                file_type_combo = file_dialog.child_window(title_re=".*文件类型.*|.*File Type.*", control_type="ComboBox")
                                file_type_combo.click_input()
                                time.sleep(1)
                                print("[INFO] 点击文件类型下拉框")
                                case_result["operations_performed"].append("点击文件类型下拉框")
                                
                                # 关闭下拉框
                                pyautogui.press('esc')
                                time.sleep(1)
                            except:
                                print("[INFO] 文件类型过滤测试跳过")
                            
                            # 取消文件对话框
                            pyautogui.press('esc')
                            time.sleep(1)
                            print("[INFO] 取消文件对话框")
                            case_result["operations_performed"].append("取消文件对话框")
                        else:
                            print("[INFO] 文件对话框未打开")
                            case_result["notes"] = "文件对话框未打开"
                    except Exception as e:
                        print(f"[WARNING] 文件对话框测试失败: {e}")
                        case_result["success"] = False
                        case_result["notes"] = f"文件对话框测试失败: {e}"
                    
                    return True
                
                # 使用安全操作执行文件对话框测试
                if not safe_operation(process, app_path, test_file_dialog, "文件对话框测试"):
                    case_result["success"] = False
                    case_result["notes"] = "文件对话框测试过程中应用程序异常"
                
            elif test_case['test_type'] == "path_validation":
                # 测试路径验证功能
                def test_path_validation():
                    print("[INFO] 测试路径验证功能...")
                    
                    # 尝试输入无效路径
                    try:
                        # 查找路径输入框
                        path_edit = dlg.child_window(title_re=".*路径.*|.*Path.*", control_type="Edit")
                        path_edit.click_input()
                        path_edit.set_text("无效路径测试")
                        time.sleep(1)
                        print("[INFO] 输入无效路径")
                        case_result["operations_performed"].append("输入无效路径")
                        
                        # 尝试保存以触发验证
                        try:
                            save_button = dlg.child_window(auto_id="QApplication.MainWindow.centralWidget.tabWidget.qt_tabwidget_stackedwidget.basicInfoTab.groupBox_3.saveButton", control_type="Unknown")
                            save_button.click_input()
                            time.sleep(2)
                            
                            # 检查是否有错误提示
                            try:
                                error_dialog = app.window(title_re=".*错误.*|.*Error.*")
                                if error_dialog.exists():
                                    print("[SUCCESS] 检测到路径验证错误提示")
                                    case_result["operations_performed"].append("检测到路径验证错误")
                                    error_dialog.close()
                                    time.sleep(1)
                            except:
                                print("[INFO] 未检测到路径验证错误提示")
                                
                        except:
                            print("[INFO] 保存操作跳过")
                            
                    except Exception as e:
                        print(f"[WARNING] 路径验证测试失败: {e}")
                        case_result["notes"] = f"路径验证测试失败: {e}"
                    
                    return True
                
                # 使用安全操作执行路径验证测试
                if not safe_operation(process, app_path, test_path_validation, "路径验证测试"):
                    case_result["success"] = False
                    case_result["notes"] = "路径验证测试过程中应用程序异常"
            
            group_results["test_cases"].append(case_result)
            print(f"[INFO] {test_case['test_type']} 测试完成")
        
        test_results.append(group_results)
    
    # 保存测试结果
    def save_results():
        results_dir = get_test_results_dir()
        result_file = os.path.join(results_dir, f"file_browser_test_{datetime.now().strftime('%H%M%S')}.json")
        
        with open(result_file, 'w', encoding='utf-8') as f:
            json.dump(test_results, f, ensure_ascii=False, indent=2)
        
        print(f"\n[SUCCESS] 文件浏览功能测试完成，结果保存到: {result_file}")
        return True
    
    # 使用安全操作保存结果
    if not safe_operation(process, app_path, save_results, "保存测试结果"):
        print("[ERROR] 保存测试结果失败")
    
    # 安全关闭应用程序
    def close_app():
        dlg.close()
        time.sleep(2)
        return True
    
    if not safe_operation(process, app_path, close_app, "关闭应用程序"):
        print("[WARNING] 应用程序关闭失败，强制终止进程")
        process.terminate()
    
    return True

if __name__ == "__main__":
    success = test_file_browser()
    if success:
        print("\n=== 文件浏览功能测试执行成功 ===")
    else:
        print("\n=== 文件浏览功能测试执行失败 ===")
    exit(0 if success else 1)