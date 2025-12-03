#!/usr/bin/env python3
"""
可视化Web界面后端API服务
提供进度数据和验收状态接口
"""

import json
import os
import sys
from datetime import datetime
from flask import Flask, jsonify, request, send_from_directory, send_file
from flask_cors import CORS
import subprocess

# 添加项目根目录到Python路径
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

# 导入进度跟踪和验收检查模块
try:
    from scripts.progress_tracker import ProgressTracker
    from scripts.acceptance_checker import AcceptanceChecker
except ImportError:
    # 如果导入失败，创建模拟数据
    class ProgressTracker:
        def get_progress_data(self):
            return {
                "overall_progress": 2.5,
                "current_week": 3,
                "week_progress": 10.0,
                "total_tasks": 20,
                "completed_tasks": 1,
                "in_progress_tasks": 1,
                "pending_tasks": 18,
                "tasks": [
                    {
                        "id": "task_3_11",
                        "name": "事件-动作编辑器框架",
                        "week": 3,
                        "day": 11,
                        "progress": 50.0,
                        "status": "in_progress",
                        "files": ["src/uilayout/eventactioneditor.h", "src/uilayout/eventactioneditor.cpp"]
                    }
                ]
            }
    
    class AcceptanceChecker:
        def check_acceptance(self, week=None, day=None):
            return {
                "overall_score": 55.0,
                "max_score": 100.0,
                "passed_tasks": 0,
                "total_tasks": 1,
                "pass_rate": 0.0,
                "week": week or 3,
                "day": day or 11,
                "acceptance_status": "failed",
                "tasks": [
                    {
                        "id": "task_3_11",
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

app = Flask(__name__, static_folder='.', static_url_path='')
CORS(app)  # 允许跨域请求

# 项目根目录
PROJECT_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

# 文档目录
DOCS_DIR = os.path.join(PROJECT_ROOT, 'docs')

# 构建目录
BUILD_DIR = os.path.join(PROJECT_ROOT, 'build')

# 模拟数据模式
USE_MOCK_DATA = False

# 获取项目根目录
project_root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

# 初始化跟踪器
try:
    tracker = ProgressTracker(project_root)
    acceptance_checker = AcceptanceChecker(project_root)
except Exception as e:
    print(f"初始化跟踪器失败: {e}")
    print("使用模拟数据模式")
    
    # 使用模拟数据类
    class MockProgressTracker:
        def get_progress_data(self):
            return {
                "overall_progress": 2.5,
                "current_week": 3,
                "week_progress": 10.0,
                "total_tasks": 20,
                "completed_tasks": 1,
                "in_progress_tasks": 1,
                "pending_tasks": 18,
                "tasks": [
                    {
                        "id": "task_3_11",
                        "name": "事件-动作编辑器框架",
                        "week": 3,
                        "day": 11,
                        "progress": 50.0,
                        "status": "in_progress",
                        "files": ["src/uilayout/eventactioneditor.h", "src/uilayout/eventactioneditor.cpp"]
                    }
                ]
            }
    
    class MockAcceptanceChecker:
        def check_acceptance(self, week=None, day=None):
            return {
                "overall_score": 55.0,
                "max_score": 100.0,
                "passed_tasks": 0,
                "total_tasks": 1,
                "pass_rate": 0.0,
                "week": week or 3,
                "day": day or 11,
                "acceptance_status": "failed",
                "tasks": [
                    {
                        "id": "task_3_11",
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
    
    tracker = MockProgressTracker()
    acceptance_checker = MockAcceptanceChecker()

@app.route('/')
def index():
    """重定向到前端页面"""
    return send_from_directory('.', 'index.html')

@app.route('/demo')
def demo():
    """演示页面"""
    return send_file('demo.html')

@app.route('/api/progress')
def get_progress():
    """获取开发进度数据"""
    try:
        # 尝试使用真实的进度跟踪器
        if tracker:
            progress_data = tracker.get_progress_data()
            return jsonify({
                "success": True,
                "data": progress_data,
                "timestamp": datetime.now().isoformat()
            })
    except Exception as e:
        print(f"获取真实进度数据失败: {e}")
    
    # 使用模拟数据作为备用
    return jsonify({
        "success": True,
        "data": {
            "overall_progress": 2.5,
            "current_week": 3,
            "week_progress": 10.0,
            "total_tasks": 20,
            "completed_tasks": 1,
            "in_progress_tasks": 1,
            "pending_tasks": 18,
            "tasks": [
                {
                    "id": "task_3_11",
                    "name": "事件-动作编辑器框架",
                    "week": 3,
                    "day": 11,
                    "progress": 50.0,
                    "status": "in_progress",
                    "files": ["src/uilayout/eventactioneditor.h", "src/uilayout/eventactioneditor.cpp"]
                }
            ]
        },
        "timestamp": datetime.now().isoformat()
    })

@app.route('/api/acceptance')
def get_acceptance():
    """获取验收检查数据"""
    try:
        # 尝试使用真实的验收检查器
        if acceptance_checker:
            week = request.args.get('week', type=int)
            day = request.args.get('day', type=int)
            acceptance_data = acceptance_checker.check_acceptance(week, day)
            return jsonify({
                "success": True,
                "data": acceptance_data,
                "timestamp": datetime.now().isoformat()
            })
    except Exception as e:
        print(f"获取真实验收数据失败: {e}")
    
    # 使用模拟数据作为备用
    return jsonify({
        "success": True,
        "data": {
            "overall_score": 55.0,
            "max_score": 100.0,
            "passed_tasks": 0,
            "total_tasks": 1,
            "pass_rate": 0.0,
            "week": 3,
            "day": 11,
            "acceptance_status": "failed",
            "tasks": [
                {
                    "id": "task_3_11",
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
        },
        "timestamp": datetime.now().isoformat()
    })

@app.route('/api/refresh')
def refresh_data():
    """刷新所有数据"""
    try:
        # 检查是否使用模拟数据模式
        if hasattr(tracker, '__class__') and tracker.__class__.__name__ == 'MockProgressTracker':
            # 使用模拟数据模式
            progress_data = tracker.get_progress_data()
            week = request.args.get('week', type=int, default=3)
            day = request.args.get('day', type=int, default=11)
            acceptance_data = acceptance_checker.check_acceptance(week, day)
        else:
            # 尝试使用真实的进度跟踪器
            try:
                progress_data = tracker.get_progress_data()
            except Exception as e:
                print(f"获取真实进度数据失败: {e}")
                # 使用模拟数据作为备用
                progress_data = {
                    "overall_progress": 2.5,
                    "current_week": 3,
                    "week_progress": 10.0,
                    "total_tasks": 20,
                    "completed_tasks": 1,
                    "in_progress_tasks": 1,
                    "pending_tasks": 18,
                    "tasks": [
                        {
                            "id": "task_3_11",
                            "name": "事件-动作编辑器框架",
                            "week": 3,
                            "day": 11,
                            "progress": 50.0,
                            "status": "in_progress",
                            "files": ["src/uilayout/eventactioneditor.h", "src/uilayout/eventactioneditor.cpp"]
                        }
                    ]
                }
            
            # 尝试使用真实的验收检查器
            try:
                week = request.args.get('week', type=int, default=3)
                day = request.args.get('day', type=int, default=11)
                acceptance_data = acceptance_checker.check_acceptance(week, day)
            except Exception as e:
                print(f"获取真实验收数据失败: {e}")
                # 使用模拟数据作为备用
                acceptance_data = {
                    "overall_score": 55.0,
                    "max_score": 100.0,
                    "passed_tasks": 0,
                    "total_tasks": 1,
                    "pass_rate": 0.0,
                    "week": week or 3,
                    "day": day or 11,
                    "acceptance_status": "failed",
                    "tasks": [
                        {
                            "id": "task_3_11",
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
        
        return jsonify({
            "success": True,
            "data": {
                "progress": progress_data,
                "acceptance": acceptance_data
            },
            "timestamp": datetime.now().isoformat()
        })
    except Exception as e:
        return jsonify({
            "success": False,
            "error": str(e),
            "timestamp": datetime.now().isoformat()
        }), 500

@app.route('/api/reports')
def get_reports():
    """获取报告文件列表"""
    try:
        reports_dir = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), 'docs')
        reports = []
        
        if os.path.exists(reports_dir):
            for file in os.listdir(reports_dir):
                if file.endswith('.md') and ('进度报告' in file or '验收报告' in file):
                    file_path = os.path.join(reports_dir, file)
                    stats = os.stat(file_path)
                    reports.append({
                        "name": file,
                        "path": file_path,
                        "size": stats.st_size,
                        "modified": datetime.fromtimestamp(stats.st_mtime).isoformat()
                    })
        
        return jsonify({
            "success": True,
            "data": reports,
            "timestamp": datetime.now().isoformat()
        })
    except Exception as e:
        return jsonify({
            "success": False,
            "error": str(e),
            "timestamp": datetime.now().isoformat()
        }), 500

@app.route('/api/report/<report_name>')
def get_report_content(report_name):
    """获取报告文件内容"""
    try:
        reports_dir = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), 'docs')
        file_path = os.path.join(reports_dir, report_name)
        
        if os.path.exists(file_path) and file_path.endswith('.md'):
            with open(file_path, 'r', encoding='utf-8') as f:
                content = f.read()
            
            return jsonify({
                "success": True,
                "data": {
                    "name": report_name,
                    "content": content
                },
                "timestamp": datetime.now().isoformat()
            })
        else:
            return jsonify({
                "success": False,
                "error": "报告文件不存在",
                "timestamp": datetime.now().isoformat()
            }), 404
    except Exception as e:
        return jsonify({
            "success": False,
            "error": str(e),
            "timestamp": datetime.now().isoformat()
        }), 500

@app.route('/api/build-status')
def get_build_status():
    """获取构建状态"""
    try:
        # 检查构建目录是否存在
        build_dir = os.path.join(PROJECT_ROOT, 'build')
        exe_path = os.path.join(build_dir, 'software-editor.exe')
        
        build_exists = os.path.exists(build_dir)
        exe_exists = os.path.exists(exe_path)
        
        return jsonify({
            "success": True,
            "data": {
                "build_directory_exists": build_exists,
                "executable_exists": exe_exists,
                "executable_path": exe_path if exe_exists else None,
                "last_checked": datetime.now().isoformat()
            },
            "timestamp": datetime.now().isoformat()
        })
    except Exception as e:
        return jsonify({
            "success": False,
            "error": str(e),
            "timestamp": datetime.now().isoformat()
        }), 500

@app.route('/api/generate-word-report', methods=['POST'])
def generate_word_report():
    """生成Word格式报告"""
    try:
        # 获取报告数据
        report_data = request.json
        
        # 生成唯一文件名
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        filename = f"progress_report_{timestamp}.docx"
        output_path = os.path.join(PROJECT_ROOT, 'docs', filename)
        
        # 确保docs目录存在
        os.makedirs(os.path.dirname(output_path), exist_ok=True)
        
        # 导入并调用Word报告生成器
        sys.path.append(os.path.join(PROJECT_ROOT, 'scripts'))
        from generate_progress_report import generate_progress_report
        
        # 生成报告
        success = generate_progress_report(report_data, output_path)
        
        if success:
            # 返回下载URL
            download_url = f"/api/download-report/{filename}"
            return jsonify({
                "success": True,
                "message": "Word报告生成成功",
                "download_url": download_url,
                "filename": filename,
                "timestamp": datetime.now().isoformat()
            })
        else:
            return jsonify({
                "success": False,
                "error": "报告生成失败",
                "timestamp": datetime.now().isoformat()
            }), 500
            
    except Exception as e:
        print(f"生成Word报告失败: {e}")
        return jsonify({
            "success": False,
            "error": str(e),
            "timestamp": datetime.now().isoformat()
        }), 500

@app.route('/api/download-report/<filename>')
def download_report(filename):
    """下载报告文件"""
    try:
        file_path = os.path.join(PROJECT_ROOT, 'docs', filename)
        
        if os.path.exists(file_path):
            return send_file(file_path, as_attachment=True)
        else:
            return jsonify({
                "success": False,
                "error": "文件不存在",
                "timestamp": datetime.now().isoformat()
            }), 404
    except Exception as e:
        return jsonify({
            "success": False,
            "error": str(e),
            "timestamp": datetime.now().isoformat()
        }), 500

@app.route('/api/launch')
def launch_software():
    """启动软件"""
    exe_path = os.path.join(BUILD_DIR, 'software-editor.exe')
    
    if os.path.exists(exe_path):
        try:
            # 在Windows上启动应用程序
            if sys.platform == "win32":
                subprocess.Popen([exe_path], shell=True)
            else:
                subprocess.Popen([exe_path])
            
            return jsonify({
                "success": True,
                "message": "应用程序已启动"
            })
        except Exception as e:
            return jsonify({
                "success": False,
                "error": str(e)
            })
    
    return jsonify({
        "success": False,
        "error": "可执行文件不存在"
    })

# 演示平台专用API接口

@app.route('/api/software-status')
def get_software_status():
    """获取软件状态信息"""
    exe_path = os.path.join(BUILD_DIR, 'software-editor.exe')
    
    if os.path.exists(exe_path):
        stat = os.stat(exe_path)
        return jsonify({
            "exists": True,
            "size": f"{stat.st_size / 1024 / 1024:.1f} MB",
            "modified": datetime.fromtimestamp(stat.st_mtime).strftime("%Y-%m-%d %H:%M:%S"),
            "status": "ready"
        })
    
    return jsonify({
        "exists": False,
        "status": "error"
    })

@app.route('/api/doc-content')
def get_doc_content():
    """获取文档内容"""
    doc_type = request.args.get('type', 'dev-plan')
    
    # 映射文档类型到文件名
    doc_files = {
        'dev-plan': '开发计划.md',
        'detailed-plan': '详细开发实施计划.md', 
        'architecture': '代码架构文档.md'
    }
    
    filename = doc_files.get(doc_type)
    if not filename:
        return jsonify({
            "success": False,
            "error": "未知的文档类型"
        })
    
    file_path = os.path.join(DOCS_DIR, filename)
    
    if os.path.exists(file_path):
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                content = f.read()
            
            return jsonify({
                "success": True,
                "type": doc_type,
                "filename": filename,
                "content": content
            })
        except Exception as e:
            return jsonify({
                "success": False,
                "error": f"读取文件失败: {str(e)}"
            })
    
    return jsonify({
        "success": False,
        "error": "文档文件不存在"
    })

@app.route('/api/launch-software', methods=['POST'])
def launch_software_demo():
    """启动软件（演示平台专用）"""
    exe_path = os.path.join(BUILD_DIR, 'software-editor.exe')
    
    if os.path.exists(exe_path):
        try:
            # 在Windows上启动应用程序
            if sys.platform == "win32":
                subprocess.Popen([exe_path], shell=True)
            else:
                subprocess.Popen([exe_path])
            
            return jsonify({
                "success": True,
                "message": "Software Editor 启动成功"
            })
        except Exception as e:
            return jsonify({
                "success": False,
                "error": str(e)
            })
    
    return jsonify({
        "success": False,
        "error": "可执行文件不存在，请先构建项目"
    })

@app.route('/api/demo-info')
def get_demo_info():
    """获取演示相关信息"""
    return jsonify({
        "project_name": "Software Editor",
        "version": "1.0.0",
        "description": "专为产品工程师设计的可视化软件开发工具",
        "features": [
            "产品配置编辑",
            "UI布局设计", 
            "智能打包",
            "模块化架构",
            "可视化编程"
        ],
        "tech_stack": ["Qt 5.15+", "C++17", "CMake", "模块化架构"],
        "development_phases": [
            {
                "phase": "第一阶段",
                "name": "核心功能完善",
                "duration": "1-2个月",
                "status": "进行中"
            },
            {
                "phase": "第二阶段", 
                "name": "高级功能开发",
                "duration": "2-3个月",
                "status": "规划中"
            },
            {
                "phase": "第三阶段",
                "name": "扩展功能",
                "duration": "3-4个月", 
                "status": "规划中"
            }
        ]
    })

if __name__ == '__main__':
    print("启动可视化Web界面后端服务...")
    print("服务地址: http://localhost:5000")
    print("前端页面: http://localhost:5000/")
    print("API文档:")
    print("  - GET /api/progress - 获取开发进度")
    print("  - GET /api/acceptance - 获取验收状态")
    print("  - GET /api/refresh - 刷新所有数据")
    print("  - GET /api/reports - 获取报告列表")
    print("  - GET /api/report/<name> - 获取报告内容")
    print("  - GET /api/build-status - 获取构建状态")
    
    app.run(host='0.0.0.0', port=5000, debug=True)