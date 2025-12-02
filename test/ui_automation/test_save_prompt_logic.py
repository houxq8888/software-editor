import pytest
import pyautogui
import time
import os
import subprocess
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

class TestSavePromptLogic:
    """测试UI布局编辑器保存提示框逻辑"""
    
    def setup_method(self, method):
        """每个测试方法开始前执行"""
        self.app = None
        self.main_window = None
        self.uilayout_window = None
        self.test_results_dir = get_test_results_dir()
        
        # 先关闭可能存在的软件编辑器进程
        print("关闭可能存在的软件编辑器进程...")
        try:
            subprocess.run(["taskkill", "/f", "/im", "software-editor.exe"], capture_output=True)
            time.sleep(2)  # 等待进程完全关闭
        except Exception as e:
            print(f"关闭进程时出错: {e}")
        
        # 启动完整的软件编辑器
        print("启动软件编辑器...")
        self.app = Application().start("d:\\virtualMachine\\github\\software-editor\\build\\software-editor.exe", wait_for_idle=False)
        time.sleep(5)  # 等待应用程序启动
        
        # 获取主窗口
        print("尝试获取主窗口...")
        self.main_window = self.app.window(title_re="产品编辑器 - 新建.*")
        print(f"找到主窗口: {self.main_window.exists()}")
        self.main_window.wait("visible", timeout=10)
        print("主窗口已获取")
    
    def teardown_method(self, method):
        """每个测试方法结束后执行"""
        # 关闭应用程序
        print("关闭软件编辑器...")
        try:
            if self.app:
                self.app.kill()
                time.sleep(1)
        except Exception as e:
            print(f"关闭应用程序时出错: {e}")
    
    def open_ui_layout_editor(self):
        """打开UI布局编辑器并返回窗口对象"""
        print("=== 打开UI布局编辑器 ===")
        
        # 使用uia后端连接
        app_uia = Application(backend="uia").connect(process=self.main_window.process_id())
        uia_window = app_uia.window(handle=self.main_window.handle)
        
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
                    self.main_window.set_focus()
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
        
        for w in self.app.windows():
            window_text = w.window_text()
            if "UI布局编辑器" in window_text or "UI Layout" in window_text:
                uilayout_window = w
                print(f"找到UI布局编辑器窗口: {window_text}")
                break
        
        if not uilayout_window:
            # 如果找不到特定窗口，尝试查找包含"布局"或"Layout"的窗口
            for w in self.app.windows():
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
    
    def close_ui_layout_editor(self, uilayout_window):
        """关闭UI布局编辑器并检查是否弹出保存提示框"""
        print("=== 关闭UI布局编辑器 ===")
        
        # 激活UI布局编辑器窗口
        uilayout_window.set_focus()
        time.sleep(1)
        
        # 截图记录关闭前的状态
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        screenshot_path = os.path.join(self.test_results_dir, f"close_uilayout_before_{timestamp}.png")
        take_screenshot(screenshot_path)
        
        # 关闭窗口
        uilayout_window.close()
        time.sleep(2)  # 等待可能的保存提示框出现
        
        # 检查是否弹出保存提示框
        save_prompt_found = False
        for w in self.app.windows():
            window_text = w.window_text()
            if "保存" in window_text or "Save" in window_text or "提示" in window_text or "提示" in window_text:
                save_prompt_found = True
                print(f"找到保存提示框: {window_text}")
                
                # 截图记录保存提示框
                screenshot_path = os.path.join(self.test_results_dir, f"save_prompt_{timestamp}.png")
                take_screenshot(screenshot_path)
                
                # 点击取消或关闭按钮
                try:
                    # 查找取消按钮
                    cancel_button = w.child_window(title_re=".*取消.*|.*Cancel.*", control_type="Button")
                    if cancel_button.exists():
                        cancel_button.click()
                        print("点击取消按钮")
                    else:
                        # 如果没有取消按钮，尝试关闭对话框
                        w.close()
                        print("关闭保存提示框")
                except Exception as e:
                    print(f"处理保存提示框时出错: {e}")
                break
        
        if not save_prompt_found:
            print("未找到保存提示框")
        
        return save_prompt_found
    
    def close_main_window(self):
        """关闭主窗口并检查是否弹出保存提示框"""
        print("=== 关闭主窗口 ===")
        
        # 激活主窗口
        self.main_window.set_focus()
        time.sleep(1)
        
        # 截图记录关闭前的状态
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        screenshot_path = os.path.join(self.test_results_dir, f"close_main_before_{timestamp}.png")
        take_screenshot(screenshot_path)
        
        # 关闭主窗口
        self.main_window.close()
        time.sleep(2)  # 等待可能的保存提示框出现
        
        # 检查是否弹出保存提示框
        save_prompt_found = False
        for w in self.app.windows():
            window_text = w.window_text()
            if "保存" in window_text or "Save" in window_text or "提示" in window_text or "提示" in window_text:
                save_prompt_found = True
                print(f"找到保存提示框: {window_text}")
                
                # 截图记录保存提示框
                screenshot_path = os.path.join(self.test_results_dir, f"main_save_prompt_{timestamp}.png")
                take_screenshot(screenshot_path)
                
                # 点击取消或关闭按钮
                try:
                    # 查找取消按钮
                    cancel_button = w.child_window(title_re=".*取消.*|.*Cancel.*", control_type="Button")
                    if cancel_button.exists():
                        cancel_button.click()
                        print("点击取消按钮")
                    else:
                        # 如果没有取消按钮，尝试关闭对话框
                        w.close()
                        print("关闭保存提示框")
                except Exception as e:
                    print(f"处理保存提示框时出错: {e}")
                break
        
        if not save_prompt_found:
            print("未找到保存提示框")
        
        return save_prompt_found
    
    def modify_ui_layout(self, uilayout_window):
        """在UI布局编辑器中进行修改操作"""
        print("=== 修改UI布局 ===")
        
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
            return True
        else:
            print("❌ 未找到可拖拽的控件")
            return False
    
    def test_no_modification_no_save_prompt(self):
        """测试1: 打开UI布局编辑器但不修改，关闭时不应弹出保存提示框"""
        print("\n=== 测试1: 不修改UI布局时的保存提示框逻辑 ===")
        
        # 打开UI布局编辑器
        self.uilayout_window = self.open_ui_layout_editor()
        if not self.uilayout_window:
            pytest.skip("UI布局编辑器未成功打开")
        
        # 等待UI布局编辑器完全加载
        time.sleep(3)
        
        # 截图记录打开状态
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        screenshot_path = os.path.join(self.test_results_dir, f"test1_open_{timestamp}.png")
        take_screenshot(screenshot_path)
        
        # 不进行任何修改，直接关闭UI布局编辑器
        save_prompt_found = self.close_ui_layout_editor(self.uilayout_window)
        
        # 验证：不应该弹出保存提示框
        assert not save_prompt_found, "❌ 测试失败：未修改UI布局时关闭UI布局编辑器弹出了保存提示框"
        print("✅ 测试通过：未修改UI布局时关闭UI布局编辑器没有弹出保存提示框")
        
        # 等待UI布局编辑器完全关闭
        time.sleep(2)
        
        # 关闭主窗口
        main_save_prompt_found = self.close_main_window()
        
        # 验证：关闭主窗口时也不应该弹出保存提示框
        assert not main_save_prompt_found, "❌ 测试失败：未修改UI布局时关闭主窗口弹出了保存提示框"
        print("✅ 测试通过：未修改UI布局时关闭主窗口也没有弹出保存提示框")
    
    def test_with_modification_save_prompt(self):
        """测试2: 打开UI布局编辑器并进行修改，关闭时应弹出保存提示框"""
        print("\n=== 测试2: 修改UI布局时的保存提示框逻辑 ===")
        
        # 打开UI布局编辑器
        self.uilayout_window = self.open_ui_layout_editor()
        if not self.uilayout_window:
            pytest.skip("UI布局编辑器未成功打开")
        
        # 等待UI布局编辑器完全加载
        time.sleep(3)
        
        # 截图记录打开状态
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        screenshot_path = os.path.join(self.test_results_dir, f"test2_open_{timestamp}.png")
        take_screenshot(screenshot_path)
        
        # 在UI布局编辑器中进行修改
        modification_success = self.modify_ui_layout(self.uilayout_window)
        
        if not modification_success:
            pytest.skip("无法在UI布局编辑器中进行修改操作")
        
        # 截图记录修改后的状态
        screenshot_path = os.path.join(self.test_results_dir, f"test2_modified_{timestamp}.png")
        take_screenshot(screenshot_path)
        
        # 关闭UI布局编辑器
        save_prompt_found = self.close_ui_layout_editor(self.uilayout_window)
        
        # 验证：应该弹出保存提示框
        assert save_prompt_found, "❌ 测试失败：修改UI布局后关闭UI布局编辑器没有弹出保存提示框"
        print("✅ 测试通过：修改UI布局后关闭UI布局编辑器弹出了保存提示框")
        
        # 等待UI布局编辑器完全关闭
        time.sleep(2)
        
        # 关闭主窗口
        main_save_prompt_found = self.close_main_window()
        
        # 验证：关闭主窗口时也应该弹出保存提示框
        assert main_save_prompt_found, "❌ 测试失败：修改UI布局后关闭主窗口没有弹出保存提示框"
        print("✅ 测试通过：修改UI布局后关闭主窗口也弹出了保存提示框")

if __name__ == "__main__":
    # 直接运行测试
    test_instance = TestSavePromptLogic()
    
    try:
        test_instance.setup_method(None)
        
        # 运行测试1
        print("\n" + "="*60)
        print("开始运行测试1: 不修改UI布局时的保存提示框逻辑")
        print("="*60)
        test_instance.test_no_modification_no_save_prompt()
        
        # 重新启动应用程序进行测试2
        test_instance.teardown_method(None)
        time.sleep(2)
        
        test_instance.setup_method(None)
        
        # 运行测试2
        print("\n" + "="*60)
        print("开始运行测试2: 修改UI布局时的保存提示框逻辑")
        print("="*60)
        test_instance.test_with_modification_save_prompt()
        
        print("\n" + "="*60)
        print("✅ 所有测试通过！保存提示框逻辑修复验证成功")
        print("="*60)
        
    except Exception as e:
        print(f"❌ 测试失败: {e}")
        raise
    finally:
        test_instance.teardown_method(None)