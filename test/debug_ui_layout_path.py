#!/usr/bin/env python3
"""
调试脚本：检查Product对象uiLayoutPath字段的实际值
"""

import json
import os

def debug_ui_layout_path():
    """调试uiLayoutPath字段"""
    
    print("=== 调试uiLayoutPath字段 ===")
    
    # 1. 检查测试JSON文件
    test_json_path = "test/ui_automation/test_results/2025-11-21/test_product_20251121_113422.json"
    
    if not os.path.exists(test_json_path):
        print(f"❌ 测试JSON文件不存在: {test_json_path}")
        return False
    
    print(f"✅ 测试JSON文件存在: {test_json_path}")
    
    # 2. 读取JSON文件内容
    with open(test_json_path, 'r', encoding='utf-8') as f:
        product_data = json.load(f)
    
    # 3. 检查JSON文件中的uiLayoutPath字段
    ui_layout_path_json = product_data.get('uiLayoutPath', '')
    print(f"📋 JSON文件中的uiLayoutPath字段值: {ui_layout_path_json}")
    
    # 4. 检查Product.cpp中的fromJson方法实现
    product_cpp_path = "src/product.cpp"
    if os.path.exists(product_cpp_path):
        with open(product_cpp_path, 'r', encoding='utf-8') as f:
            product_content = f.read()
        
        # 检查fromJson方法中uiLayoutPath的设置
        if 'm_uiLayoutPath = json.value("uiLayoutPath").toString();' in product_content:
            print("✅ Product::fromJson方法正确设置了uiLayoutPath字段")
        else:
            print("❌ Product::fromJson方法可能没有正确设置uiLayoutPath字段")
    
    # 5. 检查MainWindow中加载产品数据的代码
    mainwindow_cpp_path = "src/mainwindow.cpp"
    if os.path.exists(mainwindow_cpp_path):
        with open(mainwindow_cpp_path, 'r', encoding='utf-8') as f:
            mainwindow_content = f.read()
        
        # 检查loadProductData方法
        if 'm_configManager->setUiLayoutPath(product.uiLayoutPath());' in mainwindow_content:
            print("✅ MainWindow::loadProductData正确调用了setUiLayoutPath")
        else:
            print("❌ MainWindow::loadProductData可能没有正确设置uiLayoutPath")
    
    # 6. 检查PackageDialog中uiLayoutPath的使用
    packagedialog_cpp_path = "src/packagedialog.cpp"
    if os.path.exists(packagedialog_cpp_path):
        with open(packagedialog_cpp_path, 'r', encoding='utf-8') as f:
            packagedialog_content = f.read()
        
        # 检查onStartPackage方法中uiLayoutPath的使用
        if 'QString uiLayoutPath = m_product.uiLayoutPath();' in packagedialog_content:
            print("✅ PackageDialog::onStartPackage正确使用了m_product.uiLayoutPath()")
        else:
            print("❌ PackageDialog::onStartPackage可能没有正确使用uiLayoutPath")
    
    print("\n=== 分析结果 ===")
    print("如果JSON文件中的uiLayoutPath字段值正确，但打包时使用的路径不正确，")
    print("问题可能出现在：")
    print("1. Product对象在加载后uiLayoutPath字段被意外修改")
    print("2. 打包过程中有其他代码覆盖了uiLayoutPath值")
    print("3. 调试信息显示路径不一致")
    
    return True

def main():
    """主函数"""
    try:
        debug_ui_layout_path()
    except Exception as e:
        print(f"❌ 调试过程中出现错误: {e}")

if __name__ == "__main__":
    main()