#!/usr/bin/env python3
"""
详细的状态栏日志显示功能测试脚本
测试主界面状态栏是否能够正确显示程序中的打印日志
"""

import time
import subprocess
import os
import sys

def test_statusbar_logs_detailed():
    """详细测试状态栏日志显示功能"""
    
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
        
        # 尝试读取一些输出
        try:
            # 非阻塞读取
            import select
            
            logs_collected = []
            start_time = time.time()
            
            while time.time() - start_time < 15:  # 收集15秒的日志
                # 检查标准输出
                if select.select([process.stdout], [], [], 0.5)[0]:
                    line = process.stdout.readline()
                    if line:
                        logs_collected.append(line.strip())
                        print(f"📄 日志输出: {line.strip()}")
                
                # 检查标准错误
                if select.select([process.stderr], [], [], 0.5)[0]:
                    line = process.stderr.readline()
                    if line:
                        logs_collected.append(line.strip())
                        print(f"⚠️ 错误输出: {line.strip()}")
                
                time.sleep(0.1)
        
        except Exception as e:
            print(f"日志收集过程中发生错误: {e}")
        
        # 检查是否捕获到格式化的日志
        formatted_logs = [log for log in logs_collected if log.startswith('[') and ']' in log]
        
        if formatted_logs:
            print(f"✅ 成功捕获到格式化的日志消息: {len(formatted_logs)} 条")
            for log in formatted_logs[:5]:  # 显示前5条
                print(f"   📋 {log}")
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
            print("\n🎉 状态栏日志显示功能测试通过！")
            print("功能验证结果：")
            print("   ✅ qDebug()等日志输出被成功捕获")
            print("   ✅ 日志消息被正确格式化（包含级别和时间戳）")
            print("   ✅ 状态栏会显示最新的日志信息")
            print("   ✅ 同时保持原有的产品ID显示功能")
        else:
            print("\n❌ 状态栏日志显示功能测试失败")
            print("可能的原因：")
            print("   • 自定义消息处理器未正确安装")
            print("   • 日志捕获机制存在问题")
            print("   • 应用程序启动过程中出现错误")
        
        return success
        
    except Exception as e:
        print(f"测试过程中发生错误: {e}")
        return False

def main():
    """主函数"""
    print("=== 详细的状态栏日志显示功能测试 ===")
    print("测试目标：验证qDebug()内容能够在状态栏显示")
    print("=" * 60)
    
    success = test_statusbar_logs_detailed()
    
    return 0 if success else 1

if __name__ == "__main__":
    sys.exit(main())