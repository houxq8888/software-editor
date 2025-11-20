import pytest
import pyautogui
import time
import os
from pywinauto import Application
from pywinauto.findwindows import find_window
from config import APP_INFO, UI_ELEMENTS, TEST_DATA
from utils import take_screenshot

def setup_module():
    """在模块开始前执行的设置"""
    global app, main_window
    
    print("连接到已运行的应用程序...")
    # 连接到已运行的应用程序
    app = Application().connect(title=APP_INFO["window_title"])
    
    # 获取主窗口句柄
    main_window_handle = find_window(title=APP_INFO["window_title"])
    main_window = app.window(handle=main_window_handle)
    
    # 激活主窗口
    main_window.set_focus()
    time.sleep(1)
    
    # 打开UI布局编辑器
    print("打开UI布局编辑器...")
    # 点击文件菜单
    main_window.menu_select("文件->打开UI布局编辑器")
    time.sleep(2)

def teardown_module():
    """在模块结束后执行的清理"""
    print("关闭应用程序...")
    # 关闭应用程序
    pyautogui.hotkey("alt", "f4")
    time.sleep(2)

def test_main_window_title():
    """测试主窗口标题是否正确"""
    # 获取当前活动窗口
    active_window = pyautogui.getActiveWindow()
    assert active_window is not None, "没有找到活动窗口"
    assert APP_INFO["window_title"] in active_window.title, f"窗口标题不正确，应该包含 '{APP_INFO['window_title']}'"

def test_button_click():
    """测试按钮点击功能"""
    # 获取UI布局编辑器窗口
    uilayout_window = app.window(title="UI布局编辑器")
    uilayout_window.wait('visible')
    
    # 获取OK按钮控件
    ok_button = uilayout_window.child_window(name="okButton", control_type="Button")
    
    # 点击OK按钮
    ok_button.click()
    
    # 等待操作完成
    time.sleep(1)
    
    # 拍摄截图
    take_screenshot("ok_button_click")
    
    # 验证操作结果，这里需要根据实际功能调整
    # 例如：检查是否打开了新窗口或是否有其他变化
    
def test_splitter_resize():
    """测试分隔条拖动功能"""
    print("测试分隔条拖动功能...")
    
    # 获取UI布局编辑器窗口
    uilayout_window = app.window(title="UI布局编辑器")
    uilayout_window.wait('visible')
    
    # 获取QSplitter控件
    splitter = uilayout_window.child_window(name="splitter", control_type="Pane")
    
    # 获取当前分隔条位置
    current_position = splitter.rectangle().left + splitter.rectangle().width()
    
    # 向右拖动分隔条100像素
    splitter.drag_mouse_input((current_position + 100, splitter.rectangle().top + 350), button='left')
    time.sleep(1)
    take_screenshot("splitter_resized")
    
    # 向左拖动分隔条100像素回到初始位置
    splitter.drag_mouse_input((current_position, splitter.rectangle().top + 350), button='left')
    time.sleep(1)
    take_screenshot("splitter_reset")

def test_drag_label():
    """测试拖动QLabel到编辑区并编辑文本"""
    # 获取UI布局编辑器窗口
    uilayout_window = app.window(title="UI布局编辑器")
    uilayout_window.wait('visible')
    
    # 获取CustomTreeWidget控件
    widget_list = uilayout_window.child_window(name="widgetListWidget", control_type="Tree")
    
    # 获取QLabel节点
    qlabel_item = widget_list.child_window(name="QLabel", control_type="TreeItem")
    
    # 获取编辑区
    graphics_view = uilayout_window.child_window(name="graphicsView", control_type="Pane")
    
    # 拖动QLabel到编辑区
    qlabel_item.drag_mouse_input(graphics_view.rectangle().center(), button='left')
    time.sleep(2)
    take_screenshot("drag_label")
    
    # 双击编辑QLabel文本
    # 这里假设拖入编辑区的QLabel会自动获得一个ID，需要根据实际情况调整
    label_in_view = uilayout_window.child_window(name="label_1", control_type="Text")
    label_in_view.double_click()
    time.sleep(1)
    pyautogui.typewrite("测试文本", interval=0.1)  # 输入新文本
    time.sleep(2)
    take_screenshot("edit_label")
    
    # 验证文本编辑功能，这里需要根据实际功能调整
    # 例如：检查QLabel的文本是否已经更新

def test_drag_tabwidget():
    """测试拖动QTabWidget控件到编辑区并编辑标题"""
    print("测试拖动QTabWidget控件到编辑区...")
    
    # 获取UI布局编辑器窗口
    uilayout_window = app.window(title="UI布局编辑器")
    uilayout_window.wait('visible')
    
    # 获取CustomTreeWidget控件
    widget_list = uilayout_window.child_window(name="widgetListWidget", control_type="Tree")
    
    # 获取QTabWidget节点
    tabwidget_item = widget_list.child_window(name="QTabWidget", control_type="TreeItem")
    
    # 获取编辑区
    graphics_view = uilayout_window.child_window(name="graphicsView", control_type="Pane")
    
    # 拖动QTabWidget到编辑区
    tabwidget_item.drag_mouse_input(graphics_view.rectangle().center(), button='left')
    time.sleep(2)
    take_screenshot("tabwidget_dragged")
    
    # 双击编辑QTabWidget的第一个标题
    # 这里假设拖入编辑区的QTabWidget会自动获得一个ID，需要根据实际情况调整
    tabwidget_in_view = uilayout_window.child_window(name="tabwidget_1", control_type="Pane")
    # 假设第一个Tab页标题有特定的控件ID
    tab_title = tabwidget_in_view.child_window(name="tab_title_1", control_type="Text")
    tab_title.double_click()
    time.sleep(1)
    pyautogui.typewrite("Hello QTabWidget", interval=0.1)  # 输入新标题
    time.sleep(2)
    take_screenshot("tabwidget_title_edited")
    
    # 验证Tab页标题编辑功能，这里需要根据实际功能调整
    # 例如：检查Tab页的标题是否已经更新

def test_preview_function():
    """测试预览功能"""
    print("测试预览功能...")
    
    # 获取UI布局编辑器窗口
    uilayout_window = app.window(title="UI布局编辑器")
    uilayout_window.wait('visible')
    
    # 使用菜单选择预览功能 (actionPreview)
    uilayout_window.menu_select("编辑->预览")
    time.sleep(2)
    take_screenshot("preview_opened")
    
    # 获取预览窗口
    preview_window = app.window(title="预览窗口")
    preview_window.wait('visible')
    
    # 关闭预览窗口
    preview_window.close()
    time.sleep(1)

if __name__ == "__main__":
    pytest.main(["-v", "test_ui.py"])