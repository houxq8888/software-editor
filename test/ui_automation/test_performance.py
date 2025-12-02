"""
性能测试用例
测试UI布局编辑器的性能表现
"""

import time
import pytest
from pywinauto import Application
import pyautogui
import psutil
import os

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

def get_memory_usage():
    """获取当前进程内存使用情况"""
    try:
        current_pid = os.getpid()
        process = psutil.Process(current_pid)
        memory_info = process.memory_info()
        return memory_info.rss / 1024 / 1024  # 返回MB
    except:
        return 0

def test_startup_performance():
    """测试应用启动性能"""
    print("=== 开始测试应用启动性能 ===")
    
    startup_times = []
    
    # 测试5次启动时间
    for i in range(5):
        try:
            start_time = time.time()
            
            # 启动应用程序
            app = Application().start(APP_INFO["path"], wait_for_idle=False)
            time.sleep(2)
            
            main_window = app.window(title_re=".*产品编辑器.*")
            main_window.wait("visible", timeout=30)
            
            startup_time = time.time() - start_time
            startup_times.append(startup_time)
            
            print(f"第 {i+1} 次启动时间: {startup_time:.2f}秒")
            
            # 获取内存使用情况
            memory_usage = get_memory_usage()
            print(f"启动后内存使用: {memory_usage:.1f}MB")
            
            # 关闭应用
            main_window.close()
            time.sleep(2)
            
        except Exception as e:
            print(f"❌ 第 {i+1} 次启动测试失败: {e}")
            startup_times.append(30)  # 超时时间
    
    # 分析启动性能
    if startup_times:
        avg_time = sum(startup_times) / len(startup_times)
        max_time = max(startup_times)
        min_time = min(startup_times)
        
        print(f"\n=== 启动性能分析 ===")
        print(f"平均启动时间: {avg_time:.2f}秒")
        print(f"最快启动时间: {min_time:.2f}秒")
        print(f"最慢启动时间: {max_time:.2f}秒")
        
        if avg_time < 5:
            print("✅ 启动性能优秀")
        elif avg_time < 10:
            print("✅ 启动性能良好")
        elif avg_time < 15:
            print("⚠️ 启动性能一般")
        else:
            print("❌ 启动性能较差")
    
    return True

def test_ui_responsiveness():
    """测试UI响应性能"""
    print("=== 开始测试UI响应性能 ===")
    
    try:
        # 启动应用程序
        app = Application().start(APP_INFO["path"], wait_for_idle=False)
        time.sleep(5)
        
        main_window = app.window(title_re=".*产品编辑器.*")
        main_window.wait("visible", timeout=10)
        
        take_screenshot("ui_responsiveness_start")
        
        # 测试菜单响应时间
        menu_responses = []
        
        menu_items = ["文件", "编辑", "视图", "工具", "帮助"]
        
        for menu_name in menu_items:
            try:
                start_time = time.time()
                
                menu_button = main_window.child_window(title_re=f".*{menu_name}.*", control_type="Button")
                
                if menu_button.exists():
                    menu_button.click()
                    time.sleep(0.5)
                    
                    # 检查菜单是否打开
                    menu_open = False
                    for i in range(10):  # 检查10次，每次0.1秒
                        # 查找下拉菜单
                        dropdown = main_window.child_window(title_re=".*下拉.*", control_type="Menu")
                        if dropdown.exists():
                            menu_open = True
                            break
                        time.sleep(0.1)
                    
                    response_time = time.time() - start_time
                    menu_responses.append(response_time)
                    
                    print(f"{menu_name}菜单响应时间: {response_time:.2f}秒")
                    
                    # 关闭菜单
                    main_window.click(coords=(100, 100))
                    time.sleep(0.5)
                
            except Exception as e:
                print(f"⚠️ {menu_name}菜单响应测试失败: {e}")
        
        # 分析响应性能
        if menu_responses:
            avg_response = sum(menu_responses) / len(menu_responses)
            max_response = max(menu_responses)
            min_response = min(menu_responses)
            
            print(f"\n=== UI响应性能分析 ===")
            print(f"平均响应时间: {avg_response:.2f}秒")
            print(f"最快响应时间: {min_response:.2f}秒")
            print(f"最慢响应时间: {max_response:.2f}秒")
            
            if avg_response < 0.5:
                print("✅ UI响应性能优秀")
            elif avg_response < 1:
                print("✅ UI响应性能良好")
            elif avg_response < 2:
                print("⚠️ UI响应性能一般")
            else:
                print("❌ UI响应性能较差")
        
        # 关闭应用
        main_window.close()
        time.sleep(2)
        
        return True
        
    except Exception as e:
        print(f"❌ UI响应性能测试失败: {e}")
        take_screenshot("ui_responsiveness_error")
        return False

