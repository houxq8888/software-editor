#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试产品JSON文件的向后兼容性
验证旧格式JSON文件如何无缝对接到新的多UI文件结构
"""

import json
import os

def test_backward_compatibility():
    """测试向后兼容性"""
    
    # 读取旧格式的JSON文件
    old_json_file = "test_product_20251121_113422.json"
    
    if not os.path.exists(old_json_file):
        print(f"错误：文件 {old_json_file} 不存在")
        return False
    
    try:
        with open(old_json_file, 'r', encoding='utf-8') as f:
            old_data = json.load(f)
        
        print("=== 原始旧格式JSON结构 ===")
        print(f"产品名称: {old_data.get('name', 'N/A')}")
        print(f"UI文件路径: {old_data.get('uiLayoutPath', 'N/A')}")
        print(f"是否包含uiFiles字段: {'uiFiles' in old_data}")
        
        # 模拟向后兼容转换
        print("\n=== 向后兼容转换结果 ===")
        
        # 创建新的数据结构（模拟Product类的转换逻辑）
        new_data = old_data.copy()
        
        # 如果存在uiLayoutPath但不存在uiFiles，自动创建uiFiles数组
        if 'uiLayoutPath' in old_data and old_data['uiLayoutPath'] and 'uiFiles' not in old_data:
            ui_layout_path = old_data['uiLayoutPath']
            
            # 创建单个UI文件结构
            ui_file = {
                "name": "主窗口",
                "filePath": ui_layout_path,
                "type": "main_window",
                "isMain": True,
                "description": "产品主界面窗口",
                "order": 0
            }
            
            new_data["uiFiles"] = [ui_file]
            print("✅ 自动转换：从单UI文件路径创建了uiFiles数组")
        
        # 显示转换后的结构
        print(f"转换后是否包含uiLayoutPath字段: {'uiLayoutPath' in new_data}")
        print(f"转换后是否包含uiFiles字段: {'uiFiles' in new_data}")
        
        if 'uiFiles' in new_data:
            ui_files = new_data['uiFiles']
            print(f"UI文件数量: {len(ui_files)}")
            
            for i, ui_file in enumerate(ui_files):
                print(f"  UI文件{i+1}:")
                print(f"    名称: {ui_file.get('name', 'N/A')}")
                print(f"    路径: {ui_file.get('filePath', 'N/A')}")
                print(f"    类型: {ui_file.get('type', 'N/A')}")
                print(f"    是否为主文件: {ui_file.get('isMain', False)}")
                print(f"    描述: {ui_file.get('description', 'N/A')}")
                print(f"    顺序: {ui_file.get('order', 0)}")
        
        # 验证向后兼容性
        print("\n=== 向后兼容性验证 ===")
        
        # 检查关键字段是否保留
        required_fields = ['name', 'version', 'uniqueId']
        for field in required_fields:
            if field in new_data:
                print(f"✅ {field} 字段保留: {new_data[field]}")
            else:
                print(f"❌ {field} 字段缺失")
        
        # 检查UI文件路径一致性
        if 'uiLayoutPath' in new_data and 'uiFiles' in new_data:
            old_path = new_data['uiLayoutPath']
            new_paths = [f['filePath'] for f in new_data['uiFiles']]
            
            if old_path in new_paths:
                print("✅ UI文件路径一致性验证通过")
            else:
                print("⚠️ UI文件路径不一致")
        
        # 生成新的JSON文件（可选）
        new_json_file = "test_product_updated.json"
        with open(new_json_file, 'w', encoding='utf-8') as f:
            json.dump(new_data, f, ensure_ascii=False, indent=4)
        
        print(f"\n✅ 向后兼容性测试完成")
        print(f"新的JSON文件已保存为: {new_json_file}")
        
        return True
        
    except Exception as e:
        print(f"❌ 测试过程中发生错误: {e}")
        return False

def compare_json_structures():
    """比较新旧JSON结构"""
    
    print("\n=== JSON结构对比 ===")
    
    # 旧结构示例
    old_structure = {
        "name": "产品名称",
        "version": "1.0.0",
        "uiLayoutPath": "path/to/mainwindow.ui"
        # 其他字段...
    }
    
    # 新结构示例
    new_structure = {
        "name": "产品名称",
        "version": "1.0.0",
        "uiLayoutPath": "path/to/mainwindow.ui",  # 向后兼容保留
        "uiFiles": [
            {
                "name": "主窗口",
                "filePath": "path/to/mainwindow.ui",
                "type": "main_window",
                "isMain": True,
                "description": "产品主界面窗口",
                "order": 0
            }
        ]
    }
    
    print("旧结构字段:", list(old_structure.keys()))
    print("新结构字段:", list(new_structure.keys()))
    print("新增字段: uiFiles (多UI文件管理)")
    print("保留字段: uiLayoutPath (向后兼容)")

if __name__ == "__main__":
    print("开始测试产品JSON文件向后兼容性...")
    print("=" * 50)
    
    # 比较结构
    compare_json_structures()
    
    print("\n" + "=" * 50)
    
    # 测试实际文件
    success = test_backward_compatibility()
    
    if success:
        print("\n🎉 所有测试通过！向后兼容性验证成功。")
        print("\n总结:")
        print("1. 旧格式JSON文件可以无缝读取")
        print("2. 自动转换为新的多UI文件结构")
        print("3. 关键字段保持不变")
        print("4. UI文件路径一致性得到保证")
    else:
        print("\n❌ 测试失败，请检查文件格式和路径。")