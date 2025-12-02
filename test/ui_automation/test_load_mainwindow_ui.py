import pyautogui
import time
import os
import subprocess
from pywinauto import Application
from config import APP_INFO
from utils import take_screenshot

def load_mainwindow_ui():
    """加载mainwindow.ui文件到UI布局编辑器"""
    print("=== 开始加载mainwindow.ui文件 ===")
    
    # 先关闭可能存在的软件编辑器进程
    print("关闭可能存在的软件编辑器进程...")
    try:
        subprocess.run(["taskkill", "/f", "/im", "software-editor.exe"], capture_output=True)
        time.sleep(1)  # 等待进程完全关闭
    except Exception as e:
        print(f"关闭进程时出错: {e}")
    
    # 启动完整的软件编辑器
    print("启动软件编辑器...")
    app = Application(backend="uia").start("d:\\virtualMachine\\github\\software-editor-08\\build\\software-editor.exe", wait_for_idle=False)
    time.sleep(5)  # 等待应用程序启动
    
    # 获取主窗口
    print("尝试获取主窗口...")
    try:
        # 等待应用程序完全启动
        time.sleep(2)
        
        # 尝试查找UI布局编辑器窗口
        uilayout_window = None
        try:
            # 先尝试通过标题查找
            uilayout_window = app.window(title_re=".*UI布局编辑器.*|.*UI Layout.*")
        except:
            # 如果找不到，尝试查找产品编辑器窗口
            uilayout_window = app.window(title_re=".*产品编辑器.*")
        
        # 如果还是找不到，使用第一个窗口
        if not uilayout_window or not uilayout_window.exists():
            uilayout_window = app.windows()[0]
        
        print(f"选择的主窗口: {uilayout_window.window_text()}")
        time.sleep(2)  # 等待窗口完全可见
        print("主窗口已获取，当前主窗口即为UI布局编辑器")
    except Exception as e:
        print(f"获取主窗口失败: {e}")
        return False
    
    print("UI布局编辑器已准备就绪")
    
    # 加载指定路径的mainwindow.ui文件
    print("加载mainwindow.ui文件...")
    simple_ui_path = "D:\\virtualMachine\\github\\software-editor\\src\\product\\mainwindow.ui"
    uilayout_window.set_focus()
    time.sleep(1)
    
    # 点击文件菜单
    print("点击文件菜单...")
    pyautogui.hotkey('alt', 'f')  # Alt+F打开文件菜单
    time.sleep(1)
    
    # 点击加载选项
    print("点击加载选项...")
    pyautogui.press('l')  # 假设加载选项的快捷键是L
    time.sleep(2)
    
    # 输入UI文件路径
    file_path = simple_ui_path
    pyautogui.typewrite(file_path)
    time.sleep(1)
    
    # 按回车确认打开
    pyautogui.press('enter')
    time.sleep(5)  # 增加等待时间确保UI完全加载
    
    # 点击编辑区中心并刷新
    print("尝试刷新编辑区...")
    pyautogui.click(x=400, y=300)  # 点击编辑区中心位置
    time.sleep(1)
    pyautogui.hotkey('f5')  # 按F5刷新界面
    time.sleep(2)
    
    print(f"mainwindow.ui文件加载完成，已在编辑区显示整个主窗口UI")
    
    # 保存编辑区内容到临时文件
    print("保存编辑区内容到临时文件...")
    pyautogui.hotkey('ctrl', 's')
    time.sleep(3)  # 增加等待时间确保保存对话框弹出
    
    # 输入保存路径
    save_path = "D:\\virtualMachine\\github\\software-editor-08\\test\\ui_automation\\temp_test_save.ui"
    pyautogui.typewrite(save_path)
    time.sleep(2)  # 增加等待时间确保路径输入完成
    pyautogui.press('enter')
    time.sleep(4)  # 增加等待时间确保文件保存完成
    
    # 对比保存的文件和原文件
    print("对比保存的文件和原文件内容...")
    import filecmp
    import difflib
    import os
     
    original_path = simple_ui_path
    saved_path = save_path
     
    # 检查保存的文件是否存在
    if not os.path.exists(saved_path):
        print(f"❌ 错误：保存的文件 {saved_path} 不存在")
        print("可能是保存操作失败，请检查软件编辑器的保存功能是否正常")
        return False
     
    # 读取两个文件内容
    with open(original_path, 'r', encoding='utf-8') as f1, open(saved_path, 'r', encoding='utf-8') as f2:
        original_content = f1.read()
        saved_content = f2.read()
     
    # 忽略空白字符差异进行比较
    def normalize_content(content):
        # 移除多余空格和换行
        import re
        content = re.sub(r'\s+', ' ', content)
        content = content.strip()
        return content
     
    original_normalized = normalize_content(original_content)
    saved_normalized = normalize_content(saved_content)
     
    if original_normalized == saved_normalized:
        print("✅ 加载成功！保存的文件与原UI文件内容一致")
        result = True
    else:
        print("❌ 加载失败！保存的文件与原UI文件内容存在差异")
        # 显示差异
        diff = difflib.unified_diff(original_content.splitlines(), saved_content.splitlines(), lineterm='')
        print("差异详情:")
        for line in list(diff)[:20]:  # 只显示前20行差异
            print(f"  {line}")
        result = False
    
    # 拍摄截图记录UI效果
    screenshot_path = take_screenshot(f"mainwindow_ui_loaded_{int(time.time())}")
    if screenshot_path:
        print(f"已保存UI加载效果截图: {screenshot_path}")
    
    print("=== UI布局加载完成 ===")
    return result

if __name__ == "__main__":
    load_mainwindow_ui()