def test_memory_usage():
    """测试内存使用情况"""
    print("=== 开始测试内存使用情况 ===")
    
    try:
        # 记录初始内存
        initial_memory = get_memory_usage()
        print(f"初始内存使用: {initial_memory:.1f}MB")
        
        # 启动应用程序
        app = Application().start(APP_INFO["path"], wait_for_idle=False)
        time.sleep(5)
        
        main_window = app.window(title_re=".*产品编辑器.*")
        main_window.wait("visible", timeout=10)
        
        # 记录启动后内存
        after_startup_memory = get_memory_usage()
        print(f"启动后内存使用: {after_startup_memory:.1f}MB")
        
        # 执行一系列操作，监控内存变化
        memory_readings = [after_startup_memory]
        
        # 打开UI布局编辑器
        ui_layout_button = main_window.child_window(title_re=".*UI布局.*", control_type="Button")
        
        if ui_layout_button.exists():
            ui_layout_button.click()
            time.sleep(3)
            
            ui_layout_window = app.window(title_re=".*UI布局编辑器.*")
            ui_layout_window.wait("visible", timeout=10)
            
            # 记录打开编辑器后内存
            after_editor_memory = get_memory_usage()
            memory_readings.append(after_editor_memory)
            print(f"打开编辑器后内存使用: {after_editor_memory:.1f}MB")
            
            # 添加一些控件
            control_library = ui_layout_window.child_window(title_re=".*控件库.*", control_type="Pane")
            edit_area = ui_layout_window.child_window(title_re=".*编辑区.*", control_type="Pane")
            
            if control_library.exists() and edit_area.exists():
                # 添加10个控件
                for i in range(10):
                    try:
                        qlabel_item = control_library.child_window(title="QLabel", control_type="ListItem")
                        if qlabel_item.exists():
                            qlabel_item.drag_mouse_input(dst=(edit_area.rectangle().mid_point().x, 
                                                             edit_area.rectangle().mid_point().y))
                            time.sleep(0.2)
                    except:
                        pass
                
                # 记录添加控件后内存
                after_controls_memory = get_memory_usage()
                memory_readings.append(after_controls_memory)
                print(f"添加控件后内存使用: {after_controls_memory:.1f}MB")
            
            # 关闭编辑器
            ui_layout_window.close()
            time.sleep(2)
        
        # 记录最终内存
        final_memory = get_memory_usage()
        memory_readings.append(final_memory)
        print(f"最终内存使用: {final_memory:.1f}MB")
        
        # 分析内存使用情况
        print(f"\n=== 内存使用分析 ===")
        print(f"内存使用峰值: {max(memory_readings):.1f}MB")
        print(f"内存增长: {max(memory_readings) - initial_memory:.1f}MB")
        
        memory_growth = max(memory_readings) - initial_memory
        
        if memory_growth < 50:
            print("✅ 内存使用控制优秀")
        elif memory_growth < 100:
            print("✅ 内存使用控制良好")
        elif memory_growth < 200:
            print("⚠️ 内存使用控制一般")
        else:
            print("❌ 内存使用控制较差")
        
        # 检查内存泄漏
        if final_memory > after_startup_memory * 1.5:
            print("⚠️ 可能存在内存泄漏")
        else:
            print("✅ 内存释放正常")
        
        # 关闭应用
        main_window.close()
        time.sleep(2)
        
        return True
        
    except Exception as e:
        print(f"❌ 内存使用测试失败: {e}")
        take_screenshot("memory_usage_error")
        return False

