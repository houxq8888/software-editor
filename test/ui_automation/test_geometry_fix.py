import os
import sys
import xml.etree.ElementTree as ET

def test_geometry_preservation():
    # 定义文件路径
    source_ui = "../../src/product/mainwindow.ui"
    test_output = "temp_test_geometry.ui"
    
    # 检查源文件是否存在
    if not os.path.exists(source_ui):
        print(f"错误：源文件 {source_ui} 不存在！")
        return False
    
    # 读取原始UI文件
    tree = ET.parse(source_ui)
    root = tree.getroot()
    
    print("=== 原始UI文件分析 ===")
    # 统计有geometry属性的控件
    original_geometry_count = 0
    for widget in root.findall(".//widget"):
        has_geometry = widget.find(".//property[@name='geometry']") is not None
        if has_geometry:
            original_geometry_count +=1
            widget_name = widget.get('name')
            widget_class = widget.get('class')
            print(f"控件 {widget_name} ({widget_class}) 包含geometry属性")
    
    print(f"\n原始UI中有 {original_geometry_count} 个控件包含geometry属性")
    
    # 现在启动软件并进行测试
    print("\n=== 测试修复后的保存功能 ===")
    print("请手动执行以下步骤：")
    print(f"1. 运行 .\build\software-editor.exe")
    print(f"2. 打开 {source_ui}")
    print(f"3. 另存为 {test_output}")
    print(f"4. 关闭软件后按回车键继续...")
    input()
    
    # 检查生成的UI文件
    if not os.path.exists(test_output):
        print(f"错误：测试输出文件 {test_output} 不存在！")
        return False
    
    # 分析保存后的UI文件
    tree2 = ET.parse(test_output)
    root2 = tree2.getroot()
    
    print("\n=== 保存后UI文件分析 ===")
    saved_geometry_count = 0
    zero_geometry_count = 0
    
    for widget in root2.findall(".//widget"):
        geometry = widget.find(".//property[@name='geometry']")
        if geometry is not None:
            saved_geometry_count +=1
            widget_name = widget.get('name')
            widget_class = widget.get('class')
            
            # 检查是否为0值geometry
            x = geometry.find(".//x")
            y = geometry.find(".//y")
            width = geometry.find(".//width")
            height = geometry.find(".//height")
            
            if x is not None and y is not None and width is not None and height is not None:
                if x.text == "0" and y.text == "0" and width.text == "0" and height.text == "0":
                    zero_geometry_count +=1
                    print(f"控件 {widget_name} ({widget_class}) 包含0值geometry属性（已被过滤）")
                else:
                    print(f"控件 {widget_name} ({widget_class}) 包含有效geometry属性")
    
    print(f"\n保存后的UI中有 {saved_geometry_count} 个控件包含geometry属性")
    print(f"其中 {zero_geometry_count} 个控件包含0值geometry属性")
    
    if zero_geometry_count == 0:
        print("\n✅ 修复成功！没有生成0值的geometry属性")
        return True
    else:
        print(f"\n⚠️  注意：有 {zero_geometry_count} 个控件包含0值geometry属性，需要进一步优化")
        return False

if __name__ == "__main__":
    os.chdir(os.path.dirname(os.path.abspath(__file__)))
    success = test_geometry_preservation()
    sys.exit(0 if success else 1)