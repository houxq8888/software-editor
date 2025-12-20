#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>

#include "product.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    // 设置当前工作目录到项目根目录
    QDir::setCurrent("d:/virtualMachine/github/software-editor");
    
    // 加载产品配置文件
    QString configPath = "product_configurations/product_12345/product_config.json";
    QFile configFile(configPath);
    
    if (!configFile.open(QIODevice::ReadOnly)) {
        qDebug() << "无法打开产品配置文件:" << configPath;
        return -1;
    }
    
    QByteArray configData = configFile.readAll();
    configFile.close();
    
    QJsonDocument configDoc = QJsonDocument::fromJson(configData);
    if (configDoc.isNull()) {
        qDebug() << "产品配置文件格式错误";
        return -1;
    }
    
    QJsonObject configObj = configDoc.object();
    
    // 创建Product对象并加载配置
    Product product;
    product.fromJson(configObj, configPath);
    
    // 输出调试信息
    qDebug() << "产品配置加载完成";
    qDebug() << "状态机配置数量:" << product.getStateMachineConfigs().size();
    
    if (!product.getStateMachineConfigs().isEmpty()) {
        auto stateMachineConfig = product.getStateMachineConfigs().first();
        qDebug() << "状态机文件路径:" << stateMachineConfig.fileName;
        qDebug() << "wizardJsonPath原始值:" << stateMachineConfig.wizardJsonPath;
        
        // 检查wizard文件是否存在
        QString wizardPath = stateMachineConfig.wizardJsonPath;
        QFile wizardFile(wizardPath);
        if (wizardFile.exists()) {
            qDebug() << "wizard文件存在:" << wizardPath;
        } else {
            qDebug() << "wizard文件不存在:" << wizardPath;
            qDebug() << "当前工作目录:" << QDir::currentPath();
            
            // 尝试相对路径查找
            QString relativePath = "product_configurations/product_12345/" + wizardPath;
            if (QFile::exists(relativePath)) {
                qDebug() << "找到wizard文件(相对路径):" << relativePath;
            }
        }
    }
    
    return 0;
}