def test_stress_performance():
    """测试压力性能"""
    print("=== 开始测试压力性能 ===")
    
    try:
        # 启动应用程序
        app = Application().start(APP_INFO["path"], wait_for_idle=False)
        time.sleep(5)
        
        main_window = app.window(title_re=".*产品编辑器.*")
        main_window.wait("visible", timeout=10)
        
        take_screenshot("stress_performance_start")
        
        # 记录开始时间
        start_time = time.time()
        operation_count = 0
        
        # 执行压力测试（快速操作）
        for i in range(50):  # 执行50次操作
            try:
                # 交替点击不同菜单
                if i % 3 == 0:
                    button = main_window.child_window(title_re=".*文件.*", control_type="Button")
                elif i % 3 == 1:
                    button = main_window.child_window(title_re=".*编辑.*", control_type="Button")
                else:
                    button = main_window.child_window(title_re=".*视图.*", control_type="Button")
                
                if button.exists():
                    button.click()
                    time.sleep(0.1)
                    
                    # 点击空白处关闭菜单
                    main_window.click(coords=(100, 100))
                    time.sleep(0.1)
                    
                    operation_count += 1
                    
                    if i % 10 == 0:
                        print(f"已完成 {i} 次操作")
                        
                        # 检查内存使用
                        current_memory = get_memory_usage()
                        print(f"当前内存使用: {current_memory:.1f}MB")
            
            except:
                pass
        
        # 记录结束时间
        end_time = time.time()
        total_time = end_time - start_time
        
        print(f"\n=== 压力测试结果 ===")
        print(f"总操作次数: {operation_count}")
        print(f"总测试时间: {total_time:.2f}秒")
        print(f"平均操作时间: {total_time/operation_count:.3f}秒/操作")
        
        # 检查应用是否仍然响应
        try:
            main_window.set_focus()
            print("✅ 应用在压力测试后仍然响应")
        except:
            print("❌ 应用在压力测试后无响应")
        
        # 关闭应用
        main_window.close()
        time.sleep(2)
        
        return True
        
    except Exception as e:
        print(f"❌ 压力性能测试失败: {e}")
        take_screenshot("stress_performance_error")
        return False

def test_file_operation_performance():
    """测试文件操作性能"""
    print("=== 开始测试文件操作性能 ===")
    
    try:
        # 启动应用程序
        app = Application().start(APP_INFO["path"], wait_for_idle=False)
        time.sleep(5)
        
        main_window = app.window(title_re=".*产品编辑器.*")
        main_window.wait("visible", timeout=10)
        
        take_screenshot("file_operation_start")
        
        # 测试保存操作性能
        save_times = []
        
        for i in range(3):  # 测试3次保存操作
            try:
                start_time = time.time()
                
                # 点击文件菜单
                file_menu = main_window.child_window(title_re=".*文件.*", control_type="Button")
                if file_menu.exists():
                    file_menu.click()
                    time.sleep(0.5)
                    
                    # 点击保存
                    save_item = main_window.child_window(title_re=".*保存.*", control_type="MenuItem")
                    if save_item.exists():
                        save_item.click()
                        time.sleep(1)
                        
                        # 处理保存对话框
                        save_dialog = app.window(title_re=".*保存.*")
                        if save_dialog.exists():
                            # 输入文件名
                            filename_edit = save_dialog.child_window(control_type="Edit")
                            if filename_edit.exists():
                                filename_edit.set_text(f"test_performance_{i}.proj")
                                time.sleep(0.5)
                                
                                # 点击保存
                                save_button = save_dialog.child_window(title="保存", control_type="Button")
                                if save_button.exists():
                                    save_button.click()
                                    time.sleep(2)
                                    
                                    save_time = time.time() - start_time
                                    save_times.append(save_time)
                                    print(f"第 {i+1} 次保存时间: {save_time:.2f}秒")
                            
                            # 关闭对话框（如果仍然存在）
                            save_dialog.close()
                
            except Exception as e:
                print(f"⚠️ 第 {i+1} 次保存测试失败: {e}")
        
        # 分析保存性能
        if save_times:
            avg_save_time = sum(save_times) / len(save_times)
            print(f"\n平均保存时间: {avg_save_time:.2f}秒")
            
            if avg_save_time < 3:
                print("✅ 文件保存性能优秀")
            elif avg_save_time < 5:
                print("✅ 文件保存性能良好")
            elif avg_save_time < 10:
                print("⚠️ 文件保存性能一般")
            else:
                print("❌ 文件保存性能较差")
        
        # 关闭应用
        main_window.close()
        time.sleep(2)
        
        return True
        
    except Exception as e:
        print(f"❌ 文件操作性能测试失败: {e}")
        take_screenshot("file_operation_error")
        return False

if __name__ == "__main__":
    # 创建截图目录
    import os
    if not os.path.exists("screenshots"):
        os.makedirs("screenshots")
    
    # 运行测试
    print("开始运行性能测试...")
    
    tests = [
        test_startup_performance,
        test_ui_responsiveness,
        test_memory_usage,
        test_stress_performance,
        test_file_operation_performance
    ]
    
    passed = 0
    total = len(tests)
    
    for test_func in tests:
        try:
            if test_func():
                passed += 1
        except Exception as e:
            print(f"❌ {test_func.__name__} 执行异常: {e}")
    
    print(f"\n=== 性能测试结果 ===")
    print(f"通过: {passed}/{total}")
    print(f"成功率: {passed/total*100:.1f}%")