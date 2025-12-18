#!/usr/bin/env python3
"""
修正product_config.json中路径重复的问题
"""

import json
import os
import re

def fix_path_duplication(config_path):
    """修正路径中的重复目录问题"""
    
    # 读取配置文件
    with open(config_path, 'r', encoding='utf-8') as f:
        config = json.load(f)
    
    print("开始修正路径重复问题...\n")
    
    # 修正fileStructure中的basePath
    if "fileStructure" in config:
        file_structure = config["fileStructure"]
        
        if "basePath" in file_structure:
            current_base_path = file_structure["basePath"]
            
            # 修正重复的product_configurations目录
            corrected_base_path = re.sub(r'product_configurations\\product_configurations', 
                                        'product_configurations', 
                                        current_base_path)
            
            if current_base_path != corrected_base_path:
                file_structure["basePath"] = corrected_base_path
                print(f"修正basePath: {current_base_path} -> {corrected_base_path}")
    
    # 修正UI文件路径
    if "uiFiles" in config:
        for ui_file in config["uiFiles"]:
            if "fileName" in ui_file:
                current_path = ui_file["fileName"]
                
                # 修正重复的product_configurations目录
                corrected_path = re.sub(r'product_configurations\\product_configurations', 
                                       'product_configurations', 
                                       current_path)
                
                if current_path != corrected_path:
                    ui_file["fileName"] = corrected_path
                    print(f"修正UI文件路径: {current_path} -> {corrected_path}")
    
    # 修正状态机文件路径
    if "stateMachines" in config:
        for state_machine in config["stateMachines"]:
            if "fileName" in state_machine:
                current_path = state_machine["fileName"]
                
                # 修正重复的product_configurations目录
                corrected_path = re.sub(r'product_configurations\\product_configurations', 
                                       'product_configurations', 
                                       current_path)
                
                if current_path != corrected_path:
                    state_machine["fileName"] = corrected_path
                    print(f"修正状态机路径: {current_path} -> {corrected_path}")
    
    # 修正旧结构中的路径
    if "iconPath" in config:
        current_path = config["iconPath"]
        corrected_path = re.sub(r'product_configurations\\product_configurations', 
                               'product_configurations', 
                               current_path)
        if current_path != corrected_path:
            config["iconPath"] = corrected_path
            print(f"修正旧图标路径: {current_path} -> {corrected_path}")
    
    if "screenshotPath" in config:
        current_path = config["screenshotPath"]
        corrected_path = re.sub(r'product_configurations\\product_configurations', 
                               'product_configurations', 
                               current_path)
        if current_path != corrected_path:
            config["screenshotPath"] = corrected_path
            print(f"修正旧截图路径: {current_path} -> {corrected_path}")
    
    if "uiLayoutPath" in config:
        current_path = config["uiLayoutPath"]
        corrected_path = re.sub(r'product_configurations\\product_configurations', 
                               'product_configurations', 
                               current_path)
        if current_path != corrected_path:
            config["uiLayoutPath"] = corrected_path
            print(f"修正UI布局路径: {current_path} -> {corrected_path}")
    
    if "stateMachinePath" in config:
        current_path = config["stateMachinePath"]
        corrected_path = re.sub(r'product_configurations\\product_configurations', 
                               'product_configurations', 
                               current_path)
        if current_path != corrected_path:
            config["stateMachinePath"] = corrected_path
            print(f"修正状态机路径: {current_path} -> {corrected_path}")
    
    # 保存修正后的配置文件
    backup_path = config_path + ".fixed"
    
    # 创建修正后的备份
    with open(backup_path, 'w', encoding='utf-8') as f:
        json.dump(config, f, ensure_ascii=False, indent=4)
    
    print(f"\n已创建修正后的备份文件: {backup_path}")
    
    # 保存修正后的配置文件
    with open(config_path, 'w', encoding='utf-8') as f:
        json.dump(config, f, ensure_ascii=False, indent=4)
    
    print(f"已更新配置文件: {config_path}")
    
    return config

def verify_paths(config_path):
    """验证路径是否正确"""
    
    with open(config_path, 'r', encoding='utf-8') as f:
        config = json.load(f)
    
    print("\n验证路径正确性...")
    
    all_paths_valid = True
    
    # 验证fileStructure中的basePath
    if "fileStructure" in config:
        base_path = config["fileStructure"].get("basePath", "")
        if base_path and os.path.exists(base_path):
            print(f"✅ basePath有效: {base_path}")
        else:
            print(f"❌ basePath无效: {base_path}")
            all_paths_valid = False
    
    # 验证UI文件路径
    if "uiFiles" in config:
        for ui_file in config["uiFiles"]:
            file_path = ui_file.get("fileName", "")
            if file_path and os.path.exists(file_path):
                print(f"✅ UI文件路径有效: {file_path}")
            else:
                print(f"❌ UI文件路径无效: {file_path}")
                all_paths_valid = False
    
    # 验证状态机文件路径
    if "stateMachines" in config:
        for state_machine in config["stateMachines"]:
            file_path = state_machine.get("fileName", "")
            if file_path:
                # 状态机文件可能不存在，但路径应该有效
                dir_path = os.path.dirname(file_path)
                if os.path.exists(dir_path):
                    print(f"✅ 状态机目录有效: {dir_path}")
                else:
                    print(f"❌ 状态机目录无效: {dir_path}")
                    all_paths_valid = False
    
    return all_paths_valid

def main():
    """主函数"""
    config_path = r"product_configurations\product_12345\product_config.json"
    
    if not os.path.exists(config_path):
        print(f"错误: 配置文件不存在: {config_path}")
        return 1
    
    try:
        # 修正路径重复问题
        fixed_config = fix_path_duplication(config_path)
        
        # 验证路径正确性
        paths_valid = verify_paths(config_path)
        
        if paths_valid:
            print("\n✅ 所有路径验证通过!")
        else:
            print("\n⚠️ 部分路径存在问题，请检查文件是否存在")
        
        print("\n修正后的配置文件摘要:")
        print(f"- 产品名称: {fixed_config.get('productInfo', {}).get('name', 'N/A')}")
        
        if "fileStructure" in fixed_config:
            fs = fixed_config["fileStructure"]
            print(f"- 基础路径: {fs.get('basePath', 'N/A')}")
            print(f"- UI布局路径: {fs.get('uiLayoutsPath', 'N/A')}")
        
        if "uiFiles" in fixed_config:
            print(f"- UI文件数量: {len(fixed_config['uiFiles'])}")
            for ui_file in fixed_config["uiFiles"][:3]:  # 显示前3个文件
                print(f"  - {ui_file.get('name', 'N/A')}: {ui_file.get('fileName', 'N/A')}")
        
        return 0
        
    except Exception as e:
        print(f"❌ 修正过程中发生错误: {str(e)}")
        return 1

if __name__ == "__main__":
    import sys
    sys.exit(main())