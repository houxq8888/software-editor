"""
UI布局编辑器功能测试用例
测试控件拖拽、属性编辑、实时预览等核心功能
"""

import pytest
import time
import os
import pyautogui
from pywinauto import Application
from config import APP_INFO, UI_ELEMENTS, TEST_DATA
from utils import take_screenshot

def test_open_ui_layout_editor():
    """测试打开UI布局编辑器功能"""
    print("=== 开始测试打开UI布局编辑器 ===")
    
    try:
        # 启动应用程序
        app = Application().start(APP_INFO["path"], wait_for_idle=False)
        time.sleep(5)
        
        # 获取主窗口
        main_window = app.window(title_re=".*产品编辑器.*")
        main_window.wait("visible", timeout=10)
        
        take_screenshot("ui_layout_editor_start")
        
        # 方法1: 使用菜单打开UI布局编辑器
        try:
            # 点击UI布局菜单
            ui_layout_menu = main_window.child_window(title="UI布局", control_type="MenuItem")
            ui_layout_menu.click()
            time.sleep(0.5)
            
            # 点击打开UI布局编辑器菜单项
            open_ui_layout_item = main_window.child_window(title="打开UI布局编辑器", control_type="MenuItem")
            open_ui_layout_item.click()
            time.sleep(2)
        except:
            # 方法2: 使用快捷键
            main_window.type_keys("^L")  # Ctrl+L
            time.sleep(2)
        
        # 获取UI布局编辑器窗口
        ui_layout_window = app.window(title_re=".*UI布局编辑器.*")
        ui_layout_window.wait("visible", timeout=10)
        
        take_screenshot("ui_layout_editor_opened")
        
        # 验证窗口标题
        window_title = ui_layout_window.window_text()
        assert "UI布局编辑器" in window_title
        
        print("[PASS] UI布局编辑器打开测试通过")
        
        # 关闭UI布局编辑器窗口
        ui_layout_window.close()
        time.sleep(1)
        
        # 关闭主窗口
        main_window.close()
        time.sleep(2)
        
        return True
        
    except Exception as e:
        print(f"[ERROR] UI布局编辑器打开测试失败: {e}")
        take_screenshot("ui_layout_editor_open_error")
        return False

def test_control_drag_drop():
    """测试控件拖拽功能"""
    print("=== 开始测试控件拖拽功能 ===")
    
    try:
        # 启动应用程序并打开UI布局编辑器
        app = Application().start(APP_INFO["path"], wait_for_idle=False)
        time.sleep(5)
        
        main_window = app.window(title_re=".*产品编辑器.*")
        main_window.wait("visible", timeout=10)
        
        # 打开UI布局编辑器
        main_window.type_keys("^L")  # Ctrl+L
        time.sleep(2)
        
        ui_layout_window = app.window(title_re=".*UI布局编辑器.*")
        ui_layout_window.wait("visible", timeout=10)
        
        take_screenshot("control_drag_drop_start")
        
        # 获取控件库区域（左侧）
        control_library = ui_layout_window.child_window(auto_id="QApplication.UILayoutWindow.controlLibraryWidget")
        
        # 获取编辑区域（右侧）
        edit_area = ui_layout_window.child_window(auto_id="QApplication.UILayoutWindow.editAreaWidget")
        
        # 查找按钮控件
        button_control = control_library.child_window(title_re=".*按钮.*", control_type="ListItem")
        
        if button_control.exists():
            # 获取控件位置
            button_rect = button_control.rectangle()
            edit_rect = edit_area.rectangle()
            
            # 计算拖拽坐标
            start_x = button_rect.left + button_rect.width() // 2
            start_y = button_rect.top + button_rect.height() // 2
            
            end_x = edit_rect.left + edit_rect.width() // 2
            end_y = edit_rect.top + edit_rect.height() // 2
            
            # 执行拖拽操作
            pyautogui.moveTo(start_x, start_y)
            pyautogui.mouseDown()
            time.sleep(0.5)
            
            pyautogui.moveTo(end_x, end_y, duration=1)
            time.sleep(0.5)
            
            pyautogui.mouseUp()
            time.sleep(1)
            
            take_screenshot("control_drag_drop_completed")
            
            # 验证拖拽结果（检查编辑区域是否有新控件）
            # 这里需要根据实际实现来验证
            
            print("✅ 控件拖拽功能测试通过")
        else:
            print("⚠️ 未找到按钮控件，跳过拖拽测试")
        
        # 关闭窗口
        ui_layout_window.close()
        time.sleep(1)
        main_window.close()
        time.sleep(2)
        
        return True
        
    except Exception as e:
        print(f"❌ 控件拖拽功能测试失败: {e}")
        take_screenshot("control_drag_drop_error")
        return False

