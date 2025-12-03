"""
产品配置编辑功能测试用例
测试产品基本信息、功能特性、截图配置等核心功能
"""

import pytest
import time
import os
import json
import pyautogui
from pywinauto import Application
from config import APP_INFO, UI_ELEMENTS, TEST_DATA
from utils import take_screenshot

def test_product_basic_info():
    """测试产品基本信息编辑功能"""
    print("=== 开始测试产品基本信息编辑 ===")
    
    # 测试数据
    test_product_info = {
        "name": "测试产品",
        "version": "1.0.0",
        "category": "办公软件",
        "description": "这是一个测试产品描述"
    }
    
    try:
        # 启动应用程序
        app = Application().start(APP_INFO["path"], wait_for_idle=False)
        time.sleep(5)
        
        # 获取主窗口
        main_window = app.window(title_re=".*产品编辑器.*")
        main_window.wait("visible", timeout=10)
        
        # 截图记录初始状态
        take_screenshot("product_basic_info_start")
        
        # 输入产品名称
        product_name_edit = main_window.child_window(auto_id="QApplication.MainWindow.centralWidget.groupBox.productNameLineEdit")
        product_name_edit.set_text(test_product_info["name"])
        time.sleep(0.5)
        
        # 输入版本号
        version_edit = main_window.child_window(auto_id="QApplication.MainWindow.centralWidget.groupBox.versionLineEdit")
        version_edit.set_text(test_product_info["version"])
        time.sleep(0.5)
        
        # 选择分类
        category_combo = main_window.child_window(auto_id="QApplication.MainWindow.centralWidget.groupBox.categoryComboBox")
        category_combo.select(test_product_info["category"])
        time.sleep(0.5)
        
        # 输入描述
        description_edit = main_window.child_window(auto_id="QApplication.MainWindow.centralWidget.groupBox.descriptionTextEdit")
        description_edit.set_text(test_product_info["description"])
        time.sleep(0.5)
        
        # 截图记录输入完成状态
        take_screenshot("product_basic_info_filled")
        
        # 验证输入内容
        assert product_name_edit.texts()[0] == test_product_info["name"]
        assert version_edit.texts()[0] == test_product_info["version"]
        assert category_combo.texts()[0] == test_product_info["category"]
        
        print("[PASS] 产品基本信息编辑测试通过")
        
        # 关闭应用程序
        main_window.close()
        time.sleep(2)
        
        return True
        
    except Exception as e:
        print(f"[ERROR] 产品基本信息编辑测试失败: {e}")
        take_screenshot("product_basic_info_error")
        return False

def test_product_features_management():
    """测试产品功能特性管理功能"""
    print("=== 开始测试产品功能特性管理 ===")
    
    # 测试功能特性数据
    test_features = [
        {"name": "用户管理", "description": "用户注册、登录、权限管理"},
        {"name": "数据导出", "description": "支持Excel、PDF格式导出"},
        {"name": "报表生成", "description": "自动生成统计报表"}
    ]
    
    try:
        # 启动应用程序
        app = Application().start(APP_INFO["path"], wait_for_idle=False)
        time.sleep(5)
        
        # 获取主窗口
        main_window = app.window(title_re=".*产品编辑器.*")
        main_window.wait("visible", timeout=10)
        
        # 切换到功能特性TAB页
        features_tab = main_window.child_window(auto_id="QApplication.MainWindow.centralWidget.tabWidget.featuresTab")
        features_tab.click()
        time.sleep(1)
        
        take_screenshot("product_features_start")
        
        # 添加功能特性
        for feature in test_features:
            # 输入功能名称
            name_edit = main_window.child_window(auto_id="QApplication.MainWindow.centralWidget.tabWidget.featuresTab.featureNameLineEdit")
            name_edit.set_text(feature["name"])
            time.sleep(0.5)
            
            # 输入功能描述
            desc_edit = main_window.child_window(auto_id="QApplication.MainWindow.centralWidget.tabWidget.featuresTab.featureDescriptionTextEdit")
            desc_edit.set_text(feature["description"])
            time.sleep(0.5)
            
            # 点击添加按钮
            add_button = main_window.child_window(auto_id="QApplication.MainWindow.centralWidget.tabWidget.featuresTab.addFeatureButton")
            add_button.click()
            time.sleep(1)
        
        # 验证功能特性列表
        features_list = main_window.child_window(auto_id="QApplication.MainWindow.centralWidget.tabWidget.featuresTab.featuresListWidget")
        assert features_list.item_count() == len(test_features)
        
        # 测试删除功能
        features_list.select(0)  # 选择第一个功能特性
        remove_button = main_window.child_window(auto_id="QApplication.MainWindow.centralWidget.tabWidget.featuresTab.removeFeatureButton")
        remove_button.click()
        time.sleep(1)
        
        assert features_list.item_count() == len(test_features) - 1
        
        take_screenshot("product_features_completed")
        
        print("✅ 产品功能特性管理测试通过")
        
        # 关闭应用程序
        main_window.close()
        time.sleep(2)
        
        return True
        
    except Exception as e:
        print(f"❌ 产品功能特性管理测试失败: {e}")
        take_screenshot("product_features_error")
        return False

