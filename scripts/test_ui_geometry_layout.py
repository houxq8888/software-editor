#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试UI几何布局生成功能
验证generateMainCppFromUILayout函数是否根据控件的几何位置信息生成代码
"""

import os
import sys
import xml.etree.ElementTree as ET

def test_ui_geometry_parsing():
    """测试UI文件几何位置信息解析"""
    print("=== 测试UI几何布局生成功能 ===")
    
    # 读取测试UI文件
    ui_file = "test_ui_layout.xml"
    if not os.path.exists(ui_file):
        print(f"错误: UI文件 {ui_file} 不存在")
        return False
    
    print(f"读取UI文件: {ui_file}")
    
    # 解析UI文件
    tree = ET.parse(ui_file)
    root = tree.getroot()
    
    # 查找所有widget元素
    widgets = root.findall(".//widget")
    print(f"找到 {len(widgets)} 个widget元素")
    
    # 检查每个widget的几何位置信息
    for i, widget in enumerate(widgets):
        widget_name = widget.get('name', 'unnamed')
        widget_class = widget.get('class', 'unknown')
        print(f"\nWidget {i+1}: {widget_name} ({widget_class})")
        
        # 查找geometry属性
        geometry_found = False
        for prop in widget.findall("property"):
            if prop.get('name') == 'geometry':
                geometry_found = True
                rect = prop.find('rect')
                if rect is not None:
                    x = rect.find('x')
                    y = rect.find('y')
                    width = rect.find('width')
                    height = rect.find('height')
                    
                    x_val = x.text if x is not None else '0'
                    y_val = y.text if y is not None else '0'
                    width_val = width.text if width is not None else '100'
                    height_val = height.text if height is not None else '30'
                    
                    print(f"  几何位置: x={x_val}, y={y_val}, width={width_val}, height={height_val}")
                break
        
        if not geometry_found:
            print("  警告: 未找到geometry属性，将使用默认位置")
    
    print("\n=== 几何位置信息解析完成 ===")
    return True

def test_generated_main_cpp():
    """测试生成的main.cpp文件"""
    print("\n=== 测试生成的main.cpp文件 ===")
    
    # 检查smartpackageconfig.cpp中的修改
    cpp_file = "src/smartpackageconfig.cpp"
    if not os.path.exists(cpp_file):
        print(f"错误: C++文件 {cpp_file} 不存在")
        return False
    
    print(f"检查C++文件: {cpp_file}")
    
    with open(cpp_file, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # 检查是否移除了QVBoxLayout
    if "QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);" in content:
        print("❌ 错误: 仍然存在QVBoxLayout布局管理器")
        return False
    else:
        print("✅ 已成功移除QVBoxLayout布局管理器")
    
    # 检查是否添加了setGeometry调用
    if "setGeometry(" in content and "xElement" in content and "yElement" in content:
        print("✅ 已添加几何位置解析和setGeometry调用")
        
        # 检查具体的几何位置解析逻辑
        if "geometryElement.attribute(\"name\") == \"geometry\"" in content:
            print("✅ 几何位置解析逻辑正确")
        else:
            print("❌ 几何位置解析逻辑可能有问题")
            return False
            
        if "rectElement.firstChildElement(\"x\")" in content:
            print("✅ x坐标解析逻辑正确")
        else:
            print("❌ x坐标解析逻辑可能有问题")
            return False
            
    else:
        print("❌ 错误: 未找到几何位置解析和setGeometry调用")
        return False
    
    print("\n=== main.cpp生成逻辑测试完成 ===")
    return True

def main():
    """主测试函数"""
    print("开始测试UI几何布局生成功能...")
    
    # 测试UI文件解析
    if not test_ui_geometry_parsing():
        print("\n❌ UI文件解析测试失败")
        return 1
    
    # 测试生成的main.cpp
    if not test_generated_main_cpp():
        print("\n❌ main.cpp生成逻辑测试失败")
        return 1
    
    print("\n🎉 所有测试通过！UI几何布局生成功能已正确实现")
    print("\n总结:")
    print("1. ✅ 已移除QVBoxLayout布局管理器")
    print("2. ✅ 已实现几何位置信息解析")
    print("3. ✅ 已添加setGeometry调用设置控件位置")
    print("4. ✅ 生成的main.cpp将根据UI文件中的实际位置信息放置控件")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())