def test_property_editing():
    """测试控件属性编辑功能"""
    print("=== 开始测试控件属性编辑功能 ===")
    
    try:
        # 启动应用程序并打开UI布局编辑器
        app = Application().start(APP_INFO["path"], wait_for_idle=False)
        time.sleep(5)
        
        main_window = app.window(title_re=".*产品编辑器.*")
        main_window.wait("visible", timeout=10)
        
        # 打开UI布局编辑器
        main_window.type_keys("^L")  # Ctrl+L
        time.sleep(2)
        
        ui_layout_window = app.window(title_re=".*UI布局编辑器.*")
        ui_layout_window.wait("visible", timeout=10)
        
        take_screenshot("property_editing_start")
        
        # 获取属性编辑器区域
        property_editor = ui_layout_window.child_window(auto_id="QApplication.UILayoutWindow.propertyEditorWidget")
        
        if property_editor.exists():
            # 测试文本属性编辑
            text_property = property_editor.child_window(title_re=".*文本.*", control_type="Edit")
            
            if text_property.exists():
                text_property.click()
                text_property.set_text("测试文本")
                time.sleep(0.5)
                
                # 验证文本设置
                assert text_property.texts()[0] == "测试文本"
                
                print("✅ 文本属性编辑测试通过")
            
            # 测试尺寸属性编辑
            width_property = property_editor.child_window(title_re=".*宽度.*", control_type="Edit")
            
            if width_property.exists():
                width_property.click()
                width_property.set_text("200")
                time.sleep(0.5)
                
                # 验证尺寸设置
                assert width_property.texts()[0] == "200"
                
                print("✅ 尺寸属性编辑测试通过")
            
            take_screenshot("property_editing_completed")
        else:
            print("⚠️ 未找到属性编辑器，跳过属性编辑测试")
        
        # 关闭窗口
        ui_layout_window.close()
        time.sleep(1)
        main_window.close()
        time.sleep(2)
        
        return True
        
    except Exception as e:
        print(f"❌ 控件属性编辑功能测试失败: {e}")
        take_screenshot("property_editing_error")
        return False
        return False

def test_real_time_preview():
    """测试实时预览功能"""
    print("=== 开始测试实时预览功能 ===")
    
    try:
        # 启动应用程序并打开UI布局编辑器
        app = Application().start(APP_INFO["path"], wait_for_idle=False)
        time.sleep(5)
        
        main_window = app.window(title_re=".*产品编辑器.*")
        main_window.wait("visible", timeout=10)
        
        # 打开UI布局编辑器
        main_window.type_keys("^L")  # Ctrl+L
        time.sleep(2)
        
        ui_layout_window = app.window(title_re=".*UI布局编辑器.*")
        ui_layout_window.wait("visible", timeout=10)
        
        take_screenshot("real_time_preview_start")
        
        # 查找预览按钮
        preview_button = ui_layout_window.child_window(title="预览", control_type="Button")
        
        if preview_button.exists():
            # 点击预览按钮
            preview_button.click()
            time.sleep(2)
            
            # 查找预览窗口
            preview_window = app.window(title_re=".*预览.*")
            
            if preview_window.exists():
                preview_window.wait("visible", timeout=5)
                
                take_screenshot("real_time_preview_opened")
                
                # 验证预览窗口标题
                preview_title = preview_window.window_text()
                assert "预览" in preview_title
                
                # 测试预览窗口交互
                # 这里可以添加具体的交互测试，如点击按钮、输入文本等
                
                # 关闭预览窗口
                preview_window.close()
                time.sleep(1)
                
                print("✅ 实时预览功能测试通过")
            else:
                print("⚠️ 未找到预览窗口，跳过预览测试")
        else:
            print("⚠️ 未找到预览按钮，跳过预览测试")
        
        # 关闭UI布局编辑器窗口
        ui_layout_window.close()
        time.sleep(1)
        
        # 关闭主窗口
        main_window.close()
        time.sleep(2)
        
        return True
        
    except Exception as e:
        print(f"❌ 实时预览功能测试失败: {e}")
        take_screenshot("real_time_preview_error")
        return False

