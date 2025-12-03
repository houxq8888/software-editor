import pytest
import pyautogui
import time
import os
import subprocess
import json
from pywinauto import Application
from pywinauto.keyboard import send_keys
from pywinauto.findwindows import ElementNotFoundError
from config import APP_INFO, UI_ELEMENTS, TEST_DATA
from utils import take_screenshot
from datetime import datetime
import pyperclip  # 用于剪贴板操作

# 截图保存目录
SCREENSHOT_DIR = os.path.join(os.path.dirname(__file__), "screenshots")
if not os.path.exists(SCREENSHOT_DIR):
    os.makedirs(SCREENSHOT_DIR)

# 测试结果目录
TEST_RESULTS_DIR = os.path.join(os.path.dirname(__file__), "test_results")
if not os.path.exists(TEST_RESULTS_DIR):
    os.makedirs(TEST_RESULTS_DIR)

# 按日期创建子目录
def get_test_results_dir():
    """获取按日期分类的测试结果目录"""
    date_str = datetime.now().strftime("%Y-%m-%d")
    date_dir = os.path.join(TEST_RESULTS_DIR, date_str)
    if not os.path.exists(date_dir):
        os.makedirs(date_dir)
    return date_dir

def paste_text_with_clipboard(text):
    """使用剪贴板粘贴方式输入文本"""
    try:
        # 保存当前剪贴板内容
        original_clipboard = pyperclip.paste()
        
        # 设置新文本到剪贴板
        pyperclip.copy(text)
        time.sleep(0.2)  # 等待剪贴板更新
        
        # 使用Ctrl+V粘贴
        pyautogui.hotkey('ctrl', 'v')
        time.sleep(0.2)
        
        # 恢复原始剪贴板内容
        pyperclip.copy(original_clipboard)
        
        print(f"✓ 使用剪贴板粘贴: {text[:50]}{'...' if len(text) > 50 else ''}")
        return True
    except Exception as e:
        print(f"❌ 剪贴板粘贴失败: {e}")
        return False

def setup_module(module):
    """模块级别的setup函数，在所有测试开始前执行"""
    global app, main_window
    
    # 先关闭可能存在的软件编辑器进程
    print("关闭可能存在的软件编辑器进程...")
    try:
        subprocess.run(["taskkill", "/f", "/im", "software-editor.exe"], capture_output=True)
        time.sleep(1)  # 等待进程完全关闭
    except Exception as e:
        print(f"关闭进程时出错: {e}")
    
    # 启动完整的软件编辑器
    print("启动软件编辑器...")
    app = Application().start("d:\\virtualMachine\\github\\software-editor\\build\\software-editor.exe", wait_for_idle=False)
    time.sleep(5)  # 等待应用程序启动
    
    # 获取主窗口
    print("尝试获取主窗口...")
    
    # 先获取所有窗口信息用于调试
    print("当前所有窗口:")
    for w in app.windows():
        print(f"  - 标题: '{w.window_text()}', 类名: '{w.class_name()}'")
    
    # 尝试多种窗口标题匹配模式
    main_window = None
    
    # 模式1: 精确匹配
    for w in app.windows():
        if "产品编辑器" in w.window_text():
            main_window = w
            print(f"找到主窗口(模式1): {w.window_text()}")
            break
    
    # 模式2: 如果模式1失败，尝试正则匹配
    if not main_window:
        try:
            main_window = app.window(title_re=".*产品编辑器.*")
            if main_window.exists():
                print(f"找到主窗口(模式2): {main_window.window_text()}")
        except:
            pass
    
    # 模式3: 如果前两种模式都失败，使用第一个非空窗口
    if not main_window:
        for w in app.windows():
            if w.window_text().strip():  # 非空标题
                main_window = w
                print(f"找到主窗口(模式3): {w.window_text()}")
                break
    
    if main_window:
        # 使用time.sleep等待窗口可见，而不是wait方法
        time.sleep(3)
        print("主窗口已获取")
    else:
        print("❌ 无法找到主窗口")
        raise Exception("无法找到软件编辑器的主窗口")

def teardown_module(module):
    """模块级别的teardown函数，在所有测试结束后执行"""
    global app
    
    # 关闭应用程序
    print("关闭软件编辑器...")
    try:
        app.kill()
        time.sleep(1)
    except Exception as e:
        print(f"关闭应用程序时出错: {e}")

