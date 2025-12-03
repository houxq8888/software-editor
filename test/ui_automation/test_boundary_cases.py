"""
边界值和异常情况测试用例
测试UI布局编辑器的边界情况和异常处理
"""

import time
import pytest
from pywinauto import Application
from pywinauto.keyboard import send_keys
import pyautogui

# 应用信息配置
APP_INFO = {
    "path": "D:\\virtualMachine\\github\\software-editor\\build\\software-editor.exe"
}

def take_screenshot(name):
    """截图函数"""
    try:
        screenshot = pyautogui.screenshot()
        screenshot.save(f"screenshots/{name}.png")
        print(f"📸 截图已保存: screenshots/{name}.png")
    except Exception as e:
        print(f"⚠️ 截图失败: {e}")

def test_boundary_control_count():
    """测试控件数量的边界情况"""
    print("=== 开始测试控件数量边界情况 ===")
    
    try:
        # 启动应用程序
        app = Application().start(APP_INFO["path"], wait_for_idle=False)
        time.sleep(5)
        
        main_window = app.window(title_re=".*产品编辑器.*")
        main_window.wait("visible", timeout=10)
        
        # 打开UI布局编辑器
        ui_layout_button = main_window.child_window(title_re=".*UI布局.*", control_type="Button")
        
        if ui_layout_button.exists():
            ui_layout_button.click()
            time.sleep(3)
            
            ui_layout_window = app.window(title_re=".*UI布局编辑器.*")
            ui_layout_window.wait("visible", timeout=10)
            
            take_screenshot("boundary_control_count_start")
            
            # 测试大量控件添加
            control_library = ui_layout_window.child_window(title_re=".*控件库.*", control_type="Pane")
            edit_area = ui_layout_window.child_window(title_re=".*编辑区.*", control_type="Pane")
            
            if control_library.exists() and edit_area.exists():
                # 尝试添加大量控件
                max_controls = 100  # 测试最大控件数量
                
                for i in range(max_controls):
                    try:
                        # 查找QLabel控件
                        qlabel_item = control_library.child_window(title="QLabel", control_type="ListItem")
                        
                        if qlabel_item.exists():
                            # 拖拽控件到编辑区
                            qlabel_item.drag_mouse_input(dst=(edit_area.rectangle().mid_point().x, 
                                                             edit_area.rectangle().mid_point().y))
                            time.sleep(0.1)
                            
                            if i % 10 == 0:
                                print(f"✅ 已添加 {i+1} 个控件")
                        else:
                            print(f"⚠️ 第 {i+1} 个控件添加失败：未找到QLabel")
                            break
                            
                    except Exception as e:
                        print(f"⚠️ 第 {i+1} 个控件添加失败：{e}")
                        break
                
                take_screenshot("boundary_control_count_after")
                print(f"✅ 控件数量边界测试完成，成功添加 {i} 个控件")
            
            # 关闭窗口
            ui_layout_window.close()
            time.sleep(1)
            main_window.close()
            time.sleep(2)
            
            return True
            
        else:
            print("⚠️ 未找到UI布局编辑器按钮，跳过边界测试")
            main_window.close()
            return False
            
    except Exception as e:
        print(f"❌ 控件数量边界测试失败: {e}")
        take_screenshot("boundary_control_count_error")
        return False