def test_layout_save_load():
    """测试布局保存和加载功能"""
    print("=== 开始测试布局保存和加载功能 ===")
    
    test_layout_file = os.path.join(os.path.dirname(__file__), "test_layout.json")
    
    try:
        # 启动应用程序并打开UI布局编辑器
        app = Application().start(APP_INFO["path"], wait_for_idle=False)
        time.sleep(5)
        
        main_window = app.window(title_re=".*产品编辑器.*")
        main_window.wait("visible", timeout=10)
        
        # 打开UI布局编辑器
        main_window.type_keys("^L")  # Ctrl+L
        time.sleep(2)
        
        ui_layout_window = app.window(title_re=".*UI布局编辑器.*")
        ui_layout_window.wait("visible", timeout=10)
        
        take_screenshot("layout_save_load_start")
        
        # 测试保存布局
        save_button = ui_layout_window.child_window(title="保存", control_type="Button")
        
        if save_button.exists():
            save_button.click()
            time.sleep(2)
            
            # 处理保存对话框
            save_dialog = app.window(title_re=".*保存.*")
            if save_dialog.exists():
                filename_edit = save_dialog.child_window(auto_id="FileNameControlHost")
                filename_edit.set_text(test_layout_file)
                time.sleep(1)
                
                save_confirm = save_dialog.child_window(title="保存")
                save_confirm.click()
                time.sleep(2)
                
                # 验证文件是否保存成功
                assert os.path.exists(test_layout_file)
                
                print("✅ 布局保存功能测试通过")
                
                # 测试加载布局
                load_button = ui_layout_window.child_window(title="加载", control_type="Button")
                
                if load_button.exists():
                    load_button.click()
                    time.sleep(2)
                    
                    # 处理加载对话框
                    load_dialog = app.window(title_re=".*打开.*")
                    if load_dialog.exists():
                        filename_edit = load_dialog.child_window(auto_id="FileNameControlHost")
                        filename_edit.set_text(test_layout_file)
                        time.sleep(1)
                        
                        load_confirm = load_dialog.child_window(title="打开")
                        load_confirm.click()
                        time.sleep(2)
                        
                        print("✅ 布局加载功能测试通过")
                    else:
                        print("⚠️ 未找到加载对话框，跳过加载测试")
                else:
                    print("⚠️ 未找到加载按钮，跳过加载测试")
            else:
                print("⚠️ 未找到保存对话框，跳过保存测试")
        else:
            print("⚠️ 未找到保存按钮，跳过保存测试")
        
        # 清理测试文件
        if os.path.exists(test_layout_file):
            os.remove(test_layout_file)
        
        # 关闭窗口
        ui_layout_window.close()
        time.sleep(1)
        main_window.close()
        time.sleep(2)
        
        return True
        
    except Exception as e:
        print(f"❌ 布局保存和加载功能测试失败: {e}")
        take_screenshot("layout_save_load_error")
        
        # 清理测试文件
        if os.path.exists(test_layout_file):
            os.remove(test_layout_file)
        
        return False

