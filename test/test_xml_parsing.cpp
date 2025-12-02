#include <QCoreApplication>
#include <QDomDocument>
#include <QDebug>
#include <QFile>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    // 创建一个简单的XML测试文件
    QString xmlContent = R"(
<?xml version="1.0" encoding="UTF-8"?>
<ui version="4.0">
  <class>MainWindow</class>
  <widget class="QMainWindow" name="MainWindow">
    <property name="geometry">
      <rect>
        <x>0</x>
        <y>0</y>
        <width>800</width>
        <height>600</height>
      </rect>
    </property>
    <widget class="QWidget" name="centralwidget">
      <layout class="QVBoxLayout" name="verticalLayout">
        <item>
          <widget class="QLabel" name="label">
            <property name="text">
              <string>Hello World</string>
            </property>
          </widget>
        </item>
        <item>
          <widget class="QPushButton" name="pushButton">
            <property name="text">
              <string>Click Me</string>
            </property>
          </widget>
        </item>
      </layout>
    </widget>
  </widget>
</ui>
    )";
    
    // 测试XML解析
    QDomDocument doc;
    QString errorMsg;
    int errorLine, errorColumn;
    
    if (!doc.setContent(xmlContent, false, &errorMsg, &errorLine, &errorColumn)) {
        qDebug() << "XML解析错误:" << errorMsg << "at line" << errorLine << "column" << errorColumn;
        return 1;
    }
    
    qDebug() << "XML解析成功!";
    
    // 测试查找widget元素
    QDomNodeList widgets = doc.elementsByTagName("widget");
    qDebug() << "找到" << widgets.count() << "个widget元素";
    
    for (int i = 0; i < widgets.count(); ++i) {
        QDomElement widget = widgets.at(i).toElement();
        QString className = widget.attribute("class");
        QString name = widget.attribute("name");
        qDebug() << "Widget" << i + 1 << ": class=" << className << ", name=" << name;
    }
    
    return 0;
}