#!/usr/bin/env python3
"""
NSIS工具下载脚本
下载并安装NSIS工具到build目录
"""

import os
import sys
import urllib.request
import zipfile
import tempfile
import shutil
from pathlib import Path

def download_nsis():
    """下载NSIS工具"""
    print("开始下载NSIS工具...")
    
    # NSIS官方下载链接（使用稳定版本）
    nsis_url = "https://sourceforge.net/projects/nsis/files/NSIS%203/3.09/nsis-3.09.zip/download"
    
    # 临时目录
    temp_dir = Path(tempfile.gettempdir()) / "nsis_download"
    temp_dir.mkdir(exist_ok=True)
    
    # 下载文件
    zip_path = temp_dir / "nsis.zip"
    
    try:
        print(f"正在下载NSIS工具...")
        urllib.request.urlretrieve(nsis_url, zip_path)
        print("下载完成")
        
        # 解压文件
        print("正在解压NSIS工具...")
        with zipfile.ZipFile(zip_path, 'r') as zip_ref:
            zip_ref.extractall(temp_dir)
        
        # 查找makensis.exe
        nsis_dir = temp_dir / "nsis-3.09"
        if not nsis_dir.exists():
            # 尝试查找其他可能的目录结构
            for item in temp_dir.iterdir():
                if item.is_dir() and "nsis" in item.name.lower():
                    nsis_dir = item
                    break
        
        if not nsis_dir.exists():
            print("❌ 无法找到NSIS解压目录")
            return False
        
        # 查找makensis.exe
        makensis_path = None
        for root, dirs, files in os.walk(nsis_dir):
            if "makensis.exe" in files:
                makensis_path = Path(root) / "makensis.exe"
                break
        
        if not makensis_path or not makensis_path.exists():
            print("❌ 无法找到makensis.exe")
            return False
        
        # 复制到build目录
        build_dir = Path("d:/virtualMachine/github/software-editor/build")
        target_path = build_dir / "makensis.exe"
        
        # 备份原有文件（如果存在）
        if target_path.exists():
            backup_path = build_dir / "makensis.exe.backup"
            shutil.copy2(target_path, backup_path)
            print(f"✅ 已备份原有文件到: {backup_path}")
        
        # 复制新文件
        shutil.copy2(makensis_path, target_path)
        print(f"✅ NSIS工具已安装到: {target_path}")
        
        # 清理临时文件
        shutil.rmtree(temp_dir)
        
        return True
        
    except Exception as e:
        print(f"❌ 下载或安装NSIS工具失败: {e}")
        # 清理临时文件
        if temp_dir.exists():
            shutil.rmtree(temp_dir)
        return False

def download_portable_nsis():
    """下载便携版NSIS工具"""
    print("尝试下载便携版NSIS工具...")
    
    # 便携版NSIS下载链接
    portable_url = "https://github.com/nsis-share/NSISPortable/releases/download/3.09/NSISPortable_3.09.paf.exe"
    
    temp_dir = Path(tempfile.gettempdir()) / "nsis_portable"
    temp_dir.mkdir(exist_ok=True)
    
    exe_path = temp_dir / "NSISPortable.exe"
    
    try:
        print("正在下载便携版NSIS...")
        urllib.request.urlretrieve(portable_url, exe_path)
        print("下载完成")
        
        # 便携版需要解压，这里我们直接使用7z或类似工具
        # 由于便携版比较复杂，我们使用第一种方法
        print("便携版下载成功，但推荐使用标准版")
        return False
        
    except Exception as e:
        print(f"便携版下载失败: {e}")
        return False

def main():
    """主函数"""
    print("=" * 50)
    print("NSIS工具下载脚本")
    print("=" * 50)
    
    # 检查build目录
    build_dir = Path("d:/virtualMachine/github/software-editor/build")
    if not build_dir.exists():
        print("❌ build目录不存在，请先编译项目")
        return 1
    
    # 尝试下载标准版NSIS
    if download_nsis():
        print("\n✅ NSIS工具安装成功！")
        print("现在可以重新启动软件编辑器测试NSIS功能")
        return 0
    
    print("\n❌ NSIS工具安装失败")
    print("请手动下载NSIS工具：")
    print("1. 访问 https://nsis.sourceforge.net/Download")
    print("2. 下载NSIS 3.09或更新版本")
    print("3. 安装后，将makensis.exe复制到build目录")
    print("4. 或者将NSIS安装目录添加到系统PATH环境变量")
    
    return 1

if __name__ == "__main__":
    sys.exit(main())