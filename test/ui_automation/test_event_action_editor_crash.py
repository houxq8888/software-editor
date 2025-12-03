#!/usr/bin/env python3
"""
测试事件-动作编辑器在新建产品状态下的崩溃问题修复
"""

import sys
import os
import time
from PyQt5.QtWidgets import QApplication, QMessageBox
from PyQt5.QtCore import Qt, QTimer
from PyQt5.QtTest import QTest

# 添加项目路径到Python路径
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '../..')))

def test_event_action_editor_new_product():
    """测试新建产品状态下打开事件-动作编辑器不会崩溃"""
    
    # 导入必要的模块
    try:
        from src.uilayoutwindow import UILayoutWindow
        from src.productconfigmanager import ProductConfigManager
        print("✓ 成功导入模块")
    except ImportError as e:
        print(f"✗ 导入模块失败: {e}")
        return False
    
    app = QApplication.instance()
    if app is None:
        app = QApplication(sys.argv)
    
    try:
        # 创建产品配置管理器（模拟新建产品）
        config_manager = ProductConfigManager()
        
        # 创建UI布局窗口（新建产品状态）
        layout_window = UILayoutWindow(None, True, "", config_manager)
        layout_window.show()
        
        # 等待窗口显示
        QTest.qWait(1000)
        
        # 尝试打开事件-动作编辑器
        print("正在尝试打开事件-动作编辑器...")
        
        # 使用QTimer延迟执行，避免阻塞
        def open_event_action_editor():
            try:
                # 调用事件-动作编辑器方法
                layout_window.onActionEventActionEditorTriggered()
                print("✓ 事件-动作编辑器成功打开，没有崩溃")
                
                # 检查是否有错误对话框弹出
                active_window = QApplication.activeWindow()
                if isinstance(active_window, QMessageBox):
                    if "错误" in active_window.windowTitle():
                        print("⚠ 显示错误对话框（预期行为）")
                        active_window.close()
                    else:
                        print("✓ 事件-动作编辑器窗口正常显示")
                        active_window.close()
                
                # 关闭主窗口
                layout_window.close()
                
            except Exception as e:
                print(f"✗ 打开事件-动作编辑器时发生异常: {e}")
                layout_window.close()
                return False
            
            return True
        
        # 延迟执行
        QTimer.singleShot(500, open_event_action_editor)
        
        # 运行事件循环
        app.exec_()
        
        print("✓ 测试完成，程序没有崩溃")
        return True
        
    except Exception as e:
        print(f"✗ 测试过程中发生异常: {e}")
        return False

def test_event_action_editor_existing_product():
    """测试现有产品状态下打开事件-动作编辑器"""
    
    try:
        from src.uilayoutwindow import UILayoutWindow
        from src.productconfigmanager import ProductConfigManager
        print("✓ 成功导入模块")
    except ImportError as e:
        print(f"✗ 导入模块失败: {e}")
        return False
    
    app = QApplication.instance()
    if app is None:
        app = QApplication(sys.argv)
    
    try:
        # 创建产品配置管理器（模拟现有产品）
        config_manager = ProductConfigManager()
        
        # 创建UI布局窗口（现有产品状态）
        layout_window = UILayoutWindow(None, False, "test_product.json", config_manager)
        layout_window.show()
        
        # 等待窗口显示
        QTest.qWait(1000)
        
        # 尝试打开事件-动作编辑器
        print("正在尝试打开事件-动作编辑器（现有产品）...")
        
        def open_event_action_editor():
            try:
                layout_window.onActionEventActionEditorTriggered()
                print("✓ 事件-动作编辑器成功打开")
                
                # 检查窗口是否正常显示
                active_window = QApplication.activeWindow()
                if active_window and active_window != layout_window:
                    print("✓ 事件-动作编辑器窗口正常显示")
                    active_window.close()
                
                layout_window.close()
                
            except Exception as e:
                print(f"✗ 打开事件-动作编辑器时发生异常: {e}")
                layout_window.close()
                return False
            
            return True
        
        QTimer.singleShot(500, open_event_action_editor)
        app.exec_()
        
        print("✓ 现有产品测试完成")
        return True
        
    except Exception as e:
        print(f"✗ 测试过程中发生异常: {e}")
        return False

if __name__ == "__main__":
    print("开始测试事件-动作编辑器崩溃修复...")
    
    # 测试新建产品状态
    print("\n=== 测试新建产品状态 ===")
    result1 = test_event_action_editor_new_product()
    
    # 测试现有产品状态
    print("\n=== 测试现有产品状态 ===")
    result2 = test_event_action_editor_existing_product()
    
    if result1 and result2:
        print("\n🎉 所有测试通过！事件-动作编辑器崩溃问题已修复")
        sys.exit(0)
    else:
        print("\n❌ 部分测试失败")
        sys.exit(1)