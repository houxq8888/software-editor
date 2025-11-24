#!/usr/bin/env python3
"""
CI/CD测试管道 - 支持GitHub Actions环境的持续集成测试
实现开发-测试闭环，支持自动化测试、报告生成和CI/CD集成
"""

import os
import sys
import subprocess
import json
import time
import argparse
from datetime import datetime, timedelta
from pathlib import Path

try:
    from test_manager import TestManager
    HAS_TEST_MANAGER = True
except ImportError:
    HAS_TEST_MANAGER = False
    print("[WARNING] TestManager not available, using basic functionality")

def is_ci_environment():
    """检测是否在CI环境中运行"""
    return os.getenv('GITHUB_ACTIONS') == 'true' or os.getenv('CI') == 'true'

def setup_ci_environment():
    """设置CI环境"""
    if is_ci_environment():
        print("[INFO] Running in CI/CD environment")
        # 设置CI特定的环境变量
        os.environ['CI_MODE'] = 'true'
        os.environ['HEADLESS'] = 'true'
        
        # 确保测试结果目录存在
        results_dir = Path("test_results")
        results_dir.mkdir(exist_ok=True)
        
        return True
    return False

def run_ci_test_pipeline(ci_mode=False, generate_report=True):
    """执行CI测试管道"""
    
    if ci_mode or is_ci_environment():
        print("🚀 [CI/CD] Starting Continuous Integration Test Pipeline...")
        print("=" * 70)
        setup_ci_environment()
    else:
        print("🚀 启动持续集成测试管道...")
        print("=" * 70)
    
    # 初始化测试管理器（如果可用）
    manager = None
    if HAS_TEST_MANAGER:
        manager = TestManager()
    
    # 获取当前时间戳
    current_time = datetime.now()
    
    # 在CI环境中总是执行测试
    if ci_mode or is_ci_environment():
        print("1. [CI/CD] Running tests in CI environment...")
        has_new_changes = True
    else:
        # 检查是否有新的开发提交（模拟）
        print("1. 检查开发状态...")
        has_new_changes = check_recent_changes()
    
    if not has_new_changes and not ci_mode:
        print("   未检测到新的开发变更，跳过本次测试")
        return []
    
    if ci_mode or is_ci_environment():
        print("   [CI/CD] Starting test execution")
    else:
        print("   检测到新的开发变更，开始执行测试")
    
    # 执行自动化测试
    if ci_mode or is_ci_environment():
        print("\n2. [CI/CD] Executing automated test suite...")
    else:
        print("\n2. 执行自动化测试套件...")
    
    # 定义要执行的测试文件列表
    test_files = [
        "test_product_config.py",
        "test_ui_layout_editor.py", 
        "test_packaging.py",
        "test_preview.py"
    ]
    
    # 检查文件是否存在
    available_test_files = []
    for test_file in test_files:
        if Path(test_file).exists():
            available_test_files.append(test_file)
        else:
            print(f"   [WARNING] Test file {test_file} not found")
    
    test_results = []
    
    for test_file in available_test_files:
        if ci_mode or is_ci_environment():
            print(f"   - Executing: {test_file}")
        else:
            print(f"   - 执行测试文件: {test_file}")
        
        # 运行pytest测试文件
        result = run_pytest_file(test_file, ci_mode)
        
        # 解析测试结果
        test_name = test_file.replace("test_", "").replace(".py", "")
        if ci_mode or is_ci_environment():
            status = "PASS" if result["success"] else "FAIL"
        else:
            status = "通过" if result["success"] else "失败"
        
        test_results.append({
            "test_file": test_file,
            "test_module": test_name,
            "status": status,
            "execution_time": result.get("execution_time", "N/A"),
            "error_info": result.get("error_info", ""),
            "pytest_output": result.get("output", "")[-300:],
            "returncode": result.get("returncode", -1)
        })
        
        if ci_mode or is_ci_environment():
            print(f"     Status: {status}")
            if not result["success"]:
                print(f"     Error: {result.get('error_info', 'Unknown error')[:100]}...")
        else:
            print(f"     状态: {status}")
            if not result["success"]:
                print(f"     错误: {result.get('error_info', '未知错误')[:100]}...")
    
    # 统计测试结果
    if ci_mode or is_ci_environment():
        print("\n3. [CI/CD] Analyzing test results...")
    else:
        print("\n3. 分析测试结果...")
    
    total_tests = len(test_results)
    passed_tests = sum(1 for result in test_results if result["status"] in ["通过", "PASS"])
    failed_tests = total_tests - passed_tests
    pass_rate = (passed_tests / total_tests) * 100 if total_tests > 0 else 0
    
    if ci_mode or is_ci_environment():
        print(f"   Total test files: {total_tests}")
        print(f"   Passed: {passed_tests}")
        print(f"   Failed: {failed_tests}")
        print(f"   Pass rate: {pass_rate:.1f}%")
    else:
        print(f"   测试文件总数: {total_tests}")
        print(f"   通过: {passed_tests}")
        print(f"   失败: {failed_tests}")
        print(f"   通过率: {pass_rate:.1f}%")
    
    # 生成开发反馈报告
    print("\n4. 生成开发反馈报告...")
    
    if failed_tests > 0:
        print("   ⚠️  检测到测试失败，生成修复建议...")
        
        # 生成详细的修复建议
        fix_suggestions = generate_fix_suggestions(test_results)
        
        # 保存修复建议到文件
        fix_suggestions_file = save_fix_suggestions(fix_suggestions, manager)
        
        # 生成开发反馈报告
        feedback_report = manager._generate_development_feedback(test_results, pass_rate)
        feedback_file = save_feedback_report(feedback_report, manager)
        
        print(f"   ✅ 修复建议已生成: {fix_suggestions_file}")
        print(f"   ✅ 开发反馈报告已生成: {feedback_file}")
        
        # 启动开发修复流程
        print("\n5. 启动开发修复流程...")
        print("   请开发人员根据修复建议进行代码修复")
        print("   修复完成后，重新运行测试验证修复效果")
        
    else:
        print("   ✅ 所有测试通过，无需修复")
        
        # 生成成功报告
        success_report = generate_success_report(test_results, pass_rate)
        success_file = save_success_report(success_report, manager)
        
        print(f"   ✅ 成功报告已生成: {success_file}")
    
    # 监控开发进度
    print("\n6. 监控开发进度...")
    
    progress_data = manager.monitor_development_progress()
    progress_file = save_progress_data(progress_data, manager)
    
    print(f"   ✅ 开发进度数据已保存: {progress_file}")
    
    print("\n" + "=" * 70)
    print("🎉 持续集成测试管道执行完成!")
    print("=" * 70)
    
    return test_results