def test_product_info_save_flow():
    """测试产品信息保存的完整闭环流程"""
    
    # 生成唯一的时间戳用于测试文件
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    test_file_path = os.path.join(get_test_results_dir(), f"test_product_{timestamp}.json")
    
    # 确保测试文件不存在
    if os.path.exists(test_file_path):
        os.remove(test_file_path)
        print(f"删除已存在的测试文件: {test_file_path}")
    
    # 记录测试开始前的文件状态
    test_dir = os.path.dirname(__file__)
    initial_files = set([f for f in os.listdir(test_dir) if f.endswith('.json')])
    
    # 重新获取主窗口，因为setup_module中的main_window可能不可用
    print("重新获取主窗口...")
    main_window = None
    
    # 尝试多种窗口标题匹配模式
    for w in app.windows():
        if "产品编辑器" in w.window_text():
            main_window = w
            print(f"找到主窗口: {w.window_text()}")
            break
    
    if not main_window:
        print("❌ 无法找到主窗口")
        raise Exception("无法找到软件编辑器的主窗口")
    
    try:
        # 步骤1: 输入产品信息
        print("=== 步骤1: 输入产品信息 ===")
        
        # 输入测试数据（使用中文测试剪贴板粘贴功能）
        test_product_data = {
            "name": f"测试产品_{timestamp}",
            "version": "1.0.0",
            "category": "工具软件",
            "developer": "测试开发者",
            "website": "https://example.com",
            "iconPath": "C:\\test\\icon.png",
            "screenshotPath": "C:\\test\\screenshot.png",
            "description": f"这是一个测试产品，创建于{timestamp}"
        }
        
        # 使用pyautogui直接输入，避免控件查找问题
        main_window.set_focus()
        time.sleep(0.5)
        
        # 获取对话框对象（类似hascoauto-test-tool-master中的self.app.dlg）
        dlg = app.window(handle=main_window.handle)
        
        # 先打印所有控件信息，了解实际的控件名称
        print("=== 打印控件标识符 ===")
        try:
            dlg.print_control_identifiers()
        except Exception as e:
            print(f"打印控件标识符失败: {e}")
            # 尝试使用其他方法获取控件信息
            print("=== 尝试使用child_window方法查找控件 ===")
        
        # 步骤1: 输入产品名称
        print("步骤1: 输入产品名称")
        try:
            # 使用坐标精确定位产品名称输入框 (599, 208, 1428, 229)
            name_edit = dlg.child_window(auto_id="QApplication.MainWindow.centralWidget.tabWidget.qt_tabwidget_stackedwidget.basicInfoTab.groupBox.nameLineEdit", control_type="Unknown")
            name_edit.click_input()
            name_edit.set_text(test_product_data['name'])
            time.sleep(1)
        except Exception as e:
            print(f"步骤1失败: {e}")
            # 如果失败，尝试使用坐标点击输入框并使用剪贴板粘贴
            center_x = (599 + 1428) // 2
            center_y = (208 + 229) // 2
            pyautogui.click(center_x, center_y)
            pyautogui.hotkey('ctrl', 'a')
            paste_text_with_clipboard(test_product_data['name'])
            time.sleep(1)
        
        # 步骤2: 输入版本号
        print("步骤2: 输入版本号")
        try:
            # 使用自动化ID精确定位版本号输入框
            version_edit = dlg.child_window(auto_id="QApplication.MainWindow.centralWidget.tabWidget.qt_tabwidget_stackedwidget.basicInfoTab.groupBox.versionLineEdit", control_type="Unknown")
            version_edit.click_input()
            version_edit.set_text(test_product_data['version'])
            time.sleep(1)
        except Exception as e:
            print(f"步骤2失败: {e}")
            # 如果失败，尝试使用坐标点击输入框 (599, 235, 1428, 256)
            center_x = (599 + 1428) // 2
            center_y = (235 + 256) // 2
            pyautogui.click(center_x, center_y)
            pyautogui.hotkey('ctrl', 'a')
            paste_text_with_clipboard(test_product_data['version'])
            time.sleep(1)
        
        # 步骤3: 输入分类
        print("步骤3: 输入分类")
        try:
            # 使用自动化ID精确定位分类输入框
            category_edit = dlg.child_window(auto_id="QApplication.MainWindow.centralWidget.tabWidget.qt_tabwidget_stackedwidget.basicInfoTab.groupBox.categoryLineEdit", control_type="Unknown")
            category_edit.click_input()
            category_edit.set_text(test_product_data['category'])
            time.sleep(1)
        except Exception as e:
            print(f"步骤3失败: {e}")
            # 如果失败，尝试使用坐标点击输入框 (599, 262, 1428, 283)
            center_x = (599 + 1428) // 2
            center_y = (262 + 283) // 2
            pyautogui.click(center_x, center_y)
            pyautogui.hotkey('ctrl', 'a')
            paste_text_with_clipboard(test_product_data['category'])
            time.sleep(1)
        
        # 步骤4: 输入开发者
        print("步骤4: 输入开发者")
        try:
            # 使用自动化ID精确定位开发者输入框
            developer_edit = dlg.child_window(auto_id="QApplication.MainWindow.centralWidget.tabWidget.qt_tabwidget_stackedwidget.basicInfoTab.groupBox.developerLineEdit", control_type="Unknown")
            developer_edit.click_input()
            developer_edit.set_text(test_product_data['developer'])
            time.sleep(1)
        except Exception as e:
            print(f"步骤4失败: {e}")
            # 如果失败，尝试使用坐标点击输入框 (599, 289, 1428, 310)
            center_x = (599 + 1428) // 2
            center_y = (289 + 310) // 2
            pyautogui.click(center_x, center_y)
            pyautogui.hotkey('ctrl', 'a')
            paste_text_with_clipboard(test_product_data['developer'])
            time.sleep(1)
        
        # 步骤5: 输入网站
        print("步骤5: 输入网站")
        try:
            # 使用自动化ID精确定位网站输入框
            website_edit = dlg.child_window(auto_id="QApplication.MainWindow.centralWidget.tabWidget.qt_tabwidget_stackedwidget.basicInfoTab.groupBox.websiteLineEdit", control_type="Unknown")
            website_edit.click_input()
            website_edit.set_text(test_product_data['website'])
            time.sleep(1)
        except Exception as e:
            print(f"步骤5失败: {e}")
            # 如果失败，尝试使用坐标点击输入框 (599, 316, 1428, 337)
            center_x = (599 + 1428) // 2
            center_y = (316 + 337) // 2
            pyautogui.click(center_x, center_y)
            pyautogui.hotkey('ctrl', 'a')
            paste_text_with_clipboard(test_product_data['website'])
            time.sleep(1)
        
        # 步骤6: 输入图标路径
        print("步骤6: 输入图标路径")
        try:
            # 使用自动化ID精确定位图标路径输入框
            icon_path_edit = dlg.child_window(auto_id="QApplication.MainWindow.centralWidget.tabWidget.qt_tabwidget_stackedwidget.basicInfoTab.groupBox_2.iconPathLineEdit", control_type="Unknown")
            icon_path_edit.click_input()
            icon_path_edit.set_text(test_product_data['iconPath'])
            time.sleep(1)
        except Exception as e:
            print(f"步骤6失败: {e}")
            # 如果失败，尝试使用坐标点击输入框 (599, 379, 1347, 400)
            center_x = (599 + 1347) // 2
            center_y = (379 + 400) // 2
            pyautogui.click(center_x, center_y)
            pyautogui.hotkey('ctrl', 'a')
            paste_text_with_clipboard(test_product_data['iconPath'])
            time.sleep(1)
        
        # 步骤7: 输入截图路径
        print("步骤7: 输入截图路径")
        try:
            # 使用自动化ID精确定位截图路径输入框
            screenshot_path_edit = dlg.child_window(auto_id="QApplication.MainWindow.centralWidget.tabWidget.qt_tabwidget_stackedwidget.basicInfoTab.groupBox_2.screenshotPathLineEdit", control_type="Unknown")
            screenshot_path_edit.click_input()
            screenshot_path_edit.set_text(test_product_data['screenshotPath'])
            time.sleep(1)
        except Exception as e:
            print(f"步骤7失败: {e}")
            # 如果失败，尝试使用坐标点击输入框 (599, 408, 1347, 429)
            center_x = (599 + 1347) // 2
            center_y = (408 + 429) // 2
            pyautogui.click(center_x, center_y)
            pyautogui.hotkey('ctrl', 'a')
            paste_text_with_clipboard(test_product_data['screenshotPath'])
            time.sleep(1)
        
        # 步骤8: 输入产品描述
        print("步骤8: 输入产品描述")
        try:
            # 使用自动化ID精确定位产品描述输入框
            description_edit = dlg.child_window(auto_id="QApplication.MainWindow.centralWidget.tabWidget.qt_tabwidget_stackedwidget.basicInfoTab.groupBox_3.descriptionTextEdit", control_type="Unknown")
            description_edit.click_input()
            description_edit.set_text(test_product_data['description'])
            time.sleep(1)
        except Exception as e:
            print(f"步骤8失败: {e}")
            # 如果失败，尝试使用坐标点击输入框 (533, 471, 1428, 804)
            center_x = (533 + 1428) // 2
            center_y = (471 + 804) // 2
            pyautogui.click(center_x, center_y)
            pyautogui.hotkey('ctrl', 'a')
            paste_text_with_clipboard(test_product_data['description'])
            time.sleep(1)
        
        print(f"产品名称: {test_product_data['name']}")
        print(f"产品版本: {test_product_data['version']}")
        print(f"产品分类: {test_product_data['category']}")
        print(f"开发者: {test_product_data['developer']}")
        print(f"网站: {test_product_data['website']}")
        print(f"图标路径: {test_product_data['iconPath']}")
        print(f"截图路径: {test_product_data['screenshotPath']}")
        print(f"产品描述: {test_product_data['description']}")
        
        # 步骤2: 使用快捷键Ctrl+S触发保存
        print("=== 步骤2: 触发保存功能 ===")
        
        # 确保窗口有焦点
        main_window.set_focus()
        time.sleep(0.5)
        
        # 使用Ctrl+S快捷键
        pyautogui.hotkey('ctrl', 's')
        time.sleep(2)  # 等待保存对话框出现
        
        # 步骤3: 处理保存对话框
        print("=== 步骤3: 处理保存对话框 ===")
        
        # 查找保存对话框
        save_dialog = None
        
        # 尝试查找文件保存对话框
        for w in app.windows():
            if "保存" in w.window_text() or "Save" in w.window_text():
                save_dialog = w
                print(f"找到保存对话框: {w.window_text()}")
                break
        
        if save_dialog:
            # 切换到uia后端处理对话框
            try:
                app_uia_dialog = Application(backend="uia").connect(handle=save_dialog.handle)
                uia_dialog = app_uia_dialog.window(handle=save_dialog.handle)
                
                # 查找文件名输入框
                filename_edit = None
                dialog_controls = uia_dialog.descendants()
                
                for control in dialog_controls:
                    if control.class_name() in ["QLineEdit", "Edit"]:
                        filename_edit = control
                        print("找到文件名输入框")
                        break
                
                if filename_edit:
                    # 输入测试文件完整路径
                    filename_edit.click_input()
                    time.sleep(0.5)
                    pyautogui.hotkey('ctrl', 'a')
                    time.sleep(0.5)
                    pyautogui.write(test_file_path)
                    time.sleep(1)
                    
                    # 查找保存按钮
                    save_button = None
                    for control in dialog_controls:
                        if control.class_name() in ["QPushButton", "Button"]:
                            button_text = control.window_text()
                            if "保存" in button_text or "Save" in button_text:
                                save_button = control
                                print(f"找到保存按钮: {button_text}")
                                break
                    
                    if save_button:
                        # 点击保存按钮
                        save_button.click()
                        time.sleep(2)
                        print("保存按钮已点击")
                    else:
                        # 如果找不到保存按钮，尝试使用键盘回车
                        pyautogui.press('enter')
                        time.sleep(2)
                        print("使用键盘回车保存")
                else:
                    print("未找到文件名输入框")
                    
            except Exception as e:
                print(f"处理保存对话框时出错: {e}")
                # 如果对话框处理失败，尝试直接输入路径
                pyautogui.write("test_product.json")
                time.sleep(0.5)
                pyautogui.press('enter')
                time.sleep(2)
        else:
            print("未找到保存对话框，尝试其他保存方式")
            
            # 尝试通过菜单栏保存
            try:
                # 使用Alt+F打开文件菜单
                main_window.set_focus()
                time.sleep(0.5)
                pyautogui.hotkey('alt', 'f')
                time.sleep(1)
                # 按S选择保存
                pyautogui.press('s')
                time.sleep(2)
                print("通过菜单栏触发保存")
                
                # 再次尝试处理保存对话框
                for w in app.windows():
                    if "保存" in w.window_text() or "Save" in w.window_text():
                        save_dialog = w
                        print(f"找到保存对话框: {w.window_text()}")
                        
                        # 输入完整文件路径并保存
                        pyautogui.write(test_file_path)
                        time.sleep(0.5)
                        pyautogui.press('enter')
                        time.sleep(2)
                        break
                        
            except Exception as e:
                print(f"菜单栏保存失败: {e}")
        
        # 步骤4: 验证文件是否保存成功
        print("=== 步骤4: 验证文件保存结果 ===")
        
        # 等待文件保存完成
        time.sleep(3)
        
        # 检查测试过程中是否创建了新文件
        current_files = set([f for f in os.listdir(test_dir) if f.endswith('.json')])
        new_files = current_files - initial_files
        
        # 验证文件是否通过保存操作创建
        file_created_by_save = False
        if os.path.exists(test_file_path):
            # 检查文件是否是在保存操作后创建的
            file_creation_time = os.path.getctime(test_file_path)
            current_time = time.time()
            
            # 如果文件是在最近10秒内创建的，认为是保存操作创建的
            if current_time - file_creation_time < 10:
                file_created_by_save = True
                print(f"[SUCCESS] 文件保存成功: {test_file_path}")
            else:
                print(f"[WARNING] 文件已存在但不是通过本次保存操作创建的: {test_file_path}")
        
        # 如果有新创建的文件但不是预期文件
        if new_files and not file_created_by_save:
            print(f"[INFO] 发现新创建的JSON文件: {new_files}")
        
        if file_created_by_save:
            # 验证文件内容格式
            try:
                with open(test_file_path, 'r', encoding='utf-8') as f:
                    content = f.read()
                    
                # 尝试解析JSON
                product_data = json.loads(content)
                print("[SUCCESS] 文件格式正确，为有效的JSON格式")
                
                # 验证基本结构
                if isinstance(product_data, dict):
                    print("[SUCCESS] 文件内容为字典格式")
                    
                    # 检查是否包含产品信息字段
                    expected_fields = ["name", "version", "category", "developer", "website", "iconPath", "screenshotPath", "description"]
                    missing_fields = []
                    
                    for field in expected_fields:
                        if field in product_data:
                            print(f"[SUCCESS] 包含字段: {field}")
                        else:
                            missing_fields.append(field)
                    
                    if missing_fields:
                        print(f"[WARNING] 缺少字段: {missing_fields}")
                    else:
                        print("[SUCCESS] 所有必需字段都存在")
                        
                    # 验证字段值是否正确
                    for field, expected_value in test_product_data.items():
                        if field in product_data:
                            if product_data[field] == expected_value:
                                print(f"[SUCCESS] 字段 {field} 值正确: {expected_value}")
                            else:
                                print(f"[WARNING] 字段 {field} 值不匹配: 期望 {expected_value}, 实际 {product_data[field]}")
                        else:
                            print(f"[ERROR] 字段 {field} 不存在")
                        
                else:
                    print("[WARNING] 文件内容不是字典格式")
                    
            except json.JSONDecodeError as e:
                print(f"[ERROR] 文件不是有效的JSON格式: {e}")
            except Exception as e:
                print(f"[ERROR] 读取文件时出错: {e}")
                
        else:
            print(f"[ERROR] 文件保存失败，未找到通过保存操作创建的文件: {test_file_path}")
            
            # 检查是否有其他新创建的文件
            if new_files:
                print(f"[INFO] 但发现了其他新创建的JSON文件: {new_files}")
            
        # 步骤5: 截图保存测试过程
        print("=== 步骤5: 保存测试截图 ===")
        take_screenshot("product_info_save_test")
        
        # 测试结果总结 - 基于文件是否通过保存操作创建来判断
        if file_created_by_save:
            print("[SUCCESS] 产品信息保存功能测试成功！")
            assert True, "产品信息保存功能测试通过"
        else:
            print("[ERROR] 产品信息保存功能测试失败")
            
            # 提供可能的失败原因
            print("可能的原因:")
            print("1. 保存对话框未正确出现")
            print("2. 文件名输入框未找到")
            print("3. 保存按钮未正确点击")
            print("4. 文件保存路径不正确")
            print("5. 应用程序保存功能存在问题")
            print("6. 文件已存在但不是通过本次保存操作创建的")
            
            assert False, "产品信息保存功能测试失败"
            
    except Exception as e:
        print(f"测试过程中出现异常: {e}")
        take_screenshot("product_info_save_test_error")
        raise e
    
    finally:
        # 不再删除测试文件，保留在测试结果目录中
        if os.path.exists(test_file_path):
            print(f"测试文件已保存到: {test_file_path}")

