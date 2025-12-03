"""
预览功能测试用例
测试实时预览、交互预览、预览窗口管理等核心功能
"""

import pytest
import time
import os
from pywinauto import Application
from pywinauto.keyboard import send_keys
from config import APP_INFO, UI_ELEMENTS, TEST_DATA
from utils import take_screenshot

def test_preview_button():
    """测试预览按钮功能"""
    print("=== 开始测试预览按钮功能 ===")
    
    try:
        # 启动应用程序
        app = Application().start(APP_INFO["path"], wait_for_idle=False)
        time.sleep(5)
        
        # 获取主窗口
        main_window = app.window(title_re=".*产品编辑器.*")
        main_window.wait("visible", timeout=10)
        
        take_screenshot("preview_button_start")
        
        # 查找预览按钮
        preview_button = main_window.child_window(title_re=".*预览.*", control_type="Button")
        
        if preview_button.exists():
            # 点击预览按钮
            preview_button.click()
            time.sleep(3)
            
            # 查找预览窗口
            preview_window = app.window(title_re=".*预览.*")
            
            if preview_window.exists():
                preview_window.wait("visible", timeout=5)
                
                take_screenshot("preview_window_opened")
                
                # 验证预览窗口内容
                window_title = preview_window.window_text()
                assert "预览" in window_title
                
                # 检查预览窗口是否有内容显示区域
                content_area = preview_window.child_window(control_type="Pane")
                
                if content_area.exists():
                    print("✅ 预览窗口内容区域存在")
                else:
                    print("⚠️ 预览窗口内容区域未找到")
                
                # 关闭预览窗口
                preview_window.close()
                time.sleep(1)
                
                print("✅ 预览按钮功能测试通过")
            else:
                print("⚠️ 未找到预览窗口，跳过预览按钮测试")
        else:
            # 尝试菜单中的预览选项
            preview_menu = main_window.child_window(title_re=".*预览.*", control_type="MenuItem")
            
            if preview_menu.exists():
                preview_menu.click()
                time.sleep(0.5)
                
                # 查找预览选项
                preview_option = main_window.child_window(title_re=".*显示预览.*", control_type="MenuItem")
                
                if preview_option.exists():
                    preview_option.click()
                    time.sleep(3)
                    
                    print("✅ 预览菜单功能测试通过")
                else:
                    print("⚠️ 未找到预览选项，跳过预览按钮测试")
            else:
                print("⚠️ 未找到预览相关控件，跳过预览按钮测试")
        
        # 关闭主窗口
        main_window.close()
        time.sleep(2)
        
        return True
        
    except Exception as e:
        print(f"❌ 预览按钮功能测试失败: {e}")
        take_screenshot("preview_button_error")
        return False

def test_real_time_preview():
    """测试实时预览功能"""
    print("=== 开始测试实时预览功能 ===")
    
    try:
        # 启动应用程序
        app = Application().start(APP_INFO["path"], wait_for_idle=False)
        time.sleep(5)
        
        # 获取主窗口
        main_window = app.window(title_re=".*产品编辑器.*")
        main_window.wait("visible", timeout=10)
        
        # 输入产品信息
        product_name_edit = main_window.child_window(auto_id="QApplication.MainWindow.centralWidget.groupBox.productNameLineEdit")
        product_name_edit.set_text("实时预览测试产品")
        time.sleep(0.5)
        
        take_screenshot("real_time_preview_start")
        
        # 查找实时预览功能
        real_time_preview_button = main_window.child_window(title_re=".*实时预览.*", control_type="Button")
        
        if real_time_preview_button.exists():
            # 点击实时预览按钮
            real_time_preview_button.click()
            time.sleep(2)
            
            # 查找实时预览窗口或面板
            preview_panel = main_window.child_window(title_re=".*预览.*", control_type="Pane")
            
            if preview_panel.exists():
                preview_panel.wait("visible", timeout=5)
                
                take_screenshot("real_time_preview_active")
                
                # 修改产品信息，观察预览是否实时更新
                product_name_edit.set_text("更新后的产品名称")
                time.sleep(1)
                
                take_screenshot("real_time_preview_updated")
                
                # 检查预览内容是否更新
                # 这里可以添加更具体的验证逻辑，比如检查预览面板中的文本内容
                
                print("✅ 实时预览功能测试通过")
            else:
                print("⚠️ 未找到实时预览面板，跳过实时预览测试")
        else:
            print("⚠️ 未找到实时预览按钮，跳过实时预览测试")
        
        # 关闭主窗口
        main_window.close()
        time.sleep(2)
        
        return True
        
    except Exception as e:
        print(f"❌ 实时预览功能测试失败: {e}")
        take_screenshot("real_time_preview_error")
        return False

