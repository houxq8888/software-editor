"""
产品编辑器功能特性TAB页测试用例
测试功能特性的添加、编辑、删除以及JSON序列化功能
"""

import os
import time
import json
import pyautogui
import pyperclip
from datetime import datetime
from pywinauto import Application
from pywinauto.findwindows import ElementNotFoundError

def paste_text_with_clipboard(text):
    """使用剪贴板粘贴文本的辅助函数"""
    try:
        # 保存原始剪贴板内容
        original_clipboard = pyperclip.paste()
        
        # 复制文本到剪贴板
        pyperclip.copy(text)
        time.sleep(0.5)
        
        # 使用Ctrl+V粘贴
        pyautogui.hotkey('ctrl', 'v')
        time.sleep(0.5)
        
        # 恢复原始剪贴板内容
        pyperclip.copy(original_clipboard)
        
        print(f"[INFO] 使用剪贴板粘贴文本: {text}")
    except Exception as e:
        print(f"[ERROR] 剪贴板粘贴失败: {e}")
        # 如果剪贴板失败，回退到直接输入
        pyautogui.write(text)

def get_test_results_dir():
    """获取测试结果目录"""
    today = datetime.now().strftime("%Y-%m-%d")
    results_dir = os.path.join(os.path.dirname(__file__), "test_results", today)
    os.makedirs(results_dir, exist_ok=True)
    return results_dir

