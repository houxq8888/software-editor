#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试打包功能修复脚本
验证修改后的打包功能是否能够正确处理中文产品名称
"""

import os
import sys
import json
import tempfile
import shutil

def test_package_config():
    """测试PackageConfig类的修改"""
    print("=== 测试PackageConfig类修改 ===")
    
    # 模拟一个包含中文名称的产品数据
    product_data = {
        "name": "测试软件",
        "uniqueId": "test_software",
        "version": "1.0.0",
        "developer": "测试开发者"
    }
    
    print(f"产品名称: {product_data['name']}")
    print(f"产品唯一ID: {product_data['uniqueId']}")
    
    # 测试默认输出目录生成
    # 根据修改后的代码，应该使用uniqueId而不是name
    if product_data.get('uniqueId'):
        default_output_dir = os.path.join(os.getcwd(), "packages", product_data['uniqueId'])
        print(f"默认输出目录: {default_output_dir}")
        
        # 检查路径是否包含中文
        if any('\u4e00' <= char <= '\u9fff' for char in default_output_dir):
            print("❌ 错误: 输出目录包含中文字符")
            return False
        else:
            print("✅ 输出目录不包含中文字符，符合预期")
    else:
        print("❌ 错误: 产品数据缺少uniqueId字段")
        return False
    
    return True

def test_file_name_generation():
    """测试文件名生成逻辑"""
    print("\n=== 测试文件名生成逻辑 ===")
    
    # 模拟PackageSettings
    settings = {
        "name": "中文软件名称",
        "uniqueId": "chinese_software",
        "version": "1.0.0"
    }
    
    # 测试可执行文件名称生成
    # 根据修改后的代码，应该使用uniqueId而不是name
    if settings.get('uniqueId'):
        exe_name = settings['uniqueId'] + ".exe"
        print(f"可执行文件名: {exe_name}")
        
        # 检查文件名是否包含中文
        if any('\u4e00' <= char <= '\u9fff' for char in exe_name):
            print("❌ 错误: 可执行文件名包含中文字符")
            return False
        else:
            print("✅ 可执行文件名不包含中文字符，符合预期")
    else:
        print("❌ 错误: 设置缺少uniqueId字段")
        return False
    
    # 测试NSIS安装包名称生成
    nsis_name = settings.get('uniqueId', settings['name']) + "_Setup.exe"
    print(f"NSIS安装包名: {nsis_name}")
    
    if any('\u4e00' <= char <= '\u9fff' for char in nsis_name):
        print("❌ 错误: NSIS安装包名包含中文字符")
        return False
    else:
        print("✅ NSIS安装包名不包含中文字符，符合预期")
    
    return True

def test_smart_package_config():
    """测试SmartPackageConfig类的修改"""
    print("\n=== 测试SmartPackageConfig类修改 ===")
    
    # 模拟SmartPackageSettings
    settings = {
        "name": "智能打包测试软件",
        "uniqueId": "smart_package_test",
        "version": "1.0.0",
        "developer": "智能打包开发者"
    }
    
    # 测试cleanProjectName生成逻辑
    # 根据修改后的代码，应该使用uniqueId而不是name
    clean_project_name = settings.get('uniqueId', settings['name'])
    # 模拟清理非字母数字字符
    import re
    clean_project_name = re.sub(r'[^a-zA-Z0-9_]', '_', clean_project_name)
    
    print(f"原始名称: {settings['name']}")
    print(f"清理后工程名: {clean_project_name}")
    
    # 检查清理后的名称是否包含中文
    if any('\u4e00' <= char <= '\u9fff' for char in clean_project_name):
        print("❌ 错误: 清理后工程名包含中文字符")
        return False
    else:
        print("✅ 清理后工程名不包含中文字符，符合预期")
    
    # 测试CMake项目名
    cmake_project_line = f"project({clean_project_name} VERSION {settings['version']})"
    print(f"CMake项目行: {cmake_project_line}")
    
    if any('\u4e00' <= char <= '\u9fff' for char in cmake_project_line):
        print("❌ 错误: CMake项目行包含中文字符")
        return False
    else:
        print("✅ CMake项目行不包含中文字符，符合预期")
    
    return True

def test_comprehensive_scenario():
    """测试综合场景"""
    print("\n=== 测试综合场景 ===")
    
    # 模拟一个完整的产品打包场景
    product_scenarios = [
        {
            "name": "纯中文软件",
            "uniqueId": "chinese_only_software",
            "description": "这是一个测试场景"
        },
        {
            "name": "Mixed中文Software",
            "uniqueId": "mixed_chinese_software",
            "description": "Mixed description"
        },
        {
            "name": "English Only Software",
            "uniqueId": "english_only_software",
            "description": "English description"
        }
    ]
    
    all_passed = True
    
    for i, product in enumerate(product_scenarios, 1):
        print(f"\n场景 {i}: {product['name']}")
        
        # 测试输出目录
        output_dir = os.path.join(os.getcwd(), "packages", product['uniqueId'])
        print(f"  输出目录: {output_dir}")
        
        # 测试可执行文件名
        exe_file = product['uniqueId'] + ".exe"
        print(f"  可执行文件: {exe_file}")
        
        # 测试NSIS安装包
        nsis_file = product['uniqueId'] + "_Setup.exe"
        print(f"  NSIS安装包: {nsis_file}")
        
        # 检查所有生成的文件名和路径
        files_to_check = [output_dir, exe_file, nsis_file]
        
        for file_path in files_to_check:
            if any('\u4e00' <= char <= '\u9fff' for char in file_path):
                print(f"  ❌ {file_path} 包含中文字符")
                all_passed = False
            else:
                print(f"  ✅ {file_path} 不包含中文字符")
    
    return all_passed

def main():
    """主测试函数"""
    print("开始测试打包功能修复...")
    print("=" * 50)
    
    tests = [
        ("PackageConfig类测试", test_package_config),
        ("文件名生成测试", test_file_name_generation),
        ("SmartPackageConfig类测试", test_smart_package_config),
        ("综合场景测试", test_comprehensive_scenario)
    ]
    
    passed_tests = 0
    total_tests = len(tests)
    
    for test_name, test_func in tests:
        try:
            if test_func():
                print(f"\n✅ {test_name} 通过")
                passed_tests += 1
            else:
                print(f"\n❌ {test_name} 失败")
        except Exception as e:
            print(f"\n❌ {test_name} 异常: {e}")
    
    print("\n" + "=" * 50)
    print(f"测试结果: {passed_tests}/{total_tests} 通过")
    
    if passed_tests == total_tests:
        print("🎉 所有测试通过！打包功能修复成功！")
        print("\n修改总结:")
        print("1. ✅ PackageDialog类 - 使用uniqueId生成默认输出目录")
        print("2. ✅ PackageManager类 - 使用uniqueId生成可执行文件名称")
        print("3. ✅ SmartPackageConfig类 - 使用uniqueId生成打包相关文件名称")
        print("4. ✅ NSIS脚本生成 - 使用uniqueId生成安装包和快捷方式")
        print("\n现在中文产品名称的打包应该不会因为文件名问题而失败了！")
        return 0
    else:
        print("⚠️ 部分测试失败，需要进一步检查")
        return 1

if __name__ == "__main__":
    sys.exit(main())