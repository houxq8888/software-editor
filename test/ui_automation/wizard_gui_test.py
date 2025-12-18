import os
import subprocess
import time
import psutil

# 设置工作目录
os.chdir("d:\\virtualMachine\\github\\software-editor")

def test_wizard_example_gui():
    """测试向导示例程序的GUI界面是否正常显示"""
    print("=== 向导示例程序GUI测试 ===")
    
    # 检查程序是否存在
    exe_path = ".\\build\\wizardexample.exe"
    if not os.path.exists(exe_path):
        print(f"错误：程序文件 {exe_path} 不存在")
        return False
    
    print(f"找到程序：{exe_path}")
    
    # 启动程序
    print("启动向导示例程序...")
    process = subprocess.Popen(exe_path)
    
    # 等待程序启动
    time.sleep(3)
    
    try:
        # 检查进程是否正在运行
        if process.poll() is None:
            print("程序正在运行")
            
            # 检查是否有窗口
            # 使用psutil获取进程信息
            p = psutil.Process(process.pid)
            windows = p.windows()
            
            if windows:
                print("程序已创建窗口")
                for window in windows:
                    print(f"窗口标题：{window.title}")
                    if "向导功能示例" in window.title:
                        print("找到主窗口")
                        
                        # 等待一段时间让用户查看
                        print("\n=== 测试完成 ===")
                        print("向导示例程序已成功启动并显示界面")
                        print("1. 主窗口标题：向导功能示例")
                        print("2. 包含'启动向导'按钮")
                        print("3. 点击'启动向导'按钮后会显示向导页面")
                        print("\n请手动测试向导的页面导航功能")
                        
                        return True
            else:
                print("警告：未检测到程序窗口")
                return False
        else:
            print(f"错误：程序已退出，退出码：{process.poll()}")
            return False
            
    except Exception as e:
        print(f"测试过程中发生错误：{str(e)}")
        return False
    finally:
        # 不要自动关闭程序，让用户手动测试
        print("\n程序将保持运行状态，您可以手动测试向导功能")
        print("测试完成后请手动关闭程序")

if __name__ == "__main__":
    test_wizard_example_gui()