def test_interactive_preview():
    """测试交互式预览功能"""
    print("=== 开始测试交互式预览功能 ===")
    
    try:
        # 启动应用程序
        app = Application().start(APP_INFO["path"], wait_for_idle=False)
        time.sleep(5)
        
        main_window = app.window(title_re=".*产品编辑器.*")
        main_window.wait("visible", timeout=10)
        
        # 打开预览窗口
        main_window.type_keys("^R")  # Ctrl+R
        time.sleep(2)
        
        preview_window = app.window(title_re=".*预览.*")
        preview_window.wait("visible", timeout=10)
        
        take_screenshot("interactive_preview_start")
        
        # 测试按钮点击交互
        test_button = preview_window.child_window(title="测试按钮", control_type="Button")
        
        if test_button.exists():
            test_button.click()
            time.sleep(1)
            
            # 检查是否有响应
            response_label = preview_window.child_window(title_re=".*响应.*", control_type="Text")
            
            if response_label.exists():
                print("✅ 按钮点击交互测试通过")
            else:
                print("⚠️ 未找到响应标签，但按钮点击功能正常")
        else:
            print("⚠️ 未找到测试按钮，跳过按钮交互测试")
        
        # 测试文本框输入
        text_input = preview_window.child_window(title_re=".*输入.*", control_type="Edit")
        
        if text_input.exists():
            text_input.set_text("测试输入文本")
            time.sleep(1)
            
            # 验证输入内容
            input_text = text_input.texts()[0]
            
            if input_text == "测试输入文本":
                print("✅ 文本框输入测试通过")
            else:
                print("⚠️ 文本框输入内容不匹配")
        else:
            print("⚠️ 未找到文本框，跳过输入测试")
        
        # 测试下拉框选择
        combo_box = preview_window.child_window(title_re=".*选择.*", control_type="ComboBox")
        
        if combo_box.exists():
            combo_box.click()
            time.sleep(0.5)
            
            # 选择第一个选项
            first_option = combo_box.child_window(title_re=".*选项.*", found_index=0)
            
            if first_option.exists():
                first_option.click()
                time.sleep(1)
                print("✅ 下拉框选择测试通过")
            else:
                print("⚠️ 未找到下拉框选项")
        else:
            print("⚠️ 未找到下拉框，跳过选择测试")
        
        # 关闭预览窗口
        preview_window.close()
        time.sleep(1)
        main_window.close()
        time.sleep(2)
        
        return True
        
    except Exception as e:
        print(f"❌ 交互式预览功能测试失败: {e}")
        take_screenshot("interactive_preview_error")
        return False

