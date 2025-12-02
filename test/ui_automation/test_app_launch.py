"""
测试应用程序基本启动功能
"""

import os
import time
import subprocess
from pywinauto import Application

def test_app_launch():
    """测试应用程序启动"""
    
    # 应用程序路径
    app_path = os.path.join(os.path.dirname(os.path.dirname(os.path.dirname(__file__))), "build", "software-editor.exe")
    
    if not os.path.exists(app_path):
        print(f"[ERROR] 应用程序不存在: {app_path}")
        return False
    
    print(f"应用程序路径: {app_path}")
    
    # 使用subprocess启动应用程序
    print("=== 启动软件编辑器 ===")
    process = subprocess.Popen([app_path])
    time.sleep(10)  # 等待应用程序完全启动
    
    # 尝试连接到应用程序
    try:
        print("=== 尝试连接应用程序 ===")
        app = Application().connect(path=app_path)
        print("[SUCCESS] 通过路径连接成功")
    except Exception as e:
        print(f"[WARNING] 路径连接失败: {e}")
        
        # 尝试通过窗口标题连接
        try:
            app = Application().connect(title_re=".*软件产品编辑界面.*")
            print("[SUCCESS] 通过窗口标题连接成功")
        except Exception as e2:
            print(f"[ERROR] 窗口标题连接失败: {e2}")
            
            # 尝试列出所有窗口
            try:
                print("=== 列出所有窗口 ===")
                windows = Application().windows()
                for window in windows:
                    print(f"窗口标题: {window.window_text()}")
            except Exception as e3:
                print(f"[ERROR] 列出窗口失败: {e3}")
            
            process.terminate()
            return False
    
    # 获取主窗口
    try:
        # 先获取所有窗口信息
        print("=== 获取所有窗口信息 ===")
        windows = app.windows()
        for i, window in enumerate(windows):
            print(f"窗口{i+1}: 标题='{window.window_text()}', 类名='{window.class_name()}'")
        
        # 尝试匹配窗口
        dlg = None
        for window in windows:
            if "软件" in window.window_text() or "编辑" in window.window_text() or "产品" in window.window_text():
                dlg = window
                break
        
        if dlg is None and len(windows) > 0:
            dlg = windows[0]  # 使用第一个窗口
        
        if dlg is None:
            print("[ERROR] 未找到合适的窗口")
            process.terminate()
            return False
            
        print(f"主窗口标题: {dlg.window_text()}")
        
        # 检查窗口是否可见
        if dlg.is_visible():
            print("[SUCCESS] 主窗口可见")
        else:
            print("[WARNING] 主窗口不可见")
        
        # 获取窗口控件信息
        print("=== 窗口控件信息 ===")
        try:
            children = dlg.children()
            print(f"子控件数量: {len(children)}")
            for i, child in enumerate(children[:5]):  # 只显示前5个
                print(f"控件{i+1}: {child.window_text()} - {child.class_name()}")
        except Exception as e:
            print(f"[ERROR] 获取控件信息失败: {e}")
        
        # 关闭应用程序
        print("=== 关闭应用程序 ===")
        dlg.close()
        time.sleep(2)
        
        return True
        
    except Exception as e:
        print(f"[ERROR] 处理主窗口失败: {e}")
        process.terminate()
        return False

if __name__ == "__main__":
    success = test_app_launch()
    if success:
        print("\n=== 应用程序启动测试成功 ===")
    else:
        print("\n=== 应用程序启动测试失败 ===")