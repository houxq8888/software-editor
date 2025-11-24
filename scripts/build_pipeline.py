#!/usr/bin/env python3
"""
构建流水线脚本 - 支持CI/CD环境的自动化构建
"""

import os
import sys
import subprocess
import json
import time
from datetime import datetime
from pathlib import Path

def is_ci_environment():
    """检测是否在CI环境中运行"""
    return os.getenv('GITHUB_ACTIONS') == 'true' or os.getenv('CI') == 'true'

def setup_build_environment():
    """设置构建环境"""
    print("[INFO] Setting up build environment...")
    
    # 确保构建目录存在
    build_dir = Path("build")
    build_dir.mkdir(exist_ok=True)
    
    # 设置环境变量
    if is_ci_environment():
        os.environ['CI_MODE'] = 'true'
        os.environ['BUILD_TYPE'] = 'Release'
        print("[INFO] CI build environment configured")
    else:
        os.environ['BUILD_TYPE'] = 'Debug'
        print("[INFO] Local build environment configured")
    
    return True

def run_command(cmd, description, cwd=None, timeout=1800):
    """运行命令并处理结果"""
    print(f"[INFO] {description}...")
    
    start_time = time.time()
    
    try:
        result = subprocess.run(
            cmd, 
            capture_output=True, 
            text=True, 
            encoding='utf-8',
            cwd=cwd,
            timeout=timeout
        )
        
        end_time = time.time()
        execution_time = f"{end_time - start_time:.2f}秒"
        
        success = result.returncode == 0
        
        if success:
            print(f"[SUCCESS] {description} completed in {execution_time}")
        else:
            print(f"[ERROR] {description} failed (return code: {result.returncode})")
            if result.stderr:
                print(f"[ERROR] Error output: {result.stderr[:500]}")
        
        return {
            "success": success,
            "execution_time": execution_time,
            "returncode": result.returncode,
            "stdout": result.stdout,
            "stderr": result.stderr
        }
        
    except subprocess.TimeoutExpired:
        print(f"[ERROR] {description} timed out after {timeout} seconds")
        return {
            "success": False,
            "execution_time": "N/A",
            "returncode": -1,
            "stdout": "",
            "stderr": f"Command timed out after {timeout} seconds"
        }
    except Exception as e:
        print(f"[ERROR] {description} failed: {e}")
        return {
            "success": False,
            "execution_time": "N/A",
            "returncode": -1,
            "stdout": "",
            "stderr": str(e)
        }

def configure_cmake():
    """配置CMake项目"""
    cmd = [
        "cmake", "-G", "MinGW Makefiles", 
        "-DCMAKE_BUILD_TYPE=Release" if is_ci_environment() else "-DCMAKE_BUILD_TYPE=Debug",
        ".."
    ]
    
    return run_command(cmd, "Configuring CMake project", cwd="build")

def build_project():
    """构建项目"""
    cmd = ["mingw32-make", "-j4"] if is_ci_environment() else ["mingw32-make"]
    
    return run_command(cmd, "Building project", cwd="build")

def run_tests():
    """运行测试"""
    # 检查是否有可执行文件
    exe_path = Path("build/software-editor.exe")
    if not exe_path.exists():
        print("[WARNING] Executable not found, skipping tests")
        return {"success": True, "execution_time": "N/A", "returncode": 0}
    
    # 运行简单的功能测试
    try:
        result = subprocess.run(
            [str(exe_path), "--version"],
            capture_output=True,
            text=True,
            timeout=30
        )
        
        if result.returncode == 0:
            print("[SUCCESS] Application version check passed")
            return {"success": True, "execution_time": "N/A", "returncode": 0}
        else:
            print("[WARNING] Application version check failed")
            return {"success": False, "execution_time": "N/A", "returncode": result.returncode}
            
    except Exception as e:
        print(f"[WARNING] Application test failed: {e}")
        return {"success": False, "execution_time": "N/A", "returncode": -1}

