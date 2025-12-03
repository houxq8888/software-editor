import os
import sys
import time
import shutil
import pyautogui
from pathlib import Path
from pywinauto import Application
from config import APP_INFO
from utils import take_screenshot

# 添加项目根目录到Python路径
project_root = Path(__file__).parent.parent.parent
sys.path.append(str(project_root))

def test_ui_load_save():
    """测试UI文件的加载和保存功能"""
    
    # 测试文件路径
    test_ui_file = str(project_root / "src" / "product" / "mainwindow.ui")
    temp_save_file = str(project_root / "test_output" / "temp_saved_ui.ui")
    
    # 确保测试输出目录存在
    os.makedirs(os.path.dirname(temp_save_file), exist_ok=True)
    
    try:
        # 1. 启动应用程序并打开UI布局编辑器
        print("启动应用程序并打开UI布局编辑器...")
        
        # 启动应用程序
        app = Application().start(APP_INFO["path"], wait_for_idle=False)
        time.sleep(5)
        
        # 获取主窗口
        main_window = app.window(title_re=".*产品编辑器.*")
        main_window.wait("visible", timeout=10)
        
        # 打开UI布局编辑器
        main_window.type_keys("^L")  # Ctrl+L
        time.sleep(2)
        
        # 获取UI布局编辑器窗口
        ui_layout_window = app.window(title_re=".*UI布局编辑器.*")
        ui_layout_window.wait("visible", timeout=10)
        
        take_screenshot("ui_load_save_start")
        
        # 2. 加载测试UI文件
        print(f"加载UI文件: {test_ui_file}")
        
        # 使用快捷键 Ctrl+O 打开文件对话框
        ui_layout_window.type_keys("^O")
        time.sleep(3)  # 增加等待时间
        
        # 使用pyautogui模拟键盘操作输入文件路径
        pyautogui.typewrite(test_ui_file)
        time.sleep(2)  # 等待输入完成
        
        # 按下回车键打开文件
        pyautogui.press("enter")
        time.sleep(8)  # 增加等待时间，确保UI文件加载完成
        
        take_screenshot("ui_file_loaded")
        
        # 3. 保存加载的UI内容到临时文件
        print(f"保存UI内容到临时文件: {temp_save_file}")
        
        # 使用快捷键 Ctrl+Shift+S 打开另存为对话框
        ui_layout_window.type_keys("^+S")
        time.sleep(3)  # 增加等待时间
        
        # 使用pyautogui模拟键盘操作输入文件路径
        pyautogui.typewrite(temp_save_file)
        time.sleep(2)  # 等待输入完成
        
        # 按下回车键保存文件
        pyautogui.press("enter")
        time.sleep(8)  # 增加等待时间，确保保存完成
        
        take_screenshot("ui_file_saved")
        
        # 4. 比较原UI文件和保存的UI文件是否相等
        print("比较原UI文件和保存的UI文件...")
        
        # 读取原UI文件内容
        with open(test_ui_file, 'r', encoding='utf-8') as f:
            original_content = f.read()
        
        # 读取保存的UI文件内容
        with open(temp_save_file, 'r', encoding='utf-8') as f:
            saved_content = f.read()
        
        # 比较两个文件的内容
        if original_content == saved_content:
            print("✓ UI文件加载和保存成功，文件内容相等！")
            result = True
        else:
            print("✗ UI文件加载和保存失败，文件内容不相等！")
            
            # 输出文件内容差异（前1000个字符）
            print("原文件内容前1000个字符:")
            print(original_content[:1000])
            print("\n保存的文件内容前1000个字符:")
            print(saved_content[:1000])
            
            result = False
        
        # 关闭UI布局编辑器窗口
        ui_layout_window.close()
        time.sleep(1)
        
        # 关闭主窗口
        main_window.close()
        time.sleep(2)
        
        return result
        
    except Exception as e:
        print(f"✗ 测试过程中发生错误: {str(e)}")
        take_screenshot("ui_load_save_error")
        return False
    
    finally:
        # 清理临时文件
        if os.path.exists(temp_save_file):
            os.remove(temp_save_file)

if __name__ == "__main__":
    success = test_ui_load_save()
    sys.exit(0 if success else 1)
