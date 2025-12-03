#include <iostream>
#include <string>

// 模拟Qt的XML处理逻辑
class MockXmlParser {
public:
    bool parseXml(const std::string& xmlContent) {
        // 简单的XML格式检查
        if (xmlContent.find("<?xml") == std::string::npos) {
            std::cout << "错误: 缺少XML声明" << std::endl;
            return false;
        }
        
        if (xmlContent.find("<ui") == std::string::npos) {
            std::cout << "错误: 缺少UI根元素" << std::endl;
            return false;
        }
        
        if (xmlContent.find("<widget") == std::string::npos) {
            std::cout << "错误: 缺少widget元素" << std::endl;
            return false;
        }
        
        std::cout << "XML格式验证成功!" << std::endl;
        return true;
    }
    
    void extractWidgets(const std::string& xmlContent) {
        size_t pos = 0;
        int widgetCount = 0;
        
        while ((pos = xmlContent.find("<widget", pos)) != std::string::npos) {
            widgetCount++;
            
            // 查找class属性
            size_t classPos = xmlContent.find("class=\"", pos);
            if (classPos != std::string::npos) {
                classPos += 7; // 跳过 class="
                size_t classEnd = xmlContent.find("\"", classPos);
                if (classEnd != std::string::npos) {
                    std::string className = xmlContent.substr(classPos, classEnd - classPos);
                    std::cout << "Widget " << widgetCount << ": class=" << className << std::endl;
                }
            }
            
            pos += 7; // 跳过 <widget
        }
        
        std::cout << "总共找到 " << widgetCount << " 个widget元素" << std::endl;
    }
};

int main() {
    std::cout << "测试XML处理逻辑..." << std::endl;
    
    // 模拟一个UI布局XML文件
    std::string xmlContent = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                            "<ui version=\"4.0\">"
                            "<class>MainWindow</class>"
                            "<widget class=\"QMainWindow\" name=\"MainWindow\">"
                            "<property name=\"geometry\">"
                            "<rect>"
                            "<x>0</x>"
                            "<y>0</y>"
                            "<width>800</width>"
                            "<height>600</height>"
                            "</rect>"
                            "</property>"
                            "<widget class=\"QWidget\" name=\"centralwidget\">"
                            "<layout class=\"QVBoxLayout\" name=\"verticalLayout\">"
                            "<item>"
                            "<widget class=\"QLabel\" name=\"label\">"
                            "<property name=\"text\">"
                            "<string>Hello World</string>"
                            "</property>"
                            "</widget>"
                            "</item>"
                            "<item>"
                            "<widget class=\"QPushButton\" name=\"pushButton\">"
                            "<property name=\"text\">"
                            "<string>Click Me</string>"
                            "</property>"
                            "</widget>"
                            "</item>"
                            "</layout>"
                            "</widget>"
                            "</widget>"
                            "</ui>";
    
    MockXmlParser parser;
    
    if (parser.parseXml(xmlContent)) {
        std::cout << "\\n提取widget信息:" << std::endl;
        parser.extractWidgets(xmlContent);
    }
    
    std::cout << "\\n测试完成!" << std::endl;
    return 0;
}