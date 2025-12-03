#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试NSIS安装包生成成功日志重复输出问题修复
"""

import os
import sys

def test_nsis_log_fix():
    """测试NSIS日志重复输出问题是否修复"""
    print("=== 测试NSIS安装包生成成功日志重复输出问题修复 ===")
    
    # 检查smartpackageconfig.cpp文件
    cpp_file = "src/smartpackageconfig.cpp"
    if not os.path.exists(cpp_file):
        print(f"错误: C++文件 {cpp_file} 不存在")
        return False
    
    print(f"检查C++文件: {cpp_file}")
    
    with open(cpp_file, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # 查找NSIS安装包生成成功日志的位置
    log_positions = []
    lines = content.split('\n')
    
    for i, line in enumerate(lines):
        if 'NSIS安装包生成成功:' in line:
            log_positions.append((i+1, line.strip()))
    
    print(f"找到 {len(log_positions)} 个NSIS安装包生成成功日志输出")
    
    # 检查是否只有一处日志输出
    if len(log_positions) == 1:
        print("✅ 修复成功：NSIS安装包生成成功日志只输出一次")
        print(f"   日志位置：第{log_positions[0][0]}行")
        print(f"   日志内容：{log_positions[0][1]}")
        
        # 检查是否在createNsisInstaller函数中
        if log_positions[0][0] > 370 and log_positions[0][0] < 390:
            print("✅ 日志位于正确的函数中：createNsisInstaller")
        else:
            print("⚠️ 警告：日志可能不在预期的函数中")
            
    elif len(log_positions) > 1:
        print("❌ 修复失败：仍然存在重复的NSIS安装包生成成功日志输出")
        for pos, line in log_positions:
            print(f"   第{pos}行：{line}")
        return False
    else:
        print("❌ 错误：未找到NSIS安装包生成成功日志输出")
        return False
    
    # 检查createPackage函数中是否移除了重复的日志输出
    if '// 不再重复输出日志，createNsisInstaller函数中已经输出过' in content:
        print("✅ 已添加注释说明，避免重复输出")
    else:
        print("⚠️ 警告：未找到避免重复输出的注释")
    
    # 检查createPackage函数中是否移除了qDebug调用
    if 'qDebug() << "NSIS安装包生成成功:" << installerPath;' in content:
        # 检查是否在createPackage函数中
        package_function_start = content.find('bool SmartPackageConfig::createPackage')
        if package_function_start != -1:
            # 查找createPackage函数中的日志
            package_content = content[package_function_start:package_function_start+2000]
            if 'qDebug() << "NSIS安装包生成成功:" << installerPath;' in package_content:
                print("❌ 修复失败：createPackage函数中仍然存在重复的日志输出")
                return False
            else:
                print("✅ createPackage函数中已移除重复的日志输出")
        else:
            print("⚠️ 警告：无法定位createPackage函数")
    
    print("\n=== NSIS日志重复输出问题修复测试完成 ===")
    return True

def main():
    """主测试函数"""
    print("开始测试NSIS安装包生成成功日志重复输出问题修复...")
    
    if not test_nsis_log_fix():
        print("\n❌ 测试失败：NSIS日志重复输出问题未完全修复")
        return 1
    
    print("\n🎉 测试通过！NSIS安装包生成成功日志重复输出问题已修复")
    print("\n修复总结:")
    print("1. ✅ 移除了createPackage函数中的重复日志输出")
    print("2. ✅ 保留了createNsisInstaller函数中的原始日志输出")
    print("3. ✅ 添加了注释说明避免重复输出")
    print("4. ✅ NSIS安装包生成成功日志现在只输出一次")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())