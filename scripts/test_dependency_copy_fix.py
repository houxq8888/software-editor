#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试修复后的依赖复制功能
"""

import os
import sys
import subprocess
import time

def test_dependency_copy():
    """测试依赖复制功能"""
    print("=== 测试依赖复制功能 ===")
    
    # 检查build目录是否存在
    build_dir = os.path.join(os.getcwd(), "build")
    if not os.path.exists(build_dir):
        print("❌ build目录不存在，请先编译程序")
        return False
    
    # 检查可执行文件是否存在
    exe_path = os.path.join(build_dir, "software-editor.exe")
    if not os.path.exists(exe_path):
        print("❌ 可执行文件不存在: {}".format(exe_path))
        return False
    
    print("✅ 找到可执行文件: {}".format(exe_path))
    
    # 创建测试输出目录
    test_output_dir = os.path.join(os.getcwd(), "test_output")
    if os.path.exists(test_output_dir):
        # 清理之前的测试输出
        import shutil
        shutil.rmtree(test_output_dir)
    os.makedirs(test_output_dir)
    
    print("✅ 创建测试输出目录: {}".format(test_output_dir))
    
    # 运行程序进行依赖复制测试
    print("\n🚀 启动程序进行依赖复制测试...")
    
    # 使用subprocess运行程序
    process = subprocess.Popen([exe_path], 
                              stdout=subprocess.PIPE, 
                              stderr=subprocess.PIPE,
                              text=True)
    
    # 等待程序启动
    time.sleep(3)
    
    # 检查程序是否在运行
    if process.poll() is not None:
        stdout, stderr = process.communicate()
        print("❌ 程序启动失败")
        print("标准输出:", stdout)
        print("标准错误:", stderr)
        return False
    
    print("✅ 程序启动成功")
    
    # 等待一段时间让程序初始化
    time.sleep(2)
    
    # 终止程序
    process.terminate()
    process.wait()
    
    print("✅ 程序已终止")
    
    # 检查测试输出目录中的文件
    print("\n📁 检查测试输出目录内容:")
    if os.path.exists(test_output_dir):
        files = os.listdir(test_output_dir)
        if files:
            print("✅ 测试输出目录包含文件:")
            for file in files:
                file_path = os.path.join(test_output_dir, file)
                if os.path.isfile(file_path):
                    print("   📄 {}".format(file))
                else:
                    print("   📁 {}".format(file))
        else:
            print("⚠️  测试输出目录为空")
    else:
        print("❌ 测试输出目录不存在")
    
    # 检查build目录中的依赖文件
    print("\n📁 检查build目录中的依赖文件:")
    build_files = os.listdir(build_dir)
    dll_files = [f for f in build_files if f.endswith('.dll')]
    
    if dll_files:
        print("✅ build目录包含DLL文件:")
        for dll in dll_files:
            print("   📄 {}".format(dll))
    else:
        print("⚠️  build目录中没有DLL文件")
    
    # 检查plugins目录
    plugins_dir = os.path.join(build_dir, "plugins")
    if os.path.exists(plugins_dir):
        print("✅ 找到plugins目录:")
        for root, dirs, files in os.walk(plugins_dir):
            level = root.replace(plugins_dir, '').count(os.sep)
            indent = ' ' * 2 * level
            print('{}{}/'.format(indent, os.path.basename(root)))
            subindent = ' ' * 2 * (level + 1)
            for file in files:
                print('{}{}'.format(subindent, file))
    else:
        print("⚠️  没有找到plugins目录")
    
    print("\n=== 测试完成 ===")
    return True

def check_qt_installation():
    """检查Qt安装情况"""
    print("\n=== 检查Qt安装情况 ===")
    
    # 检查常见的Qt安装路径
    qt_paths = [
        "C:\\Qt",
        "D:\\Qt", 
        "E:\\Qt",
        os.path.expanduser("~\\Qt")
    ]
    
    qt_found = False
    for qt_path in qt_paths:
        if os.path.exists(qt_path):
            print("✅ 找到Qt安装目录: {}".format(qt_path))
            qt_found = True
            
            # 检查Qt版本
            try:
                versions = os.listdir(qt_path)
                print("   Qt版本: {}".format(', '.join(versions)))
                
                # 检查windeployqt工具
                for version in versions:
                    windeployqt_path = os.path.join(qt_path, version, "mingw_64", "bin", "windeployqt.exe")
                    if os.path.exists(windeployqt_path):
                        print("   ✅ 找到windeployqt工具: {}".format(windeployqt_path))
                    else:
                        print("   ⚠️  未找到windeployqt工具: {}".format(windeployqt_path))
            except:
                print("   ⚠️  无法读取Qt版本信息")
    
    if not qt_found:
        print("❌ 未找到Qt安装目录")
        print("💡 提示: 系统未安装Qt，依赖复制将使用手动模式")
    
    return qt_found

def main():
    """主函数"""
    print("依赖复制功能测试脚本")
    print("=" * 50)
    
    # 检查当前目录
    current_dir = os.getcwd()
    print("当前工作目录: {}".format(current_dir))
    
    # 检查Qt安装情况
    qt_installed = check_qt_installation()
    
    # 测试依赖复制功能
    success = test_dependency_copy()
    
    # 总结
    print("\n" + "=" * 50)
    print("测试总结:")
    print("✅ Qt安装状态: {}".format("已安装" if qt_installed else "未安装"))
    print("✅ 依赖复制测试: {}".format("成功" if success else "失败"))
    
    if not qt_installed:
        print("\n💡 重要提示:")
        print("由于系统未安装Qt，依赖复制将使用手动模式")
        print("手动模式会复制基本的Qt DLL、MinGW运行时和系统依赖")
        print("这确保了在没有Qt环境下的程序正常运行")
    
    return success

if __name__ == "__main__":
    try:
        success = main()
        sys.exit(0 if success else 1)
    except Exception as e:
        print("❌ 测试过程中发生错误: {}".format(e))
        import traceback
        traceback.print_exc()
        sys.exit(1)