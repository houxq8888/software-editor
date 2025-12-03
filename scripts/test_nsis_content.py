#!/usr/bin/env python3
"""
测试NSIS安装包是否包含Qt插件目录
"""

import os
import subprocess
import tempfile
import zipfile

def extract_nsis_installer(installer_path, extract_dir):
    """
    使用7-Zip提取NSIS安装包内容
    """
    # 检查7-Zip是否可用
    try:
        result = subprocess.run(['7z', '--help'], capture_output=True, text=True)
        if result.returncode != 0:
            print("7-Zip不可用，无法提取NSIS安装包")
            return False
    except FileNotFoundError:
        print("7-Zip未安装，请安装7-Zip来提取NSIS安装包")
        return False
    
    # 提取NSIS安装包
    cmd = ['7z', 'x', installer_path, f'-o{extract_dir}', '-y']
    result = subprocess.run(cmd, capture_output=True, text=True)
    
    if result.returncode != 0:
        print(f"提取NSIS安装包失败: {result.stderr}")
        return False
    
    return True

def check_qt_plugins_in_installer(installer_path):
    """
    检查NSIS安装包是否包含Qt插件目录
    """
    with tempfile.TemporaryDirectory() as temp_dir:
        print(f"使用临时目录: {temp_dir}")
        
        # 提取NSIS安装包
        if not extract_nsis_installer(installer_path, temp_dir):
            return False
        
        # 检查提取的内容
        print("\n=== 检查提取的安装包内容 ===")
        
        qt_plugin_dirs = ['platforms', 'styles', 'imageformats', 'iconengines']
        missing_dirs = []
        
        for plugin_dir in qt_plugin_dirs:
            plugin_path = os.path.join(temp_dir, plugin_dir)
            if os.path.exists(plugin_path) and os.path.isdir(plugin_path):
                files = os.listdir(plugin_path)
                print(f"✓ {plugin_dir}: 包含 {len(files)} 个文件")
                for file in files:
                    print(f"    - {file}")
            else:
                print(f"✗ {plugin_dir}: 缺失")
                missing_dirs.append(plugin_dir)
        
        # 检查主程序
        exe_files = [f for f in os.listdir(temp_dir) if f.endswith('.exe') and not f.startswith('Uninstall')]
        if exe_files:
            print(f"\n✓ 主程序: {exe_files[0]}")
        else:
            print("\n✗ 主程序: 缺失")
        
        # 检查DLL文件
        dll_files = [f for f in os.listdir(temp_dir) if f.endswith('.dll')]
        print(f"✓ DLL文件: {len(dll_files)} 个")
        
        if missing_dirs:
            print(f"\n❌ 缺失的Qt插件目录: {missing_dirs}")
            return False
        else:
            print("\n✅ 所有Qt插件目录都已包含在安装包中")
            return True

def main():
    # 检查build目录中的NSIS安装包
    build_dir = "build"
    
    if not os.path.exists(build_dir):
        print("build目录不存在")
        return
    
    # 查找NSIS安装包
    nsis_installers = [f for f in os.listdir(build_dir) if f.endswith('_Setup.exe')]
    
    if not nsis_installers:
        print("未找到NSIS安装包 (*_Setup.exe)")
        print("请先运行软件编辑器并生成安装包")
        return
    
    print(f"找到NSIS安装包: {nsis_installers}")
    
    for installer in nsis_installers:
        installer_path = os.path.join(build_dir, installer)
        print(f"\n=== 检查安装包: {installer} ===")
        
        if not os.path.exists(installer_path):
            print("安装包文件不存在")
            continue
        
        file_size = os.path.getsize(installer_path)
        print(f"安装包大小: {file_size / (1024*1024):.2f} MB")
        
        # 检查安装包内容
        check_qt_plugins_in_installer(installer_path)

if __name__ == "__main__":
    main()