#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试打包时机问题 - 验证点击"自动检测工具"后是否立即开始打包
"""

import os
import sys
import re

def analyze_package_timing():
    """分析打包时机问题"""
    print("=== 打包时机问题分析 ===")
    
    # 读取packagemanager.cpp文件
    with open('src/packagemanager.cpp', 'r', encoding='utf-8') as f:
        content = f.read()
    
    # 检查onDetectToolsClicked函数
    print("\n1. 检查onDetectToolsClicked函数:")
    if 'void PackageManager::onDetectToolsClicked()' in content:
        print("   ✓ 找到onDetectToolsClicked函数")
        
        # 检查函数内容
        pattern = r'void PackageManager::onDetectToolsClicked\(\)\s*\{([^}]+)\}'
        match = re.search(pattern, content, re.DOTALL)
        if match:
            func_content = match.group(1)
            
            # 检查是否调用了executePackage或startPackage
            if 'executePackage' in func_content:
                print("   ✗ 问题: onDetectToolsClicked函数中调用了executePackage")
                print("     这会导致点击'自动检测工具'后立即开始打包")
                return False
            elif 'startPackage' in func_content:
                print("   ✗ 问题: onDetectToolsClicked函数中调用了startPackage")
                print("     这会导致点击'自动检测工具'后立即开始打包")
                return False
            else:
                print("   ✓ onDetectToolsClicked函数中没有调用打包函数")
                print("     函数只负责检测工具路径，不会自动开始打包")
    else:
        print("   ✗ 未找到onDetectToolsClicked函数")
        return False
    
    # 检查onPackageClicked函数
    print("\n2. 检查onPackageClicked函数:")
    if 'void PackageManager::onPackageClicked()' in content:
        print("   ✓ 找到onPackageClicked函数")
        
        # 检查函数内容
        pattern = r'void PackageManager::onPackageClicked\(\)\s*\{([^}]+)\}'
        match = re.search(pattern, content, re.DOTALL)
        if match:
            func_content = match.group(1)
            
            # 检查是否调用了executePackage
            if 'executePackage' in func_content:
                print("   ✓ onPackageClicked函数中调用了executePackage")
                print("     这是正确的，打包只在用户点击'开始打包'按钮时启动")
            else:
                print("   ✗ 问题: onPackageClicked函数中没有调用executePackage")
                print("     这会导致'开始打包'按钮无法正常工作")
                return False
    else:
        print("   ✗ 未找到onPackageClicked函数")
        return False
    
    # 检查信号连接
    print("\n3. 检查信号连接:")
    if 'connect(m_detectToolsButton, &QPushButton::clicked, this, &PackageManager::onDetectToolsClicked)' in content:
        print("   ✓ 自动检测工具按钮正确连接到onDetectToolsClicked函数")
    else:
        print("   ✗ 自动检测工具按钮连接不正确")
        return False
        
    if 'connect(m_packageButton, &QPushButton::clicked, this, &PackageManager::onPackageClicked)' in content:
        print("   ✓ 开始打包按钮正确连接到onPackageClicked函数")
    else:
        print("   ✗ 开始打包按钮连接不正确")
        return False
    
    # 检查mainwindow.cpp中的智能打包触发逻辑
    print("\n4. 检查mainwindow.cpp中的智能打包触发逻辑:")
    with open('src/mainwindow.cpp', 'r', encoding='utf-8') as f:
        main_content = f.read()
    
    if 'void MainWindow::on_actionSmart_Package_Software_triggered()' in main_content:
        print("   ✓ 找到智能打包触发函数")
        
        # 检查是否在检测工具后立即开始打包
        pattern = r'void MainWindow::on_actionSmart_Package_Software_triggered\(\)\s*\{([^}]+)\}'
        match = re.search(pattern, main_content, re.DOTALL)
        if match:
            func_content = match.group(1)
            
            # 检查是否调用了startPackage或executePackage
            if 'startPackage' in func_content or 'executePackage' in func_content:
                print("   ✗ 问题: 智能打包触发函数中直接调用了打包函数")
                print("     这会导致打开打包界面后立即开始打包")
                return False
            else:
                print("   ✓ 智能打包触发函数中只显示打包对话框，不会自动开始打包")
    else:
        print("   ✗ 未找到智能打包触发函数")
        return False
    
    print("\n=== 分析结果 ===")
    print("✓ 代码逻辑正确：")
    print("  - 点击'自动检测工具'只检测工具路径，不会开始打包")
    print("  - 打包只在用户点击'开始打包'按钮时启动")
    print("  - 智能打包界面打开后等待用户操作")
    print("")
    print("如果实际运行中出现问题，可能是以下原因：")
    print("  1. 界面控件连接错误")
    print("  2. 信号/槽连接问题")
    print("  3. 运行时状态管理错误")
    
    return True

def check_ui_connections():
    """检查UI连接是否正确"""
    print("\n=== UI连接检查 ===")
    
    # 读取packagemanager.cpp文件
    with open('src/packagemanager.cpp', 'r', encoding='utf-8') as f:
        content = f.read()
    
    # 检查setupConnections函数
    if 'void PackageManager::setupConnections()' in content:
        print("✓ 找到setupConnections函数")
        
        # 检查连接语句
        connections = [
            'connect(m_detectToolsButton, &QPushButton::clicked, this, &PackageManager::onDetectToolsClicked)',
            'connect(m_packageButton, &QPushButton::clicked, this, &PackageManager::onPackageClicked)'
        ]
        
        for conn in connections:
            if conn in content:
                print(f"✓ {conn.split(',')[1].strip()}连接正确")
            else:
                print(f"✗ {conn.split(',')[1].strip()}连接缺失")
                return False
    else:
        print("✗ 未找到setupConnections函数")
        return False
    
    return True

def main():
    """主函数"""
    print("打包时机问题分析工具")
    print("=" * 50)
    
    # 切换到项目根目录
    os.chdir(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    
    # 分析打包时机
    timing_ok = analyze_package_timing()
    
    # 检查UI连接
    ui_ok = check_ui_connections()
    
    print("\n" + "=" * 50)
    if timing_ok and ui_ok:
        print("✓ 代码分析完成，打包时机逻辑正确")
        print("\n建议：")
        print("  1. 如果实际运行中仍有问题，请检查运行时状态")
        print("  2. 确保界面控件正确初始化")
        print("  3. 检查是否有其他代码干扰打包流程")
    else:
        print("✗ 发现潜在问题，请检查上述错误信息")
    
    return timing_ok and ui_ok

if __name__ == "__main__":
    if main():
        sys.exit(0)
    else:
        sys.exit(1)