import xml.etree.ElementTree as ET

try:
    tree = ET.parse('mainwindow.ui')
    root = tree.getroot()
    print("XML文件格式正确！")
except ET.ParseError as e:
    print(f"XML解析错误: {e}")
    print(f"错误位置: 第{e.lineno}行，第{e.offset}列")
    # 显示错误位置附近的内容
    with open('mainwindow.ui', 'r', encoding='utf-8') as f:
        lines = f.readlines()
        # 显示错误行前后5行
        start = max(0, e.lineno - 6)
        end = min(len(lines), e.lineno + 5)
        for i in range(start, end):
            line_num = i + 1
            line_content = lines[i].rstrip()
            print(f"{line_num:4d}: {line_content}")
