import os
import sys
import time
import pywinauto
from pywinauto.application import Application
import filecmp

# 配置应用程序路径和测试文件路径
APP_PATH = r"d:\virtualMachine\github\software-editor\build\software-editor.exe"
TEST_UI_FILE = r"d:\virtualMachine\github\software-editor\test\data\mainwindow.ui"
SAVED_UI_FILE = r"d:\virtualMachine\github\software-editor\test\data\mainwindow_saved.ui"

# 等待时间配置
WAIT_APP_START = 5  # 等待应用程序启动的时间（秒）
WAIT_FILE_DIALOG = 3  # 等待文件对话框出现的时间（秒）
WAIT_UI_LOAD = 8  # 等待UI文件加载完成的时间（秒）
WAIT_UI_SAVE = 8  # 等待UI文件保存完成的时间（秒）

def test_ui_load_save_compare():
    """测试UI文件加载和保存功能，并比较原文件和保存后的文件是否相等"""
    try:
        # 1. 启动应用程序
        print("启动应用程序...")
        app = Application(backend="uia").start(APP_PATH, wait_for_idle=False)
        time.sleep(WAIT_APP_START)
        
        # 2. 等待主窗口出现
        print("等待主窗口出现...")
        # 尝试获取所有窗口
        windows = app.windows()
        print(f"找到 {len(windows)} 个窗口")
        
        # 遍历所有窗口，找到主窗口
        main_window = None
        for window in windows:
            title = window.window_text()
            print(f"窗口标题: {title}")
            if "产品编辑器" in title:
                main_window = window
                break
        
        # 如果找到主窗口，等待其可见
        if main_window:
            time.sleep(5)  # 等待主窗口可见
        else:
            raise Exception("未找到主窗口")
        
        # 3. 进入UI布局页面
        print("进入UI布局页面...")
        # 点击菜单栏上的“UI布局”->“打开UI布局编辑器”
        # 首先点击“UI布局”菜单
        main_window.menu_select("UI布局")
        time.sleep(1)
        # 然后点击“打开UI布局编辑器”菜单项
        main_window.menu_select("UI布局->打开UI布局编辑器")
        time.sleep(5)  # 等待UI布局编辑器打开
        
        # 找到UI布局编辑器窗口
        ui_layout_window = None
        windows = app.windows()
        print(f"找到 {len(windows)} 个窗口")
        for window in windows:
            title = window.window_text()
            print(f"窗口标题: {title}")
            if "UI布局编辑器" in title or "UI布局" in title:
                ui_layout_window = window
                break
        
        if not ui_layout_window:
            # 如果没有找到UI布局编辑器窗口，尝试使用主窗口
            ui_layout_window = main_window
        
        # 4. 打开UI文件
        print("打开UI文件...")
        # 点击菜单栏上的“文件”->“加载”
        ui_layout_window.menu_select("文件")
        time.sleep(1)
        ui_layout_window.menu_select("文件->加载")
        time.sleep(WAIT_FILE_DIALOG)
        
        # 5. 在文件对话框中输入UI文件路径并确认
        print("选择UI文件...")
        # 使用pyautogui模拟键盘输入文件路径
        import pyautogui
        # 等待文件对话框出现
        time.sleep(2)
        # 输入UI文件路径
        pyautogui.typewrite(TEST_UI_FILE)
        # 等待输入完成
        time.sleep(1)
        # 按下回车键确认
        pyautogui.press("enter")
        
        # 6. 等待UI文件加载完成
        print("等待UI文件加载完成...")
        time.sleep(WAIT_UI_LOAD)
        
        # 7. 保存UI文件
        print("保存UI文件...")
        # 点击菜单栏上的“文件”->“保存”
        ui_layout_window.menu_select("文件")
        time.sleep(1)
        ui_layout_window.menu_select("文件->保存")
        time.sleep(WAIT_FILE_DIALOG)
        
        # 8. 在保存对话框中输入保存路径并确认
        print("选择保存路径...")
        pyautogui.typewrite(SAVED_UI_FILE)
        pyautogui.press("enter")
        
        # 9. 等待UI文件保存完成
        print("等待UI文件保存完成...")
        time.sleep(WAIT_UI_SAVE)
        
        # 9. 比较原UI文件和保存后的UI文件是否相等
        print("比较原UI文件和保存后的UI文件...")
        if filecmp.cmp(TEST_UI_FILE, SAVED_UI_FILE, shallow=False):
            print("✓ UI文件加载和保存功能正常，原文件和保存后的文件相等")
            return True
        else:
            print("✗ UI文件加载和保存功能异常，原文件和保存后的文件不相等")
            return False
            
    except Exception as e:
        print(f"✗ 测试失败: {str(e)}")
        return False
    finally:
        # 关闭应用程序
        try:
            app.kill()
        except:
            pass

if __name__ == "__main__":
    success = test_ui_load_save_compare()
    sys.exit(0 if success else 1)