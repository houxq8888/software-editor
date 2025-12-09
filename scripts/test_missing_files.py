#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试文件缺失情况的演示脚本
"""

import os
import shutil
import sys
from product_loader import ProductLoader


def test_missing_files_scenario():
    """测试文件缺失场景"""
    print("=" * 60)
    print("测试文件缺失场景演示")
    print("=" * 60)
    
    # 备份原始文件
    ui_layouts_path = "product_configurations/product_12345/ui_layouts"
    backup_files = []
    
    # 移动一些文件来模拟缺失情况
    files_to_move = [
        "43214.ui",  # 主UI文件
        "state_machine_1.json"
    ]
    
    print("\n1. 模拟文件缺失场景...")
    for file_name in files_to_move:
        if file_name.endswith(".ui"):
            source_path = os.path.join(ui_layouts_path, file_name)
        else:
            source_path = os.path.join("product_configurations/product_12345/state_machines", file_name)
        
        backup_path = source_path + ".backup"
        
        if os.path.exists(source_path):
            shutil.move(source_path, backup_path)
            backup_files.append((source_path, backup_path))
            print(f"  已移动: {file_name}")
    
    print("\n2. 启动应用程序进行验证...")
    
    # 创建产品加载器
    loader = ProductLoader("product_configurations")
    
    # 启动应用程序
    success = loader.start_application()
    
    print("\n3. 恢复原始文件...")
    for source_path, backup_path in backup_files:
        if os.path.exists(backup_path):
            shutil.move(backup_path, source_path)
            print(f"  已恢复: {os.path.basename(source_path)}")
    
    print("\n" + "=" * 60)
    print("测试完成")
    print("=" * 60)
    
    return success


def test_complete_validation():
    """测试完整验证场景"""
    print("\n" + "=" * 60)
    print("测试完整验证场景演示")
    print("=" * 60)
    
    # 创建产品加载器
    loader = ProductLoader("product_configurations")
    
    # 启动应用程序
    success = loader.start_application()
    
    print("\n" + "=" * 60)
    print("测试完成")
    print("=" * 60)
    
    return success


def main():
    """主函数"""
    print("文件验证系统演示")
    print("=" * 60)
    
    # 测试完整验证
    complete_success = test_complete_validation()
    
    # 测试缺失文件场景
    missing_success = test_missing_files_scenario()
    
    print("\n" + "=" * 60)
    print("演示总结")
    print("=" * 60)
    print(f"完整验证测试: {'✓ 成功' if complete_success else '✗ 失败'}")
    print(f"缺失文件测试: {'✓ 成功检测到缺失文件' if not missing_success else '✗ 未能正确检测缺失文件'}")
    print("=" * 60)
    
    return complete_success and not missing_success


if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)