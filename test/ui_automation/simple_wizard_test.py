import os
import subprocess
import time

# 设置工作目录
os.chdir("d:\\virtualMachine\\github\\software-editor")

def test_wizard_example():
    """简单测试向导示例程序是否能正常启动"""
    print("=== 向导示例程序简单测试 ===")
    
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
    time.sleep(2)
    
    try:
        # 检查进程是否正在运行
        if process.poll() is None:
            print("✓ 程序已成功启动并正在运行")
            
            # 等待一段时间让用户查看
            print("\n=== 测试结果 ===")
            print("✅ 向导示例程序已成功编译并启动")
            print("\n功能说明：")
            print("1. 主窗口标题：向导功能示例")
            print("2. 包含'启动向导'按钮")
            print("3. 点击'启动向导'按钮后会显示向导页面")
            print("\n请手动测试以下功能：")
            print("- 点击'启动向导'按钮，确认向导页面显示")
            print("- 测试页面导航功能（如果有导航按钮）")
            print("\n测试完成后请手动关闭程序")
            
            return True
        else:
            print(f"✗ 错误：程序已退出，退出码：{process.poll()}")
            return False
            
    except Exception as e:
        print(f"✗ 测试过程中发生错误：{str(e)}")
        return False

if __name__ == "__main__":
    test_wizard_example()