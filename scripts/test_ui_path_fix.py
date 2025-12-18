#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试UI文件路径修复效果
"""

import json
import os

def test_ui_file_paths():
    """测试UI文件路径是否正确"""
    
    config_path = "product_configurations/product_12345/product_config.json"
    
    if not os.path.exists(config_path):
        print("❌ 配置文件不存在:", config_path)
        return False
    
    # 读取配置文件
    with open(config_path, 'r', encoding='utf-8') as f:
        config = json.load(f)
    
    print("🔍 检查UI文件路径修复效果...")
    print("=" * 60)
    
    # 检查UI文件路径
    ui_files = config.get("uiFiles", [])
    print(f"📄 UI文件数量: {len(ui_files)}")
    
    all_paths_correct = True
    
    for i, ui_file in enumerate(ui_files):
        file_name = ui_file.get("fileName", "")
        name = ui_file.get("name", "")
        
        print(f"\n📄 UI文件 {i+1}: {name}")
        print(f"   fileName: {file_name}")
        
        # 检查路径格式
        if file_name.startswith("../"):
            print("   ❌ 路径格式错误 - 包含 '../'")
            all_paths_correct = False
        elif file_name.startswith("ui_layouts/"):
            print("   ✅ 路径格式正确 - 使用相对路径")
            
            # 构建完整路径并检查文件是否存在
            full_path = os.path.join("product_configurations", "product_12345", file_name)
            if os.path.exists(full_path):
                print("   ✅ 文件存在")
            else:
                print("   ❌ 文件不存在")
                all_paths_correct = False
        else:
            print("   ⚠️  路径格式需要检查")
    
    print()
    print("=" * 60)
    
    if all_paths_correct:
        print("🎉 所有UI文件路径修复成功！")
        print("✅ 路径格式正确")
        print("✅ 所有文件都存在")
        print("✅ 程序应该能够正确加载UI文件")
        return True
    else:
        print("❌ 存在路径问题，需要进一步修复")
        return False

def verify_specific_file():
    """验证特定的1208.ui文件"""
    
    print("\n🔍 验证1208.ui文件路径...")
    
    # 构建正确的路径
    correct_path = "product_configurations/product_12345/ui_layouts/1208.ui"
    
    if os.path.exists(correct_path):
        print(f"✅ 文件存在: {correct_path}")
        
        # 检查文件大小
        file_size = os.path.getsize(correct_path)
        print(f"📊 文件大小: {file_size} 字节")
        
        return True
    else:
        print(f"❌ 文件不存在: {correct_path}")
        
        # 检查可能的错误路径
        wrong_path = "D:/virtualMachine/github/software-editor/../product_12345/ui_layouts/1208.ui"
        print(f"⚠️  之前错误的路径: {wrong_path}")
        
        return False

if __name__ == "__main__":
    print("开始测试UI文件路径修复效果...\n")
    
    # 测试所有UI文件路径
    test_result = test_ui_file_paths()
    
    # 验证特定文件
    verify_specific_file()
    
    print("\n" + "=" * 60)
    
    if test_result:
        print("🎉 路径修复测试通过！")
        print("💡 建议：重新启动软件测试UI文件加载功能")
    else:
        print("❌ 路径修复测试失败，需要进一步检查")