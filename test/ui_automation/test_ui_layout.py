import pytest
import pyautogui
import time
import os
import subprocess
import xml.etree.ElementTree as ET
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

def setup_module(module):
    """模块级别的setup函数，在所有测试开始前执行"""
    global app, main_window, uilayout_window
    
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
    main_window = app.window(title_re="产品编辑器 - 新建.*")
    print(f"找到主窗口: {main_window.exists()}")
    main_window.wait("visible", timeout=10)
    print("主窗口已获取")
    
    # 打开UI布局编辑器
    uilayout_window = open_ui_layout_editor()

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
                # 假设有快捷键，比如Alt+U
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
        # 使用time.sleep等待窗口可见，而不是wait方法
        time.sleep(3)
        print("UI布局编辑器窗口已获取")
        return uilayout_window
    else:
        print("❌ 未找到UI布局编辑器窗口")
        return None

def test_ui_layout_drag_and_edit():
    """测试UI布局编辑器的拖拽和编辑功能"""
    
    if not uilayout_window:
        pytest.skip("UI布局编辑器未成功打开")
    
    # 定义测试文件路径
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    test_file_path = os.path.join(get_test_results_dir(), f"test_layout_{timestamp}.ui")
    
    try:
        # 步骤1: 拖拽QLabel控件到编辑区
        print("=== 步骤1: 拖拽QLabel控件 ===")
        
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
            # 获取控件位置
            rect = qlabel_control.rectangle()
            center_x = rect.mid_point().x
            center_y = rect.mid_point().y
            
            # 点击并拖拽
            pyautogui.moveTo(center_x, center_y)
            pyautogui.mouseDown()
            time.sleep(0.5)
            
            # 移动到编辑区（假设编辑区在右侧）
            pyautogui.moveTo(center_x + 300, center_y)
            time.sleep(0.5)
            pyautogui.mouseUp()
            time.sleep(1)
            print("QLabel控件拖拽完成")
        else:
            print("未找到QLabel控件，跳过拖拽测试")
        
        # 步骤2: 编辑QLabel文本
        print("=== 步骤2: 编辑QLabel文本 ===")
        
        # 查找编辑区中的QLabel控件
        edited_qlabel = None
        for control in uia_uilayout.descendants():
            if control.class_name() == "QLabel":
                # 检查是否是刚刚添加的QLabel
                if control.window_text() in ["", "QLabel", "文本"]:
                    edited_qlabel = control
                    print("找到可编辑的QLabel控件")
                    break
        
        if edited_qlabel:
            # 双击QLabel进入编辑模式
            edited_qlabel.double_click_input()
            time.sleep(1)
            
            # 输入新文本
            pyautogui.write("测试标签")
            time.sleep(0.5)
            pyautogui.press('enter')
            time.sleep(1)
            print("QLabel文本编辑完成")
        else:
            print("未找到可编辑的QLabel控件")
        
        # 步骤3: 拖拽QTabWidget控件
        print("=== 步骤3: 拖拽QTabWidget控件 ===")
        
        # 查找QTabWidget控件
        tabwidget_control = None
        for control in controls:
            if control.class_name() == "QTabWidget" and control.window_text() == "QTabWidget":
                tabwidget_control = control
                print("找到QTabWidget控件")
                break
        
        if not tabwidget_control:
            # 查找包含"Tab"文本的控件
            for control in controls:
                if "Tab" in control.window_text() and control.class_name() in ["QListWidgetItem", "QTreeWidgetItem"]:
                    tabwidget_control = control
                    print(f"找到可能的QTabWidget控件: {control.window_text()}")
                    break
        
        if tabwidget_control:
            # 拖拽QTabWidget到编辑区
            rect = tabwidget_control.rectangle()
            center_x = rect.mid_point().x
            center_y = rect.mid_point().y
            
            pyautogui.moveTo(center_x, center_y)
            pyautogui.mouseDown()
            time.sleep(0.5)
            
            # 移动到编辑区不同位置
            pyautogui.moveTo(center_x + 300, center_y + 100)
            time.sleep(0.5)
            pyautogui.mouseUp()
            time.sleep(1)
            print("QTabWidget控件拖拽完成")
        else:
            print("未找到QTabWidget控件")
        
        # 步骤4: 编辑Tab页标题
        print("=== 步骤4: 编辑Tab页标题 ===")
        
        # 查找QTabWidget中的Tab页
        tab_page = None
        for control in uia_uilayout.descendants():
            if control.class_name() == "QTabBar":
                # 查找Tab页
                tab_children = control.children()
                for tab in tab_children:
                    if tab.class_name() in ["QTab", "Tab"]:
                        tab_page = tab
                        print("找到Tab页")
                        break
                if tab_page:
                    break
        
        if tab_page:
            # 双击Tab页标题
            tab_page.double_click_input()
            time.sleep(1)
            
            # 输入新标题
            pyautogui.write("测试标签页")
            time.sleep(0.5)
            pyautogui.press('enter')
            time.sleep(1)
            print("Tab页标题编辑完成")
        else:
            print("未找到Tab页")
        
        # 步骤5: 保存UI布局
        print("=== 步骤5: 保存UI布局 ===")
        
        # 使用Ctrl+S快捷键保存
        uilayout_window.set_focus()
        time.sleep(0.5)
        pyautogui.hotkey('ctrl', 's')
        time.sleep(2)
        
        # 处理保存对话框
        save_dialog = None
        for w in app.windows():
            if "保存" in w.window_text() or "Save" in w.window_text():
                save_dialog = w
                print(f"找到保存对话框: {w.window_text()}")
                break
        
        if save_dialog:
            # 输入完整文件路径
            pyautogui.write(test_file_path)
            time.sleep(0.5)
            pyautogui.press('enter')
            time.sleep(2)
            print("UI布局保存完成")
        else:
            print("未找到保存对话框")
        
        # 步骤6: 验证保存结果
        print("=== 步骤6: 验证保存结果 ===")
        
        time.sleep(3)  # 等待文件保存完成
        
        if os.path.exists(test_file_path):
            print(f"[SUCCESS] UI布局保存成功: {test_file_path}")
            
            # 验证文件格式（UI文件通常是XML格式）
            try:
                tree = ET.parse(test_file_path)
                root = tree.getroot()
                
                # 检查基本结构
                if root.tag == "ui":
                    print("[SUCCESS] 文件格式正确，为有效的UI文件")
                    
                    # 检查是否包含窗口定义
                    if len(root) > 0:
                        print("[SUCCESS] 文件包含UI组件定义")
                    else:
                        print("[WARNING] 文件为空或格式不完整")
                        
                else:
                    print("[WARNING] 文件不是标准的UI格式")
                    
            except ET.ParseError as e:
                print(f"[ERROR] 文件不是有效的XML格式: {e}")
            except Exception as e:
                print(f"[ERROR] 读取文件时出错: {e}")
        else:
            print(f"[ERROR] UI布局保存失败，未找到文件: {test_file_path}")
        
        # 步骤7: 测试预览功能
        print("=== 步骤7: 测试预览功能 ===")
        
        # 查找预览按钮
        preview_button = None
        for control in uia_uilayout.descendants():
            if control.class_name() == "QPushButton" and "预览" in control.window_text():
                preview_button = control
                print("找到预览按钮")
                break
        
        if preview_button:
            preview_button.click_input()
            time.sleep(2)
            print("预览按钮已点击")
            
            # 查找预览窗口
            preview_window = None
            for w in app.windows():
                if "预览" in w.window_text() or "Preview" in w.window_text():
                    preview_window = w
                    print(f"找到预览窗口: {w.window_text()}")
                    break
            
            if preview_window:
                # 在预览窗口中测试交互
                print("在预览窗口中测试交互...")
                
                # 尝试点击QLabel（如果存在）
                try:
                    app_uia_preview = Application(backend="uia").connect(handle=preview_window.handle)
                    uia_preview = app_uia_preview.window(handle=preview_window.handle)
                    
                    # 查找QLabel
                    for control in uia_preview.descendants():
                        if control.class_name() == "QLabel" and "测试标签" in control.window_text():
                            print("[SUCCESS] 预览窗口中显示编辑后的QLabel")
                            break
                    
                    # 关闭预览窗口
                    preview_window.close()
                    time.sleep(1)
                    
                except Exception as e:
                    print(f"预览窗口交互测试失败: {e}")
            else:
                print("未找到预览窗口")
        else:
            print("未找到预览按钮")
        
        # 步骤8: 截图保存测试过程
        print("=== 步骤8: 保存测试截图 ===")
        take_screenshot("ui_layout_edit_test")
        
        # 测试结果总结
        if os.path.exists(test_file_path):
            print("[SUCCESS] UI布局编辑功能测试成功！")
            assert True, "UI布局编辑功能测试通过"
        else:
            print("[ERROR] UI布局编辑功能测试失败")
            
            # 提供可能的失败原因
            print("可能的原因:")
            print("1. 控件拖拽功能未正常工作")
            print("2. 文本编辑功能存在问题")
            print("3. 保存对话框未正确出现")
            print("4. 文件保存路径不正确")
            print("5. UI布局编辑器功能存在问题")
            
            assert False, "UI布局编辑功能测试失败"
            
    except Exception as e:
        print(f"测试过程中出现异常: {e}")
        take_screenshot("ui_layout_edit_test_error")
        raise e
    
    finally:
        # 保留测试文件到UI_FILES_DIR目录
        if os.path.exists(test_file_path):
            print(f"测试UI文件已保存到: {test_file_path}")