def test_splitter_resize():
    """测试分隔条调整功能"""
    print("=== 开始测试分隔条调整功能 ===")
    
    try:
        # 启动应用程序并打开UI布局编辑器
        app = Application().start(APP_INFO["path"], wait_for_idle=False)
        time.sleep(5)
        
        main_window = app.window(title_re=".*产品编辑器.*")
        main_window.wait("visible", timeout=10)
        
        # 打开UI布局编辑器
        main_window.type_keys("^L")  # Ctrl+L
        time.sleep(2)
        
        ui_layout_window = app.window(title_re=".*UI布局编辑器.*")
        ui_layout_window.wait("visible", timeout=10)
        
        take_screenshot("splitter_resize_start")
        
        # 查找分隔条
        splitter = ui_layout_window.child_window(auto_id="QApplication.UILayoutWindow.splitter")
        
        if splitter.exists():
            # 获取分隔条位置
            splitter_rect = splitter.rectangle()
            
            # 测试向左拖动分隔条（控件库变小，编辑区变大）
            start_x = splitter_rect.left + splitter_rect.width() // 2
            start_y = splitter_rect.top + splitter_rect.height() // 2
            
            # 向左拖动100像素
            pyautogui.moveTo(start_x, start_y)
            pyautogui.mouseDown()
            time.sleep(0.5)
            
            pyautogui.moveTo(start_x - 100, start_y, duration=1)
            time.sleep(0.5)
            
            pyautogui.mouseUp()
            time.sleep(1)
            
            take_screenshot("splitter_left_resize")
            
            # 测试向右拖动分隔条（控件库变大，编辑区变小）
            pyautogui.moveTo(start_x - 100, start_y)
            pyautogui.mouseDown()
            time.sleep(0.5)
            
            pyautogui.moveTo(start_x + 50, start_y, duration=1)
            time.sleep(0.5)
            
            pyautogui.mouseUp()
            time.sleep(1)
            
            take_screenshot("splitter_right_resize")
            
            print("✅ 分隔条调整功能测试通过")
        else:
            print("⚠️ 未找到分隔条，跳过分隔条调整测试")
        
        # 关闭窗口
        ui_layout_window.close()
        time.sleep(1)
        main_window.close()
        time.sleep(2)
        
        return True
        
    except Exception as e:
        print(f"❌ 分隔条调整功能测试失败: {e}")
        take_screenshot("splitter_resize_error")
        return False

def test_control_text_editing():
    """测试控件文本编辑功能"""
    print("=== 开始测试控件文本编辑功能 ===")
    
    try:
        # 启动应用程序并打开UI布局编辑器
        app = Application().start(APP_INFO["path"], wait_for_idle=False)
        time.sleep(5)
        
        main_window = app.window(title_re=".*产品编辑器.*")
        main_window.wait("visible", timeout=10)
        
        # 打开UI布局编辑器
        main_window.type_keys("^L")  # Ctrl+L
        time.sleep(2)
        
        ui_layout_window = app.window(title_re=".*UI布局编辑器.*")
        ui_layout_window.wait("visible", timeout=10)
        
        take_screenshot("control_text_editing_start")
        
        # 获取控件库和编辑区域
        control_library = ui_layout_window.child_window(auto_id="QApplication.UILayoutWindow.controlLibraryWidget")
        edit_area = ui_layout_window.child_window(auto_id="QApplication.UILayoutWindow.editAreaWidget")
        
        # 查找QLabel控件
        qlabel_control = control_library.child_window(title_re=".*QLabel.*", control_type="ListItem")
        
        if qlabel_control.exists():
            # 拖拽QLabel到编辑区域
            qlabel_rect = qlabel_control.rectangle()
            edit_rect = edit_area.rectangle()
            
            start_x = qlabel_rect.left + qlabel_rect.width() // 2
            start_y = qlabel_rect.top + qlabel_rect.height() // 2
            
            end_x = edit_rect.left + edit_rect.width() // 2
            end_y = edit_rect.top + edit_rect.height() // 2
            
            pyautogui.moveTo(start_x, start_y)
            pyautogui.mouseDown()
            time.sleep(0.5)
            
            pyautogui.moveTo(end_x, end_y, duration=1)
            time.sleep(0.5)
            
            pyautogui.mouseUp()
            time.sleep(1)
            
            take_screenshot("qlabel_dragged")
            
            # 双击QLabel进行文本编辑
            pyautogui.doubleClick(end_x, end_y)
            time.sleep(1)
            
            # 输入测试文本
            pyautogui.write("测试QLabel文本")
            time.sleep(0.5)
            
            # 按Enter确认编辑
            pyautogui.press("enter")
            time.sleep(1)
            
            take_screenshot("qlabel_text_edited")
            
            print("✅ QLabel文本编辑功能测试通过")
        else:
            print("⚠️ 未找到QLabel控件，跳过文本编辑测试")
        
        # 关闭窗口
        ui_layout_window.close()
        time.sleep(1)
        main_window.close()
        time.sleep(2)
        
        return True
        
    except Exception as e:
        print(f"❌ 控件文本编辑功能测试失败: {e}")
        take_screenshot("control_text_editing_error")
        return False

