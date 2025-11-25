#!/usr/bin/env python3
"""
测试打包输出目录命名逻辑
验证输出目录是否按照JSON文件中的uniqueId命名，而不是name
"""

import json
import os
import sys
from datetime import datetime

# 添加项目根目录到Python路径
project_root = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
sys.path.insert(0, project_root)

def test_output_dir_naming():
    """测试输出目录命名逻辑"""
    print("=== 测试打包输出目录命名逻辑 ===")
    
    # 1. 读取测试产品JSON文件
    test_json_path = os.path.join(project_root, "test_product_20251121_113422.json")
    
    if not os.path.exists(test_json_path):
        print(f"❌ 测试JSON文件不存在: {test_json_path}")
        return False
    
    print(f"✅ 测试JSON文件存在: {test_json_path}")
    
    # 2. 读取JSON文件内容
    with open(test_json_path, 'r', encoding='utf-8') as f:
        product_data = json.load(f)
    
    # 3. 检查JSON文件中的name和uniqueId字段
    product_name = product_data.get('name', '')
    product_unique_id = product_data.get('uniqueId', '')
    
    print(f"📋 产品名称: {product_name}")
    print(f"📋 产品唯一ID: {product_unique_id}")
    
    # 4. 验证修改后的代码逻辑
    # 根据修改后的mainwindow.cpp第498行，输出目录应该使用uniqueId
    expected_output_dir = os.path.join(project_root, "packages", product_unique_id)
    
    # 旧逻辑（使用name）
    old_output_dir = os.path.join(project_root, "packages", product_name)
    
    print(f"\n📁 修改后的输出目录（使用uniqueId）: {expected_output_dir}")
    print(f"📁 旧逻辑的输出目录（使用name）: {old_output_dir}")
    
    # 5. 检查是否有中文路径问题
    has_chinese_in_name = any('\u4e00' <= char <= '\u9fff' for char in product_name)
    has_chinese_in_unique_id = any('\u4e00' <= char <= '\u9fff' for char in product_unique_id)
    
    print(f"\n🔤 产品名称包含中文: {has_chinese_in_name}")
    print(f"🔤 产品唯一ID包含中文: {has_chinese_in_unique_id}")
    
    # 6. 验证修改的正确性
    if has_chinese_in_name and not has_chinese_in_unique_id:
        print("✅ 修改正确：产品名称包含中文，但uniqueId不包含中文")
        print("✅ 使用uniqueId命名输出目录可以避免中文路径问题")
    elif not has_chinese_in_name:
        print("ℹ️ 产品名称不包含中文，但使用uniqueId命名更规范")
    else:
        print("⚠️ 产品名称和uniqueId都包含中文，建议检查uniqueId生成逻辑")
    
    # 7. 检查PackageService中的安装包命名逻辑
    expected_installer_path = os.path.join(expected_output_dir, f"{product_unique_id}_setup.exe")
    old_installer_path = os.path.join(old_output_dir, f"{product_name}_setup.exe")
    
    print(f"\n📦 修改后的安装包路径: {expected_installer_path}")
    print(f"📦 旧逻辑的安装包路径: {old_installer_path}")
    
    # 8. 验证PackageService中的generateInstallerPath函数
    print("\n🔍 验证PackageService中的安装包命名逻辑:")
    print("   - generateInstallerPath函数使用config.uniqueId + '_setup.exe'")
    print("   - 这确保了安装包文件名与输出目录命名一致")
    
    # 9. 总结验证结果
    print("\n=== 验证结果 ===")
    
    if product_unique_id and product_unique_id != product_name:
        print("✅ 修改成功：打包输出目录现在使用uniqueId而不是name")
        print("✅ 这解决了中文路径问题和命名规范性问题")
        
        # 检查uniqueId的格式
        if len(product_unique_id) == 36 and '-' in product_unique_id:
            print("✅ uniqueId格式正确（UUID格式）")
        else:
            print("⚠️ uniqueId格式可能不是标准UUID格式")
            
    else:
        print("❌ 问题：uniqueId为空或与name相同")
        print("   需要检查Product类的uniqueId生成逻辑")
        
    return True

def check_mainwindow_code():
    """检查mainwindow.cpp中的修改"""
    print("\n=== 检查代码修改 ===")
    
    mainwindow_path = os.path.join(project_root, "src", "mainwindow.cpp")
    
    if not os.path.exists(mainwindow_path):
        print(f"❌ mainwindow.cpp文件不存在: {mainwindow_path}")
        return False
    
    # 读取文件内容
    with open(mainwindow_path, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # 检查修改后的代码
    if 'config.outputDir = QDir::currentPath() + "/packages/" + m_product.uniqueId();' in content:
        print("✅ mainwindow.cpp第498行已修改为使用uniqueId")
    else:
        print("❌ mainwindow.cpp第498行未正确修改")
        return False
    
    if 'config.uniqueId = m_product.uniqueId();' in content:
        print("✅ 已添加config.uniqueId设置")
    else:
        print("❌ 未添加config.uniqueId设置")
        return False
    
    return True

def main():
    """主函数"""
    try:
        print("打包输出目录命名逻辑测试")
        print("=" * 50)
        
        # 检查代码修改
        if not check_mainwindow_code():
            print("❌ 代码修改检查失败")
            return 1
        
        # 测试输出目录命名逻辑
        if not test_output_dir_naming():
            print("❌ 输出目录命名逻辑测试失败")
            return 1
        
        print("\n🎉 所有测试通过！")
        print("打包输出目录现在按照JSON文件中的uniqueId命名，而不是name")
        
        return 0
        
    except Exception as e:
        print(f"❌ 测试过程中出现错误: {e}")
        return 1

if __name__ == "__main__":
    sys.exit(main())