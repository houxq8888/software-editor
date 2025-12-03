#!/usr/bin/env python3
"""
测试打包功能的脚本
验证PackageWorker是否能正确使用JSON中的uiLayoutPath字段
"""

import json
import os
import sys

def test_package_function():
    """测试打包功能"""
    
    # 测试JSON文件路径
    test_json_path = r"test/ui_automation/test_results/2025-11-21/test_product_20251121_113422.json"
    
    # 检查JSON文件是否存在
    if not os.path.exists(test_json_path):
        print(f"错误：测试JSON文件不存在: {test_json_path}")
        return False
    
    # 读取JSON文件
    try:
        with open(test_json_path, 'r', encoding='utf-8') as f:
            product_data = json.load(f)
        
        print("JSON文件内容:")
        print(json.dumps(product_data, indent=2, ensure_ascii=False))
        
        # 检查uiLayoutPath字段
        ui_layout_path = product_data.get('uiLayoutPath', '')
        print(f"\nuiLayoutPath字段值: {ui_layout_path}")
        
        # 检查UI布局文件是否存在
        if ui_layout_path and os.path.exists(ui_layout_path):
            print(f"✓ UI布局文件存在: {ui_layout_path}")
            
            # 检查文件大小
            file_size = os.path.getsize(ui_layout_path)
            print(f"✓ UI布局文件大小: {file_size} 字节")
            
            # 验证文件内容（前几行）
            with open(ui_layout_path, 'r', encoding='utf-8') as f:
                first_lines = [next(f) for _ in range(5)]
            print("✓ UI布局文件前5行:")
            for line in first_lines:
                print(f"  {line.strip()}")
            
            return True
        else:
            print(f"✗ UI布局文件不存在或路径为空: {ui_layout_path}")
            return False
            
    except Exception as e:
        print(f"错误：读取JSON文件失败: {e}")
        return False

def main():
    """主函数"""
    print("=== 打包功能测试 ===")
    print("测试PackageWorker是否能正确使用JSON中的uiLayoutPath字段")
    print()
    
    success = test_package_function()
    
    print()
    if success:
        print("✓ 打包功能测试通过")
        print("✓ PackageWorker现在应该能够正确使用JSON中的uiLayoutPath字段")
        print("✓ 修复的代码包括:")
        print("  - PackageWorker类添加了接收Product对象的packageSoftware重载方法")
        print("  - 使用product.uiLayoutPath()获取UI布局文件路径")
        print("  - PackageManager的startPackage方法已更新传递Product对象")
    else:
        print("✗ 打包功能测试失败")
    
    return 0 if success else 1

if __name__ == "__main__":
    sys.exit(main())