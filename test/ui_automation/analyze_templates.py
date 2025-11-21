#!/usr/bin/env python3
"""
分析测试报告模板和测试用例模板的结构
"""

import pandas as pd
import os
from datetime import datetime

def analyze_template(file_path, template_name):
    """分析Excel模板文件的结构"""
    print(f"\n=== 分析 {template_name} ===")
    print(f"文件路径: {file_path}")
    
    if not os.path.exists(file_path):
        print(f"错误: 文件不存在 - {file_path}")
        return None
    
    try:
        # 读取Excel文件
        xl = pd.ExcelFile(file_path)
        
        print(f"工作表数量: {len(xl.sheet_names)}")
        print(f"工作表名称: {xl.sheet_names}")
        
        # 分析每个工作表
        for sheet_name in xl.sheet_names:
            print(f"\n--- 工作表: {sheet_name} ---")
            
            # 读取工作表数据
            df = pd.read_excel(file_path, sheet_name=sheet_name)
            
            print(f"数据形状: {df.shape}")
            print(f"列名: {list(df.columns)}")
            
            # 显示前几行数据
            print("\n前5行数据:")
            print(df.head())
            
            # 显示数据类型
            print("\n数据类型:")
            print(df.dtypes)
            
            # 显示非空值统计
            print("\n非空值统计:")
            print(df.notnull().sum())
            
        return xl.sheet_names
        
    except Exception as e:
        print(f"读取Excel文件时出错: {e}")
        return None

def main():
    """主函数"""
    # 模板文件路径
    test_case_template = "d:\\virtualMachine\\github\\software-editor\\docs\\测试用例模板.xlsx"
    test_report_template = "d:\\virtualMachine\\github\\software-editor\\docs\\测试报告模板.xlsx"
    
    print("开始分析测试模板结构...")
    
    # 分析测试用例模板
    test_case_sheets = analyze_template(test_case_template, "测试用例模板")
    
    # 分析测试报告模板
    test_report_sheets = analyze_template(test_report_template, "测试报告模板")
    
    # 生成分析报告
    print("\n=== 模板分析总结 ===")
    print(f"当前日期: {datetime.now().strftime('%Y-%m-%d')}")
    
    if test_case_sheets:
        print(f"测试用例模板工作表: {test_case_sheets}")
    
    if test_report_sheets:
        print(f"测试报告模板工作表: {test_report_sheets}")
    
    # 生成模板字段建议
    print("\n=== 模板字段建议 ===")
    print("基于常见测试模板结构，建议包含以下字段:")
    print("\n测试用例模板字段:")
    print("- 用例编号")
    print("- 用例名称") 
    print("- 测试模块")
    print("- 前置条件")
    print("- 测试步骤")
    print("- 预期结果")
    print("- 实际结果")
    print("- 测试状态")
    print("- 测试人员")
    print("- 测试日期")
    
    print("\n测试报告模板字段:")
    print("- 报告编号")
    print("- 测试项目")
    print("- 测试周期")
    print("- 测试环境")
    print("- 测试统计")
    print("- 通过率")
    print("- 缺陷统计")
    print("- 测试总结")
    print("- 测试人员")
    print("- 报告日期")

if __name__ == "__main__":
    main()