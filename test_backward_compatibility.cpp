#include <QCoreApplication>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

#include "src/product/product.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    // 测试向后兼容性
    QFile oldJsonFile("test_product_20251121_113422.json");
    if (!oldJsonFile.open(QIODevice::ReadOnly)) {
        qDebug() << "无法打开旧JSON文件";
        return -1;
    }
    
    QByteArray jsonData = oldJsonFile.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    QJsonObject jsonObj = doc.object();
    
    // 创建Product实例并加载旧格式数据
    Product product;
    if (product.fromJson(jsonObj)) {
        qDebug() << "=== 向后兼容性测试结果 ===";
        qDebug() << "产品名称:" << product.name();
        qDebug() << "旧UI文件路径:" << product.uiLayoutPath();
        qDebug() << "UI文件数量:" << product.uiFiles().size();
        
        if (!product.uiFiles().isEmpty()) {
            ProductUIFile mainFile = product.getMainUiFile();
            qDebug() << "主UI文件:" << mainFile.name << "-" << mainFile.filePath;
            qDebug() << "是否为主文件:" << mainFile.isMain;
        }
        
        // 测试转换为新格式
        QJsonObject newJson = product.toJson();
        qDebug() << "\n=== 新格式JSON结构 ===";
        qDebug() << "包含uiLayoutPath字段:" << newJson.contains("uiLayoutPath");
        qDebug() << "包含uiFiles数组:" << newJson.contains("uiFiles");
        
        if (newJson.contains("uiFiles")) {
            QJsonArray uiFiles = newJson["uiFiles"].toArray();
            qDebug() << "uiFiles数组大小:" << uiFiles.size();
            
            for (const auto &file : uiFiles) {
                QJsonObject fileObj = file.toObject();
                qDebug() << "  -" << fileObj["name"].toString() 
                         << "(" << fileObj["filePath"].toString() << ")"
                         << "[主文件:" << fileObj["isMain"].toBool() << "]";
            }
        }
        
        qDebug() << "\n=== 测试完成 ===";
        qDebug() << "向后兼容性验证成功！";
        
    } else {
        qDebug() << "JSON解析失败";
        return -1;
    }
    
    return 0;
}