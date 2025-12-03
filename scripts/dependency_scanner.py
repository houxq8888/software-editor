#!/usr/bin/env python3
"""
智能依赖检测工具 - 自动扫描和复制EXE文件的所有依赖项
功能：
1. 扫描EXE文件的动态链接库依赖
2. 自动查找Qt、MinGW等运行时依赖
3. 复制必要的插件和资源文件
4. 生成依赖报告
"""

import os
import sys
import subprocess
import shutil
import re
from pathlib import Path
from typing import List, Set, Dict

class DependencyScanner:
    def __init__(self, project_root):
        self.project_root = Path(project_root)
        
        # 默认路径配置
        self.qt_paths = [
            Path("D:/Qt/6.9.1/mingw_64"),
            Path("C:/Qt/6.9.1/mingw_64"),
            Path(os.environ.get('QT_DIR', ''))
        ]
        
        self.mingw_paths = [
            Path("D:/Qt/Tools/mingw1310_64"),
            Path("C:/Qt/Tools/mingw1310_64"),
            Path(os.environ.get('MINGW_DIR', ''))
        ]
        
        # 检测到的路径
        self.qt_dir = None
        self.mingw_dir = None
        
        # 依赖缓存
        self.dependencies = set()
        
    def detect_qt_path(self):
        """自动检测Qt安装路径"""
        for path in self.qt_paths:
            if path.exists() and (path / "bin").exists():
                self.qt_dir = path
                print(f"✅ 检测到Qt路径: {self.qt_dir}")
                return True
        
        # 尝试通过环境变量检测
        for key, value in os.environ.items():
            if "QT" in key and "DIR" in key and Path(value).exists():
                self.qt_dir = Path(value)
                print(f"✅ 通过环境变量检测到Qt路径: {self.qt_dir}")
                return True
        
        print("❌ 未检测到Qt安装路径")
        return False
    
    def detect_mingw_path(self):
        """自动检测MinGW安装路径"""
        for path in self.mingw_paths:
            if path.exists() and (path / "bin").exists():
                self.mingw_dir = path
                print(f"✅ 检测到MinGW路径: {self.mingw_dir}")
                return True
        
        # 尝试通过PATH检测
        path_env = os.environ.get('PATH', '')
        for path_str in path_env.split(';'):
            path = Path(path_str)
            if path.exists() and path.name == "mingw1310_64":
                self.mingw_dir = path
                print(f"✅ 通过PATH检测到MinGW路径: {self.mingw_dir}")
                return True
        
        print("❌ 未检测到MinGW安装路径")
        return False
    
    def scan_exe_dependencies(self, exe_path: Path) -> List[Path]:
        """扫描EXE文件的动态链接库依赖"""
        if not exe_path.exists():
            print(f"❌ EXE文件不存在: {exe_path}")
            return []
        
        print(f"🔍 扫描EXE依赖: {exe_path.name}")
        
        # 使用objdump或dumpbin工具分析依赖
        dependencies = set()
        
        # 方法1: 使用objdump (MinGW)
        try:
            result = subprocess.run(
                ["objdump", "-p", str(exe_path)],
                capture_output=True, text=True, encoding='utf-8'
            )
            
            if result.returncode == 0:
                # 解析DLL依赖
                for line in result.stdout.split('\n'):
                    if "DLL Name:" in line:
                        dll_name = line.split("DLL Name:")[1].strip()
                        dependencies.add(dll_name)
        except:
            pass
        
        # 方法2: 使用dumpbin (Visual Studio)
        try:
            result = subprocess.run(
                ["dumpbin", "/dependents", str(exe_path)],
                capture_output=True, text=True, encoding='utf-8'
            )
            
            if result.returncode == 0:
                # 解析依赖信息
                for line in result.stdout.split('\n'):
                    if ".dll" in line.lower():
                        dll_name = line.strip()
                        if dll_name.endswith('.dll'):
                            dependencies.add(dll_name)
        except:
            pass
        
        print(f"📋 发现 {len(dependencies)} 个直接依赖")
        return list(dependencies)
    
    def find_dll_file(self, dll_name: str) -> Path:
        """查找DLL文件的实际路径"""
        # 搜索路径优先级
        search_paths = []
        
        if self.qt_dir:
            search_paths.append(self.qt_dir / "bin")
        
        if self.mingw_dir:
            search_paths.append(self.mingw_dir / "bin")
        
        # 系统路径
        system_paths = [
            Path("C:/Windows/System32"),
            Path("C:/Windows/SysWOW64"),
        ]
        
        search_paths.extend(system_paths)
        
        # 搜索DLL文件
        for search_path in search_paths:
            if search_path.exists():
                dll_path = search_path / dll_name
                if dll_path.exists():
                    return dll_path
        
        return None
    
    def get_qt_plugins(self) -> List[Path]:
        """获取必要的Qt插件"""
        if not self.qt_dir:
            return []
        
        plugins_dir = self.qt_dir / "plugins"
        if not plugins_dir.exists():
            return []
        
        essential_plugins = [
            # 平台插件
            ("platforms", "qwindows.dll"),
            # 样式插件
            ("styles", "qwindowsvistastyle.dll"),
            # 图像格式插件
            ("imageformats", "qjpeg.dll"),
            ("imageformats", "qgif.dll"),
            ("imageformats", "qsvg.dll"),
            ("imageformats", "qico.dll"),
            ("imageformats", "qpng.dll"),
            # 其他可能需要的插件
            ("iconengines", "qsvgicon.dll"),
        ]
        
        plugins = []
        for subdir, plugin_name in essential_plugins:
            plugin_path = plugins_dir / subdir / plugin_name
            if plugin_path.exists():
                plugins.append(plugin_path)
        
        return plugins
    
    def get_mingw_runtime(self) -> List[Path]:
        """获取MinGW运行时文件"""
        if not self.mingw_dir:
            return []
        
        runtime_files = [
            "libgcc_s_seh-1.dll",
            "libstdc++-6.dll", 
            "libwinpthread-1.dll",
        ]
        
        runtime_paths = []
        for rt_file in runtime_files:
            rt_path = self.mingw_dir / "bin" / rt_file
            if rt_path.exists():
                runtime_paths.append(rt_path)
        
        return runtime_paths
    
    def scan_all_dependencies(self, exe_path: Path) -> Set[Path]:
        """扫描所有依赖（包括递归依赖）"""
        print("🔍 开始深度依赖扫描...")
        
        all_dependencies = set()
        to_scan = {exe_path}
        scanned = set()
        
        while to_scan:
            current_file = to_scan.pop()
            
            if current_file in scanned:
                continue
            
            scanned.add(current_file)
            
            # 扫描当前文件的依赖
            if current_file.suffix.lower() in ['.exe', '.dll']:
                dependencies = self.scan_exe_dependencies(current_file)
                
                for dll_name in dependencies:
                    dll_path = self.find_dll_file(dll_name)
                    
                    if dll_path and dll_path not in scanned:
                        all_dependencies.add(dll_path)
                        to_scan.add(dll_path)
        
        # 添加Qt插件
        qt_plugins = self.get_qt_plugins()
        all_dependencies.update(qt_plugins)
        
        # 添加MinGW运行时
        mingw_runtime = self.get_mingw_runtime()
        all_dependencies.update(mingw_runtime)
        
        print(f"📊 总共发现 {len(all_dependencies)} 个依赖文件")
        return all_dependencies
    
    def copy_dependencies(self, dependencies: Set[Path], target_dir: Path):
        """复制依赖文件到目标目录"""
        print(f"📂 复制依赖文件到: {target_dir}")
        
        target_dir.mkdir(parents=True, exist_ok=True)
        
        # 复制主程序
        for dep in dependencies:
            if dep.suffix.lower() == '.exe':
                shutil.copy2(dep, target_dir / dep.name)
        
        # 复制DLL文件
        for dep in dependencies:
            if dep.suffix.lower() == '.dll':
                shutil.copy2(dep, target_dir / dep.name)
        
        # 复制插件（保持目录结构）
        for dep in dependencies:
            if "plugins" in str(dep):
                # 计算相对路径
                if self.qt_dir and str(dep).startswith(str(self.qt_dir)):
                    rel_path = dep.relative_to(self.qt_dir)
                    target_path = target_dir / rel_path
                    target_path.parent.mkdir(parents=True, exist_ok=True)
                    shutil.copy2(dep, target_path)
        
        print("✅ 依赖文件复制完成")
    
    def generate_dependency_report(self, dependencies: Set[Path], report_file: Path):
        """生成依赖报告"""
        print(f"📄 生成依赖报告: {report_file}")
        
        report_content = ["# 依赖分析报告\n"]
        report_content.append(f"生成时间: {os.popen('date /t').read().strip()}")
        report_content.append(f"目标文件: {list(dependencies)[0] if dependencies else 'N/A'}")
        report_content.append(f"依赖总数: {len(dependencies)}")
        report_content.append("\n## 依赖文件列表\n")
        
        # 分类统计
        dll_files = []
        exe_files = []
        plugin_files = []
        
        for dep in sorted(dependencies):
            if dep.suffix.lower() == '.dll':
                dll_files.append(dep)
            elif dep.suffix.lower() == '.exe':
                exe_files.append(dep)
            elif "plugins" in str(dep):
                plugin_files.append(dep)
        
        report_content.append(f"- EXE文件: {len(exe_files)} 个")
        report_content.append(f"- DLL文件: {len(dll_files)} 个")
        report_content.append(f"- 插件文件: {len(plugin_files)} 个")
        
        report_content.append("\n### 详细文件列表\n")
        
        for dep in sorted(dependencies):
            size = dep.stat().st_size if dep.exists() else 0
            size_kb = size / 1024
            report_content.append(f"- `{dep.name}` ({size_kb:.1f} KB) - {dep}")
        
        with open(report_file, 'w', encoding='utf-8') as f:
            f.write('\n'.join(report_content))
        
        print("✅ 依赖报告生成完成")
    
    def scan_and_copy(self, exe_path: Path, target_dir: Path):
        """完整的扫描和复制流程"""
        print("🚀 开始智能依赖扫描和复制")
        
        # 1. 检测环境
        if not self.detect_qt_path():
            return False
        
        if not self.detect_mingw_path():
            return False
        
        # 2. 扫描依赖
        dependencies = self.scan_all_dependencies(exe_path)
        
        if not dependencies:
            print("❌ 未发现任何依赖文件")
            return False
        
        # 3. 复制依赖
        self.copy_dependencies(dependencies, target_dir)
        
        # 4. 生成报告
        report_file = target_dir / "dependency_report.md"
        self.generate_dependency_report(dependencies, report_file)
        
        print("🎉 智能依赖扫描和复制完成")
        return True

def main():
    if len(sys.argv) != 3:
        print("用法: python dependency_scanner.py <exe文件路径> <目标目录>")
        sys.exit(1)
    
    exe_path = Path(sys.argv[1])
    target_dir = Path(sys.argv[2])
    
    scanner = DependencyScanner(".")
    success = scanner.scan_and_copy(exe_path, target_dir)
    
    if success:
        print(f"\n✅ 依赖处理完成")
        print(f"📦 目标目录: {target_dir}")
        print(f"📄 报告文件: {target_dir / 'dependency_report.md'}")
        sys.exit(0)
    else:
        print("\n❌ 依赖处理失败")
        sys.exit(1)

if __name__ == "__main__":
    main()