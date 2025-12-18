#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
修复配置文件中的路径问题，确保使用相对于产品目录的相对路径
"""

import json
import os
import shutil

def fix_relative_paths():
    """修复配置文件中的路径问题"""
    
    config_path = "product_configurations/product_12345/product_config.json"
    
    if not os.path.exists(config_path):
        print("❌ 配置文件不存在:", config_path)
        return
    
    # 备份原文件
    backup_path = config_path + ".backup"
    shutil.copy2(config_path, backup_path)
    print(f"✅ 已备份原文件: {backup_path}")
    
    # 读取配置文件
    with open(config_path, 'r', encoding='utf-8') as f:
        config = json.load(f)
    
    print("🔧 开始修复路径问题...")
    
    # 修复fileStructure中的basePath
    if "fileStructure" in config:
        file_structure = config["fileStructure"]
        
        # 将basePath改为相对路径
        old_base_path = file_structure.get("basePath", "")
        if os.path.isabs(old_base_path):
            # 提取相对于项目根目录的路径
            project_root = os.path.abspath(".")
            if old_base_path.startswith(project_root):
                relative_path = os.path.relpath(old_base_path, project_root)
                file_structure["basePath"] = relative_path
                print(f"✅ 修复basePath: {old_base_path} -> {relative_path}")
            else:
                # 如果不在项目根目录下，使用相对路径
                file_structure["basePath"] = "product_configurations/product_12345"
                print(f"✅ 设置basePath为: product_configurations/product_12345")
        
        # 确保uiLayoutsPath正确
        if file_structure.get("uiLayoutsPath") == "product_12345":
            file_structure["uiLayoutsPath"] = "ui_layouts"
            print(f"✅ 修复uiLayoutsPath: product_12345 -> ui_layouts")
    
    # 修复UI文件路径
    if "uiFiles" in config:
        ui_files = config["uiFiles"]
        
        for ui_file in ui_files:
            old_file_name = ui_file.get("fileName", "")
            
            if os.path.isabs(old_file_name):
                # 绝对路径转换为相对路径
                base_path = config["fileStructure"].get("basePath", "")
                if base_path and old_file_name.startswith(base_path):
                    # 相对于basePath的路径
                    relative_path = os.path.relpath(old_file_name, base_path)
                    # 确保使用正斜杠
                    relative_path = relative_path.replace("\\", "/")
                    ui_file["fileName"] = relative_path
                    print(f"✅ 修复绝对路径: {old_file_name} -> {relative_path}")
                else:
                    # 提取文件名并构建相对路径
                    file_name = os.path.basename(old_file_name)
                    relative_path = f"ui_layouts/{file_name}"
                    ui_file["fileName"] = relative_path
                    print(f"✅ 构建相对路径: {old_file_name} -> {relative_path}")
            else:
                # 修复相对路径中的格式问题
                new_file_name = old_file_name.replace("\\\\", "/").replace("\\", "/")
                
                # 移除重复的ui_layouts目录
                if "/ui_layouts/" in new_file_name or "ui_layouts/" in new_file_name:
                    # 提取最后一个ui_layouts之后的部分
                    parts = new_file_name.split("ui_layouts")
                    if len(parts) > 1:
                        # 取最后一个ui_layouts之后的部分
                        last_part = parts[-1]
                        if last_part.startswith("/"):
                            last_part = last_part[1:]
                        new_file_name = "ui_layouts/" + last_part
                
                # 确保路径格式正确
                if not new_file_name.startswith("ui_layouts/") and not new_file_name.startswith("product_configurations/"):
                    # 添加ui_layouts前缀
                    file_name = os.path.basename(new_file_name)
                    new_file_name = f"ui_layouts/{file_name}"
                
                if new_file_name != old_file_name:
                    ui_file["fileName"] = new_file_name
                    print(f"✅ 修复路径格式: {old_file_name} -> {new_file_name}")
    
    # 修复状态机路径
    if "stateMachines" in config and config["stateMachines"]:
        state_machine = config["stateMachines"][0]
        old_file_name = state_machine.get("fileName", "")
        
        if os.path.isabs(old_file_name):
            # 提取文件名
            file_name = os.path.basename(old_file_name)
            state_machine["fileName"] = file_name
            print(f"✅ 修复状态机路径: {old_file_name} -> {file_name}")
    
    # 修复资源路径
    if "resources" in config:
        resources = config["resources"]
        
        for resource_type in ["icon", "screenshot"]:
            if resource_type in resources:
                old_path = resources[resource_type]
                if os.path.isabs(old_path):
                    # 提取文件名
                    file_name = os.path.basename(old_path)
                    resources[resource_type] = f"resources/{file_name}"
                    print(f"✅ 修复资源路径 {resource_type}: {old_path} -> resources/{file_name}")
    
    # 向后兼容字段
    for field in ["iconPath", "screenshotPath", "uiLayoutPath", "stateMachinePath"]:
        if field in config and os.path.isabs(config[field]):
            # 保留绝对路径用于向后兼容
            print(f"⚠️  保留向后兼容字段 {field}: {config[field]}")
    
    # 保存修复后的配置文件
    with open(config_path, 'w', encoding='utf-8') as f:
        json.dump(config, f, ensure_ascii=False, indent=4)
    
    print(f"✅ 配置文件已修复: {config_path}")
    
    # 验证修复结果
    print("\n🔍 验证修复结果...")
    
    with open(config_path, 'r', encoding='utf-8') as f:
        fixed_config = json.load(f)
    
    # 检查basePath
    base_path = fixed_config["fileStructure"].get("basePath", "")
    if not os.path.isabs(base_path):
        print("✅ basePath是相对路径")
    else:
        print("❌ basePath仍然是绝对路径")
    
    # 检查UI文件路径
    ui_files = fixed_config.get("uiFiles", [])
    all_relative = True
    for ui_file in ui_files:
        file_name = ui_file.get("fileName", "")
        if os.path.isabs(file_name):
            print(f"❌ UI文件 {ui_file.get('name')} 仍然是绝对路径: {file_name}")
            all_relative = False
    
    if all_relative:
        print("✅ 所有UI文件路径都是相对路径")
    
    print("\n🎉 修复完成！产品现在可以移动到任意位置。")

if __name__ == "__main__":
    fix_relative_paths()