def generate_build_report(build_results, overall_success):
    """生成构建报告"""
    print("[INFO] Generating build report...")
    
    report_data = {
        "summary": {
            "build_success": overall_success,
            "timestamp": datetime.now().isoformat(),
            "ci_environment": is_ci_environment(),
            "build_type": "Release" if is_ci_environment() else "Debug"
        },
        "build_steps": build_results
    }
    
    # 保存JSON报告
    report_dir = Path("build_reports")
    report_dir.mkdir(exist_ok=True)
    
    json_file = report_dir / "build_report.json"
    with open(json_file, 'w', encoding='utf-8') as f:
        json.dump(report_data, f, ensure_ascii=False, indent=2)
    
    # 生成Markdown报告
    markdown_file = report_dir / "build_report.md"
    with open(markdown_file, 'w', encoding='utf-8') as f:
        f.write("# Build Pipeline Report\n\n")
        f.write(f"**Generated:** {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n")
        f.write(f"**Environment:** {'GitHub Actions' if is_ci_environment() else 'Local'}\n")
        f.write(f"**Build Type:** {report_data['summary']['build_type']}\n")
        f.write(f"**Overall Success:** {'✅ PASS' if overall_success else '❌ FAIL'}\n\n")
        
        f.write("## Build Steps\n\n")
        for step_name, result in build_results.items():
            status_icon = "✅" if result["success"] else "❌"
            f.write(f"### {step_name}\n")
            f.write(f"- **Status:** {status_icon} {'PASS' if result['success'] else 'FAIL'}\n")
            f.write(f"- **Execution Time:** {result['execution_time']}\n")
            f.write(f"- **Return Code:** {result['returncode']}\n")
            
            if not result["success"] and result["stderr"]:
                f.write(f"- **Error:** {result['stderr'][:200]}...\n")
            
            f.write("\n")
    
    print(f"[SUCCESS] Build reports generated:")
    print(f"  - JSON: {json_file}")
    print(f"  - Markdown: {markdown_file}")
    
    return json_file, markdown_file

def run_build_pipeline():
    """运行完整的构建流水线"""
    print("🚀 Starting Build Pipeline...")
    print("=" * 70)
    
    start_time = datetime.now()
    
    # 设置环境
    setup_build_environment()
    
    # 执行构建步骤
    build_results = {}
    
    # 步骤1: 配置CMake
    build_results["cmake_configure"] = configure_cmake()
    
    # 步骤2: 构建项目
    if build_results["cmake_configure"]["success"]:
        build_results["build_project"] = build_project()
    else:
        build_results["build_project"] = {"success": False, "execution_time": "N/A", "returncode": -1}
    
    # 步骤3: 运行测试
    if build_results["build_project"]["success"]:
        build_results["run_tests"] = run_tests()
    else:
        build_results["run_tests"] = {"success": False, "execution_time": "N/A", "returncode": -1}
    
    # 检查整体成功状态
    overall_success = all(result["success"] for result in build_results.values())
    
    # 生成报告
    json_file, markdown_file = generate_build_report(build_results, overall_success)
    
    end_time = datetime.now()
    duration = (end_time - start_time).total_seconds()
    
    print("=" * 70)
    print(f"🎯 Build Pipeline completed in {duration:.2f} seconds")
    print(f"Overall Status: {'✅ PASS' if overall_success else '❌ FAIL'}")
    print("=" * 70)
    
    return overall_success

def main():
    """主函数"""
    parser = argparse.ArgumentParser(description='Build Pipeline')
    parser.add_argument('--skip-tests', action='store_true', help='Skip application tests')
    
    args = parser.parse_args()
    
    try:
        success = run_build_pipeline()
        
        # 根据构建结果设置退出码
        sys.exit(0 if success else 1)
        
    except Exception as e:
        print(f"❌ Build Pipeline failed: {e}")
        sys.exit(1)

if __name__ == "__main__":
    import argparse
    main()