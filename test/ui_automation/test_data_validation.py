"""
产品编辑器数据验证测试用例
测试字段格式、必填项验证等数据验证功能
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

def test_data_validation():
    """测试数据验证功能"""
    print("=== 开始数据验证测试 ===")
    
    # 应用程序路径
    app_path = r"d:\virtualMachine\github\software-editor\build\software-editor.exe"
    
    # 检查应用程序是否存在
    if not os.path.exists(app_path):
        print(f"[ERROR] 应用程序不存在: {app_path}")
        return False
    
    # 启动应用程序
    print("启动应用程序...")
    import subprocess
    process = subprocess.Popen([app_path])
    time.sleep(5)  # 等待应用程序启动
    
    # 连接应用程序
    def connect_app():
        try:
            app = Application().connect(path=app_path)
            return app
        except:
            try:
                app = Application().connect(title_re=".*产品编辑器.*")
                return app
            except:
                print("[ERROR] 无法连接应用程序")
                return None
    
    app = safe_operation(process, app_path, connect_app, "连接应用程序")
    if not app:
        process.terminate()
        return False
    
    # 获取主窗口
    def get_main_window():
        try:
            dlg = app.window(title_re=".*产品编辑器.*")
            return dlg
        except ElementNotFoundError:
            print("[ERROR] 无法找到主窗口")
            return None
    
    dlg = safe_operation(process, app_path, get_main_window, "获取主窗口")
    if not dlg:
        process.terminate()
        return False
    
    # 数据验证测试用例
    validation_test_cases = [
        # 必填项验证测试
        {
            "name": "必填项验证测试",
            "test_cases": [
                {"name": "", "version": "1.0.0", "category": "工具软件", "developer": "测试开发者", "description": "测试必填项验证"},
                {"name": "产品名称", "version": "", "category": "工具软件", "developer": "测试开发者", "description": "测试必填项验证"},
                {"name": "产品名称", "version": "1.0.0", "category": "", "developer": "测试开发者", "description": "测试必填项验证"},
                {"name": "产品名称", "version": "1.0.0", "category": "工具软件", "developer": "", "description": "测试必填项验证"},
            ],
            "expected_behavior": "应该显示必填项验证错误提示"
        },
        
        # 版本号格式验证测试
        {
            "name": "版本号格式验证测试",
            "test_cases": [
                {"name": "产品A", "version": "1.0", "category": "工具软件", "developer": "开发者A", "description": "标准版本号"},
                {"name": "产品B", "version": "v1.0.0", "category": "工具软件", "developer": "开发者B", "description": "带v前缀版本号"},
                {"name": "产品C", "version": "1.0.0-beta", "category": "工具软件", "developer": "开发者C", "description": "带beta后缀版本号"},
                {"name": "产品D", "version": "1.0.0.1", "category": "工具软件", "developer": "开发者D", "description": "四段版本号"},
                {"name": "产品E", "version": "abc", "category": "工具软件", "developer": "开发者E", "description": "非标准版本号"},
                {"name": "产品F", "version": "1.0.0.0.0", "category": "工具软件", "developer": "开发者F", "description": "超长版本号"},
            ],
            "expected_behavior": "应该验证版本号格式"
        },
        
        # 字段长度验证测试
        {
            "name": "字段长度验证测试",
            "test_cases": [
                {"name": "A" * 100, "version": "1.0.0", "category": "工具软件", "developer": "开发者", "description": "正常长度"},
                {"name": "A" * 200, "version": "1.0.0", "category": "工具软件", "developer": "开发者", "description": "较长名称"},
                {"name": "A" * 500, "version": "1.0.0", "category": "工具软件", "developer": "开发者", "description": "超长名称"},
                {"name": "产品", "version": "1.0.0", "category": "A" * 50, "developer": "开发者", "description": "长分类"},
                {"name": "产品", "version": "1.0.0", "category": "工具软件", "developer": "A" * 100, "description": "长开发者"},
            ],
            "expected_behavior": "应该处理不同长度的字段输入"
        },
        
        # 特殊字符验证测试
        {
            "name": "特殊字符验证测试",
            "test_cases": [
                {"name": "产品<test>", "version": "1.0.0", "category": "工具软件", "developer": "开发者", "description": "包含尖括号"},
                {"name": "产品&test", "version": "1.0.0", "category": "工具软件", "developer": "开发者", "description": "包含&符号"},
                {"name": "产品'test", "version": "1.0.0", "category": "工具软件", "developer": "开发者", "description": "包含单引号"},
                {"name": '产品"test', "version": "1.0.0", "category": "工具软件", "developer": "开发者", "description": "包含双引号"},
                {"name": "产品\\test", "version": "1.0.0", "category": "工具软件", "developer": "开发者", "description": "包含反斜杠"},
                {"name": "产品/test", "version": "1.0.0", "category": "工具软件", "developer": "开发者", "description": "包含斜杠"},
            ],
            "expected_behavior": "应该正确处理特殊字符"
        }
    ]
    
    print("=== 启动软件编辑器进行数据验证测试 ===")
    
    # 切换到功能特性TAB页
    def switch_to_features_tab():
        dlg.TabControl.select("功能特性")
        time.sleep(1)
        return True
    
    if not safe_operation(process, app_path, switch_to_features_tab, "切换到功能特性TAB页"):
        process.terminate()
        return False
    
    # 切换到第二个TAB页
    def switch_to_second_tab():
        dlg.TabControl.select(1)
        time.sleep(1)
        return True
    
    if not safe_operation(process, app_path, switch_to_second_tab, "切换到第二个TAB页"):
        process.terminate()
        return False
    
    # 切换到产品信息TAB页
    def switch_to_product_info_tab():
        dlg.TabControl.select("产品信息")
        time.sleep(1)
        return True
    
    if not safe_operation(process, app_path, switch_to_product_info_tab, "切换到产品信息TAB页"):
        process.terminate()
        return False
    
    test_results = []
    
    for i, test_case in enumerate(validation_test_cases):
        print(f"\n--- 执行测试用例 {i+1}: {test_case['name']} ---")
        
        # 清空所有字段
        def clear_fields():
            # 只清空实际存在的字段
            for i in range(100):  # 假设最多100个字段
                try:
                    edit_control = getattr(dlg, f"Edit{i}")
                    edit_control.set_text("")
                except:
                    break
            return True
        
        if not safe_operation(process, app_path, clear_fields, f"清空字段 - {test_case['name']}"):
            continue
        
        # 输入测试数据
        def input_test_data():
            for field_name, value in test_case['data'].items():
                try:
                    edit_control = getattr(dlg, field_name)
                    edit_control.set_text(value)
                    time.sleep(0.1)
                except Exception as e:
                    print(f"[WARNING] 设置字段 {field_name} 失败: {e}")
            return True
        
        if not safe_operation(process, app_path, input_test_data, f"输入测试数据 - {test_case['name']}"):
            continue
        
        # 点击保存按钮
        def click_save_button():
            try:
                dlg.Button0.click()
                time.sleep(2)
                return True
            except Exception as e:
                print(f"[WARNING] 点击保存按钮失败: {e}")
                return False
        
        if not safe_operation(process, app_path, click_save_button, f"点击保存按钮 - {test_case['name']}"):
            continue
        
        # 检查JSON文件
        def check_json_file():
            json_file_path = os.path.join(os.path.dirname(app_path), "product_info.json")
            if not os.path.exists(json_file_path):
                print(f"[FAIL] JSON文件不存在: {json_file_path}")
                return {
                    'test_case': test_case['name'],
                    'passed': False,
                    'error': 'JSON文件不存在'
                }
            
            try:
                with open(json_file_path, 'r', encoding='utf-8') as f:
                    json_data = json.load(f)
                
                # 验证数据
                validation_passed = True
                for field_name, expected_value in test_case['data'].items():
                    if field_name in json_data:
                        actual_value = json_data[field_name]
                        if actual_value != expected_value:
                            print(f"[FAIL] 字段 {field_name} 不匹配: 期望 '{expected_value}', 实际 '{actual_value}'")
                            validation_passed = False
                    else:
                        print(f"[FAIL] 字段 {field_name} 在JSON中不存在")
                        validation_passed = False
                
                return {
                    'test_case': test_case['name'],
                    'passed': validation_passed,
                    'expected': test_case['data'],
                    'actual': json_data if validation_passed else {}
                }
                
            except Exception as e:
                print(f"[ERROR] 读取JSON文件失败: {e}")
                return {
                    'test_case': test_case['name'],
                    'passed': False,
                    'error': str(e)
                }
        
        result = safe_operation(process, app_path, check_json_file, f"检查JSON文件 - {test_case['name']}")
        if result:
            test_results.append(result)
            if result['passed']:
                print(f"[PASS] {test_case['name']} 验证通过")
            else:
                print(f"[FAIL] {test_case['name']} 验证失败")
        
        time.sleep(1)
    
    # 保存测试结果
    def save_test_results():
        results_dir = get_test_results_dir()
        timestamp = datetime.now().strftime("%H-%M-%S")
        result_file = os.path.join(results_dir, f"data_validation_test_{timestamp}.json")
        
        with open(result_file, 'w', encoding='utf-8') as f:
            json.dump({
                'test_time': datetime.now().isoformat(),
                'total_cases': len(validation_test_cases),
                'passed_cases': len([r for r in test_results if r['passed']]),
                'failed_cases': len([r for r in test_results if not r['passed']]),
                'results': test_results
            }, f, ensure_ascii=False, indent=2)
        
        return result_file
    
    result_file = safe_operation(process, app_path, save_test_results, "保存测试结果")
    
    print(f"\n=== 测试完成 ===")
    print(f"总测试用例: {len(validation_test_cases)}")
    print(f"通过: {len([r for r in test_results if r['passed']])}")
    print(f"失败: {len([r for r in test_results if not r['passed']])}")
    if result_file:
        print(f"结果文件: {result_file}")
    
    # 关闭应用程序
    def close_application():
        try:
            process.terminate()
            time.sleep(2)
            return True
        except Exception as e:
            print(f"[WARNING] 关闭应用程序失败: {e}")
            return False
    
    safe_operation(process, app_path, close_application, "关闭应用程序")
    
    return len([r for r in test_results if r['passed']]) == len(validation_test_cases)

if __name__ == "__main__":
    success = test_data_validation()
    if success:
        print("\n=== 数据验证测试执行成功 ===")
    else:
        print("\n=== 数据验证测试执行失败 ===")
    exit(0 if success else 1)