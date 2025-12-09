#!/usr/bin/env python3
"""
修正product_config.json中实际文件路径的问题
"""

import json
import os

def fix_actual_file_paths(config_path):
    """修正实际文件路径"""
    
    # 读取配置文件
    with open(config_path, 'r', encoding='utf-8') as f:
        config = json.load(f)
    
    print("开始修正实际文件路径...\n")
    
    # 获取项目根目录
    project_root = os.path.dirname(os.path.abspath(config_path))
    project_root = os.path.dirname(project_root)  # 上一级目录
    
    print(f"项目根目录: {project_root}")
    
    # 修正fileStructure中的basePath - 应该指向项目根目录
    if "fileStructure" in config:
        file_structure = config["fileStructure"]
        file_structure["basePath"] = project_root
        print(f"设置basePath为项目根目录: {project_root}")
    
    # 修正UI文件路径 - UI文件位于项目根目录
    if "uiFiles" in config:
        for ui_file in config["uiFiles"]:
            if "fileName" in ui_file:
                file_name = ui_file["fileName"]
                
                # 提取文件名（不含路径）
                base_name = os.path.basename(file_name)
                
                # 构建正确的路径（项目根目录下的文件）
                correct_path = os.path.join(project_root, base_name)
                ui_file["fileName"] = correct_path
                print(f"修正UI文件路径: {file_name} -> {correct_path}")
    
    # 修正状态机文件路径 - 状态机文件位于项目根目录
    if "stateMachines" in config:
        for state_machine in config["stateMachines"]:
            if "fileName" in state_machine:
                file_name = state_machine["fileName"]
                
                # 提取文件名（不含路径）
                base_name = os.path.basename(file_name)
                
                # 构建正确的路径（项目根目录下的文件）
                correct_path = os.path.join(project_root, base_name)
                state_machine["fileName"] = correct_path
                print(f"修正状态机路径: {file_name} -> {correct_path}")
    
    # 修正旧结构中的路径
    if "iconPath" in config:
        # 图标文件位于产品配置目录下
        icon_name = os.path.basename(config["iconPath"])
        config_dir = os.path.dirname(config_path)
        correct_icon_path = os.path.join(config_dir, icon_name)
        config["iconPath"] = correct_icon_path
        print(f"修正图标路径: {config['iconPath']} -> {correct_icon_path}")
    
    if "screenshotPath" in config:
        # 截图文件位于产品配置目录下
        screenshot_name = os.path.basename(config["screenshotPath"])
        config_dir = os.path.dirname(config_path)
        correct_screenshot_path = os.path.join(config_dir, screenshot_name)
        config["screenshotPath"] = correct_screenshot_path
        print(f"修正截图路径: {config['screenshotPath']} -> {correct_screenshot_path}")
    
    if "uiLayoutPath" in config:
        # UI布局路径应该指向项目根目录
        config["uiLayoutPath"] = project_root
        print(f"修正UI布局路径为项目根目录: {project_root}")
    
    if "stateMachinePath" in config:
        # 状态机文件位于项目根目录
        state_machine_name = os.path.basename(config["stateMachinePath"])
        correct_state_machine_path = os.path.join(project_root, state_machine_name)
        config["stateMachinePath"] = correct_state_machine_path
        print(f"修正状态机路径: {config['stateMachinePath']} -> {correct_state_machine_path}")
    
    # 修正resources字段中的路径
    if "resources" in config:
        resources = config["resources"]
        
        if "icon" in resources:
            icon_name = os.path.basename(resources["icon"])
            config_dir = os.path.dirname(config_path)
            correct_icon_path = os.path.join(config_dir, icon_name)
            resources["icon"] = correct_icon_path
            print(f"修正resources图标路径: {resources['icon']} -> {correct_icon_path}")
        
        if "screenshot" in resources:
            screenshot_name = os.path.basename(resources["screenshot"])
            config_dir = os.path.dirname(config_path)
            correct_screenshot_path = os.path.join(config_dir, screenshot_name)
            resources["screenshot"] = correct_screenshot_path
            print(f"修正resources截图路径: {resources['screenshot']} -> {correct_screenshot_path}")
    
    # 保存修正后的配置文件
    backup_path = config_path + ".actual_fixed"
    
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
            if file_path and os.path.exists(file_path):
                print(f"✅ 状态机文件路径有效: {file_path}")
            else:
                print(f"❌ 状态机文件路径无效: {file_path}")
                all_paths_valid = False
    
    # 验证图标和截图路径
    if "iconPath" in config:
        icon_path = config["iconPath"]
        if icon_path and os.path.exists(icon_path):
            print(f"✅ 图标路径有效: {icon_path}")
        else:
            print(f"❌ 图标路径无效: {icon_path}")
            all_paths_valid = False
    
    if "screenshotPath" in config:
        screenshot_path = config["screenshotPath"]
        if screenshot_path and os.path.exists(screenshot_path):
            print(f"✅ 截图路径有效: {screenshot_path}")
        else:
            print(f"❌ 截图路径无效: {screenshot_path}")
            all_paths_valid = False
    
    return all_paths_valid

def main():
    """主函数"""
    config_path = r"product_configurations\product_12345\product_config.json"
    
    if not os.path.exists(config_path):
        print(f"错误: 配置文件不存在: {config_path}")
        return 1
    
    try:
        # 修正实际文件路径
        fixed_config = fix_actual_file_paths(config_path)
        
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