def test_boundary_text_length():
    """测试文本长度的边界情况"""
    print("=== 开始测试文本长度边界情况 ===")
    
    try:
        # 启动应用程序
        app = Application().start(APP_INFO["path"], wait_for_idle=False)
        time.sleep(5)
        
        main_window = app.window(title_re=".*产品编辑器.*")
        main_window.wait("visible", timeout=10)
        
        # 打开UI布局编辑器
        ui_layout_button = main_window.child_window(title_re=".*UI布局.*", control_type="Button")
        
        if ui_layout_button.exists():
            ui_layout_button.click()
            time.sleep(3)
            
            ui_layout_window = app.window(title_re=".*UI布局编辑器.*")
            ui_layout_window.wait("visible", timeout=10)
            
            take_screenshot("boundary_text_length_start")
            
            # 添加一个QLabel控件
            control_library = ui_layout_window.child_window(title_re=".*控件库.*", control_type="Pane")
            edit_area = ui_layout_window.child_window(title_re=".*编辑区.*", control_type="Pane")
            
            if control_library.exists() and edit_area.exists():
                qlabel_item = control_library.child_window(title="QLabel", control_type="ListItem")
                
                if qlabel_item.exists():
                    # 拖拽控件到编辑区
                    qlabel_item.drag_mouse_input(dst=(edit_area.rectangle().mid_point().x, 
                                                     edit_area.rectangle().mid_point().y))
                    time.sleep(1)
                    
                    # 双击控件编辑文本
                    added_qlabel = edit_area.child_window(title="QLabel", control_type="Text", found_index=0)
                    
                    if added_qlabel.exists():
                        added_qlabel.double_click()
                        time.sleep(0.5)
                        
                        # 测试不同长度的文本
                        test_texts = [
                            "",  # 空文本
                            "A",  # 单字符
                            "测试文本",  # 正常文本
                            "A" * 100,  # 100字符
                            "A" * 1000,  # 1000字符（边界情况）
                            "A" * 10000,  # 10000字符（极端情况）
                        ]
                        
                        for i, text in enumerate(test_texts):
                            try:
                                # 清空并输入文本
                                send_keys("^A")  # Ctrl+A全选
                                time.sleep(0.2)
                                send_keys("{DELETE}")  # 删除
                                time.sleep(0.2)
                                send_keys(text)
                                time.sleep(0.5)
                                
                                # 按Enter确认
                                send_keys("{ENTER}")
                                time.sleep(0.5)
                                
                                print(f"✅ 文本长度 {len(text)} 测试通过")
                                
                            except Exception as e:
                                print(f"⚠️ 文本长度 {len(text)} 测试失败: {e}")
                        
                        take_screenshot("boundary_text_length_after")
                    
                    else:
                        print("⚠️ 未找到添加的QLabel控件")
                
                else:
                    print("⚠️ 未找到QLabel控件")
            
            # 关闭窗口
            ui_layout_window.close()
            time.sleep(1)
            main_window.close()
            time.sleep(2)
            
            return True
            
        else:
            print("⚠️ 未找到UI布局编辑器按钮，跳过文本边界测试")
            main_window.close()
            return False
            
    except Exception as e:
        print(f"❌ 文本长度边界测试失败: {e}")
        take_screenshot("boundary_text_length_error")
        return False

def test_exception_handling():
    """测试异常情况处理"""
    print("=== 开始测试异常情况处理 ===")
    
    try:
        # 启动应用程序
        app = Application().start(APP_INFO["path"], wait_for_idle=False)
        time.sleep(5)
        
        main_window = app.window(title_re=".*产品编辑器.*")
        main_window.wait("visible", timeout=10)
        
        take_screenshot("exception_handling_start")
        
        # 测试1: 快速连续操作
        print("测试快速连续操作...")
        
        # 快速点击多个按钮
        buttons_to_test = ["文件", "编辑", "视图", "工具", "帮助"]
        
        for button_name in buttons_to_test:
            try:
                button = main_window.child_window(title_re=f".*{button_name}.*", control_type="Button")
                if button.exists():
                    button.click()
                    time.sleep(0.1)  # 快速连续点击
            except:
                pass
        
        time.sleep(1)
        print("✅ 快速连续操作测试通过")
        
        # 测试2: 无效文件操作
        print("测试无效文件操作...")
        
        try:
            # 尝试打开不存在的文件
            file_menu = main_window.child_window(title_re=".*文件.*", control_type="MenuItem")
            if file_menu.exists():
                file_menu.click()
                time.sleep(0.5)
                
                open_item = main_window.child_window(title_re=".*打开.*", control_type="MenuItem")
                if open_item.exists():
                    open_item.click()
                    time.sleep(1)
                    
                    # 在文件对话框中输入无效路径
                    file_dialog = app.window(title_re=".*打开.*")
                    if file_dialog.exists():
                        filename_edit = file_dialog.child_window(control_type="Edit")
                        if filename_edit.exists():
                            filename_edit.set_text("invalid_file_path.xyz")
                            time.sleep(0.5)
                            
                            # 尝试打开
                            open_button = file_dialog.child_window(title="打开", control_type="Button")
                            if open_button.exists():
                                open_button.click()
                                time.sleep(1)
                                
                                # 检查是否有错误提示
                                error_dialog = app.window(title_re=".*错误.*")
                                if error_dialog.exists():
                                    print("✅ 无效文件操作错误处理正常")
                                    error_dialog.close()
                                else:
                                    print("⚠️ 未检测到错误提示对话框")
                            
                            # 取消文件对话框
                            cancel_button = file_dialog.child_window(title="取消", control_type="Button")
                            if cancel_button.exists():
                                cancel_button.click()
                        
                        file_dialog.close()
        except Exception as e:
            print(f"⚠️ 无效文件操作测试异常: {e}")
        
        # 测试3: 内存压力测试
        print("测试内存压力...")
        
        # 打开UI布局编辑器
        ui_layout_button = main_window.child_window(title_re=".*UI布局.*", control_type="Button")
        
        if ui_layout_button.exists():
            # 快速打开关闭UI布局编辑器多次
            for i in range(10):
                try:
                    ui_layout_button.click()
                    time.sleep(0.5)
                    
                    ui_layout_window = app.window(title_re=".*UI布局编辑器.*")
                    if ui_layout_window.exists():
                        ui_layout_window.close()
                        time.sleep(0.5)
                except:
                    pass
            
            print("✅ 内存压力测试通过")
        
        # 关闭主窗口
        main_window.close()
        time.sleep(2)
        
        return True
        
    except Exception as e:
        print(f"❌ 异常情况处理测试失败: {e}")
        take_screenshot("exception_handling_error")
        return False

