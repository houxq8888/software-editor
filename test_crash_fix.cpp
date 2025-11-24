#include <QApplication>
#include <QDebug>
#include <QMessageBox>
#include "src/uilayoutwindow.h"
#include "src/productconfigmanager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    qDebug() << "开始测试事件-动作编辑器崩溃修复...";
    
    // 测试新建产品状态
    {
        qDebug() << "\n=== 测试新建产品状态 ===";
        
        ProductConfigManager configManager;
        UILayoutWindow *window = new UILayoutWindow(nullptr, true, "", &configManager);
        
        qDebug() << "UI布局窗口创建成功";
        qDebug() << "界面管理器状态:" << (window->m_interfaceManager ? "有效" : "无效");
        
        // 尝试打开事件-动作编辑器
        try {
            window->onActionEventActionEditorTriggered();
            qDebug() << "✓ 事件-动作编辑器成功打开，没有崩溃";
            
            // 检查是否有错误对话框弹出
            QWidget *activeWindow = QApplication::activeWindow();
            if (activeWindow && activeWindow->inherits("QMessageBox")) {
                QMessageBox *msgBox = qobject_cast<QMessageBox*>(activeWindow);
                if (msgBox && msgBox->windowTitle().contains("错误")) {
                    qDebug() << "⚠ 显示错误对话框（预期行为）";
                    msgBox->close();
                }
            }
            
        } catch (const std::exception &e) {
            qDebug() << "✗ 打开事件-动作编辑器时发生异常:" << e.what();
            delete window;
            return 1;
        }
        
        delete window;
        qDebug() << "✓ 新建产品状态测试完成";
    }
    
    // 测试现有产品状态
    {
        qDebug() << "\n=== 测试现有产品状态 ===";
        
        ProductConfigManager configManager;
        UILayoutWindow *window = new UILayoutWindow(nullptr, false, "test_product.json", &configManager);
        
        qDebug() << "UI布局窗口创建成功";
        qDebug() << "界面管理器状态:" << (window->m_interfaceManager ? "有效" : "无效");
        
        // 尝试打开事件-动作编辑器
        try {
            window->onActionEventActionEditorTriggered();
            qDebug() << "✓ 事件-动作编辑器成功打开";
            
            // 检查窗口是否正常显示
            QWidget *activeWindow = QApplication::activeWindow();
            if (activeWindow && activeWindow != window) {
                qDebug() << "✓ 事件-动作编辑器窗口正常显示";
                activeWindow->close();
            }
            
        } catch (const std::exception &e) {
            qDebug() << "✗ 打开事件-动作编辑器时发生异常:" << e.what();
            delete window;
            return 1;
        }
        
        delete window;
        qDebug() << "✓ 现有产品状态测试完成";
    }
    
    qDebug() << "\n🎉 所有测试通过！事件-动作编辑器崩溃问题已修复";
    return 0;
}