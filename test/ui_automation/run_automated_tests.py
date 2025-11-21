#!/usr/bin/env python3
"""
自动化测试执行脚本 - 集成测试用例和报告生成
"""

import os
import sys
import subprocess
import json
import pandas as pd
from datetime import datetime
from test_manager import TestManager

def run_pytest_test(test_module, test_function):
    """运行指定的pytest测试"""
    cmd = ["python", "-m", "pytest", f"{test_module}::{test_function}", "-v", "-s"]
    
    try:
        result = subprocess.run(cmd, capture_output=True, text=True, encoding='utf-8', errors='ignore', cwd=os.path.dirname(__file__))
        
        # 解析测试结果
        success = result.returncode == 0
        output = result.stdout if result.stdout else ""
        error = result.stderr if result.stderr else ""
        
        return {
            "success": success,
            "output": output,
            "error": error,
            "returncode": result.returncode
        }
    except Exception as e:
        return {
            "success": False,
            "output": "",
            "error": str(e),
            "returncode": -1
        }

def execute_automated_testing():
    """执行完整的自动化测试流程"""
    print("开始执行自动化测试流程...")
    print("=" * 60)
    
    # 初始化测试管理器
    manager = TestManager()
    
    # 定义测试用例
    test_cases = [
        {
            "name": "产品信息保存功能测试",
            "module": "产品信息编辑",
            "test_module": "test_product_info.py",
            "test_function": "test_product_info_save_flow",
            "steps": [
                "启动软件产品编辑器",
                "点击产品信息编辑按钮",
                "填写产品名称、版本、描述等信息",
                "点击保存按钮",
                "验证产品信息文件是否正确生成"
            ],
            "expected": [
                "软件正常启动",
                "产品信息编辑界面正常显示",
                "产品信息能够正常输入",
                "保存操作成功执行",
                "生成包含所有产品信息的JSON文件"
            ]
        },
        {
            "name": "UI布局拖拽编辑功能测试", 
            "module": "UI布局编辑",
            "test_module": "test_ui_layout.py",
            "test_function": "test_ui_layout_drag_and_edit",
            "steps": [
                "启动软件产品编辑器",
                "点击UI布局编辑按钮",
                "从控件库拖拽QLabel到编辑区",
                "双击QLabel编辑文本内容",
                "从控件库拖拽QTabWidget到编辑区",
                "双击Tab页标题编辑文本",
                "点击预览按钮验证交互功能"
            ],
            "expected": [
                "软件正常启动",
                "UI布局编辑界面正常显示",
                "控件能够正常拖拽到编辑区",
                "文本编辑功能正常工作",
                "Tab控件能够正常添加",
                "Tab页标题能够正常编辑",
                "预览窗口能够正常交互"
            ]
        }
    ]
    
    # 生成测试用例
    print("1. 检查测试用例...")
    
    # 检查是否已存在当天的测试用例文件
    existing_files = manager._find_existing_test_case_files()
    if existing_files:
        print(f"   发现已存在的测试用例文件，直接使用: {existing_files[0]}")
        test_case_file = existing_files[0]
        
        # 从现有文件中读取测试用例信息（用于生成报告）
        try:
            df = pd.read_excel(test_case_file, sheet_name='测试用例')
            generated_cases = df.to_dict('records')
            print(f"   从现有文件加载了 {len(generated_cases)} 个测试用例")
        except Exception as e:
            print(f"   读取现有测试用例文件失败，重新生成: {e}")
            generated_cases = []
            for case in test_cases:
                test_case = manager.generate_test_case(
                    case["name"],
                    case["module"],
                    case["steps"],
                    case["expected"]
                )
                generated_cases.append(test_case)
            test_case_file = manager.save_test_cases_to_excel(generated_cases, force_generate=True)
    else:
        # 生成新的测试用例
        generated_cases = []
        for case in test_cases:
            test_case = manager.generate_test_case(
                case["name"],
                case["module"],
                case["steps"],
                case["expected"]
            )
            generated_cases.append(test_case)
            print(f"   - 生成测试用例: {case['name']}")
        
        # 保存测试用例
        test_case_file = manager.save_test_cases_to_excel(generated_cases)
    
    print(f"   使用的测试用例文件: {test_case_file}")
    
    # 执行测试
    print("\n2. 执行自动化测试...")
    test_results = []
    
    for i, (case, generated_case) in enumerate(zip(test_cases, generated_cases)):
        print(f"   - 执行测试: {case['name']}")
        
        # 运行pytest测试
        test_result = run_pytest_test(case["test_module"], case["test_function"])
        
        # 解析测试结果
        status = "通过" if test_result["success"] else "失败"
        execution_time = "2.5秒"  # 模拟执行时间
        
        error_info = ""
        if not test_result["success"]:
            # 提取错误信息
            if test_result.get("error"):
                error_info = test_result["error"]
            elif test_result.get("output") and "FAILED" in test_result["output"]:
                # 从pytest输出中提取失败信息
                lines = test_result["output"].split('\n')
                for line in lines:
                    if "FAILED" in line or "ERROR" in line:
                        error_info = line.strip()
                        break
        
        # 记录测试结果
        result_record = {
            "用例编号": generated_case["用例编号"],
            "用例名称": generated_case["用例名称"],
            "测试模块": generated_case["测试模块"],
            "状态": status,
            "执行时间": execution_time,
            "错误信息": error_info,
            "测试人员": "自动化测试系统",
            "执行日期": datetime.now().strftime('%Y-%m-%d'),
            "pytest_output": test_result["output"][-500:],  # 保留最后500字符
            "returncode": test_result["returncode"]
        }
        
        test_results.append(result_record)
        
        print(f"     状态: {status}")
        if error_info:
            print(f"     错误: {error_info[:100]}...")
    
    # 生成测试报告
    print("\n3. 生成测试报告...")
    test_report = manager.generate_test_report(test_results, len(generated_cases))
    test_report_file = manager.save_test_report_to_excel(test_report, test_results)
    
    # 统计结果
    passed = sum(1 for result in test_results if result["状态"] == "通过")
    failed = sum(1 for result in test_results if result["状态"] == "失败")
    
    print("\n" + "=" * 60)
    print("自动化测试流程完成!")
    print("=" * 60)
    print(f"测试用例总数: {len(test_cases)}")
    print(f"通过: {passed}")
    print(f"失败: {failed}")
    print(f"通过率: {test_report['通过率']}")
    print(f"\n生成的文件:")
    print(f"- 测试用例: {test_case_file}")
    print(f"- 测试报告: {test_report_file}")
    
    # 保存详细的测试结果到JSON文件
    date_dir = manager.get_current_date_dir()
    detailed_results_file = os.path.join(date_dir, f"详细测试结果_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json")
    
    with open(detailed_results_file, 'w', encoding='utf-8') as f:
        json.dump({
            "test_cases": generated_cases,
            "test_results": test_results,
            "test_report": test_report,
            "summary": {
                "total_cases": len(test_cases),
                "passed": passed,
                "failed": failed,
                "pass_rate": test_report['通过率']
            }
        }, f, ensure_ascii=False, indent=2)
    
    print(f"- 详细结果: {detailed_results_file}")
    
    return test_case_file, test_report_file, detailed_results_file

def main():
    """主函数"""
    print("软件产品编辑器 - 自动化测试执行器")
    print("此脚本将执行以下流程:")
    print("1. 根据模板生成测试用例")
    print("2. 执行自动化测试")
    print("3. 根据模板生成测试报告")
    print("4. 保存所有文件到按日期分类的目录")
    print()
    
    try:
        test_case_file, test_report_file, detailed_results_file = execute_automated_testing()
        
        print("\n✅ 自动化测试流程执行成功!")
        print("所有文件已按模板格式生成并保存到相应目录。")
        
    except Exception as e:
        print(f"❌ 自动化测试流程执行失败: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()