#!/usr/bin/env python3
"""
将product_config.json中的相对路径转换为绝对路径
"""

import json
import os
import sys

def convert_paths_to_absolute(config_path):
    """将配置文件中的相对路径转换为绝对路径"""
    
    # 获取当前工作目录
    current_dir = os.path.dirname(os.path.abspath(config_path))
    project_root = os.path.dirname(current_dir)
    
    print(f"当前配置文件路径: {config_path}")
    print(f"项目根目录: {project_root}")
    
    # 读取配置文件
    with open(config_path, 'r', encoding='utf-8') as f:
        config = json.load(f)
    
    # 转换fileStructure中的路径为绝对路径
    if "fileStructure" in config:
        file_structure = config["fileStructure"]
        
        # 将basePath转换为绝对路径
        if "basePath" in file_structure:
            base_path = file_structure["basePath"]
            if not os.path.isabs(base_path):
                absolute_base_path = os.path.join(project_root, base_path)
                file_structure["basePath"] = absolute_base_path
                print(f"转换basePath: {base_path} -> {absolute_base_path}")
    
    # 转换resources中的路径为绝对路径
    if "resources" in config:
        resources = config["resources"]
        
        # 转换图标路径
        if "icon" in resources:
            icon_path = resources["icon"]
            if not os.path.isabs(icon_path):
                # 图标文件相对于产品配置目录
                absolute_icon_path = os.path.join(current_dir, icon_path)
                resources["icon"] = absolute_icon_path
                print(f"转换图标路径: {icon_path} -> {absolute_icon_path}")
        
        # 转换截图路径
        if "screenshot" in resources:
            screenshot_path = resources["screenshot"]
            if not os.path.isabs(screenshot_path):
                absolute_screenshot_path = os.path.join(current_dir, screenshot_path)
                resources["screenshot"] = absolute_screenshot_path
                print(f"转换截图路径: {screenshot_path} -> {absolute_screenshot_path}")
    
    # 转换UI文件路径
    if "uiFiles" in config:
        for ui_file in config["uiFiles"]:
            if "fileName" in ui_file:
                file_name = ui_file["fileName"]
                if not os.path.isabs(file_name):
                    # 构建完整的UI文件路径
                    if "fileStructure" in config:
                        file_structure = config["fileStructure"]
                        base_path = file_structure.get("basePath", "")
                        ui_layouts_path = file_structure.get("uiLayoutsPath", "")
                        
                        if base_path and ui_layouts_path:
                            absolute_ui_path = os.path.join(base_path, ui_layouts_path, file_name)
                            ui_file["fileName"] = absolute_ui_path
                            print(f"转换UI文件路径: {file_name} -> {absolute_ui_path}")
    
    # 转换状态机文件路径
    if "stateMachines" in config:
        for state_machine in config["stateMachines"]:
            if "fileName" in state_machine:
                file_name = state_machine["fileName"]
                if not os.path.isabs(file_name):
                    # 构建完整的状态机文件路径
                    if "fileStructure" in config:
                        file_structure = config["fileStructure"]
                        base_path = file_structure.get("basePath", "")
                        state_machines_path = file_structure.get("stateMachinesPath", "")
                        
                        if base_path and state_machines_path:
                            absolute_state_machine_path = os.path.join(base_path, state_machines_path, file_name)
                            state_machine["fileName"] = absolute_state_machine_path
                            print(f"转换状态机路径: {file_name} -> {absolute_state_machine_path}")
    
    # 向后兼容：转换旧结构中的路径
    if "iconPath" in config:
        icon_path = config["iconPath"]
        if not os.path.isabs(icon_path):
            absolute_icon_path = os.path.join(current_dir, icon_path)
            config["iconPath"] = absolute_icon_path
            print(f"转换旧图标路径: {icon_path} -> {absolute_icon_path}")
    
    if "screenshotPath" in config:
        screenshot_path = config["screenshotPath"]
        if not os.path.isabs(screenshot_path):
            absolute_screenshot_path = os.path.join(current_dir, screenshot_path)
            config["screenshotPath"] = absolute_screenshot_path
            print(f"转换旧截图路径: {screenshot_path} -> {absolute_screenshot_path}")
    
    if "uiLayoutPath" in config:
        ui_layout_path = config["uiLayoutPath"]
        if not os.path.isabs(ui_layout_path):
            absolute_ui_layout_path = os.path.join(project_root, ui_layout_path)
            config["uiLayoutPath"] = absolute_ui_layout_path
            print(f"转换UI布局路径: {ui_layout_path} -> {absolute_ui_layout_path}")
    
    if "stateMachinePath" in config:
        state_machine_path = config["stateMachinePath"]
        if not os.path.isabs(state_machine_path):
            absolute_state_machine_path = os.path.join(project_root, state_machine_path)
            config["stateMachinePath"] = absolute_state_machine_path
            print(f"转换状态机路径: {state_machine_path} -> {absolute_state_machine_path}")
    
    # 保存更新后的配置文件
    backup_path = config_path + ".backup"
    
    # 创建备份
    with open(backup_path, 'w', encoding='utf-8') as f:
        json.dump(config, f, ensure_ascii=False, indent=4)
    
    print(f"\n已创建备份文件: {backup_path}")
    
    # 保存更新后的配置文件
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
    
    print("开始转换相对路径为绝对路径...\n")
    
    try:
        updated_config = convert_paths_to_absolute(config_path)
        
        print("\n✅ 路径转换完成!")
        print("\n转换后的配置文件摘要:")
        print(f"- 产品名称: {updated_config.get('productInfo', {}).get('name', 'N/A')}")
        
        if "fileStructure" in updated_config:
            fs = updated_config["fileStructure"]
            print(f"- 基础路径: {fs.get('basePath', 'N/A')}")
            print(f"- UI布局路径: {fs.get('uiLayoutsPath', 'N/A')}")
        
        if "uiFiles" in updated_config:
            print(f"- UI文件数量: {len(updated_config['uiFiles'])}")
            for ui_file in updated_config["uiFiles"][:3]:  # 显示前3个文件
                print(f"  - {ui_file.get('name', 'N/A')}: {ui_file.get('fileName', 'N/A')}")
        
        return 0
        
    except Exception as e:
        print(f"❌ 转换过程中发生错误: {str(e)}")
        return 1

if __name__ == "__main__":
    sys.exit(main())