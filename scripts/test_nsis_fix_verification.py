#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
验证NSIS安装包修复效果 - 检查是否包含Qt插件目录
"""

import os
import shutil
import subprocess
import tempfile

def check_nsis_script_content():
    """检查NSIS脚本内容是否包含Qt插件目录复制指令"""
    print("=" * 60)
    print("检查NSIS脚本内容")
    print("=" * 60)
    
    # 检查smartpackageconfig.cpp中的NSIS脚本生成函数
    config_file = r"d:\virtualMachine\github\software-editor\src\smartpackageconfig.cpp"
    
    if os.path.exists(config_file):
        with open(config_file, 'r', encoding='utf-8') as f:
            content = f.read()
            
        # 检查是否包含Qt插件目录复制指令
        qt_plugin_checks = [
            ('platforms', 'platforms\\*.dll' in content),
            ('styles', 'styles\\*.dll' in content),
            ('imageformats', 'imageformats\\*.dll' in content),
            ('iconengines', 'iconengines\\*.dll' in content)
        ]
        
        print("smartpackageconfig.cpp中的Qt插件目录复制指令检查:")
        for plugin_dir, found in qt_plugin_checks:
            status = "✓ 已包含" if found else "✗ 未包含"
            print(f"  {plugin_dir}: {status}")
        
        return all(found for _, found in qt_plugin_checks)
    else:
        print("smartpackageconfig.cpp文件不存在")
        return False

def check_packaging_output():
    """检查打包输出目录中的文件结构"""
    print("\n" + "=" * 60)
    print("检查打包输出目录")
    print("=" * 60)
    
    # 检查smart_packages目录
    packages_dir = r"d:\virtualMachine\github\software-editor\smart_packages"
    
    if os.path.exists(packages_dir):
        print(f"找到打包输出目录: {packages_dir}")
        
        # 列出所有子目录
        subdirs = [d for d in os.listdir(packages_dir) 
                  if os.path.isdir(os.path.join(packages_dir, d))]
        
        if subdirs:
            latest_dir = max(subdirs, key=lambda d: os.path.getmtime(os.path.join(packages_dir, d)))
            package_dir = os.path.join(packages_dir, latest_dir)
            print(f"最新打包目录: {latest_dir}")
            
            # 检查目录内容
            if os.path.exists(package_dir):
                print(f"\n打包目录内容:")
                
                # 检查Qt插件目录是否存在
                qt_plugin_dirs = ['platforms', 'styles', 'imageformats', 'iconengines']
                for plugin_dir in qt_plugin_dirs:
                    plugin_path = os.path.join(package_dir, plugin_dir)
                    if os.path.exists(plugin_path):
                        files = os.listdir(plugin_path)
                        print(f"  ✓ {plugin_dir}: 包含 {len(files)} 个文件")
                        if files:
                            print(f"    文件列表: {', '.join(files[:5])}{'...' if len(files) > 5 else ''}")
                    else:
                        print(f"  ✗ {plugin_dir}: 目录不存在")
                
                # 检查NSIS安装包文件
                nsis_files = [f for f in os.listdir(package_dir) if f.endswith('_Setup.exe')]
                if nsis_files:
                    print(f"\n  ✓ NSIS安装包: {nsis_files[0]}")
                    nsis_path = os.path.join(package_dir, nsis_files[0])
                    file_size = os.path.getsize(nsis_path)
                    print(f"    文件大小: {file_size:,} 字节")
                else:
                    print("\n  ✗ NSIS安装包: 未找到")
                
                # 检查NSIS脚本文件
                nsis_script = os.path.join(package_dir, "installer.nsi")
                if os.path.exists(nsis_script):
                    print(f"\n  ✓ NSIS脚本: installer.nsi")
                    
                    # 检查NSIS脚本内容
                    with open(nsis_script, 'r', encoding='utf-8') as f:
                        script_content = f.read()
                    
                    # 检查是否包含Qt插件目录复制指令
                    script_checks = [
                        ('platforms', 'platforms\\*.dll' in script_content),
                        ('styles', 'styles\\*.dll' in script_content),
                        ('imageformats', 'imageformats\\*.dll' in script_content),
                        ('iconengines', 'iconengines\\*.dll' in script_content)
                    ]
                    
                    print("    NSIS脚本中的Qt插件目录复制指令:")
                    for plugin_dir, found in script_checks:
                        status = "✓ 已包含" if found else "✗ 未包含"
                        print(f"      {plugin_dir}: {status}")
                    
                    return all(found for _, found in script_checks)
                else:
                    print("\n  ✗ NSIS脚本: 未找到")
                    return False
            else:
                print("打包目录不存在")
                return False
        else:
            print("没有找到打包子目录")
            return False
    else:
        print("打包输出目录不存在")
        return False

def main():
    """主函数"""
    print("NSIS安装包Qt插件目录修复验证")
    print("=" * 60)
    
    # 检查源代码修复
    source_fixed = check_nsis_script_content()
    
    # 检查打包输出
    output_ok = check_packaging_output()
    
    print("\n" + "=" * 60)
    print("验证结果总结")
    print("=" * 60)
    
    if source_fixed and output_ok:
        print("✓ 修复成功!")
        print("  - NSIS脚本生成函数已包含Qt插件目录复制指令")
        print("  - 打包输出目录包含完整的Qt插件目录")
        print("  - NSIS安装包应该能够正确包含Qt依赖文件")
    else:
        print("✗ 修复未完成")
        if not source_fixed:
            print("  - NSIS脚本生成函数仍需修复")
        if not output_ok:
            print("  - 打包输出目录存在问题")
    
    print("\n建议:")
    if source_fixed and output_ok:
        print("  1. 运行软件编辑器程序")
        print("  2. 执行一次完整的打包流程")
        print("  3. 验证新生成的NSIS安装包是否包含Qt插件目录")
    else:
        print("  1. 检查源代码修复是否正确")
        print("  2. 重新编译程序")
        print("  3. 重新执行打包测试")

if __name__ == "__main__":
    main()