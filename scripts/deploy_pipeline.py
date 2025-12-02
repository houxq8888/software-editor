#!/usr/bin/env python3
"""
部署流水线脚本 - 支持CI/CD环境的自动化部署
"""

import os
import sys
import subprocess
import json
import shutil
import time
from datetime import datetime
from pathlib import Path

def is_ci_environment():
    """检测是否在CI环境中运行"""
    return os.getenv('GITHUB_ACTIONS') == 'true' or os.getenv('CI') == 'true'

def setup_deploy_environment():
    """设置部署环境"""
    print("[INFO] Setting up deployment environment...")
    
    # 确保部署目录存在
    deploy_dir = Path("deploy")
    if deploy_dir.exists():
        shutil.rmtree(deploy_dir)
    deploy_dir.mkdir(exist_ok=True)
    
    # 设置环境变量
    if is_ci_environment():
        os.environ['DEPLOY_MODE'] = 'ci'
        print("[INFO] CI deployment environment configured")
    else:
        os.environ['DEPLOY_MODE'] = 'local'
        print("[INFO] Local deployment environment configured")
    
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

def prepare_deployment_package():
    """准备部署包"""
    print("[INFO] Preparing deployment package...")
    
    # 检查构建输出
    build_dir = Path("build")
    if not build_dir.exists():
        return {"success": False, "execution_time": "N/A", "returncode": -1, "stderr": "Build directory not found"}
    
    # 查找可执行文件
    exe_files = list(build_dir.glob("*.exe"))
    if not exe_files:
        return {"success": False, "execution_time": "N/A", "returncode": -1, "stderr": "No executable files found"}
    
    # 创建部署目录结构
    deploy_dir = Path("deploy")
    
    # 复制可执行文件
    for exe_file in exe_files:
        shutil.copy2(exe_file, deploy_dir / exe_file.name)
        print(f"[INFO] Copied {exe_file.name} to deployment package")
    
    # 复制必要的依赖文件
    dll_files = list(build_dir.glob("*.dll"))
    for dll_file in dll_files:
        shutil.copy2(dll_file, deploy_dir / dll_file.name)
        print(f"[INFO] Copied {dll_file.name} to deployment package")
    
    # 创建版本信息文件
    version_info = {
        "version": "1.0.0",
        "build_date": datetime.now().isoformat(),
        "build_environment": "GitHub Actions" if is_ci_environment() else "Local",
        "commit_hash": os.getenv('GITHUB_SHA', 'local')
    }
    
    with open(deploy_dir / "version.json", 'w', encoding='utf-8') as f:
        json.dump(version_info, f, indent=2, ensure_ascii=False)
    
    # 创建README文件
    readme_content = """# Software Product Editor

## Deployment Package

This package contains the built executable and necessary dependencies.

### Files:
- software-editor.exe: Main application executable
- version.json: Build version information

### Usage:
Run software-editor.exe to start the application.

### Build Information:
- Build Date: {build_date}
- Environment: {environment}
- Commit: {commit}
""".format(
        build_date=datetime.now().strftime('%Y-%m-%d %H:%M:%S'),
        environment="GitHub Actions" if is_ci_environment() else "Local",
        commit=os.getenv('GITHUB_SHA', 'local')
    )
    
    with open(deploy_dir / "README.md", 'w', encoding='utf-8') as f:
        f.write(readme_content)
    
    print("[SUCCESS] Deployment package prepared")
    return {"success": True, "execution_time": "N/A", "returncode": 0}

def create_installer():
    """创建安装程序（模拟）"""
    print("[INFO] Creating installer package...")
    
    # 在实际项目中，这里会使用NSIS、Inno Setup等工具创建安装程序
    # 这里模拟创建简单的ZIP包
    
    deploy_dir = Path("deploy")
    if not deploy_dir.exists():
        return {"success": False, "execution_time": "N/A", "returncode": -1}
    
    # 创建ZIP包
    import zipfile
    
    zip_filename = f"software-editor-{datetime.now().strftime('%Y%m%d')}.zip"
    zip_path = Path("dist") / zip_filename
    Path("dist").mkdir(exist_ok=True)
    
    with zipfile.ZipFile(zip_path, 'w', zipfile.ZIP_DEFLATED) as zipf:
        for file_path in deploy_dir.rglob('*'):
            if file_path.is_file():
                zipf.write(file_path, file_path.relative_to(deploy_dir))
    
    print(f"[SUCCESS] Installer package created: {zip_path}")
    return {"success": True, "execution_time": "N/A", "returncode": 0}

def deploy_to_staging():
    """部署到测试环境（模拟）"""
    print("[INFO] Deploying to staging environment...")
    
    # 在实际项目中，这里会使用FTP、SCP、Docker等工具进行部署
    # 这里模拟部署过程
    
    time.sleep(2)  # 模拟部署时间
    
    print("[SUCCESS] Deployment to staging completed")
    return {"success": True, "execution_time": "N/A", "returncode": 0}