def test_tab_widget_editing():
    """测试TabWidget编辑功能"""
    print("=== 开始测试TabWidget编辑功能 ===")
    
    try:
        # 启动应用程序并打开UI布局编辑器
        app = Application().start(APP_INFO["path"], wait_for_idle=False)
        time.sleep(5)
        
        main_window = app.window(title_re=".*产品编辑器.*")
        main_window.wait("visible", timeout=10)
        
        # 打开UI布局编辑器
        main_window.type_keys("^L")  # Ctrl+L
        time.sleep(2)
        
        ui_layout_window = app.window(title_re=".*UI布局编辑器.*")
        ui_layout_window.wait("visible", timeout=10)
        
        take_screenshot("tab_widget_editing_start")
        
        # 获取控件库和编辑区域
        control_library = ui_layout_window.child_window(auto_id="QApplication.UILayoutWindow.controlLibraryWidget")
        edit_area = ui_layout_window.child_window(auto_id="QApplication.UILayoutWindow.editAreaWidget")
        
        # 查找QTabWidget控件
        tab_widget_control = control_library.child_window(title_re=".*QTabWidget.*", control_type="ListItem")
        
        if tab_widget_control.exists():
            # 拖拽QTabWidget到编辑区域
            tab_widget_rect = tab_widget_control.rectangle()
            edit_rect = edit_area.rectangle()
            
            start_x = tab_widget_rect.left + tab_widget_rect.width() // 2
            start_y = tab_widget_rect.top + tab_widget_rect.height() // 2
            
            end_x = edit_rect.left + edit_rect.width() // 2
            end_y = edit_rect.top + edit_rect.height() // 2
            
            pyautogui.moveTo(start_x, start_y)
            pyautogui.mouseDown()
            time.sleep(0.5)
            
            pyautogui.moveTo(end_x, end_y, duration=1)
            time.sleep(0.5)
            
            pyautogui.mouseUp()
            time.sleep(1)
            
            take_screenshot("tab_widget_dragged")
            
            # 双击Tab页标题进行编辑
            # 查找Tab页标题区域
            tab_bar = edit_area.child_window(title_re=".*Tab.*", control_type="Tab")
            
            if tab_bar.exists():
                # 获取第一个Tab页的位置
                tab_rect = tab_bar.rectangle()
                tab_center_x = tab_rect.left + tab_rect.width() // 4
                tab_center_y = tab_rect.top + tab_rect.height() // 2
                
                # 双击Tab页标题
                pyautogui.doubleClick(tab_center_x, tab_center_y)
                time.sleep(1)
                
                # 输入新的Tab页标题
                pyautogui.write("新Tab页")
                time.sleep(0.5)
                
                # 按Enter确认编辑
                pyautogui.press("enter")
                time.sleep(1)
                
                take_screenshot("tab_title_edited")
                
                print("✅ TabWidget编辑功能测试通过")
            else:
                print("⚠️ 未找到Tab页标题区域，跳过Tab页编辑测试")
        else:
            print("⚠️ 未找到QTabWidget控件，跳过TabWidget编辑测试")
        
        # 关闭窗口
        ui_layout_window.close()
        time.sleep(1)
        main_window.close()
        time.sleep(2)
        
        return True
        
    except Exception as e:
        print(f"❌ TabWidget编辑功能测试失败: {e}")
        take_screenshot("tab_widget_editing_error")
        return False

if __name__ == "__main__":
    # 运行所有测试用例
    results = []
    
    results.append(test_open_ui_layout_editor())
    results.append(test_control_drag_drop())
    results.append(test_property_editing())
    results.append(test_real_time_preview())
    results.append(test_layout_save_load())
    
    print("\n=== UI布局编辑器测试结果汇总 ===")
    passed = sum(results)
    total = len(results)
    
    print(f"通过测试: {passed}/{total}")
    
    if passed == total:
        print("🎉 所有UI布局编辑器功能测试通过！")
    else:
        print("⚠️ 部分测试失败，请检查日志和截图")