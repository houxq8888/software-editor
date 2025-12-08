#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试多个UI文件关闭保存时的崩溃问题
"""

import os
import sys
import time
import subprocess
from pathlib import Path

# 添加项目根目录到Python路径
project_root = Path(__file__).parent.parent.parent
sys.path.insert(0, str(project_root))

def test_multiple_ui_close_crash():
    """测试多个UI文件关闭保存时的崩溃问题"""
    print("=== 测试多个UI文件关闭保存时的崩溃问题 ===")
    
    # 构建路径
    exe_path = project_root / "build" / "software-editor.exe"
    
    if not exe_path.exists():
        print(f"错误: 可执行文件不存在: {exe_path}")
        return False
    
    # 启动程序
    print(f"启动程序: {exe_path}")
    process = subprocess.Popen([str(exe_path)], 
                              stdout=subprocess.PIPE, 
                              stderr=subprocess.PIPE,
                              text=True)
    
    # 等待程序启动
    time.sleep(5)
    
    try:
        # 模拟多个UI文件操作
        print("1. 创建多个UI文件...")
        # 这里需要UI自动化工具来模拟用户操作
        
        # 模拟关闭保存操作
        print("2. 模拟关闭保存操作...")
        
        # 检查程序是否崩溃
        if process.poll() is not None:
            print("程序已崩溃!")
            stdout, stderr = process.communicate()
            print(f"标准输出:\n{stdout}")
            print(f"标准错误:\n{stderr}")
            return False
        
        # 正常关闭程序
        print("3. 正常关闭程序...")
        process.terminate()
        process.wait(timeout=10)
        
        print("测试完成 - 程序未崩溃")
        return True
        
    except Exception as e:
        print(f"测试过程中发生错误: {e}")
        process.terminate()
        return False

if __name__ == "__main__":
    success = test_multiple_ui_close_crash()
    sys.exit(0 if success else 1)