def test_product_features():
    """测试产品功能特性功能"""
    
    # 测试数据 - 功能特性
    test_features = [
        {"name": "用户管理", "description": "支持用户注册、登录、权限管理功能"},
        {"name": "数据导出", "description": "支持将数据导出为Excel、PDF格式"},
        {"name": "报表生成", "description": "自动生成各类统计报表和图表"},
        {"name": "系统设置", "description": "提供系统参数配置和个性化设置"},
        {"name": "数据备份", "description": "自动定时备份数据，支持恢复功能"}
    ]
    
    # 启动软件编辑器
    app_path = os.path.join(os.path.dirname(os.path.dirname(os.path.dirname(__file__))), "build", "software-editor.exe")
    
    if not os.path.exists(app_path):
        print(f"[ERROR] 应用程序不存在: {app_path}")
        return False
    
    print("=== 启动软件编辑器 ===")
    # 使用subprocess启动应用程序，避免GUI进程等待问题
    import subprocess
    process = subprocess.Popen([app_path])
    time.sleep(8)  # 增加等待时间，确保应用程序完全启动
    
    # 连接到已启动的应用程序
    try:
        app = Application().connect(path=app_path)
        time.sleep(2)
    except Exception as e:
        print(f"[WARNING] 连接应用程序失败: {e}")
        print("[INFO] 尝试使用窗口标题连接")
        app = Application().connect(title_re=".*产品编辑器.*")
        time.sleep(2)
    
    try:
        # 连接到应用程序
        dlg = app.window(title_re=".*产品编辑器.*")
        dlg.wait("visible", timeout=20)  # 增加超时时间
        
        print("[SUCCESS] 软件编辑器启动成功")
        
        # 切换到功能特性TAB页
        print("=== 切换到功能特性TAB页 ===")
        try:
            # 尝试通过自动化ID定位TAB页
            features_tab = dlg.child_window(auto_id="QApplication.MainWindow.centralWidget.tabWidget.qt_tabwidget_stackedwidget.featuresTab", control_type="Unknown")
            features_tab.click_input()
            print("[SUCCESS] 切换到功能特性TAB页")
        except Exception as e:
            print(f"[WARNING] 自动化ID定位TAB页失败: {e}")
            # 使用控件位置点击第二个TAB页
            try:
                tab_widget = dlg.child_window(auto_id="QApplication.MainWindow.centralWidget.tabWidget", control_type="Unknown")
                rect = tab_widget.rectangle()
                # 计算第二个TAB页的位置（假设TAB页宽度为100px）
                tab2_x = rect.left + 150  # 第二个TAB页的X坐标
                tab_y = rect.top + 20    # TAB页的Y坐标
                pyautogui.click(tab2_x, tab_y)
                time.sleep(1)
                print("[INFO] 使用控件位置切换到功能特性TAB页")
            except Exception as e2:
                print(f"[ERROR] 控件位置定位TAB页失败: {e2}")
                print("[WARNING] 无法切换到功能特性TAB页")
        
        # 添加功能特性
        print("=== 开始添加功能特性 ===")
        for i, feature in enumerate(test_features):
            print(f"步骤{i+1}: 添加功能特性 - {feature['name']}")
            
            # 输入功能名称
            try:
                feature_name_edit = dlg.child_window(auto_id="QApplication.MainWindow.centralWidget.tabWidget.qt_tabwidget_stackedwidget.featuresTab.groupBox_4.featureNameLineEdit", control_type="Unknown")
                feature_name_edit.click_input()
                feature_name_edit.set_text(feature['name'])
                time.sleep(0.5)
            except Exception as e:
                print(f"功能名称输入失败: {e}")
                # 使用控件位置点击输入框
                try:
                    feature_name_edit = dlg.child_window(auto_id="QApplication.MainWindow.centralWidget.tabWidget.qt_tabwidget_stackedwidget.featuresTab.groupBox_4.featureNameLineEdit", control_type="Unknown")
                    rect = feature_name_edit.rectangle()
                    center_x = rect.left + (rect.width() // 2)
                    center_y = rect.top + (rect.height() // 2)
                    pyautogui.click(center_x, center_y)
                    pyautogui.hotkey('ctrl', 'a')
                    paste_text_with_clipboard(feature['name'])
                    time.sleep(0.5)
                    print("[INFO] 使用控件位置输入功能名称")
                except Exception as e2:
                    print(f"[ERROR] 控件位置定位功能名称输入框失败: {e2}")
            
            # 输入功能描述
            try:
                feature_desc_edit = dlg.child_window(auto_id="QApplication.MainWindow.centralWidget.tabWidget.qt_tabwidget_stackedwidget.featuresTab.groupBox_4.featureDescriptionTextEdit", control_type="Unknown")
                feature_desc_edit.click_input()
                feature_desc_edit.set_text(feature['description'])
                time.sleep(0.5)
            except Exception as e:
                print(f"功能描述输入失败: {e}")
                # 使用控件位置点击输入框
                try:
                    feature_desc_edit = dlg.child_window(auto_id="QApplication.MainWindow.centralWidget.tabWidget.qt_tabwidget_stackedwidget.featuresTab.groupBox_4.featureDescriptionTextEdit", control_type="Unknown")
                    rect = feature_desc_edit.rectangle()
                    center_x = rect.left + (rect.width() // 2)
                    center_y = rect.top + (rect.height() // 2)
                    pyautogui.click(center_x, center_y)
                    pyautogui.hotkey('ctrl', 'a')
                    paste_text_with_clipboard(feature['description'])
                    time.sleep(0.5)
                    print("[INFO] 使用控件位置输入功能描述")
                except Exception as e2:
                    print(f"[ERROR] 控件位置定位功能描述输入框失败: {e2}")
            
            # 点击添加按钮
            try:
                add_button = dlg.child_window(auto_id="QApplication.MainWindow.centralWidget.tabWidget.qt_tabwidget_stackedwidget.featuresTab.groupBox_4.addFeatureButton", control_type="Unknown")
                add_button.click_input()
                time.sleep(1)
                print(f"[SUCCESS] 添加功能特性: {feature['name']}")
            except Exception as e:
                print(f"添加按钮点击失败: {e}")
                # 使用控件位置点击添加按钮
                try:
                    add_button = dlg.child_window(auto_id="QApplication.MainWindow.centralWidget.tabWidget.qt_tabwidget_stackedwidget.featuresTab.groupBox_4.addFeatureButton", control_type="Unknown")
                    rect = add_button.rectangle()
                    center_x = rect.left + (rect.width() // 2)
                    center_y = rect.top + (rect.height() // 2)
                    pyautogui.click(center_x, center_y)
                    time.sleep(1)
                    print(f"[INFO] 使用控件位置添加功能特性: {feature['name']}")
                except Exception as e2:
                    print(f"[ERROR] 控件位置定位添加按钮失败: {e2}")
        
        # 验证功能特性列表
        print("=== 验证功能特性列表 ===")
        try:
            features_list = dlg.child_window(auto_id="QApplication.MainWindow.centralWidget.tabWidget.qt_tabwidget_stackedwidget.featuresTab.featuresListWidget", control_type="Unknown")
            items_count = features_list.item_count()
            print(f"功能特性列表项数: {items_count}")
            
            if items_count == len(test_features):
                print("[SUCCESS] 功能特性添加成功，列表项数正确")
            else:
                print(f"[WARNING] 功能特性列表项数不正确，期望{len(test_features)}，实际{items_count}")
        except Exception as e:
            print(f"[ERROR] 验证功能特性列表失败: {e}")
        
        # 测试删除功能特性
        print("=== 测试删除功能特性 ===")
        try:
            # 选择第一个功能特性
            features_list = dlg.child_window(auto_id="QApplication.MainWindow.centralWidget.tabWidget.qt_tabwidget_stackedwidget.featuresTab.featuresListWidget", control_type="Unknown")
            features_list.select(0)
            time.sleep(0.5)
            
            # 点击删除按钮
            remove_button = dlg.child_window(auto_id="QApplication.MainWindow.centralWidget.tabWidget.qt_tabwidget_stackedwidget.featuresTab.groupBox_4.removeFeatureButton", control_type="Unknown")
            remove_button.click_input()
            time.sleep(1)
            
            # 验证删除结果
            new_count = features_list.item_count()
            if new_count == len(test_features) - 1:
                print("[SUCCESS] 功能特性删除成功")
            else:
                print(f"[WARNING] 删除后列表项数不正确，期望{len(test_features)-1}，实际{new_count}")
        except Exception as e:
            print(f"[ERROR] 删除功能特性测试失败: {e}")
        
        # 保存产品信息
        print("=== 保存产品信息 ===")
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        filename = f"test_product_features_{timestamp}.json"
        results_dir = get_test_results_dir()
        filepath = os.path.join(results_dir, filename)
        
        try:
            save_button = dlg.child_window(auto_id="QApplication.MainWindow.centralWidget.groupBox_5.saveButton", control_type="Unknown")
            save_button.click_input()
            time.sleep(2)
            
            # 处理保存对话框
            save_dlg = app.window(title_re=".*保存.*")
            if save_dlg.exists():
                # 输入文件名
                filename_edit = save_dlg.child_window(auto_id="FileNameControlHost", control_type="Edit")
                filename_edit.set_text(filepath)
                time.sleep(1)
                
                # 点击保存按钮
                save_confirm_button = save_dlg.child_window(title="保存", control_type="Button")
                save_confirm_button.click_input()
                time.sleep(2)
                
                print(f"[SUCCESS] 产品信息保存成功: {filepath}")
            else:
                print("[INFO] 未检测到保存对话框，可能已直接保存")
        except Exception as e:
            print(f"[ERROR] 保存操作失败: {e}")
        
        # 验证JSON文件内容
        print("=== 验证JSON文件内容 ===")
        if os.path.exists(filepath):
            with open(filepath, 'r', encoding='utf-8') as f:
                product_data = json.load(f)
            
            # 检查功能特性字段
            if "features" in product_data and isinstance(product_data["features"], list):
                features_count = len(product_data["features"])
                print(f"JSON文件中功能特性数量: {features_count}")
                
                if features_count == len(test_features) - 1:  # 因为删除了一个
                    print("[SUCCESS] JSON文件功能特性数量正确")
                else:
                    print(f"[WARNING] JSON文件功能特性数量不正确，期望{len(test_features)-1}，实际{features_count}")
                
                # 打印功能特性详情
                for i, feature in enumerate(product_data["features"]):
                    print(f"功能特性{i+1}: {feature.get('name', 'N/A')} - {feature.get('description', 'N/A')}")
            else:
                print("[ERROR] JSON文件中缺少features字段或格式不正确")
        else:
            print("[ERROR] 保存的文件不存在")
        
        # 关闭应用程序
        print("=== 关闭软件编辑器 ===")
        dlg.close()
        time.sleep(2)
        
        return True
        
    except Exception as e:
        print(f"[ERROR] 测试过程中出现异常: {e}")
        return False

if __name__ == "__main__":
    success = test_product_features()
    if success:
        print("\\n=== 功能特性测试完成 ===")
        print("所有功能特性相关测试用例执行完毕")
    else:
        print("\\n=== 功能特性测试失败 ===")
        print("部分测试用例执行失败，请检查日志")
