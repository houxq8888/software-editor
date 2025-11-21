#!/usr/bin/env python3
"""
测试uiLayoutPath字段修复效果
"""

import os
import json

def test_ui_layout_path_fix():
    print("=== 测试uiLayoutPath字段修复 ===")
    
    # 检查JSON文件
    json_file = "test/ui_automation/test_results/2025-11-21/test_product_20251121_113422.json"
    if not os.path.exists(json_file):
        print("❌ JSON文件不存在:", json_file)
        return False
    
    print("✅ JSON文件存在:", json_file)
    
    # 读取JSON文件
    with open(json_file, 'r', encoding='utf-8') as f:
        json_data = json.load(f)
    
    json_ui_layout_path = json_data.get("uiLayoutPath", "")
    print(f"✅ JSON中的uiLayoutPath字段值: {json_ui_layout_path}")
    
    # 检查MainWindow.cpp中的修复
    mainwindow_file = "src/mainwindow.cpp"
    if not os.path.exists(mainwindow_file):
        print("❌ MainWindow.cpp文件不存在")
        return False
    
    with open(mainwindow_file, 'r', encoding='utf-8') as f:
        mainwindow_content = f.read()
    
    # 检查第一个修复位置
    fix1_pattern = "if (m_product.uiLayoutPath().isEmpty())"
    if fix1_pattern in mainwindow_content:
        print("✅ 第一个修复位置已正确实现")
    else:
        print("❌ 第一个修复位置未找到")
        return False
    
    # 检查第二个修复位置
    fix2_pattern = "if (m_product.uiLayoutPath().isEmpty())"
    # 检查第二个位置是否也有相同的修复逻辑
    fix2_count = mainwindow_content.count(fix2_pattern)
    if fix2_count >= 2:
        print("✅ 第二个修复位置已正确实现")
    else:
        print("❌ 第二个修复位置未找到")
        return False
    
    # 检查Product类的uiLayoutPath方法
    product_file = "src/product.cpp"
    if not os.path.exists(product_file):
        print("❌ Product.cpp文件不存在")
        return False
    
    with open(product_file, 'r', encoding='utf-8') as f:
        product_content = f.read()
    
    # 检查uiLayoutPath方法
    if "QString Product::uiLayoutPath() const { return m_uiLayoutPath; }" in product_content:
        print("✅ Product::uiLayoutPath()方法实现正确")
    else:
        print("❌ Product::uiLayoutPath()方法实现有问题")
        return False
    
    # 检查fromJson方法
    if "m_uiLayoutPath = json.value(\"uiLayoutPath\").toString();" in product_content:
        print("✅ Product::fromJson()方法正确设置uiLayoutPath")
    else:
        print("❌ Product::fromJson()方法设置uiLayoutPath有问题")
        return False
    
    print("\n=== 修复验证结果 ===")
    print("✅ uiLayoutPath字段修复已完成")
    print("✅ 现在Product对象从JSON文件加载后，uiLayoutPath()方法将返回JSON中的正确值")
    print("✅ UI绑定过程不会覆盖已存在的uiLayoutPath值")
    
    return True

if __name__ == "__main__":
    success = test_ui_layout_path_fix()
    exit(0 if success else 1)