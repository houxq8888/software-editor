#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试NSIS安装包是否包含Qt插件目录
"""

import os
import subprocess
import tempfile
import shutil

def extract_nsis_installer(installer_path, extract_dir):
    """使用NSIS解包工具提取安装包内容"""
    try:
        # 尝试使用NSIS解包工具
        nsis_unpacker = r"C:\Program Files (x86)\NSIS\Uninst.exe"
        if os.path.exists(nsis_unpacker):
            cmd = [nsis_unpacker, installer_path, "/S", f"/D={extract_dir}"]
            result = subprocess.run(cmd, capture_output=True, text=True)
            if result.returncode == 0:
                return True
        
        # 如果NSIS解包工具不可用，尝试使用7z
        seven_zip_paths = [
            r"C:\Program Files\7-Zip\7z.exe",
            r"C:\Program Files (x86)\7-Zip\7z.exe"
        ]
        
        for seven_zip in seven_zip_paths:
            if os.path.exists(seven_zip):
                cmd = [seven_zip, "x", installer_path, f"-o{extract_dir}", "-y"]
                result = subprocess.run(cmd, capture_output=True, text=True)
                if result.returncode == 0:
                    return True
        
        return False
    except Exception as e:
        print(f"解包失败: {e}")
        return False

def check_qt_plugins_in_installer(installer_path):
    """检查NSIS安装包是否包含Qt插件目录"""
    with tempfile.TemporaryDirectory() as temp_dir:
        print(f"创建临时目录: {temp_dir}")
        
        if extract_nsis_installer(installer_path, temp_dir):
            print("安装包解包成功")
            
            # 检查解包后的目录结构
            for root, dirs, files in os.walk(temp_dir):
                print(f"目录: {root}")
                print(f"  子目录: {dirs}")
                print(f"  文件: {files[:10]}...")  # 只显示前10个文件
                
                # 检查Qt插件目录
                qt_plugin_dirs = ['platforms', 'styles', 'imageformats', 'iconengines']
                for plugin_dir in qt_plugin_dirs:
                    plugin_path = os.path.join(root, plugin_dir)
                    if os.path.exists(plugin_path):
                        print(f"✓ 找到Qt插件目录: {plugin_path}")
                        # 列出插件目录中的文件
                        plugin_files = os.listdir(plugin_path)
                        print(f"  包含文件: {plugin_files}")
                    else:
                        print(f"✗ 未找到Qt插件目录: {plugin_dir}")
                
                print("-" * 50)
            
            return True
        else:
            print("安装包解包失败")
            return False

def main():
    """主函数"""
    # NSIS安装包路径
    installer_path = r"d:\virtualMachine\github\software-editor\smart_packages\b3836d3c-f2c5-4dcb-9fc7-52f5b0617cde\b3836d3c-f2c5-4dcb-9fc7-52f5b0617cde_Setup.exe"
    
    if not os.path.exists(installer_path):
        print(f"安装包文件不存在: {installer_path}")
        return
    
    print(f"检查安装包: {installer_path}")
    print(f"文件大小: {os.path.getsize(installer_path)} 字节")
    print("=" * 60)
    
    # 检查安装包内容
    check_qt_plugins_in_installer(installer_path)
    
    print("=" * 60)
    print("检查完成")

if __name__ == "__main__":
    main()