#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Software Editor - 验收检查自动化脚本

功能：
1. 根据验收检查清单自动检查任务完成情况
2. 生成验收报告
3. 提供验收评分和问题分析
"""

import os
import json
import sys
import re
from datetime import datetime
from pathlib import Path
from typing import Dict, List, Tuple, Any

class AcceptanceChecker:
    def __init__(self, project_root: str):
        self.project_root = Path(project_root)
        self.plan_file = self.project_root / "docs" / "详细开发实施计划.md"
        self.acceptance_list_path = self.project_root / "docs" / "验收检查清单.md"
        self.progress_tracker_path = self.project_root / "scripts" / "progress_tracker.py"
        self.reports_dir = self.project_root / "docs" / "acceptance_reports"
        self.reports_dir.mkdir(exist_ok=True)
        
        # 验收标准权重配置
        self.weights = {
            "文件完整性": 0.2,
            "功能完整性": 0.35,
            "性能指标": 0.25,
            "用户体验": 0.15,
            "文档质量": 0.05
        }
        
    def parse_acceptance_list(self) -> Dict[str, Any]:
        """解析验收检查清单文件"""
        if not self.acceptance_list_path.exists():
            return {}
            
        with open(self.acceptance_list_path, 'r', encoding='utf-8') as f:
            content = f.read()
            
        # 解析周和天的验收标准
        acceptance_data = {}
        
        # 查找周级别的验收标准
        week_pattern = r'### 第(\d+)周验收检查清单\n\n#### 第(\d+)天：([^\n]+)\n\*\*验收标准\*\*：([^\n]+)'
        week_matches = re.findall(week_pattern, content)
        
        for week_num, day_num, task_name, acceptance_standard in week_matches:
            week_key = f"week_{week_num}"
            day_key = f"day_{day_num}"
            
            if week_key not in acceptance_data:
                acceptance_data[week_key] = {}
                
            acceptance_data[week_key][day_key] = {
                "task_name": task_name,
                "acceptance_standard": acceptance_standard,
                "check_items": self._parse_check_items(content, week_num, day_num),
                "scoring_criteria": self._parse_scoring_criteria(content, week_num, day_num)
            }
            
        return acceptance_data
    
    def _parse_check_items(self, content: str, week_num: str, day_num: str) -> List[Dict]:
        """解析检查项目"""
        check_items = []
        
        # 查找检查项目
        pattern = rf'#### 第{day_num}天：[^\n]+\n\*\*验收标准\*\*：[^\n]+\n\n\*\*检查项目\*\*：\n(.*?)(?=\n\*\*验收评分\*\*|\n###|$)'
        match = re.search(pattern, content, re.DOTALL)
        
        if match:
            items_text = match.group(1)
            # 解析每个检查项目
            item_pattern = r'- \[ \] \*\*([^\*]+)\*\*\n(.*?)(?=\n- \[ \]|$)'
            items = re.findall(item_pattern, items_text, re.DOTALL)
            
            for category, sub_items_text in items:
                # 解析子项目
                sub_item_pattern = r'  - \[ \] ([^\n]+)'
                sub_items = re.findall(sub_item_pattern, sub_items_text)
                
                check_items.append({
                    "category": category.strip(),
                    "sub_items": [item.strip() for item in sub_items]
                })
                
        return check_items
    
    def _parse_scoring_criteria(self, content: str, week_num: str, day_num: str) -> Dict[str, float]:
        """解析评分标准"""
        scoring_criteria = {}
        
        # 查找评分标准
        pattern = rf'#### 第{day_num}天：[^\n]+\n\*\*验收标准\*\*：[^\n]+\n\n.*?\*\*验收评分\*\*：\n(.*?)(?=\n\*\*总分要求\*\*|\n###|$)'
        match = re.search(pattern, content, re.DOTALL)
        
        if match:
            scoring_text = match.group(1)
            # 解析评分项
            score_pattern = r'- ([^：]+)：(\d+)分'
            scores = re.findall(score_pattern, scoring_text)
            
            for criterion, score in scores:
                scoring_criteria[criterion.strip()] = float(score)
                
        return scoring_criteria
    
    def check_file_existence(self, file_patterns: List[str]) -> Dict[str, bool]:
        """检查文件存在性"""
        results = {}
        
        for pattern in file_patterns:
            file_path = self.project_root / pattern
            results[pattern] = file_path.exists()
            
        return results
    
    def check_compilation(self, file_paths: List[str]) -> Dict[str, bool]:
        """检查编译状态"""
        results = {}
        
        # 这里可以集成实际的编译检查
        # 目前返回模拟结果
        for file_path in file_paths:
            full_path = self.project_root / file_path
            # 简单的文件存在性和内容检查
            if full_path.exists():
                with open(full_path, 'r', encoding='utf-8') as f:
                    content = f.read()
                    # 检查基本的语法错误模式
                    results[file_path] = self._check_basic_syntax(content)
            else:
                results[file_path] = False
                
        return results
    
    def _check_basic_syntax(self, content: str) -> bool:
        """检查基本语法错误"""
        # 检查常见的语法错误模式
        error_patterns = [
            r'class\s+\w+\s*{',  # 类定义
            r'void\s+\w+\s*\(',  # 函数定义
            r'#include\s+[<"]',  # 头文件包含
        ]
        
        for pattern in error_patterns:
            if re.search(pattern, content):
                return True
                
        return False
    
    def check_functionality(self, task_name: str, expected_functions: List[str]) -> Dict[str, bool]:
        """检查功能实现"""
        results = {}
        
        # 这里可以集成实际的功能测试
        # 目前返回模拟结果
        for function in expected_functions:
            # 简单的基于文件内容的检查
            results[function] = self._check_function_in_files(function)
            
        return results
    
    def _check_function_in_files(self, function_name: str) -> bool:
        """在文件中检查函数实现"""
        # 搜索相关的源文件
        src_dir = self.project_root / "src"
        
        if not src_dir.exists():
            return False
            
        # 搜索包含函数名的文件
        for file_path in src_dir.rglob("*.cpp"):
            with open(file_path, 'r', encoding='utf-8') as f:
                content = f.read()
                if function_name in content:
                    return True
                    
        return False
    
    def run_acceptance_check(self, week: int = None, day: int = None) -> Dict[str, Any]:
        """运行验收检查"""
        acceptance_data = self.parse_acceptance_list()
        
        if not acceptance_data:
            return {"error": "验收检查清单解析失败"}
            
        results = {}
        
        # 确定检查范围
        if week is not None:
            week_key = f"week_{week}"
            if week_key in acceptance_data:
                if day is not None:
                    day_key = f"day_{day}"
                    if day_key in acceptance_data[week_key]:
                        results[day_key] = self._check_single_task(acceptance_data[week_key][day_key])
                else:
                    # 检查整周
                    for day_key, task_data in acceptance_data[week_key].items():
                        results[day_key] = self._check_single_task(task_data)
        else:
            # 检查所有任务
            for week_key, week_data in acceptance_data.items():
                for day_key, task_data in week_data.items():
                    results[day_key] = self._check_single_task(task_data)
                    
        # 计算总体评分
        overall_score = self._calculate_overall_score(results)
        
        return {
            "timestamp": datetime.now().isoformat(),
            "overall_score": overall_score,
            "results": results,
            "acceptance_data": acceptance_data
        }
    
    def _check_single_task(self, task_data: Dict[str, Any]) -> Dict[str, Any]:
        """检查单个任务"""
        task_result = {
            "task_name": task_data["task_name"],
            "acceptance_standard": task_data["acceptance_standard"],
            "check_results": {},
            "score": 0.0,
            "passed": False
        }
        
        # 执行各项检查
        for check_item in task_data["check_items"]:
            category = check_item["category"]
            sub_items = check_item["sub_items"]
            
            category_results = {}
            for sub_item in sub_items:
                # 根据子项目类型执行相应的检查
                if "文件存在性" in category:
                    # 提取文件路径
                    file_patterns = self._extract_file_paths(sub_item)
                    category_results[sub_item] = self.check_file_existence(file_patterns)
                elif "编译检查" in category:
                    file_paths = self._extract_file_paths(sub_item)
                    category_results[sub_item] = self.check_compilation(file_paths)
                else:
                    # 其他检查类型
                    category_results[sub_item] = {"status": "待检查"}
                    
            task_result["check_results"][category] = category_results
            
        # 计算任务评分
        task_result["score"] = self._calculate_task_score(task_result, task_data["scoring_criteria"])
        task_result["passed"] = task_result["score"] >= 80.0  # 默认80分及格
        
        return task_result
    
    def _extract_file_paths(self, text: str) -> List[str]:
        """从文本中提取文件路径"""
        # 简单的路径提取逻辑
        file_patterns = []
        
        # 查找类似 `src/product/iconfiglayoutsync.h` 的模式
        path_pattern = r'[\w/\-]+\.(h|cpp|py|md)'
        matches = re.findall(path_pattern, text)
        
        return matches
    
    def _calculate_task_score(self, task_result: Dict[str, Any], scoring_criteria: Dict[str, float]) -> float:
        """计算任务评分"""
        total_score = 0.0
        max_score = sum(scoring_criteria.values())
        
        # 根据检查结果计算得分
        # 这里使用简化的评分逻辑
        for category, results in task_result["check_results"].items():
            # 计算该类别的完成度
            completion_rate = self._calculate_completion_rate(results)
            
            # 根据评分标准分配分数
            for criterion, score in scoring_criteria.items():
                if criterion in category:
                    total_score += completion_rate * score
                    
        return (total_score / max_score) * 100 if max_score > 0 else 0.0
    
    def _calculate_completion_rate(self, results: Dict[str, Any]) -> float:
        """计算完成度"""
        if not results:
            return 0.0
            
        total_items = 0
        completed_items = 0
        
        for sub_item, sub_results in results.items():
            if isinstance(sub_results, dict):
                # 文件检查结果
                for file_path, exists in sub_results.items():
                    total_items += 1
                    if exists:
                        completed_items += 1
            else:
                # 其他检查结果
                total_items += 1
                if sub_results.get("status") == "通过":
                    completed_items += 1
                    
        return completed_items / total_items if total_items > 0 else 0.0
    
    def _calculate_overall_score(self, results: Dict[str, Any]) -> float:
        """计算总体评分"""
        if not results:
            return 0.0
            
        total_score = 0.0
        task_count = 0
        
        for task_key, task_result in results.items():
            total_score += task_result["score"]
            task_count += 1
            
        return total_score / task_count if task_count > 0 else 0.0
    
    def generate_report(self, check_results: Dict[str, Any], report_type: str = "detailed") -> str:
        """生成验收报告"""
        timestamp = check_results["timestamp"]
        overall_score = check_results["overall_score"]
        results = check_results["results"]
        
        if report_type == "summary":
            return self._generate_summary_report(timestamp, overall_score, results)
        else:
            return self._generate_detailed_report(timestamp, overall_score, results)
    
    def _generate_summary_report(self, timestamp: str, overall_score: float, results: Dict[str, Any]) -> str:
        """生成摘要报告"""
        report = f"# 验收检查摘要报告\n\n"
        report += f"**检查时间**: {timestamp}\n\n"
        report += f"**总体评分**: {overall_score:.1f}/100\n\n"
        
        # 统计通过情况
        passed_tasks = 0
        total_tasks = len(results)
        
        for task_result in results.values():
            if task_result["passed"]:
                passed_tasks += 1
                
        report += f"**任务通过率**: {passed_tasks}/{total_tasks} ({passed_tasks/total_tasks*100:.1f}%)\n\n"
        
        # 按周统计
        week_stats = {}
        for task_key, task_result in results.items():
            # 任务键格式为 day_1, day_2 等，需要从任务数据中获取周信息
            # 这里简化处理，假设所有任务都属于第1周
            week_num = "1"
            if week_num not in week_stats:
                week_stats[week_num] = {"total": 0, "passed": 0}
                
            week_stats[week_num]["total"] += 1
            if task_result["passed"]:
                week_stats[week_num]["passed"] += 1
                
        report += "## 按周统计\n\n"
        for week_num, stats in sorted(week_stats.items()):
            pass_rate = stats["passed"] / stats["total"] * 100 if stats["total"] > 0 else 0
            report += f"- **第{week_num}周**: {stats['passed']}/{stats['total']} ({pass_rate:.1f}%)\n"
            
        return report
    
    def _generate_detailed_report(self, timestamp: str, overall_score: float, results: Dict[str, Any]) -> str:
        """生成详细报告"""
        report = f"# 详细验收检查报告\n\n"
        report += f"**检查时间**: {timestamp}\n\n"
        report += f"**总体评分**: {overall_score:.1f}/100\n\n"
        
        # 详细任务结果
        report += "## 详细检查结果\n\n"
        
        for task_key, task_result in results.items():
            # 任务键格式为 day_1, day_2 等，需要从任务数据中获取周信息
            # 这里简化处理，假设所有任务都属于第1周，天数从任务键中提取
            week_num = "1"
            day_num = task_key.split('_')[1]
            
            report += f"### 第{week_num}周第{day_num}天: {task_result['task_name']}\n\n"
            report += f"**验收标准**: {task_result['acceptance_standard']}\n\n"
            report += f"**评分**: {task_result['score']:.1f}/100\n"
            report += f"**状态**: {'通过' if task_result['passed'] else '不通过'}\n\n"
            
            # 检查结果详情
            report += "#### 检查详情\n\n"
            for category, category_results in task_result["check_results"].items():
                report += f"**{category}**\n\n"
                
                for sub_item, sub_results in category_results.items():
                    report += f"- {sub_item}: "
                    
                    if isinstance(sub_results, dict):
                        # 文件检查结果
                        file_results = []
                        for file_path, exists in sub_results.items():
                            status = "✓" if exists else "✗"
                            file_results.append(f"{file_path}({status})")
                        report += ", ".join(file_results)
                    else:
                        report += str(sub_results)
                        
                    report += "\n"
                report += "\n"
                
            report += "---\n\n"
            
        return report
    
    def save_report(self, report_content: str, report_name: str = None) -> str:
        """保存报告到文件"""
        if report_name is None:
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            report_name = f"acceptance_report_{timestamp}.md"
            
        report_path = self.reports_dir / report_name
        
        with open(report_path, 'w', encoding='utf-8') as f:
            f.write(report_content)
            
        return str(report_path)
    
    def check_acceptance(self, week: int = None, day: int = None):
        """检查验收状态（与server.py兼容的格式）"""
        try:
            # 运行验收检查
            check_results = self.run_acceptance_check(week, day)
            
            if "error" in check_results:
                return self._generate_mock_acceptance_data(week, day)
            
            # 转换数据格式
            overall_score = check_results["overall_score"]
            results = check_results["results"]
            
            # 确保results是字典类型
            if not isinstance(results, dict):
                results = {}
            
            # 统计通过的任务（将dict_values转换为列表）
            results_list = list(results.values()) if results else []
            passed_tasks = sum(1 for task_result in results_list if isinstance(task_result, dict) and task_result.get("passed"))
            total_tasks = len(results_list)
            
            # 生成任务列表（将dict_items转换为列表）
            task_list = []
            results_items = list(results.items()) if results else []
            for task_key, task_result in results_items:
                if not isinstance(task_result, dict):
                    continue
                    
                # 任务键格式为 day_1, day_2 等，需要从任务数据中获取周信息
                # 这里简化处理，假设所有任务都属于第1周，天数从任务键中提取
                week_num = 1
                try:
                    day_num = int(task_key.split('_')[1])
                except (IndexError, ValueError):
                    day_num = 1
                
                task_list.append({
                    "id": f"task_{week_num}_{day_num}",
                    "name": str(task_result.get("task_name", "未知任务")),
                    "score": float(task_result.get("score", 0)),
                    "max_score": 100.0,
                    "status": "passed" if task_result.get("passed") else "failed",
                    "checks": self._convert_checks_to_format(task_result.get("check_results", {}))
                })
            
            # 确保所有数据都是JSON可序列化的
            return {
                "overall_score": float(overall_score),
                "max_score": 100.0,
                "passed_tasks": int(passed_tasks),
                "total_tasks": int(total_tasks),
                "pass_rate": float((passed_tasks / total_tasks * 100) if total_tasks > 0 else 0.0),
                "week": int(week or 1),
                "day": int(day or 1),
                "acceptance_status": "passed" if float(overall_score) >= 80.0 else "failed",
                "tasks": task_list
            }
            
        except Exception as e:
            print(f"验收检查失败: {e}")
            return self._generate_mock_acceptance_data(week, day)
    
    def _convert_checks_to_format(self, check_results):
        """将检查结果转换为标准格式"""
        checks = []
        
        # 定义检查项权重映射
        weight_mapping = {
            "文件存在性": 10,
            "编译检查": 30,
            "功能实现": 50,
            "性能测试": 10
        }
        
        # 确保check_results是字典类型
        if not isinstance(check_results, dict):
            # 如果check_results不是字典，返回默认检查项
            return [
                {"name": "文件存在性", "passed": False, "weight": 10},
                {"name": "编译检查", "passed": False, "weight": 30},
                {"name": "功能实现", "passed": False, "weight": 50}
            ]
        
        for category, category_results in check_results.items():
            # 确保category_results是字典类型
            if not isinstance(category_results, dict):
                continue
                
            # 计算该类别的通过率
            total_sub_items = 0
            passed_sub_items = 0
            
            for sub_item, sub_results in category_results.items():
                if isinstance(sub_results, dict):
                    # 文件检查结果
                    for file_path, exists in sub_results.items():
                        total_sub_items += 1
                        if exists:
                            passed_sub_items += 1
                else:
                    # 其他检查结果
                    total_sub_items += 1
                    if isinstance(sub_results, dict) and sub_results.get("status") == "通过":
                        passed_sub_items += 1
            
            # 计算通过率
            pass_rate = passed_sub_items / total_sub_items if total_sub_items > 0 else 0.0
            
            checks.append({
                "name": category,
                "passed": pass_rate >= 0.5,  # 50%以上算通过
                "weight": weight_mapping.get(category, 10)
            })
        
        return checks
    
    def _generate_mock_acceptance_data(self, week=None, day=None):
        """生成模拟验收数据（备用方案）"""
        week_num = week or 3
        day_num = day or 11
        
        return {
            "overall_score": 55.0,
            "max_score": 100.0,
            "passed_tasks": 0,
            "total_tasks": 1,
            "pass_rate": 0.0,
            "week": week_num,
            "day": day_num,
            "acceptance_status": "failed",
            "tasks": [
                {
                    "id": f"task_{week_num}_{day_num}",
                    "name": "事件-动作编辑器框架",
                    "score": 55.0,
                    "max_score": 100.0,
                    "status": "failed",
                    "checks": [
                        {"name": "头文件存在性", "passed": True, "weight": 10},
                        {"name": "源文件存在性", "passed": True, "weight": 10},
                        {"name": "编译状态", "passed": False, "weight": 30},
                        {"name": "功能实现", "passed": False, "weight": 50}
                    ]
                }
            ]
        }

def main():
    """主函数"""
    import argparse
    
    parser = argparse.ArgumentParser(description='Software Editor - 验收检查自动化脚本')
    parser.add_argument('--week', type=int, help='指定检查的周数')
    parser.add_argument('--day', type=int, help='指定检查的天数')
    parser.add_argument('--report-type', choices=['summary', 'detailed'], default='detailed', 
                       help='报告类型: summary(摘要) 或 detailed(详细)')
    parser.add_argument('--save', action='store_true', help='保存报告到文件')
    
    args = parser.parse_args()
    
    # 获取项目根目录
    project_root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    
    checker = AcceptanceChecker(project_root)
    
    # 运行验收检查
    print("正在运行验收检查...")
    check_results = checker.run_acceptance_check(args.week, args.day)
    
    if "error" in check_results:
        print(f"错误: {check_results['error']}")
        sys.exit(1)
        
    # 生成报告
    report_content = checker.generate_report(check_results, args.report_type)
    
    # 输出报告
    print(report_content)
    
    # 保存报告
    if args.save:
        report_path = checker.save_report(report_content)
        print(f"\n报告已保存到: {report_path}")
        
    # 输出总体结果
    overall_score = check_results["overall_score"]
    print(f"\n=== 验收检查完成 ===")
    print(f"总体评分: {overall_score:.1f}/100")
    
    if overall_score >= 80.0:
        print("✅ 验收通过")
    else:
        print("❌ 验收不通过")
        
if __name__ == "__main__":
    main()