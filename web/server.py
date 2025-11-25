#!/usr/bin/env python3
"""
可视化Web界面后端API服务
提供进度数据和验收状态接口
"""

import json
import os
import sys
from datetime import datetime
from flask import Flask, jsonify, request, send_from_directory
from flask_cors import CORS

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

@app.route('/api/progress')
def get_progress():
    """获取开发进度数据"""
    try:
        # 尝试使用真实的进度跟踪器
        if tracker:
            progress_data = tracker.get_progress_data()
            return jsonify(progress_data)
    except Exception as e:
        print(f"获取真实进度数据失败: {e}")
    
    # 使用模拟数据作为备用
    return jsonify({
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
            return jsonify(acceptance_data)
    except Exception as e:
        print(f"获取真实验收数据失败: {e}")
    
    # 使用模拟数据作为备用
    return jsonify({
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
    })

@app.route('/api/refresh')
def refresh_data():
    """刷新所有数据"""
    try:
        # 重新运行进度跟踪
        progress_data = tracker.get_progress_data()
        
        # 重新运行验收检查（使用默认参数）
        week = request.args.get('week', type=int, default=3)
        day = request.args.get('day', type=int, default=11)
        acceptance_data = acceptance_checker.check_acceptance(week, day)
        
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
        build_dir = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), 'build')
        exe_path = os.path.join(build_dir, 'software-editor.exe')
        
        build_status = {
            "exe_exists": os.path.exists(exe_path),
            "exe_size": os.path.getsize(exe_path) if os.path.exists(exe_path) else 0,
            "build_dir_exists": os.path.exists(build_dir),
            "last_modified": None
        }
        
        if build_status["exe_exists"]:
            stats = os.stat(exe_path)
            build_status["last_modified"] = datetime.fromtimestamp(stats.st_mtime).isoformat()
        
        return jsonify({
            "success": True,
            "data": build_status,
            "timestamp": datetime.now().isoformat()
        })
    except Exception as e:
        return jsonify({
            "success": False,
            "error": str(e),
            "timestamp": datetime.now().isoformat()
        }), 500

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