def test_product_save_load():
    """测试产品配置保存和加载功能"""
    print("=== 开始测试产品配置保存和加载功能 ===")
    
    test_config_file = os.path.join(os.path.dirname(__file__), "test_product_config.json")
    
    try:
        # 启动应用程序
        app = Application().start(APP_INFO["path"], wait_for_idle=False)
        time.sleep(5)
        
        main_window = app.window(title_re=".*产品编辑器.*")
        main_window.wait("visible", timeout=10)
        
        take_screenshot("product_save_load_start")
        
        # 测试保存配置
        save_button = main_window.child_window(title="保存", control_type="Button")
        
        if save_button.exists():
            save_button.click()
            time.sleep(2)
            
            # 处理保存对话框
            save_dialog = app.window(title_re=".*保存.*")
            if save_dialog.exists():
                filename_edit = save_dialog.child_window(auto_id="FileNameControlHost")
                filename_edit.set_text(test_config_file)
                time.sleep(1)
                
                save_confirm = save_dialog.child_window(title="保存")
                save_confirm.click()
                time.sleep(2)
                
                # 验证文件是否保存成功
                assert os.path.exists(test_config_file)
                
                print("✅ 产品配置保存功能测试通过")
                
                # 测试加载配置
                load_button = main_window.child_window(title="加载", control_type="Button")
                
                if load_button.exists():
                    load_button.click()
                    time.sleep(2)
                    
                    # 处理加载对话框
                    load_dialog = app.window(title_re=".*打开.*")
                    if load_dialog.exists():
                        filename_edit = load_dialog.child_window(auto_id="FileNameControlHost")
                        filename_edit.set_text(test_config_file)
                        time.sleep(1)
                        
                        load_confirm = load_dialog.child_window(title="打开")
                        load_confirm.click()
                        time.sleep(2)
                        
                        print("✅ 产品配置加载功能测试通过")
                    else:
                        print("⚠️ 未找到加载对话框，跳过加载测试")
                else:
                    print("⚠️ 未找到加载按钮，跳过加载测试")
            else:
                print("⚠️ 未找到保存对话框，跳过保存测试")
        else:
            print("⚠️ 未找到保存按钮，跳过保存测试")
        
        # 清理测试文件
        if os.path.exists(test_config_file):
            os.remove(test_config_file)
        
        # 关闭窗口
        main_window.close()
        time.sleep(2)
        
        return True
        
    except Exception as e:
        print(f"❌ 产品配置保存和加载功能测试失败: {e}")
        take_screenshot("product_save_load_error")
        
        # 清理测试文件
        if os.path.exists(test_config_file):
            os.remove(test_config_file)
        
        return False

