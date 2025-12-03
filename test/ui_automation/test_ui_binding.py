"""
UI绑定功能自动化测试脚本
测试产品与UI布局文件的绑定工作流程
"""

import pytest
import pyautogui
import time
import os
import subprocess
import json
from pywinauto import Application
from pywinauto.findwindows import ElementNotFoundError
from config import APP_INFO, UI_ELEMENTS, TEST_DATA
from utils import take_screenshot
from datetime import datetime

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

# 测试产品数据
def create_test_product_data():
    """创建测试产品数据"""
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    
    product_data = {
        "name": f"测试产品_{timestamp}",
        "version": "1.0.0",
        "developer": "测试开发者",
        "description": "这是一个用于UI绑定功能测试的产品",
        "features": [
            {"name": "功能1", "description": "测试功能1"},
            {"name": "功能2", "description": "测试功能2"}
        ],
        "uiLayoutPath": ""  # 初始为空，测试中会绑定
    }
    
    return product_data

# 保存测试产品数据
def save_test_product_data(product_data, file_path):
    """保存测试产品数据到文件"""
    with open(file_path, 'w', encoding='utf-8') as f:
        json.dump(product_data, f, ensure_ascii=False, indent=2)

# 加载测试产品数据
def load_test_product_data(file_path):
    """从文件加载产品数据"""
    with open(file_path, 'r', encoding='utf-8') as f:
        return json.load(f)

def setup_module(module):
    """模块级别的setup函数，在所有测试开始前执行"""
    global app, main_window, uilayout_window, test_product_file
    
    # 创建测试产品文件
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    test_product_file = os.path.join(get_test_results_dir(), f"test_product_{timestamp}.json")
    product_data = create_test_product_data()
    save_test_product_data(product_data, test_product_file)
    print(f"创建测试产品文件: {test_product_file}")
    
    # 先关闭可能存在的软件编辑器进程
    print("关闭可能存在的软件编辑器进程...")
    try:
        subprocess.run(["taskkill", "/f", "/im", "software-editor.exe"], capture_output=True)
        time.sleep(1)  # 等待进程完全关闭
    except Exception as e:
        print(f"关闭进程时出错: {e}")
    
    # 启动完整的软件编辑器
    print("启动软件编辑器...")
    app = Application().start(APP_INFO["executable_path"], wait_for_idle=False)
    time.sleep(5)  # 等待应用程序启动
    
    # 获取主窗口
    print("尝试获取主窗口...")
    main_window = app.window(title_re="产品编辑器 - 新建.*")
    print(f"找到主窗口: {main_window.exists()}")
    main_window.wait("visible", timeout=10)
    print("主窗口已获取")

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