def test_product_info_load_flow():
    """测试产品信息加载的完整闭环流程"""
    
    # 步骤0: 先新建一个空白文件，确保当前没有打开任何文件
    print("=== 步骤0: 新建空白文件 ===")
    
    main_window.set_focus()
    time.sleep(0.5)
    pyautogui.hotkey('ctrl', 'n')  # Ctrl+N 新建文件
    time.sleep(2)  # 等待新建操作完成
    
    # 记录新建后的窗口标题
    initial_window_title = main_window.window_text()
    print(f"新建后窗口标题: {initial_window_title}")
    
    # 查找最近保存的产品信息文件用于加载
    test_results_dir = get_test_results_dir()
    json_files = []
    
    if os.path.exists(test_results_dir):
        for file in os.listdir(test_results_dir):
            if file.endswith('.json') and file.startswith('test_product_'):
                file_path = os.path.join(test_results_dir, file)
                json_files.append((file_path, os.path.getctime(file_path)))
    
    if not json_files:
        print("[ERROR] 没有找到可用的产品信息文件用于加载测试")
        print("请先运行保存测试创建产品信息文件")
        assert False, "没有可用的产品信息文件"
    
    # 选择最新的产品信息文件
    json_files.sort(key=lambda x: x[1], reverse=True)
    test_file_path = json_files[0][0]
    print(f"选择用于加载的测试文件: {test_file_path}")
    
    try:
        
        # 步骤1: 使用快捷键Ctrl+O触发打开
        print("=== 步骤1: 触发打开功能 ===")
        
        main_window.set_focus()
        time.sleep(0.5)
        pyautogui.hotkey('ctrl', 'o')
        time.sleep(2)  # 等待打开对话框出现
        
        # 步骤2: 处理打开对话框
        print("=== 步骤2: 处理打开对话框 ===")
        
        open_dialog = None
        for w in app.windows():
            if "打开" in w.window_text() or "Open" in w.window_text():
                open_dialog = w
                print(f"找到打开对话框: {w.window_text()}")
                break
        
        if open_dialog:
            # 输入完整的文件路径
            pyautogui.write(test_file_path)
            time.sleep(0.5)
            pyautogui.press('enter')
            time.sleep(2)
            print(f"文件选择完成，路径: {test_file_path}")
        else:
            print("未找到打开对话框")
        
        # 步骤3: 验证加载结果
        print("=== 步骤3: 验证加载结果 ===")
        
        # 等待加载完成
        time.sleep(3)
        
        # 检查窗口标题是否更新
        current_window_title = main_window.window_text()
        print(f"加载后窗口标题: {current_window_title}")
        
        # 获取文件名用于验证
        file_name = os.path.basename(test_file_path)
        
        # 验证加载是否成功
        load_successful = False
        
        # 检查窗口标题是否发生变化（表明加载操作可能生效）
        if current_window_title != initial_window_title:
            print("[SUCCESS] 窗口标题已更新，表明加载操作可能生效")
            
            # 进一步检查是否包含文件名
            if file_name in current_window_title:
                print("[SUCCESS] 产品信息加载成功，窗口标题已更新")
                load_successful = True
            else:
                print(f"[WARNING] 窗口标题已更新但不包含文件名，当前标题: {current_window_title}")
                # 可能是其他操作导致的标题变化，不能确定是加载成功
        else:
            print(f"[WARNING] 窗口标题未更新，当前标题: {current_window_title}")
        
        # 截图保存测试过程
        take_screenshot("product_info_load_test")
        
        # 测试结果总结 - 检查窗口标题是否从"新建"变为包含文件名的标题
        if current_window_title != initial_window_title and file_name in current_window_title:
            print("[SUCCESS] 产品信息加载功能测试成功！")
            print("窗口标题已从 '" + initial_window_title + "' 更新为 '" + current_window_title + "'")
            assert True, "产品信息加载功能测试通过"
        else:
            print("[ERROR] 产品信息加载功能测试失败")
            
            # 提供可能的失败原因
            print("可能的原因:")
            print("1. 打开对话框未正确出现")
            print("2. 文件名输入框未找到")
            print("3. 打开按钮未正确点击")
            print("4. 文件路径不正确")
            print("5. 应用程序加载功能存在问题")
            print("6. 窗口标题更新机制不完善")
            
            assert False, "产品信息加载功能测试失败"
        
    except Exception as e:
        print(f"测试过程中出现异常: {e}")
        take_screenshot("product_info_load_test_error")
        raise e

if __name__ == "__main__":
    # 直接运行测试
    setup_module(None)
    try:
        test_product_info_save_flow()
        test_product_info_load_flow()
    finally:
        teardown_module(None)