def check_recent_changes():
    """检查最近是否有代码变更（模拟实现）"""
    # 这里可以集成Git命令来检查实际的文件变更
    # 目前先返回True模拟有变更
    return True

def run_pytest_file(test_file, ci_mode=False):
    """运行指定的pytest测试文件"""
    
    # 构建pytest命令
    cmd = ["python", "-m", "pytest", test_file, "-v", "--tb=short"]
    
    # 在CI环境中添加更多选项
    if ci_mode or is_ci_environment():
        cmd.extend([
            "--html", f"test_results/{Path(test_file).stem}_report.html",
            "--self-contained-html",
            "--timeout=300"  # 5分钟超时
        ])
    
    start_time = time.time()
    
    try:
        result = subprocess.run(
            cmd, 
            capture_output=True, 
            text=True, 
            encoding='utf-8', 
            errors='ignore', 
            cwd=os.path.dirname(__file__),
            timeout=1800  # 30分钟超时
        )
        
        end_time = time.time()
        execution_time = f"{end_time - start_time:.2f}秒"
        
        # 解析测试结果
        success = result.returncode == 0
        
        error_info = ""
        if not success:
            # 提取错误信息
            if result.stderr:
                error_info = result.stderr
            elif "FAILED" in result.stdout:
                lines = result.stdout.split('\n')
                for line in lines:
                    if "FAILED" in line or "ERROR" in line:
                        error_info = line.strip()
                        break
        
        return {
            "success": success,
            "execution_time": execution_time,
            "error_info": error_info,
            "output": result.stdout,
            "returncode": result.returncode
        }
        
    except subprocess.TimeoutExpired:
        error_msg = "Test execution timed out after 30 minutes"
        if ci_mode or is_ci_environment():
            error_msg = "[TIMEOUT] Test execution exceeded 30 minutes"
        
        return {
            "success": False,
            "execution_time": "N/A",
            "error_info": error_msg,
            "output": "",
            "returncode": -1
        }
    except Exception as e:
        return {
            "success": False,
            "execution_time": "N/A",
            "error_info": str(e),
            "output": "",
            "returncode": -1
        }

def generate_fix_suggestions(test_results):
    """根据测试结果生成修复建议"""
    suggestions = []
    
    for result in test_results:
        if result["状态"] == "失败":
            test_file = result["测试文件"]
            error_info = result["错误信息"]
            
            # 根据测试文件和错误信息生成具体建议
            suggestion = {
                "测试文件": test_file,
                "问题描述": f"测试文件 {test_file} 执行失败",
                "错误信息": error_info[:200],
                "修复建议": generate_specific_fix_suggestion(test_file, error_info),
                "优先级": "高" if "启动" in error_info or "界面" in error_info else "中"
            }
            
            suggestions.append(suggestion)
    
    return suggestions

