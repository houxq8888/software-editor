#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
进程调试脚本 - 用于监测software-editor.exe进程状态
"""

import psutil
import time
import subprocess
import os

def find_process_by_name(name):
    """根据进程名查找进程"""
    processes = []
    for proc in psutil.process_iter(['pid', 'name', 'status']):
        try:
            if name.lower() in proc.info['name'].lower():
                processes.append(proc)
        except (psutil.NoSuchProcess, psutil.AccessDenied):
            pass
    return processes

def monitor_process(process_name, interval=2, duration=30):
    """监控进程状态"""
    print(f"开始监控进程: {process_name}")
    print("=" * 50)
    
    start_time = time.time()
    
    while time.time() - start_time < duration:
        processes = find_process_by_name(process_name)
        
        if not processes:
            print(f"[{time.strftime('%H:%M:%S')}] 未找到进程: {process_name}")
        else:
            for proc in processes:
                try:
                    status = proc.status()
                    memory_info = proc.memory_info()
                    cpu_percent = proc.cpu_percent()
                    
                    print(f"[{time.strftime('%H:%M:%S')}] PID: {proc.pid}, "
                          f"状态: {status}, "
                          f"CPU: {cpu_percent:.1f}%, "
                          f"内存: {memory_info.rss / 1024 / 1024:.1f}MB")
                    
                    # 检查线程状态
                    threads = proc.threads()
                    print(f"    线程数: {len(threads)}")
                    
                    # 检查文件句柄
                    try:
                        handles = proc.num_handles()
                        print(f"    句柄数: {handles}")
                    except:
                        pass
                        
                except (psutil.NoSuchProcess, psutil.AccessDenied):
                    print(f"[{time.strftime('%H:%M:%S')}] 进程 {proc.pid} 已终止或访问被拒绝")
        
        time.sleep(interval)
    
    print("=" * 50)
    print("监控结束")

def check_system_resources():
    """检查系统资源使用情况"""
    print("系统资源使用情况:")
    print(f"CPU使用率: {psutil.cpu_percent(interval=1)}%")
    print(f"内存使用率: {psutil.virtual_memory().percent}%")
    print(f"可用内存: {psutil.virtual_memory().available / 1024 / 1024:.1f}MB")

def main():
    process_name = "software-editor.exe"
    
    print("软件编辑器进程调试工具")
    print("=" * 50)
    
    # 检查当前进程状态
    processes = find_process_by_name(process_name)
    
    if processes:
        print(f"找到 {len(processes)} 个相关进程:")
        for proc in processes:
            print(f"  PID: {proc.pid}, 名称: {proc.name()}")
        
        print("\n开始监控进程状态...")
        monitor_process(process_name, interval=2, duration=60)
    else:
        print(f"未找到进程: {process_name}")
        print("请先启动软件编辑器程序")
        
        # 提供启动选项
        choice = input("是否要启动程序? (y/n): ").lower()
        if choice == 'y':
            print("启动程序...")
            subprocess.Popen(["build/software-editor.exe"], cwd=os.getcwd())
            time.sleep(3)  # 等待程序启动
            monitor_process(process_name, interval=2, duration=60)
    
    # 检查系统资源
    check_system_resources()

if __name__ == "__main__":
    main()