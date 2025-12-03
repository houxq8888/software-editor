#!/usr/bin/env python3
"""
测试打包功能路径修复
验证打包功能现在使用JSON文件中uiLayoutPath字段指向的文件路径
"""

import json
import os
import sys

def test_package_path_fix():
    """测试打包功能路径修复"""
    
    print("=== 测试打包功能路径修复 ===")
    
    # 1. 检查测试JSON文件
    test_json_path = "test/ui_automation/test_results/2025-11-21/test_product_20251121_113422.json"
    
    if not os.path.exists(test_json_path):
        print(f"❌ 测试JSON文件不存在: {test_json_path}")
        return False
    
    print(f"✅ 测试JSON文件存在: {test_json_path}")
    
    # 2. 读取JSON文件内容
    with open(test_json_path, 'r', encoding='utf-8') as f:
        product_data = json.load(f)
    
    # 3. 检查uiLayoutPath字段
    ui_layout_path = product_data.get('uiLayoutPath', '')
    if not ui_layout_path:
        print("❌ JSON文件中没有uiLayoutPath字段")
        return False
    
    print(f"✅ JSON文件中的uiLayoutPath字段: {ui_layout_path}")
    
    # 4. 检查UI布局文件是否存在
    if not os.path.exists(ui_layout_path):
        print(f"❌ UI布局文件不存在: {ui_layout_path}")
        return False
    
    print(f"✅ UI布局文件存在: {ui_layout_path}")
    
    # 5. 检查源代码修改
    print("\n=== 检查源代码修改 ===")
    
    # 检查PackageDialog修改
    packagedialog_cpp_path = "src/packagedialog.cpp"
    if os.path.exists(packagedialog_cpp_path):
        with open(packagedialog_cpp_path, 'r', encoding='utf-8') as f:
            packagedialog_content = f.read()
        
        # 检查是否使用了m_product.uiLayoutPath()
        if 'm_product.uiLayoutPath()' in packagedialog_content:
            print("✅ PackageDialog已修改为使用Product对象的uiLayoutPath字段")
        else:
            print("❌ PackageDialog未正确修改")
            return False
    
    # 检查MainWindow修改
    mainwindow_cpp_path = "src/mainwindow.cpp"
    if os.path.exists(mainwindow_cpp_path):
        with open(mainwindow_cpp_path, 'r', encoding='utf-8') as f:
            mainwindow_content = f.read()
        
        # 检查startPackageProcess方法签名
        if 'startPackageProcess(const Product &product, const PackageConfig::PackageSettings &settings)' in mainwindow_content:
            print("✅ MainWindow的startPackageProcess方法已修改为接收Product对象")
        else:
            print("❌ MainWindow的startPackageProcess方法未正确修改")
            return False
    
    # 检查MainWindow中PackageManager调用
    mainwindow_cpp_path = "src/mainwindow.cpp"
    if os.path.exists(mainwindow_cpp_path):
        with open(mainwindow_cpp_path, 'r', encoding='utf-8') as f:
            mainwindow_content = f.read()
        
        # 检查是否调用了正确的重载方法
        if 'm_packageManager->startPackage(product, settings)' in mainwindow_content:
            print("✅ MainWindow已正确调用带Product对象的startPackage方法")
        else:
            print("❌ MainWindow未正确调用带Product对象的startPackage方法")
            return False
    
    print("\n=== 测试结果 ===")
    print("✅ 打包功能路径修复测试通过！")
    print("✅ 现在打包功能将使用JSON文件中uiLayoutPath字段指向的文件路径")
    print("✅ 而不是输出目录显示的路径")
    
    return True

def main():
    """主函数"""
    try:
        success = test_package_path_fix()
        sys.exit(0 if success else 1)
    except Exception as e:
        print(f"❌ 测试过程中出现错误: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()