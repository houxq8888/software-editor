import xml.etree.ElementTree as ET

# 创建一个简单的XML验证脚本
xml_file = 'mainwindow.ui'

try:
    # 尝试解析XML文件
    tree = ET.parse(xml_file)
    root = tree.getroot()
    print(f"XML文件 '{xml_file}' 结构有效！")
    print(f"根元素: {root.tag}")
    
    # 查找tabWidget元素
    tab_widgets = root.findall('.//QTabWidget')
    print(f"找到 {len(tab_widgets)} 个QTabWidget元素")
    
    for i, tw in enumerate(tab_widgets):
        print(f"\nQTabWidget {i}:")
        print(f"  名称: {tw.attrib.get('name', '未命名')}")
        print(f"  父元素: {tw.parent.tag}")
        
        # 查看tabWidget的子元素
        children = list(tw)
        print(f"  子元素数量: {len(children)}")
        for j, child in enumerate(children):
            print(f"    子元素 {j}: {child.tag} {child.attrib}")
            
except ET.ParseError as e:
    print(f"XML解析错误: {e}")
    print(f"错误位置: 行 {e.lineno}，列 {e.offset}")
    
    # 读取文件并显示错误位置附近的内容
    try:
        with open(xml_file, 'r', encoding='utf-8') as f:
            lines = f.readlines()
            
        # Python的列表索引是从0开始的，而错误信息中的行号是从1开始的
        line_num = e.lineno - 1
        
        # 显示错误行及其前后各5行
        start = max(0, line_num - 5)
        end = min(len(lines), line_num + 6)
        
        print(f"\n错误位置附近的内容 (行 {e.lineno}):")
        for i in range(start, end):
            line = lines[i].rstrip()
            if i == line_num:
                print(f"{i+1:4}: {line}  <<<< 错误在这里")
                print(f"      {'^':>{e.offset}}")
            else:
                print(f"{i+1:4}: {line}")
                
    except Exception as read_err:
        print(f"读取文件失败: {read_err}")