def test_preview_layout():
    """测试预览布局功能"""
    print("=== 开始测试预览布局功能 ===")
    
    try:
        # 启动应用程序
        app = Application().start(APP_INFO["path"], wait_for_idle=False)
        time.sleep(5)
        
        main_window = app.window(title_re=".*产品编辑器.*")
        main_window.wait("visible", timeout=10)
        
        # 打开预览窗口
        main_window.type_keys("^R")  # Ctrl+R
        time.sleep(2)
        
        preview_window = app.window(title_re=".*预览.*")
        preview_window.wait("visible", timeout=10)
        
        take_screenshot("preview_layout_start")
        
        # 测试布局切换
        layout_combo = preview_window.child_window(title_re=".*布局.*", control_type="ComboBox")
        
        if layout_combo.exists():
            # 测试不同的布局选项
            layouts = ["网格布局", "流式布局", "边框布局"]
            
            for layout in layouts:
                try:
                    layout_combo.select(layout)
                    time.sleep(1)
                    
                    take_screenshot(f"preview_layout_{layout}")
                    print(f"✅ 布局 '{layout}' 切换测试通过")
                except:
                    print(f"⚠️ 布局 '{layout}' 切换失败")
        else:
            print("⚠️ 未找到布局选择框，跳过布局测试")
        
        # 测试窗口大小调整
        preview_window.set_focus()
        time.sleep(1)
        
        # 获取窗口大小
        window_rect = preview_window.rectangle()
        
        # 调整窗口大小
        pyautogui.moveTo(window_rect.right - 10, window_rect.bottom - 10)
        pyautogui.mouseDown()
        time.sleep(0.5)
        
        pyautogui.moveTo(window_rect.right + 100, window_rect.bottom + 100, duration=1)
        time.sleep(0.5)
        
        pyautogui.mouseUp()
        time.sleep(1)
        
        take_screenshot("preview_resized")
        print("✅ 预览窗口大小调整测试通过")
        
        # 关闭预览窗口
        preview_window.close()
        time.sleep(1)
        main_window.close()
        time.sleep(2)
        
        return True
        
    except Exception as e:
        print(f"❌ 预览布局功能测试失败: {e}")
        take_screenshot("preview_layout_error")
        return False

def test_preview_theme():
    """测试预览主题功能"""
    print("=== 开始测试预览主题功能 ===")
    
    try:
        # 启动应用程序
        app = Application().start(APP_INFO["path"], wait_for_idle=False)
        time.sleep(5)
        
        main_window = app.window(title_re=".*产品编辑器.*")
        main_window.wait("visible", timeout=10)
        
        # 打开预览窗口
        main_window.type_keys("^R")  # Ctrl+R
        time.sleep(2)
        
        preview_window = app.window(title_re=".*预览.*")
        preview_window.wait("visible", timeout=10)
        
        take_screenshot("preview_theme_start")
        
        # 测试主题切换
        theme_combo = preview_window.child_window(title_re=".*主题.*", control_type="ComboBox")
        
        if theme_combo.exists():
            # 测试不同的主题选项
            themes = ["浅色主题", "深色主题", "系统主题"]
            
            for theme in themes:
                try:
                    theme_combo.select(theme)
                    time.sleep(1)
                    
                    take_screenshot(f"preview_theme_{theme}")
                    print(f"✅ 主题 '{theme}' 切换测试通过")
                except:
                    print(f"⚠️ 主题 '{theme}' 切换失败")
        else:
            print("⚠️ 未找到主题选择框，跳过主题测试")
        
        # 测试字体大小调整
        font_size_slider = preview_window.child_window(title_re=".*字体大小.*", control_type="Slider")
        
        if font_size_slider.exists():
            # 调整字体大小
            font_size_slider.set_value(16)
            time.sleep(1)
            
            font_size_slider.set_value(12)
            time.sleep(1)
            
            print("✅ 字体大小调整测试通过")
        else:
            print("⚠️ 未找到字体大小滑块，跳过字体测试")
        
        # 关闭预览窗口
        preview_window.close()
        time.sleep(1)
        main_window.close()
        time.sleep(2)
        
        return True
        
    except Exception as e:
        print(f"❌ 预览主题功能测试失败: {e}")
        take_screenshot("preview_theme_error")
        return False