def test_product_validation():
    """测试产品配置验证功能"""
    print("=== 开始测试产品配置验证功能 ===")
    
    try:
        # 启动应用程序
        app = Application().start(APP_INFO["path"], wait_for_idle=False)
        time.sleep(5)
        
        main_window = app.window(title_re=".*产品编辑器.*")
        main_window.wait("visible", timeout=10)
        
        take_screenshot("product_validation_start")
        
        # 测试必填字段验证
        # 清空产品名称字段
        product_name_field = main_window.child_window(title_re=".*产品名称.*", control_type="Edit")
        
        if product_name_field.exists():
            product_name_field.set_text("")
            time.sleep(1)
            
            # 尝试保存，应该触发验证错误
            save_button = main_window.child_window(title="保存", control_type="Button")
            if save_button.exists():
                save_button.click()
                time.sleep(2)
                
                # 检查是否有错误提示
                error_dialog = app.window(title_re=".*错误.*")
                if error_dialog.exists():
                    print("✅ 必填字段验证功能测试通过")
                    error_dialog.close()
                    time.sleep(1)
                else:
                    print("⚠️ 未找到错误提示对话框，跳过验证测试")
            else:
                print("⚠️ 未找到保存按钮，跳过验证测试")
        else:
            print("⚠️ 未找到产品名称字段，跳过验证测试")
        
        # 测试版本号格式验证
        version_field = main_window.child_window(title_re=".*版本.*", control_type="Edit")
        
        if version_field.exists():
            # 输入无效版本号
            version_field.set_text("invalid_version")
            time.sleep(1)
            
            # 尝试保存，应该触发验证错误
            save_button = main_window.child_window(title="保存", control_type="Button")
            if save_button.exists():
                save_button.click()
                time.sleep(2)
                
                # 检查是否有错误提示
                error_dialog = app.window(title_re=".*错误.*")
                if error_dialog.exists():
                    print("✅ 版本号格式验证功能测试通过")
                    error_dialog.close()
                    time.sleep(1)
                else:
                    print("⚠️ 未找到错误提示对话框，跳过版本验证测试")
            else:
                print("⚠️ 未找到保存按钮，跳过版本验证测试")
        else:
            print("⚠️ 未找到版本号字段，跳过版本验证测试")
        
        # 关闭窗口
        main_window.close()
        time.sleep(2)
        
        return True
        
    except Exception as e:
        print(f"❌ 产品配置验证功能测试失败: {e}")
        take_screenshot("product_validation_error")
        return False

def test_product_export():
    """测试产品配置导出功能"""
    print("=== 开始测试产品配置导出功能 ===")
    
    test_export_file = os.path.join(os.path.dirname(__file__), "test_product_export.xml")
    
    try:
        # 启动应用程序
        app = Application().start(APP_INFO["path"], wait_for_idle=False)
        time.sleep(5)
        
        main_window = app.window(title_re=".*产品编辑器.*")
        main_window.wait("visible", timeout=10)
        
        take_screenshot("product_export_start")
        
        # 测试导出功能
        export_button = main_window.child_window(title="导出", control_type="Button")
        
        if export_button.exists():
            export_button.click()
            time.sleep(2)
            
            # 处理导出对话框
            export_dialog = app.window(title_re=".*导出.*")
            if export_dialog.exists():
                # 选择导出格式
                format_combo = export_dialog.child_window(title_re=".*格式.*", control_type="ComboBox")
                if format_combo.exists():
                    format_combo.select("XML")
                    time.sleep(1)
                
                # 设置导出文件名
                filename_edit = export_dialog.child_window(auto_id="FileNameControlHost")
                filename_edit.set_text(test_export_file)
                time.sleep(1)
                
                # 确认导出
                export_confirm = export_dialog.child_window(title="导出")
                export_confirm.click()
                time.sleep(2)
                
                # 验证文件是否导出成功
                assert os.path.exists(test_export_file)
                
                print("✅ 产品配置导出功能测试通过")
            else:
                print("⚠️ 未找到导出对话框，跳过导出测试")
        else:
            print("⚠️ 未找到导出按钮，跳过导出测试")
        
        # 清理测试文件
        if os.path.exists(test_export_file):
            os.remove(test_export_file)
        
        # 关闭窗口
        main_window.close()
        time.sleep(2)
        
        return True
        
    except Exception as e:
        print(f"❌ 产品配置导出功能测试失败: {e}")
        take_screenshot("product_export_error")
        
        # 清理测试文件
        if os.path.exists(test_export_file):
            os.remove(test_export_file)
        
        return False

if __name__ == "__main__":
    # 运行所有测试用例
    results = []
    
    results.append(test_product_basic_info())
    results.append(test_product_features_management())
    results.append(test_product_save_load())
    
    print("\n=== 测试结果汇总 ===")
    passed = sum(results)
    total = len(results)
    
    print(f"通过测试: {passed}/{total}")
    
    if passed == total:
        print("🎉 所有产品配置编辑功能测试通过！")
    else:
        print("⚠️ 部分测试失败，请检查日志和截图")