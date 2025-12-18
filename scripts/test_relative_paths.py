#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试UI文件路径是否为相对于产品目录的相对路径
"""

import json
import os
import sys

def test_relative_paths():
    """测试配置文件中的路径是否为相对路径"""
    
    config_path = "product_configurations/product_12345/product_config.json"
    
    if not os.path.exists(config_path):
        print("❌ 配置文件不存在:", config_path)
        return
    
    # 读取配置文件
    with open(config_path, 'r', encoding='utf-8') as f:
        config = json.load(f)
    
    print("🔍 检查配置文件中的路径格式...")
    print("=" * 60)
    
    # 检查fileStructure
    file_structure = config.get("fileStructure", {})
    base_path = file_structure.get("basePath", "")
    ui_layouts_path = file_structure.get("uiLayoutsPath", "")
    
    print(f"📁 basePath: {base_path}")
    print(f"📁 uiLayoutsPath: {ui_layouts_path}")
    
    # 检查是否为绝对路径
    if os.path.isabs(base_path):
        print("❌ basePath是绝对路径，应该是相对路径")
    else:
        print("✅ basePath是相对路径")
    
    print()
    
    # 检查UI文件路径
    ui_files = config.get("uiFiles", [])
    print(f"📄 UI文件数量: {len(ui_files)}")
    
    for i, ui_file in enumerate(ui_files):
        file_name = ui_file.get("fileName", "")
        name = ui_file.get("name", "")
        
        print(f"\n📄 UI文件 {i+1}: {name}")
        print(f"   fileName: {file_name}")
        
        # 检查路径格式
        if os.path.isabs(file_name):
            print("   ❌ 绝对路径 - 应该使用相对路径")
        elif file_name.startswith("ui_layouts/") or file_name.startswith("product_configurations/"):
            print("   ✅ 相对路径（包含目录结构）")
        else:
            print("   ⚠️  相对路径（可能需要检查路径结构）")
        
        # 检查路径分隔符
        if "\\" in file_name:
            print("   ⚠️  包含反斜杠分隔符 - 建议使用正斜杠")
        else:
            print("   ✅ 使用正斜杠分隔符")
    
    print()
    print("=" * 60)
    
    # 验证路径拼接
    print("🔧 验证路径拼接逻辑...")
    
    if base_path and ui_files:
        # 使用第一个UI文件进行测试
        test_file = ui_files[0]
        file_name = test_file.get("fileName", "")
        
        if file_name and not os.path.isabs(file_name):
            # 拼接完整路径
            full_path = os.path.join(base_path, file_name)
            absolute_path = os.path.abspath(full_path)
            
            print(f"📄 测试文件: {file_name}")
            print(f"📁 基础路径: {base_path}")
            print(f"🔗 拼接路径: {full_path}")
            print(f"🔗 绝对路径: {absolute_path}")
            
            # 检查文件是否存在
            if os.path.exists(absolute_path):
                print("✅ 文件存在")
            else:
                print("❌ 文件不存在 - 需要检查路径结构")
    
    print()
    print("=" * 60)
    
    # 建议修复
    print("💡 建议修复:")
    
    issues_found = False
    
    # 检查basePath
    if os.path.isabs(base_path):
        print("1. basePath应该是相对路径，如 'product_configurations/product_12345'")
        issues_found = True
    
    # 检查UI文件路径
    for ui_file in ui_files:
        file_name = ui_file.get("fileName", "")
        if os.path.isabs(file_name):
            print("2. UI文件fileName应该是相对于basePath的相对路径")
            issues_found = True
            break
    
    if not issues_found:
        print("✅ 路径格式正确，产品可以移动到任意位置")

if __name__ == "__main__":
    test_relative_paths()