import sys
import os
import time
import subprocess
import pyautogui
from utils import wait_for_window, click_element, type_in_element

def fix_mainwindow_copy_ui():
    # 设置pyautogui延迟
    pyautogui.PAUSE = 1
    pyautogui.FAILSAFE = True
    
    # 启动应用程序
    app_path = os.path.abspath("../../build/software-editor.exe")
    if not os.path.exists(app_path):
        print(f"错误：未找到应用程序 {app_path}")
        print("请先运行 build.bat 编译程序")
        return
        
    process = subprocess.Popen([app_path])
    
    try:
        # 等待程序启动
        print("等待程序启动...")
        time.sleep(5)
        
        # 打开UI布局编辑器
        print("打开UI布局编辑器...")
        # 使用快捷键Ctrl+L打开UI布局编辑器
        pyautogui.hotkey('ctrl', 'l')
        time.sleep(3)
        
        print("UI布局编辑器打开成功")
        
        # 打开文件
        print("打开mainwindow_copy.ui文件...")
        pyautogui.hotkey('ctrl', 'o')
        time.sleep(1)
        
        # 输入文件路径
        file_path = os.path.abspath("../../src/product/mainwindow_copy.ui")
        pyautogui.typewrite(file_path)
        time.sleep(0.5)
        
        # 按回车键确认打开
        pyautogui.press('enter')
        time.sleep(3)
        
        print("文件打开成功，开始修复结构...")
        
        # 等待几秒钟让用户手动确认修复（或者可以添加自动修复逻辑）
        print("请在编辑器中手动修复根元素结构：")
        print("1. 将根元素从<QWidget>改为<QMainWindow>")
        print("2. 移除错误的centralWidget嵌套")
        print("3. 保存文件")
        print("等待10秒供手动操作...")
        time.sleep(10)
        
        # 自动保存文件
        pyautogui.hotkey('ctrl', 's')
        time.sleep(1)
        
        print("文件已保存，修复完成")
        
    except Exception as e:
        print(f"修复过程中出现错误: {e}")
    finally:
        # 关闭应用程序
        print("关闭应用程序...")
        pyautogui.hotkey('alt', 'f4')
        time.sleep(2)
        process.terminate()
        process.wait()

if __name__ == "__main__":
    fix_mainwindow_copy_ui()