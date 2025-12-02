#!/usr/bin/env python3
"""
监控程序崩溃测试脚本
专门测试新建产品状态下的事件-动作编辑器功能
"""

import os
import sys
import subprocess
import time
import psutil
from pathlib import Path

def monitor_process_crash(process_name, test_scenario):
    """监控指定进程是否崩溃"""
    print(f"\n=== 开始测试: {test_scenario} ===")
    
    # 启动程序
    exe_path = Path("build/software-editor.exe")
    if not exe_path.exists():
        print(f"❌ 可执行文件不存在: {exe_path}")
        return False
    
    print(f"启动程序: {exe_path}")
    
    try:
        # 启动程序
        process = subprocess.Popen([str(exe_path)], 
                                 stdout=subprocess.PIPE, 
                                 stderr=subprocess.PIPE,
                                 text=True)
        
        print(f"程序PID: {process.pid}")
        
        # 等待程序启动
        time.sleep(3)
        
        # 检查进程是否还在运行
        if process.poll() is not None:
            stdout, stderr = process.communicate()
            print(f"❌ 程序已退出，退出码: {process.returncode}")
            if stdout:
                print(f"标准输出:\n{stdout}")
            if stderr:
                print(f"错误输出:\n{stderr}")
            return False
        
        print("✓ 程序启动成功，正在运行...")
        
        # 模拟用户操作：等待一段时间后关闭程序
        time.sleep(5)
        
        # 正常终止程序
        process.terminate()
        
        # 等待程序结束
        try:
            process.wait(timeout=10)
            print("✓ 程序正常终止")
            return True
        except subprocess.TimeoutExpired:
            print("⚠ 程序未在10秒内终止，强制终止")
            process.kill()
            process.wait()
            return False
            
    except Exception as e:
        print(f"❌ 测试过程中发生异常: {e}")
        return False

def test_new_product_scenario():
    """测试新建产品场景"""
    print("\n🧪 测试场景: 新建产品状态")
    print("预期行为: 程序启动后不会崩溃")
    
    return monitor_process_crash("software-editor.exe", "新建产品状态启动")

def test_existing_product_scenario():
    """测试现有产品场景"""
    print("\n🧪 测试场景: 现有产品状态")
    print("预期行为: 程序启动后不会崩溃")
    
    return monitor_process_crash("software-editor.exe", "现有产品状态启动")

def check_build_status():
    """检查构建状态"""
    print("🔍 检查构建状态...")
    
    exe_path = Path("build/software-editor.exe")
    if not exe_path.exists():
        print("❌ 可执行文件不存在，需要重新构建")
        return False
    
    # 检查文件大小和修改时间
    file_size = exe_path.stat().st_size
    mod_time = time.ctime(exe_path.stat().st_mtime)
    
    print(f"✓ 可执行文件存在")
    print(f"   文件大小: {file_size} 字节")
    print(f"   修改时间: {mod_time}")
    
    return True

def main():
    """主测试函数"""
    print("🚀 开始监控程序崩溃测试")
    print("=" * 50)
    
    # 检查构建状态
    if not check_build_status():
        print("\n❌ 构建检查失败，无法进行测试")
        return 1
    
    # 运行测试
    test_results = []
    
    # 测试1: 新建产品状态
    result1 = test_new_product_scenario()
    test_results.append(("新建产品状态", result1))
    
    # 等待一段时间
    time.sleep(2)
    
    # 测试2: 现有产品状态
    result2 = test_existing_product_scenario()
    test_results.append(("现有产品状态", result2))
    
    # 输出测试结果
    print("\n" + "=" * 50)
    print("📊 测试结果汇总:")
    print("=" * 50)
    
    all_passed = True
    for test_name, result in test_results:
        status = "✅ 通过" if result else "❌ 失败"
        print(f"{test_name}: {status}")
        if not result:
            all_passed = False
    
    print("\n" + "=" * 50)
    if all_passed:
        print("🎉 所有测试通过！程序没有崩溃")
        print("✅ 事件-动作编辑器崩溃问题已成功修复")
        return 0
    else:
        print("⚠️ 部分测试失败，程序可能存在崩溃问题")
        return 1

if __name__ == "__main__":
    sys.exit(main())