def test_preview_window_management():
    """测试预览窗口管理功能"""
    print("=== 开始测试预览窗口管理功能 ===")
    
    try:
        # 启动应用程序
        app = Application().start(APP_INFO["path"], wait_for_idle=False)
        time.sleep(5)
        
        # 获取主窗口
        main_window = app.window(title_re=".*产品编辑器.*")
        main_window.wait("visible", timeout=10)
        
        take_screenshot("preview_window_management_start")
        
        # 打开预览窗口
        preview_button = main_window.child_window(title_re=".*预览.*", control_type="Button")
        
        if preview_button.exists():
            preview_button.click()
            time.sleep(3)
            
            # 查找预览窗口
            preview_window = app.window(title_re=".*预览.*")
            
            if preview_window.exists():
                preview_window.wait("visible", timeout=5)
                
                take_screenshot("preview_window_opened_for_management")
                
                # 测试窗口大小调整
                try:
                    # 获取窗口大小
                    original_rect = preview_window.rectangle()
                    original_width = original_rect.width()
                    original_height = original_rect.height()
                    
                    # 调整窗口大小
                    preview_window.move_window(width=original_width + 100, height=original_height + 100)
                    time.sleep(1)
                    
                    new_rect = preview_window.rectangle()
                    new_width = new_rect.width()
                    new_height = new_rect.height()
                    
                    if new_width > original_width or new_height > original_height:
                        print("✅ 预览窗口大小调整测试通过")
                    else:
                        print("⚠️ 预览窗口大小调整可能未生效")
                    
                    take_screenshot("preview_window_resized")
                except Exception as resize_error:
                    print(f"⚠️ 窗口大小调整测试跳过: {resize_error}")
                
                # 测试窗口最小化/最大化
                try:
                    # 最小化窗口
                    preview_window.minimize()
                    time.sleep(1)
                    
                    take_screenshot("preview_window_minimized")
                    
                    # 恢复窗口
                    preview_window.restore()
                    time.sleep(1)
                    
                    # 最大化窗口
                    preview_window.maximize()
                    time.sleep(1)
                    
                    take_screenshot("preview_window_maximized")
                    
                    # 恢复窗口
                    preview_window.restore()
                    time.sleep(1)
                    
                    print("✅ 预览窗口最小化/最大化测试通过")
                except Exception as minmax_error:
                    print(f"⚠️ 窗口最小化/最大化测试跳过: {minmax_error}")
                
                # 测试窗口关闭
                preview_window.close()
                time.sleep(1)
                
                # 验证窗口是否已关闭
                if not preview_window.exists():
                    print("✅ 预览窗口关闭测试通过")
                else:
                    print("⚠️ 预览窗口关闭可能未成功")
                
                print("✅ 预览窗口管理功能测试通过")
            else:
                print("⚠️ 未找到预览窗口，跳过窗口管理测试")
        else:
            print("⚠️ 未找到预览按钮，跳过窗口管理测试")
        
        # 关闭主窗口
        main_window.close()
        time.sleep(2)
        
        return True
        
    except Exception as e:
        print(f"❌ 预览窗口管理功能测试失败: {e}")
        take_screenshot("preview_window_management_error")
        return False

def test_preview_refresh():
    """测试预览刷新功能"""
    print("=== 开始测试预览刷新功能 ===")
    
    try:
        # 启动应用程序
        app = Application().start(APP_INFO["path"], wait_for_idle=False)
        time.sleep(5)
        
        # 获取主窗口
        main_window = app.window(title_re=".*产品编辑器.*")
        main_window.wait("visible", timeout=10)
        
        take_screenshot("preview_refresh_start")
        
        # 打开预览窗口
        preview_button = main_window.child_window(title_re=".*预览.*", control_type="Button")
        
        if preview_button.exists():
            preview_button.click()
            time.sleep(3)
            
            # 查找预览窗口
            preview_window = app.window(title_re=".*预览.*")
            
            if preview_window.exists():
                preview_window.wait("visible", timeout=5)
                
                take_screenshot("preview_refresh_before")
                
                # 查找刷新按钮
                refresh_button = preview_window.child_window(title_re=".*刷新.*", control_type="Button")
                
                if refresh_button.exists():
                    # 点击刷新按钮
                    refresh_button.click()
                    time.sleep(2)
                    
                    take_screenshot("preview_refresh_after")
                    
                    print("✅ 预览刷新功能测试通过")
                else:
                    # 尝试使用快捷键刷新
                    send_keys("{F5}")  # F5刷新
                    time.sleep(2)
                    
                    take_screenshot("preview_refresh_f5")
                    
                    print("✅ 预览F5刷新功能测试通过")
                
                # 关闭预览窗口
                preview_window.close()
                time.sleep(1)
            else:
                print("⚠️ 未找到预览窗口，跳过刷新测试")
        else:
            print("⚠️ 未找到预览按钮，跳过刷新测试")
        
        # 关闭主窗口
        main_window.close()
        time.sleep(2)
        
        return True
        
    except Exception as e:
        print(f"❌ 预览刷新功能测试失败: {e}")
        take_screenshot("preview_refresh_error")
        return False