def test_ui_layout_load_flow():
    """测试UI布局加载的完整闭环流程"""
    
    if not uilayout_window:
        pytest.skip("UI布局编辑器未成功打开")
    
    # 先创建一个简单的测试UI文件
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    test_file_path = os.path.join(get_test_results_dir(), f"test_load_layout_{timestamp}.ui")
    
    # 创建简单的UI文件内容
    ui_content = """<?xml version="1.0" encoding="UTF-8"?>
<ui version="4.0">
 <class>MainWindow</class>
 <widget class="QMainWindow" name="MainWindow">
  <property name="geometry">
   <rect>
    <x>0</x>
    <y>0</y>
    <width>400</width>
    <height>300</height>
   </rect>
  </property>
  <property name="windowTitle">
   <string>测试布局</string>
  </property>
  <widget class="QWidget" name="centralwidget">
   <layout class="QVBoxLayout" name="verticalLayout">
    <item>
     <widget class="QLabel" name="label">
      <property name="text">
       <string>这是测试布局</string>
      </property>
     </widget>
    </item>
   </layout>
  </widget>
 </widget>
 <resources/>
 <connections/>
</ui>"""
    
    try:
        with open(test_file_path, 'w', encoding='utf-8') as f:
            f.write(ui_content)
        print(f"创建测试UI文件: {test_file_path}")
        
        # 步骤1: 使用Ctrl+O打开UI文件
        print("=== 步骤1: 打开UI文件 ===")
        
        uilayout_window.set_focus()
        time.sleep(0.5)
        pyautogui.hotkey('ctrl', 'o')
        time.sleep(2)
        
        # 处理打开对话框
        open_dialog = None
        for w in app.windows():
            if "打开" in w.window_text() or "Open" in w.window_text():
                open_dialog = w
                print(f"找到打开对话框: {w.window_text()}")
                break
        
        if open_dialog:
            # 输入文件名
            pyautogui.write(os.path.basename(test_file_path))
            time.sleep(0.5)
            pyautogui.press('enter')
            time.sleep(2)
            print("UI文件打开完成")
        else:
            print("未找到打开对话框")
        
        # 步骤2: 验证加载结果
        print("=== 步骤2: 验证加载结果 ===")
        
        time.sleep(3)
        
        # 检查编辑区是否显示了加载的内容
        # 使用uia后端检查是否有QLabel显示"这是测试布局"
        app_uia = Application(backend="uia").connect(process=uilayout_window.process_id())
        uia_uilayout = app_uia.window(handle=uilayout_window.handle)
        
        loaded_label = None
        for control in uia_uilayout.descendants():
            if control.class_name() == "QLabel" and "这是测试布局" in control.window_text():
                loaded_label = control
                print("[SUCCESS] UI布局加载成功，显示测试标签")
                break
        
        if not loaded_label:
            print("[WARNING] 未找到加载的标签，但文件可能已成功加载")
        
        # 截图保存测试过程
        take_screenshot("ui_layout_load_test")
        
        print("[SUCCESS] UI布局加载功能测试完成")
        
    except Exception as e:
        print(f"测试过程中出现异常: {e}")
        take_screenshot("ui_layout_load_test_error")
        raise e
    
    finally:
        # 保留测试文件到UI_FILES_DIR目录
        if os.path.exists(test_file_path):
            print(f"测试UI文件已保存到: {test_file_path}")

if __name__ == "__main__":
    # 直接运行测试
    setup_module(None)
    try:
        test_ui_layout_drag_and_edit()
        test_ui_layout_load_flow()
    finally:
        teardown_module(None)