#!/usr/bin/env python3
"""
自动化打包脚本 - 支持EXE生成、依赖复制和安装包创建
功能：
1. 自动构建软件项目
2. 生成EXE可执行文件
3. 复制运行依赖项
4. 创建NSIS安装包
"""

import os
import sys
import subprocess
import shutil
import argparse
import json
from pathlib import Path
from datetime import datetime

class AutoPackageManager:
    def __init__(self, project_root):
        self.project_root = Path(project_root)
        self.build_dir = self.project_root / "build"
        self.packages_dir = self.project_root / "packages"
        self.dependencies_dir = self.project_root / "dependencies"
        
        # 确保目录存在
        self.packages_dir.mkdir(exist_ok=True)
        self.dependencies_dir.mkdir(exist_ok=True)
        
        # Qt安装路径（需要根据实际安装路径修改）
        self.qt_dir = Path("D:/Qt/6.9.1/mingw_64")
        self.mingw_dir = Path("D:/Qt/Tools/mingw1310_64")
        self.cmake_dir = Path("D:/Qt/Tools/CMake_64/bin")
        
    def log(self, message, level="INFO"):
        """日志输出"""
        timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        print(f"[{timestamp}] [{level}] {message}")
    
    def run_command(self, cmd, cwd=None, timeout=300):
        """运行命令并检查结果"""
        self.log(f"执行命令: {' '.join(cmd)}")
        
        try:
            result = subprocess.run(
                cmd, 
                cwd=cwd or self.project_root,
                capture_output=True, 
                text=True, 
                timeout=timeout,
                encoding='utf-8'
            )
            
            if result.returncode != 0:
                self.log(f"命令执行失败: {result.stderr}", "ERROR")
                return False
            
            self.log("命令执行成功")
            return True
            
        except subprocess.TimeoutExpired:
            self.log("命令执行超时", "ERROR")
            return False
        except Exception as e:
            self.log(f"命令执行异常: {e}", "ERROR")
            return False
    
    def setup_environment(self):
        """设置构建环境"""
        self.log("设置构建环境...")
        
        # 添加必要的路径到环境变量
        env_path = os.environ.get('PATH', '')
        new_paths = [
            str(self.mingw_dir / "bin"),
            str(self.cmake_dir),
            str(self.qt_dir / "bin")
        ]
        
        # 避免重复添加路径
        for path in new_paths:
            if path not in env_path:
                env_path = path + ";" + env_path
        
        os.environ['PATH'] = env_path
        
        # 设置Qt相关环境变量
        os.environ['QT_DIR'] = str(self.qt_dir)
        os.environ['MINGW_DIR'] = str(self.mingw_dir)
        
        self.log("环境设置完成")
        return True
    
    def build_project(self):
        """构建项目"""
        self.log("开始构建项目...")
        
        # 确保构建目录存在
        self.build_dir.mkdir(exist_ok=True)
        
        # 运行CMake配置
        cmake_cmd = [
            str(self.cmake_dir / "cmake.exe"),
            "-G", "MinGW Makefiles",
            f"-DCMAKE_PREFIX_PATH={self.qt_dir}",
            "-DCMAKE_BUILD_TYPE=Release",
            "-DCMAKE_C_COMPILER=gcc",
            "-DCMAKE_CXX_COMPILER=g++",
            "-DFEATURE_clang=OFF",
            ".."
        ]
        
        if not self.run_command(cmake_cmd, cwd=self.build_dir):
            return False
        
        # 使用mingw32-make构建
        build_cmd = ["mingw32-make"]
        if not self.run_command(build_cmd, cwd=self.build_dir):
            return False
        
        # 检查EXE文件是否生成
        exe_path = self.build_dir / "software-editor.exe"
        if not exe_path.exists():
            self.log("未找到生成的可执行文件", "ERROR")
            return False
        
        self.log(f"项目构建成功，EXE文件: {exe_path}")
        return True
    
    def find_qt_dependencies(self, exe_path):
        """查找Qt依赖文件"""
        self.log("查找Qt依赖文件...")
        
        dependencies = []
        
        # 基本的Qt DLL文件
        qt_dlls = [
            "Qt6Core.dll", "Qt6Gui.dll", "Qt6Widgets.dll",
            "Qt6Network.dll", "Qt6Xml.dll", "Qt6Svg.dll"
        ]
        
        for dll in qt_dlls:
            dll_path = self.qt_dir / "bin" / dll
            if dll_path.exists():
                dependencies.append(dll_path)
        
        # 平台插件
        plugins_dir = self.qt_dir / "plugins"
        platform_plugin = plugins_dir / "platforms" / "qwindows.dll"
        if platform_plugin.exists():
            dependencies.append(platform_plugin)
        
        # 样式插件
        styles_plugin = plugins_dir / "styles" / "qwindowsvistastyle.dll"
        if styles_plugin.exists():
            dependencies.append(styles_plugin)
        
        # 图像格式插件
        imageformats_dir = plugins_dir / "imageformats"
        for fmt in ["qjpeg.dll", "qgif.dll", "qsvg.dll", "qico.dll"]:
            fmt_path = imageformats_dir / fmt
            if fmt_path.exists():
                dependencies.append(fmt_path)
        
        self.log(f"找到 {len(dependencies)} 个依赖文件")
        return dependencies
    
    def copy_dependencies(self, exe_path, target_dir):
        """复制依赖文件到目标目录"""
        self.log("复制依赖文件...")
        
        # 确保目标目录存在
        target_dir = Path(target_dir)
        target_dir.mkdir(exist_ok=True)
        
        # 复制EXE文件
        shutil.copy2(exe_path, target_dir / exe_path.name)
        
        # 查找并复制Qt依赖
        dependencies = self.find_qt_dependencies(exe_path)
        
        for dep in dependencies:
            # 保持目录结构
            if "plugins" in str(dep):
                # 处理插件文件
                rel_path = dep.relative_to(self.qt_dir)
                target_path = target_dir / rel_path
                target_path.parent.mkdir(parents=True, exist_ok=True)
                shutil.copy2(dep, target_path)
            else:
                # 直接复制DLL文件
                shutil.copy2(dep, target_dir / dep.name)
        
        # 复制必要的运行时文件
        runtime_files = [
            self.mingw_dir / "bin" / "libgcc_s_seh-1.dll",
            self.mingw_dir / "bin" / "libstdc++-6.dll",
            self.mingw_dir / "bin" / "libwinpthread-1.dll"
        ]
        
        for rt_file in runtime_files:
            if rt_file.exists():
                shutil.copy2(rt_file, target_dir / rt_file.name)
        
        self.log("依赖文件复制完成")
        return True
    
    def create_nsis_script(self, package_name, exe_path, output_dir):
        """创建NSIS安装脚本"""
        self.log("创建NSIS安装脚本...")
        
        nsis_script = f"""
; NSIS安装脚本 - 自动生成
; 软件名称: {package_name}
; 生成时间: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}

!include "MUI2.nsh"

; 基本设置
Name "{package_name}"
OutFile "{package_name}_setup.exe"
InstallDir "$PROGRAMFILES\\{package_name}"
InstallDirRegKey HKLM "Software\\{package_name}" "Install_Dir"

; 请求管理员权限
RequestExecutionLevel admin

; 界面设置
!define MUI_ABORTWARNING
!define MUI_ICON "${{NSISDIR}}\\Contrib\\Graphics\\Icons\\modern-install.ico"

; 页面
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

; 语言
!insertmacro MUI_LANGUAGE "SimpChinese"

Section "主程序"
    SetOutPath $INSTDIR
    
    ; 复制所有文件
    File /r "{output_dir}\\*"
    
    ; 创建开始菜单快捷方式
    CreateDirectory "$SMPROGRAMS\\{package_name}"
    CreateShortCut "$SMPROGRAMS\\{package_name}\\{package_name}.lnk" "$INSTDIR\\software-editor.exe"
    
    ; 创建桌面快捷方式
    CreateShortCut "$DESKTOP\\{package_name}.lnk" "$INSTDIR\\software-editor.exe"
    
    ; 写入注册表信息
    WriteRegStr HKLM "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{package_name}" \
                     "DisplayName" "{package_name}"
    WriteRegStr HKLM "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{package_name}" \
                     "UninstallString" "$\"$INSTDIR\\uninstall.exe$\""
    WriteRegStr HKLM "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{package_name}" \
                     "DisplayIcon" "$INSTDIR\\software-editor.exe"
    WriteRegStr HKLM "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{package_name}" \
                     "Publisher" "Software Editor"
    WriteRegDWORD HKLM "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{package_name}" \
                     "NoModify" 1
    WriteRegDWORD HKLM "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{package_name}" \
                     "NoRepair" 1
    
    ; 创建卸载程序
    WriteUninstaller "$INSTDIR\\uninstall.exe"
SectionEnd

Section "Uninstall"
    ; 删除开始菜单快捷方式
    Delete "$SMPROGRAMS\\{package_name}\\{package_name}.lnk"
    RMDir "$SMPROGRAMS\\{package_name}"
    
    ; 删除桌面快捷方式
    Delete "$DESKTOP\\{package_name}.lnk"
    
    ; 删除安装目录
    RMDir /r "$INSTDIR"
    
    ; 删除注册表信息
    DeleteRegKey HKLM "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{package_name}"
    DeleteRegKey HKLM "Software\\{package_name}"
SectionEnd
"""
        
        nsis_file = self.packages_dir / f"{package_name}.nsi"
        with open(nsis_file, 'w', encoding='utf-8') as f:
            f.write(nsis_script)
        
        self.log(f"NSIS脚本已创建: {nsis_file}")
        return nsis_file
    
    def create_installer(self, package_name, exe_path, output_dir):
        """创建安装包"""
        self.log("创建安装包...")
        
        # 创建NSIS脚本
        nsis_script = self.create_nsis_script(package_name, exe_path, output_dir)
        
        # 检查NSIS是否安装
        nsis_path = Path("C:/Program Files (x86)/NSIS/makensis.exe")
        if not nsis_path.exists():
            nsis_path = Path("C:/Program Files/NSIS/makensis.exe")
        
        if not nsis_path.exists():
            self.log("NSIS未安装，跳过安装包创建", "WARNING")
            return False
        
        # 运行NSIS编译
        nsis_cmd = [str(nsis_path), str(nsis_script)]
        if not self.run_command(nsis_cmd, cwd=self.packages_dir):
            self.log("NSIS编译失败", "WARNING")
            return False
        
        # 检查安装包是否生成
        installer_path = self.packages_dir / f"{package_name}_setup.exe"
        if installer_path.exists():
            self.log(f"安装包创建成功: {installer_path}")
            return True
        else:
            self.log("安装包生成失败", "WARNING")
            return False
    
    def package_software(self, package_name="SoftwareEditor"):
        """完整的打包流程"""
        self.log(f"开始打包软件: {package_name}")
        
        # 1. 设置环境
        if not self.setup_environment():
            return False
        
        # 2. 构建项目
        if not self.build_project():
            return False
        
        # 3. 准备打包目录
        package_dir = self.packages_dir / package_name
        package_dir.mkdir(exist_ok=True)
        
        # 4. 复制依赖文件
        exe_path = self.build_dir / "software-editor.exe"
        if not self.copy_dependencies(exe_path, package_dir):
            return False
        
        # 5. 创建安装包
        if not self.create_installer(package_name, exe_path, package_dir):
            self.log("安装包创建失败，但便携版打包完成", "WARNING")
        
        self.log("打包流程完成")
        return True

def main():
    parser = argparse.ArgumentParser(description='自动化打包脚本')
    parser.add_argument('--project-root', default='.', help='项目根目录路径')
    parser.add_argument('--package-name', default='SoftwareEditor', help='打包名称')
    parser.add_argument('--skip-installer', action='store_true', help='跳过安装包创建')
    
    args = parser.parse_args()
    
    # 创建打包管理器
    package_manager = AutoPackageManager(args.project_root)
    
    # 执行打包
    success = package_manager.package_software(args.package_name)
    
    if success:
        print("\n✅ 打包成功完成！")
        print(f"📦 打包目录: {package_manager.packages_dir / args.package_name}")
        print(f"🔧 安装包: {package_manager.packages_dir / (args.package_name + '_setup.exe')}")
        sys.exit(0)
    else:
        print("\n❌ 打包失败")
        sys.exit(1)

if __name__ == "__main__":
    main()