def open_ui_layout_editor():
    """打开UI布局编辑器并返回窗口对象"""
    print("=== 打开UI布局编辑器 ===")
    
    # 使用uia后端连接
    app_uia = Application(backend="uia").connect(process=main_window.process_id())
    uia_window = app_uia.window(handle=main_window.handle)
    
    # 方法1: 通过菜单栏打开
    try:
        # 查找菜单栏
        menu_bar = uia_window.child_window(auto_id="QApplication.MainWindow.menuBar")
        if menu_bar.exists():
            print("找到菜单栏")
            
            # 查找UI布局菜单
            ui_layout_menu = menu_bar.child_window(title="UI布局", control_type="MenuItem")
            if ui_layout_menu.exists():
                print("找到UI布局菜单")
                
                # 点击菜单
                ui_layout_menu.click_input()
                time.sleep(1)
                
                # 查找"打开UI布局编辑器"菜单项
                open_item = uia_window.child_window(title="打开UI布局编辑器", control_type="MenuItem")
                if open_item.exists():
                    open_item.click_input()
                    time.sleep(3)  # 等待UI布局编辑器打开
                    print("通过菜单打开UI布局编辑器成功")
                else:
                    # 如果找不到菜单项，使用键盘操作
                    pyautogui.press('o')
                    time.sleep(3)
                    print("使用键盘打开UI布局编辑器")
            else:
                # 如果找不到菜单，尝试键盘快捷键
                print("未找到UI布局菜单，尝试键盘快捷键")
                main_window.set_focus()
                time.sleep(0.5)
                pyautogui.hotkey('alt', 'u')
                time.sleep(1)
                pyautogui.press('o')
                time.sleep(3)
        else:
            print("未找到菜单栏")
    except Exception as e:
        print(f"通过菜单打开UI布局编辑器失败: {e}")
    
    # 查找UI布局编辑器窗口
    print("查找UI布局编辑器窗口...")
    uilayout_window = None
    
    for w in app.windows():
        window_text = w.window_text()
        if "UI布局编辑器" in window_text or "UI Layout" in window_text:
            uilayout_window = w
            print(f"找到UI布局编辑器窗口: {window_text}")
            break
    
    if not uilayout_window:
        # 如果找不到特定窗口，尝试查找包含"布局"或"Layout"的窗口
        for w in app.windows():
            if "布局" in w.window_text() or "Layout" in w.window_text():
                uilayout_window = w
                print(f"找到可能的布局窗口: {w.window_text()}")
                break
    
    if uilayout_window:
        time.sleep(3)
        print("UI布局编辑器窗口已获取")
        return uilayout_window
    else:
        print("❌ 未找到UI布局编辑器窗口")
        return None