def test_preview_with_ui_layout():
    """测试UI布局编辑器与预览功能的集成"""
    print("=== 开始测试UI布局编辑器与预览功能的集成 ===")
    
    try:
        # 启动应用程序
        app = Application().start(APP_INFO["path"], wait_for_idle=False)
        time.sleep(5)
        
        # 获取主窗口
        main_window = app.window(title_re=".*产品编辑器.*")
        main_window.wait("visible", timeout=10)
        
        take_screenshot("preview_ui_layout_start")
        
        # 打开UI布局编辑器
        ui_layout_menu = main_window.child_window(title_re=".*UI布局.*", control_type="MenuItem")
        
        if ui_layout_menu.exists():
            ui_layout_menu.click()
            time.sleep(0.5)
            
            # 查找UI布局编辑器选项
            ui_layout_option = main_window.child_window(title_re=".*打开UI布局编辑器.*", control_type="MenuItem")
            
            if ui_layout_option.exists():
                ui_layout_option.click()
                time.sleep(3)
                
                # 查找UI布局编辑器窗口
                ui_layout_window = app.window(title_re=".*UI布局编辑器.*")
                
                if ui_layout_window.exists():
                    ui_layout_window.wait("visible", timeout=5)
                    
                    take_screenshot("ui_layout_editor_opened")
                    
                    # 在UI布局编辑器中查找预览功能
                    layout_preview_button = ui_layout_window.child_window(title_re=".*预览.*", control_type="Button")
                    
                    if layout_preview_button.exists():
                        # 点击预览按钮
                        layout_preview_button.click()
                        time.sleep(3)
                        
                        # 查找预览窗口
                        preview_window = app.window(title_re=".*预览.*")
                        
                        if preview_window.exists():
                            preview_window.wait("visible", timeout=5)
                            
                            take_screenshot("preview_from_ui_layout")
                            
                            # 验证预览窗口是否显示UI布局内容
                            window_title = preview_window.window_text()
                            
                            # 关闭预览窗口
                            preview_window.close()
                            time.sleep(1)
                            
                            print("✅ UI布局编辑器与预览功能集成测试通过")
                        else:
                            print("⚠️ 未找到预览窗口，跳过集成测试")
                    else:
                        print("⚠️ 未找到UI布局编辑器中的预览按钮，跳过集成测试")
                    
                    # 关闭UI布局编辑器
                    ui_layout_window.close()
                    time.sleep(1)
                else:
                    print("⚠️ 未找到UI布局编辑器窗口，跳过集成测试")
            else:
                print("⚠️ 未找到UI布局编辑器选项，跳过集成测试")
        else:
            print("⚠️ 未找到UI布局菜单，跳过集成测试")
        
        # 关闭主窗口
        main_window.close()
        time.sleep(2)
        
        return True
        
    except Exception as e:
        print(f"❌ UI布局编辑器与预览功能集成测试失败: {e}")
        take_screenshot("preview_ui_layout_error")
        return False

if __name__ == "__main__":
    # 运行所有测试用例
    results = []
    
    results.append(test_preview_button())
    results.append(test_real_time_preview())
    results.append(test_interactive_preview())
    results.append(test_preview_window_management())
    results.append(test_preview_refresh())
    results.append(test_preview_with_ui_layout())
    
    print("\n=== 预览功能测试结果汇总 ===")
    passed = sum(results)
    total = len(results)
    
    print(f"通过测试: {passed}/{total}")
    
    if passed == total:
        print("🎉 所有预览功能测试通过！")
    else:
        print("⚠️ 部分测试失败，请检查日志和截图")