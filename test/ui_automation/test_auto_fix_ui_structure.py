import os
import xml.etree.ElementTree as ET

def auto_fix_mainwindow_copy_ui():
    """
    自动修复mainwindow_copy.ui的根元素结构问题
    """
    ui_file_path = os.path.abspath("../../src/product/mainwindow_copy.ui")
    
    if not os.path.exists(ui_file_path):
        print(f"错误：未找到UI文件 {ui_file_path}")
        return
        
    try:
        # 解析XML文件
        tree = ET.parse(ui_file_path)
        root = tree.getroot()
        
        print(f"当前根元素: {root.tag} {root.attrib}")
        
        # 检查根元素下是否有QWidget嵌套QMainWindow结构
        mainwindow = None
        central_widget = None
        
        for child in root:
            if child.tag == "widget" and child.get("class") == "QWidget" and child.get("name") == "centralWidget":
                central_widget = child
                # 在centralWidget中查找QMainWindow
                for grandchild in child:
                    if grandchild.tag == "widget" and grandchild.get("class") == "QMainWindow":
                        mainwindow = grandchild
                        break
                break
        
        if mainwindow and central_widget:
            print("找到错误的嵌套结构，开始修复...")
            
            # 清空根元素
            for child in list(root):
                root.remove(child)
            
            # 添加class元素
            class_elem = ET.SubElement(root, "class")
            class_elem.text = "MainWindow"
            
            # 将QMainWindow移到根元素下
            root.append(mainwindow)
            
            # 添加resources和connections元素
            ET.SubElement(root, "resources")
            ET.SubElement(root, "connections")
            
            # 保存修复后的文件
            tree.write(ui_file_path, encoding="UTF-8", xml_declaration=True)
            
            print("成功修复UI文件结构：")
            print("1. 将根元素从<QWidget>改为<QMainWindow>")
            print("2. 移除了错误的centralWidget嵌套结构")
            print("3. 恢复了正确的UI文件结构")
            
            # 验证修复结果
            verify_tree = ET.parse(ui_file_path)
            verify_root = verify_tree.getroot()
            print(f"修复后根元素: {verify_root.tag}")
            mainwindow_count = len(verify_root.findall(".//widget[@class='QMainWindow']"))
            print(f"修复后QMainWindow数量: {mainwindow_count}")
            
        else:
            print("根元素已经是正确的QMainWindow结构，无需修复")
            
    except Exception as e:
        print(f"修复过程中出现错误: {e}")

if __name__ == "__main__":
    auto_fix_mainwindow_copy_ui()