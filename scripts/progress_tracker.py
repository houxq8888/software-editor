#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Software Editor - 进度跟踪工具
基于详细开发实施计划自动计算项目完成百分比
"""

import os
import json
import re
from datetime import datetime
from pathlib import Path

class ProgressTracker:
    def __init__(self, project_root):
        self.project_root = Path(project_root)
        self.plan_file = self.project_root / "docs" / "详细开发实施计划.md"
        self.task_mapping = self.load_task_mapping()
        self.results = {}
    
    def load_task_mapping(self):
        """从详细开发实施计划.md加载任务映射配置"""
        if not self.plan_file.exists():
            print(f"警告: 计划文件 {self.plan_file} 不存在，使用默认任务映射")
            return self.load_default_task_mapping()
        
        try:
            with open(self.plan_file, 'r', encoding='utf-8') as f:
                content = f.read()
            
            return self.parse_development_plan(content)
        except Exception as e:
            print(f"解析计划文件失败: {e}，使用默认任务映射")
            return self.load_default_task_mapping()
    
    def parse_development_plan(self, content):
        """解析详细开发实施计划文档"""
        import re
        
        tasks = []
        current_week = 0
        current_day = 0
        
        # 解析周和天的任务
        week_pattern = r'### 第(\d+)周：([^\n]+)'
        day_pattern = r'#### 第(\d+)天：([^\n]+)'
        
        week_matches = list(re.finditer(week_pattern, content))
        day_matches = list(re.finditer(day_pattern, content))
        
        for day_match in day_matches:
            day_num = int(day_match.group(1))
            day_description = day_match.group(2).strip()
            
            # 找到对应的周
            week_num = 1
            for week_match in week_matches:
                if week_match.start() < day_match.start():
                    week_num = int(week_match.group(1))
                else:
                    break
            
            # 提取任务描述和预期文件
            task_description = day_description
            expected_files = self.extract_expected_files(content, day_match.start(), day_match.end())
            
            # 计算权重（基于任务复杂度）
            weight = self.calculate_task_weight(day_description, expected_files)
            
            task = {
                "task_id": f"week{week_num}_day{day_num}",
                "description": task_description,
                "files": expected_files,
                "weight": weight,
                "criteria": self.generate_acceptance_criteria(day_description),
                "week": week_num,
                "day": day_num
            }
            
            tasks.append(task)
        
        return tasks
    
    def extract_expected_files(self, content, start_pos, end_pos):
        """从计划文档中提取预期文件"""
        import re
        
        # 查找文件路径模式
        file_pattern = r'[\w/\-]+\.(h|cpp|py|md|txt|bat)'
        
        # 在任务描述附近查找文件路径
        search_start = max(0, start_pos - 500)
        search_end = min(len(content), end_pos + 500)
        
        search_content = content[search_start:search_end]
        file_matches = re.findall(file_pattern, search_content)
        
        # 过滤和标准化文件路径
        expected_files = []
        for file_match in file_matches:
            file_path = file_match
            # 确保路径格式正确
            if not file_path.startswith('src/') and not file_path.startswith('test/'):
                if file_path.endswith('.h') or file_path.endswith('.cpp'):
                    file_path = f"src/{file_path}"
                elif file_path.endswith('_test.cpp'):
                    file_path = f"test/{file_path}"
            
            if file_path not in expected_files:
                expected_files.append(file_path)
        
        # 如果没有找到文件，使用基于任务描述的默认文件
        if not expected_files:
            expected_files = self.generate_default_files(content[start_pos:end_pos])
        
        return expected_files
    
    def generate_default_files(self, task_description):
        """基于任务描述生成默认文件列表"""
        files = []
        
        # 根据任务类型生成相应的文件
        if "接口" in task_description or "类" in task_description:
            # 提取类名或接口名
            import re
            class_match = re.search(r'([A-Z][a-zA-Z]+)(?:接口|类|实现)', task_description)
            if class_match:
                class_name = class_match.group(1)
                files.append(f"src/product/{class_name.lower()}.h")
                files.append(f"src/product/{class_name.lower()}.cpp")
        
        elif "测试" in task_description:
            files.append("test/integration_test.cpp")
        
        elif "界面" in task_description or "UI" in task_description:
            files.append("src/ui/mainwindow.h")
            files.append("src/ui/mainwindow.cpp")
        
        return files if files else ["src/temp/placeholder.cpp"]
    
    def calculate_task_weight(self, description, files):
        """计算任务权重"""
        weight = 2.0  # 基础权重
        
        # 基于描述复杂度调整权重
        if "集成" in description or "测试" in description:
            weight += 1.0
        if "核心" in description or "主要" in description:
            weight += 1.0
        if "优化" in description or "性能" in description:
            weight += 0.5
        
        # 基于文件数量调整权重
        if len(files) > 2:
            weight += 0.5
        if len(files) > 4:
            weight += 0.5
        
        return min(weight, 5.0)  # 最大权重为5.0
    
    def generate_acceptance_criteria(self, description):
        """生成验收标准"""
        criteria = ["文件存在", "编译通过"]
        
        if "接口" in description:
            criteria.append("接口完整")
        if "测试" in description:
            criteria.append("测试通过")
        if "集成" in description:
            criteria.append("集成验证")
        if "性能" in description:
            criteria.append("性能达标")
        
        return criteria
    
    def load_default_task_mapping(self):
        """加载默认任务映射（备用方案）"""
        return [
            # 第1周任务
            {
                "task_id": "week1_day1",
                "description": "IConfigLayoutSync接口定义",
                "files": ["src/product/iconfiglayoutsync.h", "src/product/iconfiglayoutsync.cpp"],
                "weight": 2.0,
                "criteria": ["文件存在", "编译通过", "接口完整"]
            },
            {
                "task_id": "week1_day2",
                "description": "ConfigParser类实现",
                "files": ["src/product/configparser.h", "src/product/configparser.cpp"],
                "weight": 3.0,
                "criteria": ["文件存在", "JSON解析功能", "错误处理"]
            },
            {
                "task_id": "week1_day3",
                "description": "实时同步机制",
                "files": ["src/product/configchangenotifier.h", "src/product/syncstatemanager.h"],
                "weight": 3.0,
                "criteria": ["观察者模式实现", "信号槽集成", "同步测试"]
            },
            {
                "task_id": "week1_day4",
                "description": "布局模板系统",
                "files": ["src/product/layouttemplate.h", "src/product/templateloader.h"],
                "weight": 2.0,
                "criteria": ["模板格式定义", "加载器实现", "预览功能"]
            },
            {
                "task_id": "week1_day5",
                "description": "集成测试和优化",
                "files": ["test/integration_test_config_layout.cpp"],
                "weight": 2.0,
                "criteria": ["集成测试通过", "性能优化", "验收测试"]
            },
            
            # 第2周任务
            {
                "task_id": "week2_day6",
                "description": "打包向导界面设计",
                "files": ["src/package/packagewizarddialog.h", "src/package/wizardstepmanager.h"],
                "weight": 4.0,
                "criteria": ["向导界面", "步骤管理", "项目选择"]
            },
            {
                "task_id": "week2_day7",
                "description": "打包配置界面",
                "files": ["src/package/packageconfigwidget.h", "src/package/configvalidator.h"],
                "weight": 3.0,
                "criteria": ["配置界面", "验证器", "持久化"]
            },
            {
                "task_id": "week2_day8",
                "description": "进度可视化实现",
                "files": ["src/package/progressdisplaywidget.h", "src/package/progressupdater.h"],
                "weight": 3.0,
                "criteria": ["进度显示", "异步打包", "更新机制"]
            },
            {
                "task_id": "week2_day9",
                "description": "错误处理和用户提示",
                "files": ["src/package/errorhandler.h", "src/package/userpromptsystem.h"],
                "weight": 2.0,
                "criteria": ["错误分类", "提示系统", "日志记录"]
            },
            {
                "task_id": "week2_day10",
                "description": "打包流程集成测试",
                "files": ["test/integration_test_package_flow.cpp"],
                "weight": 3.0,
                "criteria": ["端到端测试", "性能测试", "用户体验"]
            },
            
            # 第3周任务
            {
                "task_id": "week3_day11",
                "description": "事件-动作编辑器框架",
                "files": ["src/uilayout/eventactionmodel.h", "src/uilayout/actionexecutor.h"],
                "weight": 3.0,
                "criteria": ["数据模型", "动作执行器", "事件定义"]
            },
            {
                "task_id": "week3_day12",
                "description": "可视化编辑器界面",
                "files": ["src/uilayout/eventactioneditorwidget.h", "src/uilayout/editortoolbar.h"],
                "weight": 4.0,
                "criteria": ["编辑器界面", "拖拽连接", "工具栏"]
            },
            {
                "task_id": "week3_day13",
                "description": "交互逻辑配置",
                "files": ["src/uilayout/eventbinder.h", "src/uilayout/actionconfigdialog.h"],
                "weight": 3.0,
                "criteria": ["事件绑定", "配置对话框", "序列化"]
            },
            {
                "task_id": "week3_day14",
                "description": "常用交互模板",
                "files": ["src/uilayout/interactiontemplates.h"],
                "weight": 2.0,
                "criteria": ["按钮模板", "表单模板", "模板管理"]
            },
            {
                "task_id": "week3_day15",
                "description": "交互逻辑测试功能",
                "files": ["src/uilayout/testrunner.h", "src/uilayout/debuginfodisplay.h"],
                "weight": 3.0,
                "criteria": ["测试运行器", "调试显示", "集成测试"]
            },
            
            # 第4周任务
            {
                "task_id": "week4_day16",
                "description": "三大模块深度集成",
                "files": ["src/integration/modulesyncmanager.h"],
                "weight": 5.0,
                "criteria": ["自动同步", "级联更新", "集成测试"]
            },
            {
                "task_id": "week4_day17",
                "description": "打包流程与UI布局集成",
                "files": ["src/integration/layoutdependencyanalyzer.h"],
                "weight": 4.0,
                "criteria": ["布局映射", "依赖分析", "验证功能"]
            },
            {
                "task_id": "week4_day18",
                "description": "性能优化和稳定性",
                "files": ["src/optimization/memoryoptimizer.h"],
                "weight": 3.0,
                "criteria": ["内存优化", "响应优化", "稳定性测试"]
            },
            {
                "task_id": "week4_day19",
                "description": "用户体验改进",
                "files": ["src/ui/uximprovements.h"],
                "weight": 2.0,
                "criteria": ["界面优化", "帮助系统", "用户反馈"]
            },
            {
                "task_id": "week4_day20",
                "description": "第一阶段功能验收",
                "files": ["test/final_acceptance_test.cpp"],
                "weight": 5.0,
                "criteria": ["端到端测试", "性能基准", "用户体验"]
            }
        ]
    
    def check_file_existence(self, file_path):
        """检查文件是否存在"""
        full_path = self.project_root / file_path
        return full_path.exists()
    
    def check_file_content(self, file_path, keywords=None):
        """检查文件内容是否包含关键特征"""
        if not self.check_file_existence(file_path):
            return False
        
        try:
            with open(self.project_root / file_path, 'r', encoding='utf-8') as f:
                content = f.read()
            
            if keywords:
                for keyword in keywords:
                    if keyword not in content:
                        return False
            
            # 检查基本代码结构
            if file_path.endswith('.h'):
                if 'class' not in content and 'struct' not in content:
                    return False
            elif file_path.endswith('.cpp'):
                if '#include' not in content:
                    return False
            
            return True
        except Exception as e:
            print(f"检查文件 {file_path} 时出错: {e}")
            return False
    
    def check_compilation_status(self, file_path):
        """检查文件编译状态（简化版）"""
        if not self.check_file_existence(file_path):
            return False
        
        # 检查CMakeLists.txt是否包含该文件
        cmake_file = self.project_root / "CMakeLists.txt"
        if cmake_file.exists():
            try:
                with open(cmake_file, 'r', encoding='utf-8') as f:
                    cmake_content = f.read()
                
                # 简化检查：文件是否在源代码列表中
                filename = Path(file_path).name
                if filename in cmake_content:
                    return True
            except:
                pass
        
        return False
    
    def evaluate_task_completion(self, task):
        """评估单个任务的完成度"""
        task_result = {
            "task_id": task["task_id"],
            "description": task["description"],
            "files": {},
            "completion_rate": 0.0,
            "details": []
        }
        
        file_scores = []
        
        for file_path in task["files"]:
            file_result = {
                "file": file_path,
                "exists": False,
                "has_content": False,
                "compilable": False,
                "score": 0.0
            }
            
            # 检查文件存在
            exists = self.check_file_existence(file_path)
            file_result["exists"] = exists
            
            if exists:
                # 检查文件内容
                has_content = self.check_file_content(file_path)
                file_result["has_content"] = has_content
                
                # 检查编译状态
                compilable = self.check_compilation_status(file_path)
                file_result["compilable"] = compilable
                
                # 计算文件得分
                if compilable:
                    file_result["score"] = 1.0
                elif has_content:
                    file_result["score"] = 0.5
                elif exists:
                    file_result["score"] = 0.25
            
            task_result["files"][file_path] = file_result
            file_scores.append(file_result["score"])
        
        # 计算任务完成度（取文件得分的平均值）
        if file_scores:
            task_result["completion_rate"] = sum(file_scores) / len(file_scores)
        
        # 添加详细说明
        if task_result["completion_rate"] == 0:
            task_result["details"].append("任务未开始")
        elif task_result["completion_rate"] < 0.5:
            task_result["details"].append("代码框架搭建中")
        elif task_result["completion_rate"] < 0.75:
            task_result["details"].append("核心功能实现中")
        elif task_result["completion_rate"] < 1.0:
            task_result["details"].append("功能测试进行中")
        else:
            task_result["details"].append("任务已完成")
        
        return task_result
    
    def calculate_overall_progress(self):
        """计算总体进度"""
        total_weight = 0
        completed_weight = 0
        
        self.results["tasks"] = {}
        
        for task in self.task_mapping:
            task_result = self.evaluate_task_completion(task)
            self.results["tasks"][task["task_id"]] = task_result
            
            total_weight += task["weight"]
            completed_weight += task["weight"] * task_result["completion_rate"]
        
        if total_weight > 0:
            overall_progress = (completed_weight / total_weight) * 100
        else:
            overall_progress = 0
        
        self.results["overall_progress"] = overall_progress
        self.results["total_weight"] = total_weight
        self.results["completed_weight"] = completed_weight
        
        return overall_progress
    
    def generate_progress_report(self):
        """生成进度报告"""
        progress = self.calculate_overall_progress()
        
        report = {
            "timestamp": datetime.now().isoformat(),
            "project_root": str(self.project_root),
            "overall_progress": progress,
            "weekly_progress": self.calculate_weekly_progress(),
            "task_details": self.results["tasks"],
            "summary": self.generate_summary()
        }
        
        return report
    
    def calculate_weekly_progress(self):
        """计算每周进度"""
        weekly_progress = {}
        
        for week in range(1, 5):
            week_tasks = [t for t in self.task_mapping if t["task_id"].startswith(f"week{week}")]
            if not week_tasks:
                weekly_progress[f"第{week}周"] = 0.0
                continue
            
            week_weight = sum(t["weight"] for t in week_tasks)
            week_completed = sum(t["weight"] * self.results["tasks"][t["task_id"]]["completion_rate"] 
                               for t in week_tasks)
            
            if week_weight > 0:
                weekly_progress[f"第{week}周"] = (week_completed / week_weight) * 100
            else:
                weekly_progress[f"第{week}周"] = 0.0
        
        return weekly_progress
    
    def generate_summary(self):
        """生成进度摘要"""
        completed_tasks = sum(1 for task in self.results["tasks"].values() 
                             if task["completion_rate"] >= 0.75)
        total_tasks = len(self.task_mapping)
        
        return {
            "completed_tasks": completed_tasks,
            "total_tasks": total_tasks,
            "completion_rate": f"{completed_tasks}/{total_tasks}",
            "status": self.get_project_status()
        }
    
    def get_project_status(self):
        """获取项目状态描述"""
        progress = self.results["overall_progress"]
        
        if progress < 25:
            return "项目启动阶段"
        elif progress < 50:
            return "核心开发阶段"
        elif progress < 75:
            return "功能完善阶段"
        elif progress < 90:
            return "测试优化阶段"
        else:
            return "验收准备阶段"
    
    def save_report(self, output_file="progress_report.json"):
        """保存进度报告到文件"""
        report = self.generate_progress_report()
        
        output_path = self.project_root / output_file
        with open(output_path, 'w', encoding='utf-8') as f:
            json.dump(report, f, ensure_ascii=False, indent=2)
        
        return str(output_path)
    
    def print_report(self):
        """打印进度报告"""
        report = self.generate_progress_report()
        
        print("=" * 60)
        print("Software Editor - 开发进度报告")
        print("=" * 60)
        print(f"报告时间: {report['timestamp']}")
        print(f"项目路径: {report['project_root']}")
        print(f"总体进度: {report['overall_progress']:.1f}%")
        print()
        
        print("每周进度:")
        for week, progress in report['weekly_progress'].items():
            print(f"  {week}: {progress:.1f}%")
        print()
        
        print("项目摘要:")
        summary = report['summary']
        print(f"  已完成任务: {summary['completed_tasks']}/{summary['total_tasks']}")
        print(f"  项目状态: {summary['status']}")
        print()
        
        print("详细任务状态:")
        for task_id, task in report['task_details'].items():
            status_icon = "✓" if task['completion_rate'] >= 0.75 else "○"
            print(f"  {status_icon} {task_id}: {task['description']} ({task['completion_rate']*100:.0f}%)")
    
    def get_progress_data(self):
        """获取进度数据（与server.py兼容的格式）"""
        report = self.generate_progress_report()
        
        # 计算当前周（基于进度最高的周）
        weekly_progress = report['weekly_progress']
        current_week = 1
        max_progress = 0
        
        for week, progress in weekly_progress.items():
            if progress > max_progress:
                max_progress = progress
                current_week = int(week.replace("第", "").replace("周", ""))
        
        # 统计任务状态
        completed_tasks = 0
        in_progress_tasks = 0
        pending_tasks = 0
        
        task_list = []
        
        for task_id, task in report['task_details'].items():
            completion_rate = task['completion_rate']
            
            if completion_rate >= 0.75:
                completed_tasks += 1
                status = "completed"
            elif completion_rate > 0:
                in_progress_tasks += 1
                status = "in_progress"
            else:
                pending_tasks += 1
                status = "pending"
            
            # 提取周和天信息
            week_num = int(task_id.split('_')[0].replace('week', ''))
            day_num = int(task_id.split('_')[1].replace('day', ''))
            
            task_list.append({
                "id": task_id,
                "name": task['description'],
                "week": week_num,
                "day": day_num,
                "progress": completion_rate * 100,
                "status": status,
                "files": list(task['files'].keys())  # 转换为列表避免JSON序列化错误
            })
        
        return {
            "overall_progress": report['overall_progress'],
            "current_week": current_week,
            "week_progress": weekly_progress.get(f"第{current_week}周", 0.0),
            "total_tasks": len(report['task_details']),
            "completed_tasks": completed_tasks,
            "in_progress_tasks": in_progress_tasks,
            "pending_tasks": pending_tasks,
            "tasks": task_list
        }


def main():
    """主函数"""
    project_root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    tracker = ProgressTracker(project_root)
    
    # 生成并打印报告
    tracker.print_report()
    
    # 保存报告到文件
    report_file = tracker.save_report()
    print(f"详细报告已保存到: {report_file}")
    
    # 生成Markdown格式的进度报告
    generate_markdown_report(tracker)


def generate_markdown_report(tracker):
    """生成Markdown格式的进度报告"""
    report = tracker.generate_progress_report()
    
    md_content = f"""# Software Editor - 开发进度报告

