import xml.etree.ElementTree as ET
import sys

def find_tag_mismatches(xml_content):
    """Find tag mismatches in XML content"""
    try:
        # Try to parse the XML
        root = ET.fromstring(xml_content)
        print("XML解析成功！没有标签不匹配问题。")
        return True
    except ET.ParseError as e:
        print(f"XML解析错误: {e}")
        
        # Try to find the line number
        lines = xml_content.split('\n')
        line_count = len(lines)
        
        # Try to approximate the error location
        if hasattr(e, 'position'):
            approx_line = 1
            char_count = 0
            pos = e.position[0]
            for line in lines:
                char_count += len(line) + 1  # +1 for newline
                if char_count > pos:
                    break
                approx_line += 1
            
            # Show error context
            start_line = max(1, approx_line - 5)
            end_line = min(line_count, approx_line + 5)
            print(f"错误大致位置: 行 {approx_line}")
            print("上下文：")
            for i in range(start_line, end_line + 1):
                line = lines[i-1]
                print(f"{i}: {line.strip()}")
    
    return False

def main():
    if len(sys.argv) != 2:
        print(f"用法: {sys.argv[0]} <xml_file>")
        sys.exit(1)
    
    xml_file = sys.argv[1]
    
    try:
        with open(xml_file, 'r', encoding='utf-8') as f:
            xml_content = f.read()
        
        find_tag_mismatches(xml_content)
        
    except FileNotFoundError:
        print(f"文件未找到: {xml_file}")
        sys.exit(1)
    except Exception as e:
        print(f"读取文件失败: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()