def generate_specific_fix_suggestion(test_file, error_info):
    """根据具体测试文件和错误信息生成修复建议"""
    
    if "test_product_config" in test_file:
        return "检查产品配置编辑功能的相关代码，确保产品信息保存和功能特性管理功能正常"
    
    elif "test_ui_layout_editor" in test_file:
        return "检查UI布局编辑器功能，验证控件拖拽、属性编辑和分隔条调整功能是否正常"
    
    elif "test_packaging" in test_file:
        return "检查智能打包功能，确保CMake项目生成和编译构建流程正确"
    
    elif "test_preview" in test_file:
        return "检查预览功能，验证预览窗口的显示和交互功能是否正常"
    
    else:
        return "检查相关功能模块的代码实现，确保功能逻辑正确"

def save_fix_suggestions(suggestions, manager):
    """保存修复建议到文件"""
    date_dir = manager.get_current_date_dir()
    timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
    
    # 保存为JSON文件
    json_file = os.path.join(date_dir, f"修复建议_{timestamp}.json")
    
    with open(json_file, 'w', encoding='utf-8') as f:
        json.dump({
            "生成时间": datetime.now().strftime('%Y-%m-%d %H:%M:%S'),
            "修复建议": suggestions
        }, f, ensure_ascii=False, indent=2)
    
    # 保存为TXT文件（便于阅读）
    txt_file = os.path.join(date_dir, f"修复建议_{timestamp}.txt")
    
    with open(txt_file, 'w', encoding='utf-8') as f:
        f.write("软件产品编辑器 - 修复建议报告\n")
        f.write("=" * 50 + "\n\n")
        f.write(f"生成时间: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n\n")
        
        for i, suggestion in enumerate(suggestions, 1):
            f.write(f"{i}. {suggestion['测试文件']}\n")
            f.write(f"   问题: {suggestion['问题描述']}\n")
            f.write(f"   错误: {suggestion['错误信息']}\n")
            f.write(f"   建议: {suggestion['修复建议']}\n")
            f.write(f"   优先级: {suggestion['优先级']}\n\n")
    
    return txt_file

def save_feedback_report(feedback, manager):
    """保存开发反馈报告"""
    date_dir = manager.get_current_date_dir()
    timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
    
    feedback_file = os.path.join(date_dir, f"开发反馈报告_{timestamp}.txt")
    
    with open(feedback_file, 'w', encoding='utf-8') as f:
        f.write(feedback)
    
    return feedback_file

def save_success_report(success_report, manager):
    """保存成功报告"""
    date_dir = manager.get_current_date_dir()
    timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
    
    success_file = os.path.join(date_dir, f"测试成功报告_{timestamp}.txt")
    
    with open(success_file, 'w', encoding='utf-8') as f:
        f.write(success_report)
    
    return success_file

def generate_success_report(test_results, pass_rate):
    """生成成功报告"""
    report = "软件产品编辑器 - 测试成功报告\n"
    report += "=" * 50 + "\n\n"
    report += f"生成时间: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n"
    report += f"测试通过率: {pass_rate:.1f}%\n\n"
    report += "通过的测试文件:\n"
    
    for result in test_results:
        if result["状态"] == "通过":
            report += f"- {result['测试文件']} ({result['执行时间']})\n"
    
    report += "\n✅ 所有测试通过，代码质量良好！\n"
    
    return report

def save_progress_data(progress_data, manager):
    """保存开发进度数据"""
    date_dir = manager.get_current_date_dir()
    timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
    
    progress_file = os.path.join(date_dir, f"开发进度_{timestamp}.json")
    
    with open(progress_file, 'w', encoding='utf-8') as f:
        json.dump(progress_data, f, ensure_ascii=False, indent=2)
    
    return progress_file

