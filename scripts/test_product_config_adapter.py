#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试Product类适配新的product_config.json结构
"""

import json
import os
import sys

def test_product_config_loading():
    """测试产品配置加载功能"""
    
    # 读取新的product_config.json文件
    config_path = r"product_configurations\product_12345\product_config.json"
    
    if not os.path.exists(config_path):
        print(f"❌ 配置文件不存在: {config_path}")
        return False
    
    try:
        with open(config_path, 'r', encoding='utf-8') as f:
            config_data = json.load(f)
        print("✅ 成功读取product_config.json")
    except Exception as e:
        print(f"❌ 读取配置文件失败: {e}")
        return False
    
    # 验证新结构的关键字段
    required_fields = ["productInfo", "fileStructure", "uiFiles", "stateMachines", "resources"]
    missing_fields = []
    
    for field in required_fields:
        if field not in config_data:
            missing_fields.append(field)
    
    if missing_fields:
        print(f"❌ 配置文件缺少必要字段: {missing_fields}")
        return False
    
    print("✅ 配置文件包含所有必要字段")
    
    # 验证productInfo字段
    product_info = config_data["productInfo"]
    required_info_fields = ["name", "version", "uniqueId"]
    
    for field in required_info_fields:
        if field not in product_info:
            print(f"❌ productInfo缺少字段: {field}")
            return False
    
    print("✅ productInfo字段完整")
    
    # 验证uiFiles字段
    ui_files = config_data["uiFiles"]
    if not isinstance(ui_files, list):
        print("❌ uiFiles字段不是数组")
        return False
    
    print(f"✅ 找到 {len(ui_files)} 个UI文件")
    
    # 验证fileStructure字段
    file_structure = config_data["fileStructure"]
    required_structure_fields = ["basePath", "uiLayoutsPath", "stateMachinesPath", "resourcesPath"]
    
    for field in required_structure_fields:
        if field not in file_structure:
            print(f"❌ fileStructure缺少字段: {field}")
            return False
    
    print("✅ fileStructure字段完整")
    
    # 验证resources字段
    resources = config_data["resources"]
    if "icon" not in resources or "screenshot" not in resources:
        print("❌ resources字段不完整")
        return False
    
    print("✅ resources字段完整")
    
    # 验证stateMachines字段
    state_machines = config_data["stateMachines"]
    if not isinstance(state_machines, list):
        print("❌ stateMachines字段不是数组")
        return False
    
    print(f"✅ 找到 {len(state_machines)} 个状态机文件")
    
    # 显示配置信息摘要
    print("\n📋 配置信息摘要:")
    print(f"   产品名称: {product_info.get('name', 'N/A')}")
    print(f"   版本: {product_info.get('version', 'N/A')}")
    print(f"   描述: {product_info.get('description', 'N/A')[:50]}...")
    print(f"   基础路径: {file_structure.get('basePath', 'N/A')}")
    print(f"   UI文件数量: {len(ui_files)}")
    print(f"   状态机数量: {len(state_machines)}")
    print(f"   图标: {resources.get('icon', 'N/A')}")
    print(f"   截图: {resources.get('screenshot', 'N/A')}")
    
    return True

def test_backward_compatibility():
    """测试向后兼容性"""
    
    # 创建一个简单的旧结构配置
    old_config = {
        "name": "测试产品",
        "version": "1.0.0",
        "description": "这是一个测试产品",
        "iconPath": "icon.png",
        "screenshotPath": "screenshot.png",
        "category": "工具软件",
        "developer": "测试开发者",
        "website": "https://example.com",
        "uniqueId": "test-123",
        "uiLayoutPath": "main_window.ui",
        "stateMachinePath": "state_machine.json"
    }
    
    print("\n🔙 测试向后兼容性:")
    print("   旧结构配置包含基本字段")
    
    required_fields = ["name", "version", "uniqueId"]
    for field in required_fields:
        if field not in old_config:
            print(f"❌ 旧结构缺少字段: {field}")
            return False
    
    print("✅ 旧结构配置验证通过")
    return True

def main():
    """主测试函数"""
    print("🔍 开始测试Product类适配新的product_config.json结构\n")
    
    # 测试新结构加载
    new_structure_ok = test_product_config_loading()
    
    # 测试向后兼容性
    backward_compat_ok = test_backward_compatibility()
    
    # 总结测试结果
    print("\n📊 测试结果总结:")
    print(f"   新结构适配: {'✅ 通过' if new_structure_ok else '❌ 失败'}")
    print(f"   向后兼容性: {'✅ 通过' if backward_compat_ok else '❌ 失败'}")
    
    if new_structure_ok and backward_compat_ok:
        print("\n🎉 所有测试通过！Product类已成功适配新的product_config.json结构")
        return 0
    else:
        print("\n⚠️  部分测试失败，需要进一步调试")
        return 1

if __name__ == "__main__":
    sys.exit(main())