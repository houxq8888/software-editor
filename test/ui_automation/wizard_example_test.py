#!/usr/bin/env python3
"""
向导示例程序测试脚本

该脚本用于测试向导示例程序的功能，特别是验证修复后的向导查找功能是否正常工作。
"""

import subprocess
import time
import os
import sys

# 设置工作目录
def set_working_directory():
    """设置正确的工作目录"""
    # 获取当前脚本所在目录
    script_dir = os.path.dirname(os.path.abspath(__file__))
    # 切换到项目根目录
    project_root = os.path.abspath(os.path.join(script_dir, "..", ".."))
    os.chdir(project_root)
    print(f"工作目录已设置为: {os.getcwd()}")

# 测试向导示例程序
def test_wizard_example():
    """测试向导示例程序"""
    print("\n=== 测试向导示例程序 ===")
    
    # 检查向导示例程序是否存在
    wizard_example_path = ".\\build\\wizardexample.exe"
    if not os.path.exists(wizard_example_path):
        print(f"❌ 错误: 向导示例程序不存在于路径 {wizard_example_path}")
        print("请先运行 build.bat 编译程序")
        return False
    
    print(f"✅ 向导示例程序存在于路径 {wizard_example_path}")
    
    # 运行向导示例程序
    print("\n正在启动向导示例程序...")
    process = None
    try:
        process = subprocess.Popen([wizard_example_path], 
                                 stdout=subprocess.PIPE, 
                                 stderr=subprocess.PIPE, 
                                 text=True)
        
        # 等待程序启动
        time.sleep(3)
        
        # 检查程序是否在运行
        if process.poll() is None:
            print("✅ 向导示例程序已成功启动")
            print("\n=== 测试说明 ===")
            print("1. 向导示例程序窗口应该已经打开")
            print("2. 窗口中有一个'启动向导'按钮")
            print("3. 点击该按钮，应该显示'向导已启动'而不是'未找到向导'")
            print("\n=== 验证结果 ===")
            print("请手动点击'启动向导'按钮，检查是否能正常启动向导")
            print("如果向导能正常启动，则修复成功")
            
            # 等待用户输入
            input("\n按Enter键关闭程序并继续...")
            
            # 关闭程序
            process.terminate()
            try:
                process.wait(timeout=5)
                print("✅ 程序已正常关闭")
            except subprocess.TimeoutExpired:
                process.kill()
                print("⚠️  程序已强制关闭")
            
            return True
        else:
            stdout, stderr = process.communicate()
            print(f"❌ 向导示例程序启动失败，退出码: {process.poll()}")
            if stderr:
                print(f"错误输出: {stderr}")
            if stdout:
                print(f"标准输出: {stdout}")
            return False
            
    except Exception as e:
        print(f"❌ 运行向导示例程序时发生错误: {e}")
        if process:
            try:
                process.terminate()
                process.wait(timeout=5)
            except:
                pass
        return False

# 主函数
def main():
    """主函数"""
    print("向导功能自动化测试脚本")
    print("=" * 50)
    
    # 设置工作目录
    set_working_directory()
    
    # 执行测试
    success = test_wizard_example()
    
    print("\n" + "=" * 50)
    if success:
        print("🎉 测试完成，向导功能修复成功！")
        return 0
    else:
        print("❌ 测试完成，向导功能修复失败！")
        return 1

if __name__ == "__main__":
    sys.exit(main())
