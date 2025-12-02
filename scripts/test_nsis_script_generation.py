#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
NSIS脚本生成验证脚本
用于验证smartpackageconfig.cpp中的Qt插件目录复制指令是否生效
"""

import os
import sys
import subprocess
import tempfile
import shutil

def test_nsis_script_generation():
    """测试NSIS脚本生成功能"""
    print("NSIS脚本生成验证")
    print("=" * 60)
    
    # 检查smartpackageconfig.cpp文件
    cpp_file = r"d:\virtualMachine\github\software-editor\src\smartpackageconfig.cpp"
    
    if not os.path.exists(cpp_file):
        print(f"❌ 文件不存在: {cpp_file}")
        return False
    
    print(f"✓ 找到源代码文件: {cpp_file}")
    
    # 检查修复是否已应用
    with open(cpp_file, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # 检查Qt插件目录复制指令
    qt_plugin_directories = [
        "platforms",
        "styles", 
        "imageformats",
        "iconengines"
    ]
    
    print("\n检查smartpackageconfig.cpp中的Qt插件目录复制指令:")
    all_found = True
    for plugin_dir in qt_plugin_directories:
        if f'SetOutPath "$INSTDIR\\{plugin_dir}"' in content:
            print(f"  ✓ {plugin_dir}: 已包含SetOutPath指令")
        else:
            print(f"  ✗ {plugin_dir}: 未包含SetOutPath指令")
            all_found = False
            
        if f'File "{plugin_dir}\\*.dll"' in content:
            print(f"  ✓ {plugin_dir}: 已包含File指令")
        else:
            print(f"  ✗ {plugin_dir}: 未包含File指令")
            all_found = False
    
    if all_found:
        print("\n✓ 所有Qt插件目录复制指令已正确添加到源代码中")
    else:
        print("\n✗ 部分Qt插件目录复制指令缺失")
        return False
    
    # 检查打包输出目录
    packages_dir = r"d:\virtualMachine\github\software-editor\smart_packages"
    
    if os.path.exists(packages_dir):
        print(f"\n✓ 找到打包输出目录: {packages_dir}")
        
        # 查找最新的打包目录
        package_dirs = [d for d in os.listdir(packages_dir) 
                       if os.path.isdir(os.path.join(packages_dir, d))]
        
        if package_dirs:
            latest_dir = max(package_dirs, key=lambda d: os.path.getmtime(os.path.join(packages_dir, d)))
            latest_path = os.path.join(packages_dir, latest_dir)
            
            print(f"✓ 最新打包目录: {latest_dir}")
            
            # 检查NSIS脚本
            nsis_script = os.path.join(latest_path, "installer.nsi")
            
            if os.path.exists(nsis_script):
                print(f"✓ 找到NSIS脚本: {nsis_script}")
                
                with open(nsis_script, 'r', encoding='utf-8') as f:
                    nsis_content = f.read()
                
                print("\n检查NSIS脚本中的Qt插件目录复制指令:")
                nsis_all_found = True
                for plugin_dir in qt_plugin_directories:
                    if f'SetOutPath "$INSTDIR\\{plugin_dir}"' in nsis_content:
                        print(f"  ✓ {plugin_dir}: 已包含SetOutPath指令")
                    else:
                        print(f"  ✗ {plugin_dir}: 未包含SetOutPath指令")
                        nsis_all_found = False
                        
                    if f'File "{plugin_dir}\\*.dll"' in nsis_content:
                        print(f"  ✓ {plugin_dir}: 已包含File指令")
                    else:
                        print(f"  ✗ {plugin_dir}: 未包含File指令")
                        nsis_all_found = False
                
                if nsis_all_found:
                    print("\n✓ NSIS脚本已包含所有Qt插件目录复制指令")
                    return True
                else:
                    print("\n✗ NSIS脚本中缺失部分Qt插件目录复制指令")
                    print("\n当前NSIS脚本内容:")
                    print("-" * 40)
                    print(nsis_content)
                    print("-" * 40)
                    
                    # 检查打包目录中的Qt插件文件
                    print("\n检查打包目录中的Qt插件文件:")
                    for plugin_dir in qt_plugin_directories:
                        plugin_path = os.path.join(latest_path, plugin_dir)
                        if os.path.exists(plugin_path):
                            files = os.listdir(plugin_path)
                            print(f"  ✓ {plugin_dir}: 包含 {len(files)} 个文件")
                            for f in files:
                                print(f"      - {f}")
                        else:
                            print(f"  ✗ {plugin_dir}: 目录不存在")
                    
                    return False
            else:
                print(f"✗ NSIS脚本不存在: {nsis_script}")
        else:
            print("✗ 打包目录为空")
    else:
        print(f"✗ 打包输出目录不存在: {packages_dir}")
    
    return False

def main():
    """主函数"""
    print("NSIS脚本生成验证")
    print("=" * 60)
    
    success = test_nsis_script_generation()
    
    print("\n" + "=" * 60)
    if success:
        print("✓ 验证成功: NSIS脚本已正确包含Qt插件目录复制指令")
    else:
        print("✗ 验证失败: NSIS脚本中缺失Qt插件目录复制指令")
        print("\n可能的原因:")
        print("1. 打包过程未使用修复后的代码")
        print("2. 需要重新编译程序")
        print("3. 需要重新执行打包测试")
    
    return 0 if success else 1

if __name__ == "__main__":
    sys.exit(main())