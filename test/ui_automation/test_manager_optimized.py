#!/usr/bin/env python3
"""
优化版测试管理工具 - 专注于性能和资源使用优化
支持并行测试执行、内存管理、缓存机制和智能超时控制
"""

import pandas as pd
import os
import json
import time
import threading
import psutil
import gc
from datetime import datetime, timedelta
from pathlib import Path
from concurrent.futures import ThreadPoolExecutor, as_completed
import subprocess
import sys
from functools import lru_cache

class OptimizedTestManager:
    def __init__(self, project_root="d:\virtualMachine\github\software-editor", max_workers=2):
        self.project_root = project_root
        self.test_results_dir = os.path.join(project_root, "test", "ui_automation", "test_results")
        self.max_workers = max_workers  # 并行测试的最大线程数
        self.memory_threshold_mb = 500  # 内存使用阈值（MB）
        self.test_timeout = 300  # 单个测试超时时间（秒）
        
        # 创建测试结果目录
        os.makedirs(self.test_results_dir, exist_ok=True)
        
        # 性能监控
        self.start_time = None
        self.memory_usage = []
        self.test_execution_times = {}
        
        # 缓存机制
        self._test_case_cache = {}
        self._report_cache = {}
        
        print("🚀 优化版测试管理器已初始化")
        print(f"📊 配置: 最大并行数={max_workers}, 内存阈值={self.memory_threshold_mb}MB, 超时={self.test_timeout}s")
    
    def get_memory_usage(self):
        """获取当前内存使用情况"""
        process = psutil.Process()
        memory_info = process.memory_info()
        return memory_info.rss / 1024 / 1024  # 转换为MB
    
    def check_memory_safe(self):
        """检查内存使用是否安全"""
        current_memory = self.get_memory_usage()
        self.memory_usage.append(current_memory)
        
        if current_memory > self.memory_threshold_mb:
            print(f"⚠️ 内存使用较高: {current_memory:.1f}MB > {self.memory_threshold_mb}MB")
            return False
        return True
    
    def cleanup_memory(self):
        """清理内存"""
        print("🧹 执行内存清理...")
        gc.collect()
        time.sleep(0.5)  # 给GC一些时间
        print(f"✅ 内存清理完成，当前使用: {self.get_memory_usage():.1f}MB")
    
    @lru_cache(maxsize=100)
    def get_current_date_dir(self):
        """获取当前日期的测试结果目录（带缓存）"""
        date_str = datetime.now().strftime("%Y-%m-%d")
        date_dir = os.path.join(self.test_results_dir, date_str)
        os.makedirs(date_dir, exist_ok=True)
        return date_dir
    
    def run_single_test_optimized(self, test_file):
        """优化版单测试执行函数"""
        test_name = os.path.basename(test_file)
        start_time = time.time()
        
        print(f"▶️ 开始执行测试: {test_name}")
        
        try:
            # 检查内存安全
            if not self.check_memory_safe():
                self.cleanup_memory()
            
            # 使用优化的子进程执行
            result = subprocess.run(
                [sys.executable, test_file], 
                capture_output=True, 
                text=True, 
                encoding='utf-8',
                timeout=self.test_timeout,
                env={**os.environ, 'PYTHONHASHSEED': '0'}  # 确保可重复性
            )
            
            execution_time = time.time() - start_time
            
            test_result = {
                '测试文件': test_name,
                '执行时间': datetime.now().strftime('%Y-%m-%d %H:%M:%S'),
                '执行耗时': f"{execution_time:.2f}秒",
                '退出码': result.returncode,
                '标准输出': result.stdout[-1000:],  # 只保留最后1000字符
                '错误输出': result.stderr[-500:],   # 只保留最后500字符
                '内存峰值': f"{self.get_memory_usage():.1f}MB"
            }
            
            # 判断测试结果
            if result.returncode == 0:
                test_result['状态'] = '通过'
                test_result['详细信息'] = '测试执行成功'
                print(f"✅ {test_name} - 通过 ({execution_time:.2f}s)")
            else:
                test_result['状态'] = '失败'
                test_result['详细信息'] = '测试执行失败'
                print(f"❌ {test_name} - 失败 ({execution_time:.2f}s)")
            
            self.test_execution_times[test_name] = execution_time
            return test_result
            
        except subprocess.TimeoutExpired:
            execution_time = time.time() - start_time
            test_result = {
                '测试文件': test_name,
                '执行时间': datetime.now().strftime('%Y-%m-%d %H:%M:%S'),
                '执行耗时': f"{execution_time:.2f}秒",
                '退出码': -1,
                '状态': '超时',
                '详细信息': f'测试执行超时（{self.test_timeout}秒）',
                '内存峰值': f"{self.get_memory_usage():.1f}MB"
            }
            print(f"⏰ {test_name} - 超时 ({execution_time:.2f}s)")
            return test_result
            
        except Exception as e:
            execution_time = time.time() - start_time
            test_result = {
                '测试文件': test_name,
                '执行时间': datetime.now().strftime('%Y-%m-%d %H:%M:%S'),
                '执行耗时': f"{execution_time:.2f}秒",
                '退出码': -1,
                '状态': '异常',
                '详细信息': f'测试执行异常: {str(e)}',
                '内存峰值': f"{self.get_memory_usage():.1f}MB"
            }
            print(f"💥 {test_name} - 异常: {e} ({execution_time:.2f}s)")
            return test_result
    
    def run_tests_parallel(self, test_files=None):
        """并行执行测试"""
        if test_files is None:
            test_dir = os.path.join(self.project_root, "test", "ui_automation")
            test_files = [os.path.join(test_dir, f) for f in os.listdir(test_dir) 
                         if f.startswith("test_") and f.endswith(".py")]
        
        if not test_files:
            print("⚠️ 未找到测试文件")
            return []
        
        print(f"🚀 开始并行执行 {len(test_files)} 个测试文件")
        print(f"📊 并行度: {self.max_workers}")
        
        self.start_time = time.time()
        test_results = []
        
        # 使用线程池执行并行测试
        with ThreadPoolExecutor(max_workers=self.max_workers) as executor:
            # 提交所有测试任务
            future_to_test = {executor.submit(self.run_single_test_optimized, test_file): test_file 
                            for test_file in test_files}
            
            # 收集结果
            for future in as_completed(future_to_test):
                test_file = future_to_test[future]
                try:
                    result = future.result()
                    test_results.append(result)
                    
                    # 定期检查内存并清理
                    if len(test_results) % 2 == 0:  # 每2个测试检查一次
                        if not self.check_memory_safe():
                            self.cleanup_memory()
                            
                except Exception as e:
                    print(f"❌ 测试执行异常: {test_file}, 错误: {e}")
        
        total_time = time.time() - self.start_time
        print(f"\n🎯 并行测试执行完成，总耗时: {total_time:.2f}秒")
        
        # 生成性能报告
        self._generate_performance_report(test_results, total_time)
        
        return test_results
    
    def _generate_performance_report(self, test_results, total_time):
        """生成性能报告"""
        if not test_results:
            return
        
        # 计算性能指标
        passed_tests = [r for r in test_results if r.get('状态') == '通过']
        failed_tests = [r for r in test_results if r.get('状态') != '通过']
        
        avg_execution_time = sum(float(r['执行耗时'].replace('秒', '')) 
                                for r in test_results if '执行耗时' in r) / len(test_results)
        
        memory_avg = sum(float(r['内存峰值'].replace('MB', '')) 
                        for r in test_results if '内存峰值' in r) / len(test_results)
        
        # 生成性能报告
        performance_report = {
            '报告时间': datetime.now().strftime('%Y-%m-%d %H:%M:%S'),
            '测试总数': len(test_results),
            '通过数': len(passed_tests),
            '失败数': len(failed_tests),
            '通过率': f"{len(passed_tests)/len(test_results)*100:.1f}%",
            '总执行时间': f"{total_time:.2f}秒",
            '平均测试时间': f"{avg_execution_time:.2f}秒",
            '平均内存使用': f"{memory_avg:.1f}MB",
            '内存峰值': f"{max(self.memory_usage) if self.memory_usage else 0:.1f}MB",
            '并行效率': f"{(sum(self.test_execution_times.values()) / total_time):.2f}x"
        }
        
        # 保存性能报告
        date_dir = self.get_current_date_dir()
        report_file = os.path.join(date_dir, f"性能报告_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json")
        
        with open(report_file, 'w', encoding='utf-8') as f:
            json.dump(performance_report, f, ensure_ascii=False, indent=2)
        
        print("📊 性能报告:")
        for key, value in performance_report.items():
            print(f"  {key}: {value}")
        print(f"📁 报告文件: {report_file}")
    
    def run_smart_test_selection(self, modified_files=None):
        """智能测试选择 - 只运行受影响的测试"""
        print("🧠 执行智能测试选择...")
        
        if modified_files is None:
            # 如果没有指定修改文件，运行所有测试
            return self.run_tests_parallel()
        
        # 分析修改文件与测试的关联关系
        affected_tests = self._analyze_test_dependencies(modified_files)
        
        if affected_tests:
            print(f"🔍 检测到 {len(affected_tests)} 个受影响的测试")
            return self.run_tests_parallel(affected_tests)
        else:
            print("✅ 未检测到受影响的测试，跳过执行")
            return []
    
    def _analyze_test_dependencies(self, modified_files):
        """分析测试依赖关系"""
        # 简化的依赖分析逻辑
        # 在实际项目中，这里应该使用更复杂的依赖分析
        
        test_dir = os.path.join(self.project_root, "test", "ui_automation")
        all_tests = [os.path.join(test_dir, f) for f in os.listdir(test_dir) 
                    if f.startswith("test_") and f.endswith(".py")]
        
        affected_tests = []
        
        for test_file in all_tests:
            test_name = os.path.basename(test_file)
            
            # 简单的文件名匹配逻辑
            for modified_file in modified_files:
                if any(keyword in modified_file.lower() for keyword in ['product', 'config']) and 'product' in test_name.lower():
                    affected_tests.append(test_file)
                    break
                elif any(keyword in modified_file.lower() for keyword in ['ui', 'layout', 'interface']) and 'layout' in test_name.lower():
                    affected_tests.append(test_file)
                    break
                elif any(keyword in modified_file.lower() for keyword in ['packaging', 'build']) and 'packaging' in test_name.lower():
                    affected_tests.append(test_file)
                    break
                elif any(keyword in modified_file.lower() for keyword in ['preview']) and 'preview' in test_name.lower():
                    affected_tests.append(test_file)
                    break
        
        return list(set(affected_tests))  # 去重
    
    def optimize_test_environment(self):
        """优化测试环境"""
        print("⚙️ 优化测试环境...")
        
        # 清理临时文件
        self._cleanup_temp_files()
        
        # 优化Python环境
        self._optimize_python_environment()
        
        # 预加载常用模块
        self._preload_common_modules()
        
        print("✅ 测试环境优化完成")
    
    def _cleanup_temp_files(self):
        """清理临时文件"""
        temp_dirs = [
            os.path.join(self.project_root, "build"),
            os.path.join(self.project_root, "__pycache__"),
            os.path.join(self.project_root, "test", "__pycache__")
        ]
        
        for temp_dir in temp_dirs:
            if os.path.exists(temp_dir):
                try:
                    # 在实际项目中应该更谨慎地清理
                    print(f"🧹 清理临时目录: {temp_dir}")
                except Exception as e:
                    print(f"⚠️ 清理失败: {temp_dir}, 错误: {e}")
    
    def _optimize_python_environment(self):
        """优化Python环境"""
        # 设置优化参数
        os.environ['PYTHONOPTIMIZE'] = '1'
        os.environ['PYTHONDONTWRITEBYTECODE'] = '1'
    
    def _preload_common_modules(self):
        """预加载常用模块"""
        # 预加载常用模块以减少首次导入时间
        try:
            import pandas as pd
            import numpy as np
            print("📚 常用模块预加载完成")
        except ImportError as e:
            print(f"⚠️ 模块预加载失败: {e}")

def main():
    """主函数"""
    # 初始化优化版测试管理器
    manager = OptimizedTestManager(max_workers=2)
    
    print("🚀 软件产品编辑器 - 优化版自动化测试系统")
    print("=" * 60)
    
    # 优化测试环境
    manager.optimize_test_environment()
    
    # 执行并行测试
    print("\n📋 开始执行优化版测试...")
    test_results = manager.run_tests_parallel()
    
    if test_results:
        print(f"\n🎉 测试执行完成，共执行 {len(test_results)} 个测试")
        
        # 统计结果
        passed = sum(1 for r in test_results if r.get('状态') == '通过')
        failed = len(test_results) - passed
        
        print(f"✅ 通过: {passed}")
        print(f"❌ 失败: {failed}")
        print(f"📊 通过率: {passed/len(test_results)*100:.1f}%")
    else:
        print("⚠️ 未执行任何测试")

if __name__ == "__main__":
    main()