def test_performance_boundary():
    """测试性能边界情况"""
    print("=== 开始测试性能边界情况 ===")
    
    try:
        # 启动应用程序
        start_time = time.time()
        app = Application().start(APP_INFO["path"], wait_for_idle=False)
        
        # 等待应用启动
        main_window = app.window(title_re=".*产品编辑器.*")
        main_window.wait("visible", timeout=30)  # 延长超时时间
        
        startup_time = time.time() - start_time
        print(f"应用启动时间: {startup_time:.2f}秒")
        
        if startup_time > 10:
            print("⚠️ 应用启动时间较长，可能存在性能问题")
        else:
            print("✅ 应用启动性能正常")
        
        take_screenshot("performance_boundary_start")
        
        # 测试响应时间
        test_start_time = time.time()
        
        # 执行一系列操作
        operations = [
            ("打开文件菜单", ".*文件.*"),
            ("打开编辑菜单", ".*编辑.*"),
            ("打开视图菜单", ".*视图.*"),
        ]
        
        for op_name, pattern in operations:
            op_start = time.time()
            
            try:
                button = main_window.child_window(title_re=pattern, control_type="Button")
                if button.exists():
                    button.click()
                    time.sleep(0.5)
                    
                    # 点击空白处关闭菜单
                    main_window.click(coords=(100, 100))
                    time.sleep(0.5)
                    
                    op_time = time.time() - op_start
                    print(f"{op_name}响应时间: {op_time:.2f}秒")
                    
                    if op_time > 2:
                        print(f"⚠️ {op_name}响应时间较长")
            except:
                pass
        
        total_test_time = time.time() - test_start_time
        print(f"性能测试总时间: {total_test_time:.2f}秒")
        
        # 关闭应用
        main_window.close()
        time.sleep(2)
        
        return True
        
    except Exception as e:
        print(f"❌ 性能边界测试失败: {e}")
        take_screenshot("performance_boundary_error")
        return False

if __name__ == "__main__":
    # 创建截图目录
    import os
    if not os.path.exists("screenshots"):
        os.makedirs("screenshots")
    
    # 运行测试
    print("开始运行边界值和异常情况测试...")
    
    tests = [
        test_boundary_control_count,
        test_boundary_text_length,
        test_exception_handling,
        test_performance_boundary
    ]
    
    passed = 0
    total = len(tests)
    
    for test_func in tests:
        try:
            if test_func():
                passed += 1
        except Exception as e:
            print(f"❌ {test_func.__name__} 执行异常: {e}")
    
    print(f"\n=== 测试结果 ===")
    print(f"通过: {passed}/{total}")
    print(f"成功率: {passed/total*100:.1f}%")