**报告时间**: {report['timestamp']}  
**项目路径**: {report['project_root']}  
**总体进度**: **{report['overall_progress']:.1f}%**  
**项目状态**: {report['summary']['status']}

## 进度概览

| 指标 | 数值 |
|------|------|
| 总体进度 | {report['overall_progress']:.1f}% |
| 已完成任务 | {report['summary']['completed_tasks']}/{report['summary']['total_tasks']} |
| 项目状态 | {report['summary']['status']} |

## 每周进度

| 周数 | 进度 | 状态 |
|------|------|------|"""
    
    for week, progress in report['weekly_progress'].items():
        status = "✅ 已完成" if progress >= 90 else "🔄 进行中" if progress > 0 else "⏳ 未开始"
        md_content += f"\n| {week} | {progress:.1f}% | {status} |"
    
    md_content += """

## 详细任务状态

| 任务ID | 描述 | 进度 | 状态 |
|--------|------|------|------|"""
    
    for task_id, task in report['task_details'].items():
        status_icon = "✅" if task['completion_rate'] >= 0.75 else "🔄" if task['completion_rate'] > 0 else "⏳"
        status_text = "已完成" if task['completion_rate'] >= 0.75 else "进行中" if task['completion_rate'] > 0 else "未开始"
        md_content += f"\n| {task_id} | {task['description']} | {task['completion_rate']*100:.0f}% | {status_icon} {status_text} |"
    
    md_content += """

## 下一步建议

根据当前进度，建议重点关注以下任务：
"""
    
    # 找出进度较低的任务
    low_progress_tasks = [t for t in report['task_details'].values() 
                         if t['completion_rate'] < 0.5]
    
    if low_progress_tasks:
        for task in low_progress_tasks[:3]:  # 只显示前3个
            md_content += f"\n- **{task['description']}** (当前进度: {task['completion_rate']*100:.0f}%)"
    else:
        md_content += "\n- 所有任务进展良好，继续保持！"
    
    # 保存Markdown报告
    md_file = tracker.project_root / "docs" / "进度报告.md"
    with open(md_file, 'w', encoding='utf-8') as f:
        f.write(md_content)
    
    print(f"Markdown报告已保存到: {md_file}")


if __name__ == "__main__":
    main()