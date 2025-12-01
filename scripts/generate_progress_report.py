from docx import Document
from docx.shared import Inches
from docx.enum.text import WD_ALIGN_PARAGRAPH
from docx.enum.section import WD_ORIENT
from docx.shared import Pt
from datetime import datetime
import json
import os

def generate_progress_report(report_data, output_path):
    """生成进度报告Word文档"""
    
    # 创建文档
    doc = Document()
    
    # 设置页面大小为A4
    section = doc.sections[0]
    section.page_width = Inches(8.27)
    section.page_height = Inches(11.69)
    section.left_margin = Inches(1.0)
    section.right_margin = Inches(1.0)
    section.top_margin = Inches(1.0)
    section.bottom_margin = Inches(1.0)
    
    # 标题
    title = doc.add_heading('Software Editor 开发进度报告', level=0)
    title.alignment = WD_ALIGN_PARAGRAPH.CENTER
    
    # 报告时间
    timestamp = datetime.fromisoformat(report_data['timestamp'].replace('Z', '+00:00'))
    time_para = doc.add_paragraph(f'生成时间: {timestamp.strftime("%Y年%m月%d日 %H:%M:%S")}')
    time_para.alignment = WD_ALIGN_PARAGRAPH.CENTER
    
    doc.add_paragraph()  # 空行
    
    # 总体进度摘要
    summary = report_data['summary']
    doc.add_heading('总体进度摘要', level=1)
    
    summary_table = doc.add_table(rows=4, cols=2)
    summary_table.style = 'Light Grid Accent 1'
    
    # 表头
    hdr_cells = summary_table.rows[0].cells
    hdr_cells[0].text = '指标'
    hdr_cells[1].text = '数值'
    
    # 总体进度
    row1 = summary_table.rows[1].cells
    row1[0].text = '总体进度'
    row1[1].text = f"{summary['overall_progress']:.1f}%"
    
    # 当前周
    row2 = summary_table.rows[2].cells
    row2[0].text = '当前开发周'
    row2[1].text = f"第{summary['current_week']}周"
    
    # 验收分数
    row3 = summary_table.rows[3].cells
    row3[0].text = '验收分数'
    row3[1].text = f"{summary['acceptance_score']:.1f}/100"
    
    doc.add_paragraph()  # 空行
    
    # 任务统计
    doc.add_heading('任务统计', level=1)
    
    task_table = doc.add_table(rows=4, cols=2)
    task_table.style = 'Light Grid Accent 1'
    
    # 表头
    hdr_cells = task_table.rows[0].cells
    hdr_cells[0].text = '任务状态'
    hdr_cells[1].text = '数量'
    
    # 已完成任务
    row1 = task_table.rows[1].cells
    row1[0].text = '已完成'
    row1[1].text = str(summary['completed_tasks'])
    
    # 进行中任务
    row2 = task_table.rows[2].cells
    row2[0].text = '进行中'
    row2[1].text = str(summary['in_progress_tasks'])
    
    # 待处理任务
    row3 = task_table.rows[3].cells
    row3[0].text = '待处理'
    row3[1].text = str(summary['pending_tasks'])
    
    doc.add_paragraph()  # 空行
    
    # 详细任务列表
    doc.add_heading('详细任务列表', level=1)
    
    progress_data = report_data['progress']
    if 'tasks' in progress_data and progress_data['tasks']:
        tasks_table = doc.add_table(rows=len(progress_data['tasks']) + 1, cols=5)
        tasks_table.style = 'Light Grid Accent 1'
        
        # 表头
        hdr_cells = tasks_table.rows[0].cells
        hdr_cells[0].text = '任务名称'
        hdr_cells[1].text = '周/日'
        hdr_cells[2].text = '进度'
        hdr_cells[3].text = '状态'
        hdr_cells[4].text = '相关文件'
        
        # 任务数据
        for i, task in enumerate(progress_data['tasks']):
            row_cells = tasks_table.rows[i + 1].cells
            row_cells[0].text = task.get('name', '未知任务')
            row_cells[1].text = f"第{task.get('week', '?')}周第{task.get('day', '?')}天"
            row_cells[2].text = f"{task.get('progress', 0):.1f}%"
            
            # 状态文本
            status = task.get('status', 'pending')
            if status == 'completed':
                status_text = '已完成'
            elif status == 'in_progress':
                status_text = '进行中'
            else:
                status_text = '待处理'
            row_cells[3].text = status_text
            
            # 相关文件
            files = task.get('files', [])
            row_cells[4].text = ', '.join(files) if files else '无'
    
    doc.add_paragraph()  # 空行
    
    # 验收检查结果
    doc.add_heading('验收检查结果', level=1)
    
    acceptance_data = report_data['acceptance']
    if 'tasks' in acceptance_data and acceptance_data['tasks']:
        for task in acceptance_data['tasks']:
            doc.add_heading(f"任务: {task.get('name', '未知任务')}", level=2)
            
            # 任务分数
            score_para = doc.add_paragraph()
            score_para.add_run(f"得分: {task.get('score', 0):.1f}/{task.get('max_score', 100):.1f} ")
            
            status = task.get('status', 'failed')
            if status == 'passed':
                score_para.add_run('✓ 通过').bold = True
            else:
                score_para.add_run('✗ 未通过').bold = True
            
            # 检查项
            if 'checks' in task and task['checks']:
                checks_table = doc.add_table(rows=len(task['checks']) + 1, cols=3)
                checks_table.style = 'Light List Accent 1'
                
                # 表头
                hdr_cells = checks_table.rows[0].cells
                hdr_cells[0].text = '检查项'
                hdr_cells[1].text = '权重'
                hdr_cells[2].text = '结果'
                
                # 检查数据
                for i, check in enumerate(task['checks']):
                    row_cells = checks_table.rows[i + 1].cells
                    row_cells[0].text = check.get('name', '未知检查项')
                    row_cells[1].text = str(check.get('weight', 0))
                    
                    if check.get('passed', False):
                        row_cells[2].text = '✓ 通过'
                    else:
                        row_cells[2].text = '✗ 未通过'
            
            doc.add_paragraph()  # 空行
    
    # 总结
    doc.add_heading('总结', level=1)
    
    if summary['overall_progress'] >= 80:
        conclusion = "项目进展顺利，大部分任务已完成，验收通过率较高。"
    elif summary['overall_progress'] >= 50:
        conclusion = "项目进展良好，部分任务已完成，需要继续推进剩余任务。"
    else:
        conclusion = "项目处于早期阶段，需要加快开发进度，重点关注验收标准。"
    
    doc.add_paragraph(conclusion)
    
    # 保存文档
    doc.save(output_path)
    return True

