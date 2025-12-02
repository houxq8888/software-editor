#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
软件打包构建脚本
用于将UI布局编辑器生成的软件打包成EXE和安装包
"""

import os
import sys
import json
import shutil
import subprocess
import tempfile
from pathlib import Path
from datetime import datetime

class PackageBuilder:
    """软件打包构建器"""
    
    def __init__(self, project_config):
        self.project_config = project_config
        self.build_dir = Path("build")
        self.dist_dir = Path("dist")
        self.temp_dir = Path(tempfile.mkdtemp())
        
    def build_exe(self):
        """构建EXE可执行文件"""
        print("开始构建EXE文件...")
        
        # 确保构建目录存在
        self.build_dir.mkdir(exist_ok=True)
        
        # 执行CMake构建
        try:
            # 配置项目
            subprocess.run(["cmake", "-S", ".", "-B", str(self.build_dir)], check=True)
            # 编译项目
            subprocess.run(["cmake", "--build", str(self.build_dir), "--config", "Release"], check=True)
            
            print("EXE构建完成")
            return True
        except subprocess.CalledProcessError as e:
            print(f"构建失败: {e}")
            return False
    
    def collect_dependencies(self):
        """收集依赖文件"""
        print("收集依赖文件...")
        
        # 创建临时目录用于收集依赖
        deps_dir = self.temp_dir / "dependencies"
        deps_dir.mkdir(exist_ok=True)
        
        # 复制Qt依赖库
        qt_deps = [
            "Qt6Core.dll", "Qt6Gui.dll", "Qt6Widgets.dll",
            "Qt6Network.dll", "Qt6Xml.dll", "Qt6Svg.dll"
        ]
        
        # 查找Qt安装目录
        qt_paths = [
            Path("C:/Qt/6.x.x/msvc2019_64/bin"),
            Path("C:/Qt/6.x.x/mingw_64/bin"),
            Path(os.environ.get("QTDIR", "")) / "bin"
        ]
        
        for qt_path in qt_paths:
            if qt_path.exists():
                for dep in qt_deps:
                    dep_path = qt_path / dep
                    if dep_path.exists():
                        shutil.copy2(dep_path, deps_dir)
                        print(f"复制依赖: {dep}")
        
        # 复制平台插件
        platforms_dir = deps_dir / "platforms"
        platforms_dir.mkdir(exist_ok=True)
        
        for qt_path in qt_paths:
            if qt_path.exists():
                platform_plugin = qt_path.parent / "plugins" / "platforms" / "qwindows.dll"
                if platform_plugin.exists():
                    shutil.copy2(platform_plugin, platforms_dir)
                    print("复制平台插件: qwindows.dll")
        
        return deps_dir
    
    def create_installer(self, exe_path):
        """创建安装包"""
        print("创建安装包...")
        
        # 确保分发目录存在
        self.dist_dir.mkdir(exist_ok=True)
        
        # 创建NSIS安装脚本
        nsis_script = self._create_nsis_script(exe_path)
        
        # 执行NSIS编译
        try:
            nsis_path = Path("C:/Program Files (x86)/NSIS/makensis.exe")
            if nsis_path.exists():
                subprocess.run([str(nsis_script)], check=True)
                print("安装包创建完成")
                return True
            else:
                print("NSIS未安装，跳过安装包创建")
                return False
        except subprocess.CalledProcessError as e:
            print(f"安装包创建失败: {e}")
            return False
    
    def _create_nsis_script(self, exe_path):
        """创建NSIS安装脚本"""
        config = self.project_config
        
        script_content = f"""
; NSIS安装脚本
Name "{config.get('name', '软件编辑器')}"
OutFile "{self.dist_dir / (config.get('name', 'software-editor') + '_setup.exe')}"
InstallDir "$PROGRAMFILES\\{config.get('developer', 'Unknown')}\\{config.get('name', '软件编辑器')}"

; 现代UI
!include "MUI2.nsh"

; 界面设置
!define MUI_ABORTWARNING
!define MUI_ICON "${{NSISDIR}}\\Contrib\\Graphics\\Icons\\modern-install.ico"

; 页面
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "license.txt"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_LANGUAGE "SimpChinese"

Section "主程序"
    SetOutPath "$INSTDIR"
    
    ; 复制可执行文件
    File "{exe_path}"
    
    ; 复制依赖文件
    File /r "{self.temp_dir / 'dependencies'}\\*.*"
    
    ; 创建开始菜单快捷方式
    CreateShortcut "$SMPROGRAMS\\{config.get('name', '软件编辑器')}.lnk" "$INSTDIR\\{exe_path.name}"
    
    ; 创建桌面快捷方式
    CreateShortcut "$DESKTOP\\{config.get('name', '软件编辑器')}.lnk" "$INSTDIR\\{exe_path.name}"
    
    ; 写入卸载信息
    WriteUninstaller "$INSTDIR\\uninstall.exe"
    WriteRegStr HKLM "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{config.get('name', '软件编辑器')}" \
                     "DisplayName" "{config.get('name', '软件编辑器')}"
    WriteRegStr HKLM "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{config.get('name', '软件编辑器')}" \
                     "UninstallString" "$INSTDIR\\uninstall.exe"
SectionEnd

Section "Uninstall"
    Delete "$INSTDIR\\*.*"
    RMDir /r "$INSTDIR"
    
    Delete "$SMPROGRAMS\\{config.get('name', '软件编辑器')}.lnk"
    Delete "$DESKTOP\\{config.get('name', '软件编辑器')}.lnk"
    
    DeleteRegKey HKLM "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{config.get('name', '软件编辑器')}"
SectionEnd
"""
        
        script_path = self.temp_dir / "installer.nsi"
        with open(script_path, 'w', encoding='utf-8') as f:
            f.write(script_content)
        
        # 创建许可证文件
        license_content = "软件许可证信息\n================\n\n请在此处添加软件许可证信息。"
        license_path = self.temp_dir / "license.txt"
        with open(license_path, 'w', encoding='utf-8') as f:
            f.write(license_content)
        
        return script_path
    
    def package_software(self):
        """打包软件"""
        print("开始打包软件...")
        
        # 构建EXE
        if not self.build_exe():
            return False
        
        # 收集依赖
        deps_dir = self.collect_dependencies()
        
        # 复制EXE到分发目录
        exe_name = self.project_config.get('name', 'software-editor') + '.exe'
        exe_source = self.build_dir / "Release" / "software-editor.exe"
        exe_dest = self.dist_dir / exe_name
        
        if exe_source.exists():
            shutil.copy2(exe_source, exe_dest)
            
            # 复制依赖到分发目录
            for item in deps_dir.iterdir():
                if item.is_dir():
                    shutil.copytree(item, self.dist_dir / item.name, dirs_exist_ok=True)
                else:
                    shutil.copy2(item, self.dist_dir)
            
            # 创建安装包
            self.create_installer(exe_dest)
            
            print(f"打包完成！文件位于: {self.dist_dir}")
            return True
        else:
            print("EXE文件未找到")
            return False
    
    def cleanup(self):
        """清理临时文件"""
        if self.temp_dir.exists():
            shutil.rmtree(self.temp_dir)

if __name__ == "__main__":
    # 测试配置
    test_config = {
        "name": "软件编辑器",
        "version": "1.0.0",
        "developer": "软件工作室",
        "description": "专业的软件UI布局编辑器"
    }
    
    builder = PackageBuilder(test_config)
    try:
        if builder.package_software():
            print("打包成功！")
        else:
            print("打包失败！")
    finally:
        builder.cleanup()