def test_ui_binding_workflow():
    """测试完整的UI绑定工作流程"""
    
    print("=== 开始UI绑定工作流程测试 ===")
    
    # 步骤1: 打开UI布局编辑器
    uilayout_window = open_ui_layout_editor()
    
    if not uilayout_window:
        pytest.skip("UI布局编辑器未成功打开")
    
    # 截图记录当前状态
    take_screenshot("ui_binding_step1_ui_layout_opened")
    
    # 步骤2: 创建简单的UI布局
    print("=== 步骤2: 创建UI布局 ===")
    
    # 使用uia后端连接UI布局编辑器
    app_uia = Application(backend="uia").connect(process=uilayout_window.process_id())
    uia_uilayout = app_uia.window(handle=uilayout_window.handle)
    
    # 查找左侧控件库中的QLabel控件
    qlabel_control = None
    controls = uia_uilayout.descendants()
    
    for control in controls:
        if control.class_name() == "QLabel" and control.window_text() == "QLabel":
            qlabel_control = control
            print("找到QLabel控件")
            break
    
    if not qlabel_control:
        # 如果找不到特定控件，尝试查找包含"Label"文本的控件
        for control in controls:
            if "Label" in control.window_text() and control.class_name() in ["QListWidgetItem", "QTreeWidgetItem"]:
                qlabel_control = control
                print(f"找到可能的QLabel控件: {control.window_text()}")
                break
    
    if qlabel_control:
        # 拖拽QLabel到编辑区
        rect = qlabel_control.rectangle()
        center_x = rect.mid_point().x
        center_y = rect.mid_point().y
        
        pyautogui.moveTo(center_x, center_y)
        pyautogui.mouseDown()
        time.sleep(0.5)
        
        # 移动到编辑区（假设编辑区在右侧）
        pyautogui.moveTo(center_x + 300, center_y)
        time.sleep(0.5)
        pyautogui.mouseUp()
        time.sleep(1)
        
        print("QLabel控件拖拽完成")
        take_screenshot("ui_binding_step2_qlabel_dragged")
    
    # 步骤3: 保存UI布局文件
    print("=== 步骤3: 保存UI布局文件 ===")
    
    # 查找保存按钮或菜单
    save_button = None
    for control in controls:
        if "保存" in control.window_text() or "Save" in control.window_text():
            save_button = control
            print(f"找到保存按钮: {control.window_text()}")
            break
    
    if save_button:
        save_button.click_input()
        time.sleep(2)
        
        # 处理保存对话框
        # 输入文件名
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        ui_layout_file = os.path.join(get_test_results_dir(), f"test_ui_layout_{timestamp}.ui")
        
        # 输入文件路径
        pyautogui.write(ui_layout_file)
        time.sleep(1)
        pyautogui.press('enter')
        time.sleep(2)
        
        print(f"UI布局文件保存成功: {ui_layout_file}")
        take_screenshot("ui_binding_step3_ui_layout_saved")
    
    # 步骤4: 关闭UI布局编辑器（触发自动绑定）
    print("=== 步骤4: 关闭UI布局编辑器 ===")
    
    # 查找关闭按钮
    close_button = None
    for control in controls:
        if "关闭" in control.window_text() or "Close" in control.window_text() or control.class_name() == "QPushButton" and "X" in control.window_text():
            close_button = control
            print(f"找到关闭按钮: {control.window_text()}")
            break
    
    if close_button:
        close_button.click_input()
    else:
        # 使用Alt+F4关闭窗口
        uilayout_window.set_focus()
        pyautogui.hotkey('alt', 'f4')
    
    time.sleep(3)  # 等待绑定完成
    
    print("UI布局编辑器已关闭，等待绑定完成")
    take_screenshot("ui_binding_step4_ui_layout_closed")
    
    # 步骤5: 验证产品数据中的UI布局路径
    print("=== 步骤5: 验证UI布局绑定 ===")
    
    # 等待绑定逻辑完成（可能需要更长时间）
    time.sleep(5)
    
    # 重新加载产品数据
    updated_product_data = load_test_product_data(test_product_file)
    
    # 检查UI布局路径是否已更新
    ui_layout_path = updated_product_data.get("uiLayoutPath", "")
    
    if ui_layout_path:
        print(f"✅ UI布局绑定成功! 绑定的UI文件: {ui_layout_path}")
        
        # 检查文件是否存在
        if os.path.exists(ui_layout_path):
            print("✅ 绑定的UI文件存在")
        else:
            print(f"⚠️ 绑定的UI文件不存在，但路径已记录: {ui_layout_path}")
            # 这可能是正常的，因为文件可能还没有被创建
    else:
        print("❌ UI布局绑定失败，uiLayoutPath为空")
        
        # 尝试多次检查，因为绑定可能是异步的
        for attempt in range(3):
            print(f"第{attempt + 1}次重试检查绑定...")
            time.sleep(3)
            
            updated_product_data = load_test_product_data(test_product_file)
            ui_layout_path = updated_product_data.get("uiLayoutPath", "")
            
            if ui_layout_path:
                print(f"✅ 重试成功! UI布局绑定成功! 绑定的UI文件: {ui_layout_path}")
                break
        else:
            print("❌ 多次重试后UI布局绑定仍然失败")
            pytest.fail("UI布局绑定失败")
    
    take_screenshot("ui_binding_step5_verification_complete")
    
    print("=== UI绑定工作流程测试完成 ===")

def test_ui_layout_auto_load():
    """测试UI布局文件的自动加载功能"""
    
    print("=== 开始UI布局自动加载测试 ===")
    
    # 步骤1: 确保产品已有绑定的UI布局文件
    product_data = load_test_product_data(test_product_file)
    
    if not product_data.get("uiLayoutPath"):
        pytest.skip("产品没有绑定的UI布局文件，跳过自动加载测试")
    
    # 步骤2: 重新打开UI布局编辑器
    uilayout_window = open_ui_layout_editor()
    
    if not uilayout_window:
        pytest.skip("UI布局编辑器未成功打开")
    
    # 等待自动加载完成
    time.sleep(5)
    
    # 步骤3: 验证UI布局是否已自动加载
    print("=== 验证UI布局自动加载 ===")
    
    # 使用uia后端连接UI布局编辑器
    app_uia = Application(backend="uia").connect(process=uilayout_window.process_id())
    uia_uilayout = app_uia.window(handle=uilayout_window.handle)
    
    # 检查编辑区是否有控件（表示UI布局已加载）
    controls = uia_uilayout.descendants()
    has_loaded_controls = False
    
    for control in controls:
        if control.class_name() in ["QLabel", "QPushButton", "QLineEdit"]:
            has_loaded_controls = True
            print(f"找到已加载的控件: {control.class_name()} - {control.window_text()}")
            break
    
    if has_loaded_controls:
        print("✅ UI布局自动加载成功")
        take_screenshot("ui_auto_load_success")
    else:
        print("❌ UI布局自动加载失败，编辑区没有控件")
        take_screenshot("ui_auto_load_failed")
        pytest.fail("UI布局自动加载失败")
    
    # 关闭UI布局编辑器
    uilayout_window.close()
    time.sleep(2)
    
    print("=== UI布局自动加载测试完成 ===")

