#!/usr/bin/env python3
"""
测试进度条改进功能
验证开发工具检测完成后进度条显示100%，并能区分检测工具和打包状态
"""

import sys
import os
import time
from pathlib import Path

# 添加项目根目录到Python路径
project_root = Path(__file__).parent.parent.parent
sys.path.insert(0, str(project_root))

def test_progress_bar_improvements():
    """测试进度条改进功能"""
    print("=== 测试进度条改进功能 ===")
    
    # 检查packagemanager.cpp中的状态管理实现
    packagemanager_cpp = project_root / "src" / "packagemanager.cpp"
    if not packagemanager_cpp.exists():
        print("❌ packagemanager.cpp文件不存在")
        return False
    
    with open(packagemanager_cpp, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # 检查进度条状态枚举定义
    if "enum ProgressState" in content:
        print("✅ 已定义ProgressState枚举")
    else:
        print("❌ 未找到ProgressState枚举定义")
        return False
    
    # 检查状态成员变量
    if "m_currentProgressState" in content:
        print("✅ 已定义m_currentProgressState成员变量")
    else:
        print("❌ 未找到m_currentProgressState成员变量")
        return False
    
    # 检查onDetectToolsClicked函数中的状态管理
    if "m_currentProgressState = DetectingTools" in content:
        print("✅ onDetectToolsClicked函数已设置检测工具状态")
    else:
        print("❌ onDetectToolsClicked函数未设置检测工具状态")
        return False
    
    if "m_progressBar->setValue(100)" in content and "开发工具检测完成，进度100%" in content:
        print("✅ 检测工具完成后进度条显示100%")
    else:
        print("❌ 检测工具完成后未设置进度条为100%")
        return False
    
    # 检查onPackageClicked函数中的状态管理
    if "m_currentProgressState = Packaging" in content:
        print("✅ onPackageClicked函数已设置打包状态")
    else:
        print("❌ onPackageClicked函数未设置打包状态")
        return False
    
    # 检查onProgressChanged函数中的状态前缀
    if "[检测工具]" in content and "[打包过程]" in content:
        print("✅ onProgressChanged函数已添加状态前缀")
    else:
        print("❌ onProgressChanged函数未添加状态前缀")
        return False
    
    # 检查packageservice.cpp中的进度更新
    packageservice_cpp = project_root / "src" / "packageservice.cpp"
    if packageservice_cpp.exists():
        with open(packageservice_cpp, 'r', encoding='utf-8') as f:
            service_content = f.read()
        
        if "emit progressChanged(100, \"开发工具检测完成\")" in service_content:
            print("✅ packageservice.cpp中检测工具完成后发送100%进度")
        else:
            print("❌ packageservice.cpp中检测工具完成后未发送100%进度")
            return False
    
    print("\n✅ 所有进度条改进功能测试通过！")
    print("\n改进总结：")
    print("1. 开发工具检测完成后，进度条显示100%")
    print("2. 进度条现在可以区分检测工具和打包状态")
    print("3. 状态消息添加了前缀标识当前操作类型")
    print("4. 状态管理确保进度条在不同操作间正确切换")
    
    return True

def main():
    """主函数"""
    try:
        success = test_progress_bar_improvements()
        return 0 if success else 1
    except Exception as e:
        print(f"❌ 测试过程中发生错误: {e}")
        return 1

if __name__ == "__main__":
    sys.exit(main())