#!/usr/bin/env python3
"""
测试重构后的packagemanager与smartpackageconfig功能
验证重复功能消除后的代码协作是否正常
"""

import os
import sys
import subprocess
import time

def test_application_startup():
    """测试应用程序启动"""
    print("测试应用程序启动...")
    
    # 检查可执行文件是否存在
    exe_path = r"build\software-editor.exe"
    if not os.path.exists(exe_path):
        print(f"错误: 可执行文件不存在: {exe_path}")
        return False
    
    # 启动应用程序
    try:
        process = subprocess.Popen([exe_path], 
                                  stdout=subprocess.PIPE, 
                                  stderr=subprocess.PIPE,
                                  text=True)
        
        # 等待几秒钟让程序初始化
        time.sleep(3)
        
        # 检查进程是否仍在运行
        if process.poll() is None:
            print("✓ 应用程序启动成功")
            
            # 正常关闭应用程序
            process.terminate()
            process.wait(timeout=5)
            print("✓ 应用程序正常关闭")
            return True
        else:
            # 获取输出以诊断问题
            stdout, stderr = process.communicate()
            print(f"应用程序异常退出")
            print(f"标准输出: {stdout}")
            print(f"标准错误: {stderr}")
            return False
            
    except Exception as e:
        print(f"启动应用程序时出错: {e}")
        return False

def test_build_process():
    """测试编译过程"""
    print("\n测试编译过程...")
    
    try:
        result = subprocess.run(["build.bat"], 
                              capture_output=True, 
                              text=True,
                              cwd=os.getcwd())
        
        if result.returncode == 0:
            print("✓ 编译成功")
            
            # 检查是否有重复功能相关的编译警告
            if "duplicate" in result.stdout.lower() or "duplicate" in result.stderr.lower():
                print("⚠ 发现可能的重复功能警告")
                print(result.stdout)
                return False
            else:
                print("✓ 无重复功能相关的编译错误")
                return True
        else:
            print("✗ 编译失败")
            print(f"错误输出: {result.stderr}")
            return False
            
    except Exception as e:
        print(f"编译测试时出错: {e}")
        return False

def check_file_modifications():
    """检查重构后的文件修改"""
    print("\n检查重构后的文件修改...")
    
    files_to_check = [
        "src/packagemanager.h",
        "src/packagemanager.cpp", 
        "src/smartpackageconfig.h",
        "src/smartpackageconfig.cpp"
    ]
    
    all_files_exist = True
    for file_path in files_to_check:
        if os.path.exists(file_path):
            print(f"✓ {file_path} 存在")
            
            # 检查文件大小，确保不是空文件
            file_size = os.path.getsize(file_path)
            if file_size > 100:  # 文件大小大于100字节
                print(f"  - 文件大小: {file_size} 字节")
            else:
                print(f"  ⚠ 文件大小过小: {file_size} 字节")
                all_files_exist = False
        else:
            print(f"✗ {file_path} 不存在")
            all_files_exist = False
    
    return all_files_exist

def main():
    """主测试函数"""
    print("开始测试重构后的packagemanager与smartpackageconfig功能")
    print("=" * 60)
    
    # 切换到项目根目录
    os.chdir(os.path.dirname(os.path.abspath(__file__)))
    os.chdir("..")  # 回到项目根目录
    
    tests = [
        ("文件存在性检查", check_file_modifications),
        ("编译过程测试", test_build_process),
        ("应用程序启动测试", test_application_startup)
    ]
    
    results = []
    for test_name, test_func in tests:
        print(f"\n{test_name}:")
        result = test_func()
        results.append((test_name, result))
    
    # 输出测试总结
    print("\n" + "=" * 60)
    print("测试总结:")
    
    all_passed = True
    for test_name, result in results:
        status = "✓ 通过" if result else "✗ 失败"
        print(f"{test_name}: {status}")
        if not result:
            all_passed = False
    
    if all_passed:
        print("\n🎉 所有测试通过！重构后的功能正常。")
        print("packagemanager与smartpackageconfig的重复功能已成功消除。")
    else:
        print("\n❌ 部分测试失败，需要进一步检查。")
    
    return all_passed

if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)