def run_smoke_tests():
    """运行冒烟测试"""
    print("[INFO] Running smoke tests...")
    
    # 检查部署的可执行文件
    exe_path = Path("deploy/software-editor.exe")
    if not exe_path.exists():
        return {"success": False, "execution_time": "N/A", "returncode": -1}
    
    # 运行简单的功能测试
    try:
        result = subprocess.run(
            [str(exe_path), "--version"],
            capture_output=True,
            text=True,
            timeout=30
        )
        
        if result.returncode == 0:
            print("[SUCCESS] Smoke test passed")
            return {"success": True, "execution_time": "N/A", "returncode": 0}
        else:
            print("[ERROR] Smoke test failed")
            return {"success": False, "execution_time": "N/A", "returncode": result.returncode}
            
    except Exception as e:
        print(f"[ERROR] Smoke test failed: {e}")
        return {"success": False, "execution_time": "N/A", "returncode": -1}

def generate_deploy_report(deploy_results, overall_success):
    """生成部署报告"""
    print("[INFO] Generating deployment report...")
    
    report_data = {
        "summary": {
            "deploy_success": overall_success,
            "timestamp": datetime.now().isoformat(),
            "environment": "GitHub Actions" if is_ci_environment() else "Local",
            "deploy_mode": "CI" if is_ci_environment() else "Local"
        },
        "deploy_steps": deploy_results
    }
    
    # 保存JSON报告
    report_dir = Path("deploy_reports")
    report_dir.mkdir(exist_ok=True)
    
    json_file = report_dir / "deploy_report.json"
    with open(json_file, 'w', encoding='utf-8') as f:
        json.dump(report_data, f, ensure_ascii=False, indent=2)
    
    # 生成Markdown报告
    markdown_file = report_dir / "deploy_report.md"
    with open(markdown_file, 'w', encoding='utf-8') as f:
        f.write("# Deployment Pipeline Report\n\n")
        f.write(f"**Generated:** {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n")
        f.write(f"**Environment:** {report_data['summary']['environment']}\n")
        f.write(f"**Deploy Mode:** {report_data['summary']['deploy_mode']}\n")
        f.write(f"**Overall Success:** {'✅ PASS' if overall_success else '❌ FAIL'}\n\n")
        
        f.write("## Deployment Steps\n\n")
        for step_name, result in deploy_results.items():
            status_icon = "✅" if result["success"] else "❌"
            f.write(f"### {step_name}\n")
            f.write(f"- **Status:** {status_icon} {'PASS' if result['success'] else 'FAIL'}\n")
            f.write(f"- **Execution Time:** {result['execution_time']}\n")
            f.write(f"- **Return Code:** {result['returncode']}\n")
            
            if not result["success"] and result["stderr"]:
                f.write(f"- **Error:** {result['stderr'][:200]}...\n")
            
            f.write("\n")
    
    print(f"[SUCCESS] Deployment reports generated:")
    print(f"  - JSON: {json_file}")
    print(f"  - Markdown: {markdown_file}")
    
    return json_file, markdown_file

def run_deploy_pipeline():
    """运行完整的部署流水线"""
    print("🚀 Starting Deployment Pipeline...")
    print("=" * 70)
    
    start_time = datetime.now()
    
    # 设置环境
    setup_deploy_environment()
    
    # 执行部署步骤
    deploy_results = {}
    
    # 步骤1: 准备部署包
    deploy_results["prepare_package"] = prepare_deployment_package()
    
    # 步骤2: 创建安装程序
    if deploy_results["prepare_package"]["success"]:
        deploy_results["create_installer"] = create_installer()
    else:
        deploy_results["create_installer"] = {"success": False, "execution_time": "N/A", "returncode": -1}
    
    # 步骤3: 部署到测试环境
    if deploy_results["create_installer"]["success"]:
        deploy_results["deploy_staging"] = deploy_to_staging()
    else:
        deploy_results["deploy_staging"] = {"success": False, "execution_time": "N/A", "returncode": -1}
    
    # 步骤4: 运行冒烟测试
    if deploy_results["deploy_staging"]["success"]:
        deploy_results["smoke_tests"] = run_smoke_tests()
    else:
        deploy_results["smoke_tests"] = {"success": False, "execution_time": "N/A", "returncode": -1}
    
    # 检查整体成功状态
    overall_success = all(result["success"] for result in deploy_results.values())
    
    # 生成报告
    json_file, markdown_file = generate_deploy_report(deploy_results, overall_success)
    
    end_time = datetime.now()
    duration = (end_time - start_time).total_seconds()
    
    print("=" * 70)
    print(f"🎯 Deployment Pipeline completed in {duration:.2f} seconds")
    print(f"Overall Status: {'✅ PASS' if overall_success else '❌ FAIL'}")
    print("=" * 70)
    
    return overall_success

def main():
    """主函数"""
    parser = argparse.ArgumentParser(description='Deployment Pipeline')
    parser.add_argument('--skip-staging', action='store_true', help='Skip staging deployment')
    
    args = parser.parse_args()
    
    try:
        success = run_deploy_pipeline()
        
        # 根据部署结果设置退出码
        sys.exit(0 if success else 1)
        
    except Exception as e:
        print(f"❌ Deployment Pipeline failed: {e}")
        sys.exit(1)

if __name__ == "__main__":
    import argparse
    main()