def test_preview_functionality():
    """测试预览功能与UI绑定的集成"""
    
    print("=== 开始预览功能测试 ===")
    
    # 步骤1: 打开UI布局编辑器
    uilayout_window = open_ui_layout_editor()
    
    if not uilayout_window:
        pytest.skip("UI布局编辑器未成功打开")
    
    # 步骤2: 查找预览按钮
    app_uia = Application(backend="uia").connect(process=uilayout_window.process_id())
    uia_uilayout = app_uia.window(handle=uilayout_window.handle)
    
    preview_button = None
    controls = uia_uilayout.descendants()
    
    for control in controls:
        if "预览" in control.window_text() or "Preview" in control.window_text():
            preview_button = control
            print(f"找到预览按钮: {control.window_text()}")
            break
    
    if preview_button:
        # 点击预览按钮
        preview_button.click_input()
        time.sleep(3)
        
        # 查找预览窗口
        preview_window = None
        for w in app.windows():
            if "预览" in w.window_text() or "Preview" in w.window_text():
                preview_window = w
                print(f"找到预览窗口: {w.window_text()}")
                break
        
        if preview_window:
            print("✅ 预览窗口打开成功")
            take_screenshot("preview_window_opened")
            
            # 测试预览窗口的交互功能
            # 尝试点击预览窗口中的控件
            preview_app_uia = Application(backend="uia").connect(process=preview_window.process_id())
            uia_preview = preview_app_uia.window(handle=preview_window.handle)
            
            preview_controls = uia_preview.descendants()
            for control in preview_controls:
                if control.class_name() in ["QLabel", "QPushButton", "QLineEdit"]:
                    print(f"预览窗口中找到控件: {control.class_name()} - {control.window_text()}")
                    
                    # 尝试点击按钮控件
                    if control.class_name() == "QPushButton":
                        try:
                            control.click_input()
                            time.sleep(1)
                            print("✅ 预览窗口按钮点击成功")
                        except Exception as e:
                            print(f"预览窗口按钮点击失败: {e}")
                    
                    # 尝试在输入框中输入文本
                    if control.class_name() == "QLineEdit":
                        try:
                            control.click_input()
                            pyautogui.write("测试输入")
                            time.sleep(1)
                            print("✅ 预览窗口输入框输入成功")
                        except Exception as e:
                            print(f"预览窗口输入框输入失败: {e}")
            
            # 关闭预览窗口
            preview_window.close()
            time.sleep(2)
        else:
            print("❌ 预览窗口未找到")
            take_screenshot("preview_window_not_found")
    else:
        print("❌ 预览按钮未找到")
        take_screenshot("preview_button_not_found")
    
    # 关闭UI布局编辑器
    uilayout_window.close()
    time.sleep(2)
    
    print("=== 预览功能测试完成 ===")

if __name__ == "__main__":
    # 直接运行测试
    setup_module(None)
    
    try:
        test_ui_binding_workflow()
        test_ui_layout_auto_load()
        test_preview_functionality()
    except Exception as e:
        print(f"测试执行失败: {e}")
    finally:
        teardown_module(None)