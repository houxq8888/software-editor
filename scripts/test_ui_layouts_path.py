#!/usr/bin/env python3
"""
测试UI文件路径是否包含ui_layouts目录
"""

import json
import os

def test_ui_layouts_path(config_path):
    """测试UI文件路径是否包含ui_layouts目录"""
    
    # 读取配置文件
    with open(config_path, 'r', encoding='utf-8') as f:
        config = json.load(f)
    
    print("测试UI文件路径是否包含ui_layouts目录...\n")
    
    # 检查uiFiles中的fileName字段
    if "uiFiles" in config:
        all_valid = True
        for ui_file in config["uiFiles"]:
            file_name = ui_file.get("fileName", "")
            
            # 检查是否包含ui_layouts路径
            if "ui_layouts" in file_name:
                print(f"✅ UI文件路径包含ui_layouts: {file_name}")
            else:
                print(f"❌ UI文件路径不包含ui_layouts: {file_name}")
                all_valid = False
        
        return all_valid
    
    return False

def fix_ui_layouts_path(config_path):
    """修正UI文件路径，确保包含ui_layouts目录"""
    
    # 读取配置文件
    with open(config_path, 'r', encoding='utf-8') as f:
        config = json.load(f)
    
    print("修正UI文件路径，添加ui_layouts目录...\n")
    
    # 修正uiFiles中的fileName字段
    if "uiFiles" in config:
        for ui_file in config["uiFiles"]:
            file_name = ui_file.get("fileName", "")
            
            # 如果路径不包含ui_layouts，添加ui_layouts目录
            if file_name and "ui_layouts" not in file_name:
                # 提取文件名
                base_name = os.path.basename(file_name)
                # 添加ui_layouts目录
                new_file_name = "ui_layouts/" + base_name
                ui_file["fileName"] = new_file_name
                print(f"修正路径: {file_name} -> {new_file_name}")
    
    # 保存修正后的配置文件
    backup_path = config_path + ".ui_layouts_fixed"
    
    # 创建修正后的备份
    with open(backup_path, 'w', encoding='utf-8') as f:
        json.dump(config, f, ensure_ascii=False, indent=4)
    
    print(f"\n已创建修正后的备份文件: {backup_path}")
    
    # 保存修正后的配置文件
    with open(config_path, 'w', encoding='utf-8') as f:
        json.dump(config, f, ensure_ascii=False, indent=4)
    
    print(f"已更新配置文件: {config_path}")
    
    return config

def main():
    """主函数"""
    config_path = r"product_configurations\product_12345\product_config.json"
    
    if not os.path.exists(config_path):
        print(f"错误: 配置文件不存在: {config_path}")
        return 1
    
    try:
        # 测试当前配置
        print("=== 测试当前配置 ===")
        current_valid = test_ui_layouts_path(config_path)
        
        if not current_valid:
            print("\n当前配置存在问题，开始修正...")
            
            # 修正配置
            fixed_config = fix_ui_layouts_path(config_path)
            
            # 测试修正后的配置
            print("\n=== 测试修正后的配置 ===")
            fixed_valid = test_ui_layouts_path(config_path)
            
            if fixed_valid:
                print("\n✅ 所有UI文件路径已正确包含ui_layouts目录")
            else:
                print("\n❌ 修正后仍然存在问题")
        else:
            print("\n✅ 当前配置已正确包含ui_layouts目录")
        
        return 0
        
    except Exception as e:
        print(f"❌ 测试过程中发生错误: {str(e)}")
        return 1

if __name__ == "__main__":
    import sys
    sys.exit(main())