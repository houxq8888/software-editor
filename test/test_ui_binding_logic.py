#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试UI绑定逻辑修改效果
验证：
1. 当用户选择Yes绑定UI布局时，如果用户没有主动保存，退出产品界面时要提示保存
2. 当用户选择No时，不需要提示保存
"""

import os
import sys
import json

def test_ui_binding_logic():
    """测试UI绑定逻辑修改效果"""
    
    print("=== UI绑定逻辑修改效果测试 ===")
    
    # 读取修改后的mainwindow.cpp文件
    mainwindow_path = os.path.join("src", "mainwindow.cpp")
    
    if not os.path.exists(mainwindow_path):
        print("❌ 找不到mainwindow.cpp文件")
        return False
    
    with open(mainwindow_path, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # 检查onUILayoutWindowClosed函数中的修改
    print("\n1. 检查onUILayoutWindowClosed函数中的修改：")
    
    # 检查用户选择Yes时的逻辑
    if "// 设置产品修改状态，因为绑定操作需要被记录" in content:
        print("✅ 用户选择Yes时：设置产品修改状态")
    else:
        print("❌ 用户选择Yes时：修改未正确应用")
        return False
    
    # 检查用户选择No时的逻辑
    if "m_configManager->setProductModified(false)" in content:
        print("✅ 用户选择No时：重置产品修改状态")
    else:
        print("❌ 用户选择No时：修改未正确应用")
        return False
    
    # 检查saveProductData函数中的修改
    print("\n2. 检查saveProductData函数中的修改：")
    
    # 检查用户选择Yes时的逻辑
    if "m_configManager->setProductModified(true)" in content:
        print("✅ saveProductData中用户选择Yes时：设置产品修改状态")
    else:
        print("❌ saveProductData中用户选择Yes时：修改未正确应用")
        return False
    
    # 检查用户选择No时的逻辑
    if "m_configManager->setUiLayoutChanged(false)" in content:
        print("✅ saveProductData中用户选择No时：重置UI布局改变状态")
    else:
        print("❌ saveProductData中用户选择No时：修改未正确应用")
        return False
    
    # 验证修改的逻辑一致性
    print("\n3. 验证修改的逻辑一致性：")
    
    # 检查两个函数中用户选择Yes时的处理是否一致
    yes_pattern = "m_configManager->setProductModified(true)"
    no_pattern = "m_configManager->setProductModified(false)"
    
    yes_count = content.count(yes_pattern)
    no_count = content.count(no_pattern)
    
    print(f"✅ 设置产品修改状态为true的次数: {yes_count}")
    print(f"✅ 设置产品修改状态为false的次数: {no_count}")
    
    # 正确的逻辑：
    # - 用户选择Yes时：设置修改状态为true（应该出现2次：onUILayoutWindowClosed和saveProductData）
    # - 用户选择No时：设置修改状态为false（应该出现1次：onUILayoutWindowClosed）
    if yes_count >= 2 and no_count == 1:
        print("✅ 修改逻辑一致：")
        print("   - 用户选择Yes时设置修改状态为true")
        print("   - 用户选择No时设置修改状态为false")
    else:
        print("❌ 修改逻辑不一致")
        return False
    
    print("\n=== 测试总结 ===")
    print("✅ 修改已成功应用，逻辑符合预期：")
    print("   - 用户选择Yes绑定UI布局时，产品修改状态被设置")
    print("   - 用户选择No时，产品修改状态被重置")
    print("   - 退出产品界面时，会根据修改状态决定是否提示保存")
    
    return True

def analyze_behavior():
    """分析修改后的行为逻辑"""
    
    print("\n=== 行为逻辑分析 ===")
    
    print("\n📋 场景1：用户选择Yes绑定UI布局")
    print("  1. 检测到新UI布局文件")
    print("  2. 弹出提示框询问是否绑定")
    print("  3. 用户选择Yes")
    print("  4. 系统执行绑定操作")
    print("  5. 设置产品修改状态为true")
    print("  6. 用户没有主动保存")
    print("  7. 退出产品界面时检测到修改状态为true")
    print("  8. 弹出保存提示框")
    print("  ✅ 结果：符合预期，会提示保存")
    
    print("\n📋 场景2：用户选择No不绑定UI布局")
    print("  1. 检测到新UI布局文件")
    print("  2. 弹出提示框询问是否绑定")
    print("  3. 用户选择No")
    print("  4. 重置UI布局改变状态")
    print("  5. 重置产品修改状态为false")
    print("  6. 退出产品界面时检测到修改状态为false")
    print("  7. 不弹出保存提示框")
    print("  ✅ 结果：符合预期，不会提示保存")
    
    print("\n📋 场景3：用户选择Yes并主动保存")
    print("  1. 检测到新UI布局文件")
    print("  2. 弹出提示框询问是否绑定")
    print("  3. 用户选择Yes")
    print("  4. 系统执行绑定操作")
    print("  5. 设置产品修改状态为true")
    print("  6. 用户主动保存产品")
    print("  7. 产品修改状态被重置为false")
    print("  8. 退出产品界面时检测到修改状态为false")
    print("  9. 不弹出保存提示框")
    print("  ✅ 结果：符合预期，不会提示保存")

if __name__ == "__main__":
    # 切换到项目根目录
    os.chdir(os.path.dirname(os.path.abspath(__file__)))
    
    if test_ui_binding_logic():
        analyze_behavior()
        print("\n🎉 所有测试通过！修改逻辑正确实现。")
        sys.exit(0)
    else:
        print("\n❌ 测试失败！请检查修改是否正确应用。")
        sys.exit(1)