def generate_ci_report(test_results, pass_rate, ci_mode=False):
    """生成CI/CD测试报告"""
    
    # 生成JSON格式的报告
    report_data = {
        "summary": {
            "total_tests": len(test_results),
            "passed_tests": sum(1 for result in test_results if result["status"] in ["通过", "PASS"]),
            "failed_tests": len(test_results) - sum(1 for result in test_results if result["status"] in ["通过", "PASS"]),
            "pass_rate": pass_rate,
            "timestamp": datetime.now().isoformat(),
            "ci_environment": ci_mode or is_ci_environment()
        },
        "test_results": test_results
    }
    
    # 确保测试结果目录存在
    os.makedirs("test_results", exist_ok=True)
    
    # 保存JSON报告
    json_file = "test_results/ci_test_report.json"
    with open(json_file, 'w', encoding='utf-8') as f:
        json.dump(report_data, f, ensure_ascii=False, indent=2)
    
    # 生成Markdown报告
    markdown_file = "test_results/ci_test_report.md"
    with open(markdown_file, 'w', encoding='utf-8') as f:
        if ci_mode or is_ci_environment():
            f.write("# CI/CD Test Report\n\n")
            f.write(f"**Generated:** {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n")
            f.write(f"**Environment:** GitHub Actions\n\n")
            f.write("## Summary\n\n")
            f.write(f"- **Total Tests:** {len(test_results)}\n")
            f.write(f"- **Passed Tests:** {report_data['summary']['passed_tests']}\n")
            f.write(f"- **Failed Tests:** {report_data['summary']['failed_tests']}\n")
            f.write(f"- **Pass Rate:** {pass_rate:.1f}%\n\n")
            
            f.write("## Detailed Results\n\n")
            for result in test_results:
                status_icon = "✅" if result["status"] == "PASS" else "❌"
                f.write(f"### {result['test_file']}\n")
                f.write(f"- **Status:** {status_icon} {result['status']}\n")
                f.write(f"- **Execution Time:** {result['execution_time']}\n")
                if result["status"] == "FAIL":
                    f.write(f"- **Error:** {result['error_info'][:200]}...\n")
                f.write("\n")
            
            if pass_rate >= 80:
                f.write("## ✅ Conclusion: Tests PASSED")
            else:
                f.write("## ❌ Conclusion: Tests FAILED")
        else:
            # 中文报告
            f.write("# CI/CD 测试报告\n\n")
            f.write(f"**生成时间:** {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n")
            f.write(f"**环境:** 本地开发环境\n\n")
            f.write("## 摘要\n\n")
            f.write(f"- **测试文件总数:** {len(test_results)}\n")
            f.write(f"- **通过测试:** {report_data['summary']['passed_tests']}\n")
            f.write(f"- **失败测试:** {report_data['summary']['failed_tests']}\n")
            f.write(f"- **通过率:** {pass_rate:.1f}%\n\n")
    
    return json_file, markdown_file

def main():
    """主函数"""
    parser = argparse.ArgumentParser(description='CI/CD Test Pipeline')
    parser.add_argument('--ci', action='store_true', help='Run in CI/CD mode')
    parser.add_argument('--generate-report', action='store_true', default=True, 
                       help='Generate test reports')
    parser.add_argument('--test-file', type=str, help='Run specific test file')
    
    args = parser.parse_args()
    
    if args.ci or is_ci_environment():
        print("Software Product Editor - CI/CD Test Pipeline")
        print("This pipeline implements development-testing closed-loop:")
        print("1. Detect development changes")
        print("2. Execute automated tests")
        print("3. Analyze test results")
        print("4. Generate fix suggestions (if failures)")
        print("5. Start development fix process")
        print("6. Monitor development progress")
        print()
    else:
        print("软件产品编辑器 - 持续集成测试管道")
        print("此管道实现开发-测试闭环流程:")
        print("1. 检测开发变更")
        print("2. 执行自动化测试")
        print("3. 分析测试结果")
        print("4. 生成修复建议（如有失败）")
        print("5. 启动开发修复流程")
        print("6. 监控开发进度")
        print()
    
    try:
        test_results = run_ci_test_pipeline(ci_mode=args.ci, generate_report=args.generate_report)
        
        # 计算通过率
        total_tests = len(test_results)
        passed_tests = sum(1 for result in test_results if result["status"] in ["通过", "PASS"])
        pass_rate = (passed_tests / total_tests) * 100 if total_tests > 0 else 0
        
        # 生成CI报告
        if args.generate_report:
            json_file, markdown_file = generate_ci_report(test_results, pass_rate, args.ci)
            if args.ci or is_ci_environment():
                print(f"[CI/CD] Test reports generated:")
                print(f"  - JSON: {json_file}")
                print(f"  - Markdown: {markdown_file}")
            else:
                print(f"测试报告已生成:")
                print(f"  - JSON格式: {json_file}")
                print(f"  - Markdown格式: {markdown_file}")
        
        if args.ci or is_ci_environment():
            print("\n🎯 CI/CD Test Pipeline completed!")
            print("Developers can use the generated reports for fixes.")
        else:
            print("\n🎯 持续集成测试管道执行完成！")
            print("开发人员可根据生成的报告进行相应的修复工作。")
        
        # 根据通过率设置退出码
        sys.exit(0 if pass_rate >= 80 else 1)
        
    except Exception as e:
        if args.ci or is_ci_environment():
            print(f"❌ CI/CD Test Pipeline failed: {e}")
        else:
            print(f"❌ 持续集成测试管道执行失败: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()