if __name__ == "__main__":
    # 测试代码
    sample_data = {
        "timestamp": "2024-01-01T12:00:00Z",
        "progress": {
            "overall_progress": 65.5,
            "current_week": 3,
            "tasks": [
                {
                    "name": "事件-动作编辑器框架",
                    "week": 3,
                    "day": 11,
                    "progress": 50.0,
                    "status": "in_progress",
                    "files": ["src/uilayout/eventactioneditor.h", "src/uilayout/eventactioneditor.cpp"]
                }
            ]
        },
        "acceptance": {
            "overall_score": 75.0,
            "tasks": [
                {
                    "name": "事件-动作编辑器框架",
                    "score": 75.0,
                    "max_score": 100.0,
                    "status": "passed",
                    "checks": [
                        {"name": "头文件存在性", "passed": True, "weight": 10},
                        {"name": "源文件存在性", "passed": True, "weight": 10},
                        {"name": "编译状态", "passed": True, "weight": 30},
                        {"name": "功能实现", "passed": True, "weight": 50}
                    ]
                }
            ]
        },
        "summary": {
            "overall_progress": 65.5,
            "current_week": 3,
            "total_tasks": 20,
            "completed_tasks": 5,
            "in_progress_tasks": 3,
            "pending_tasks": 12,
            "acceptance_score": 75.0,
            "passed_tasks": 1,
            "total_acceptance_tasks": 1
        }
    }
    
    generate_progress_report(sample_data, "test_report.docx")
    print("测试报告生成完成")