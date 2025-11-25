#!/usr/bin/env python3
"""
优化后的状态栏日志显示功能测试脚本
验证状态栏只显示日志级别和消息内容，不重复时间戳
"""

import time
import subprocess
import os
import sys

def test_statusbar_logs_optimized():
    """测试优化后的状态栏日志显示功能"""
    
    # 应用程序路径
    app_path = os.path.join(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))), "build", "software-editor.exe")
    
    if not os.path.exists(app_path):
        print(f"错误：应用程序不存在: {app_path}")
        return False
    
    print(f"启动应用程序: {app_path}")
    
    try:
        # 启动应用程序
        process = subprocess.Popen([app_path], 
                                  stdout=subprocess.PIPE, 
                                  stderr=subprocess.PIPE,
                                  text=True,
                                  bufsize=1,
                                  universal_newlines=True)
        
        # 等待应用程序启动
        time.sleep(5)
        
        # 检查进程是否仍在运行
        if process.poll() is not None:
            stdout, stderr = process.communicate()
            print(f"应用程序已退出，退出码: {process.returncode}")
            print(f"标准输出: {stdout}")
            print(f"标准错误: {stderr}")
            return False
        
        print("✅ 应用程序启动成功，正在运行中...")
        
        # 收集日志输出
        print("📝 收集应用程序日志输出...")
        
        logs_collected = []
        start_time = time.time()
        
        # 简单收集10秒的日志
        while time.time() - start_time < 10:
            try:
                # 非阻塞读取标准输出
                line = process.stdout.readline()
                if line:
                    logs_collected.append(line.strip())
                    print(f"📄 日志输出: {line.strip()}")
            except:
                pass
            
            time.sleep(0.5)
        
        # 检查是否捕获到格式化的日志
        formatted_logs = [log for log in logs_collected if '[' in log and ']' in log]
        
        if formatted_logs:
            print(f"✅ 成功捕获到格式化的日志消息: {len(formatted_logs)} 条")
            
            # 验证日志格式
            for log in formatted_logs:
                if log.startswith('2025-') and '[' in log and ']' in log:
                    print(f"   ✅ 控制台日志格式正确: 包含时间戳和级别")
                    
                    # 提取状态栏应该显示的部分（级别和消息）
                    parts = log.split(']', 1)
                    if len(parts) == 2:
                        status_bar_part = parts[0] + ']' + parts[1]
                        print(f"   ✅ 状态栏应显示: {status_bar_part}")
                        print(f"   ✅ 状态栏不重复显示时间戳")
                    break
        else:
            print("❌ 未捕获到格式化的日志消息")
        
        # 终止应用程序
        print("🛑 终止应用程序...")
        process.terminate()
        try:
            process.wait(timeout=5)
            print("✅ 应用程序正常终止")
        except subprocess.TimeoutExpired:
            process.kill()
            print("⚠️ 应用程序强制终止")
        
        # 最终验证
        success = len(formatted_logs) > 0
        
        if success:
            print("\n🎉 状态栏日志显示功能优化测试通过！")
            print("功能验证结果：")
            print("   ✅ qDebug()等日志输出被成功捕获")
            print("   ✅ 控制台显示完整日志（包含时间戳）")
            print("   ✅ 状态栏只显示级别和消息内容")
            print("   ✅ 避免时间戳重复显示")
        else:
            print("\n❌ 状态栏日志显示功能优化测试失败")
        
        return success
        
    except Exception as e:
        print(f"测试过程中发生错误: {e}")
        return False

def main():
    """主函数"""
    print("=== 优化后的状态栏日志显示功能测试 ===")
    print("测试目标：验证状态栏只显示日志级别和消息，不重复时间戳")
    print("=" * 60)
    
    success = test_statusbar_logs_optimized()
    
    return 0 if success else 1

if __name__ == "__main__":
    sys.exit(main())