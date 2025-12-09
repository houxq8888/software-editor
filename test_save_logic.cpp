#include <iostream>
#include <QString>
#include <QDir>
#include <QFileInfo>

// 模拟Product类中的路径处理逻辑
QString toAbsolutePath(const QString& relativePath) {
    QDir currentDir(QDir::currentPath());
    return currentDir.absoluteFilePath(relativePath);
}

QString ensureUiLayoutsPath(const QString& filePath, const QString& uiLayoutsPath = "ui_layouts") {
    QFileInfo fileInfo(filePath);
    QString fileName = fileInfo.fileName();
    
    // 如果已经是相对路径且包含ui_layouts，直接返回
    if (filePath.contains(uiLayoutsPath) && !QDir::isAbsolutePath(filePath)) {
        return filePath;
    }
    
    // 如果是绝对路径，转换为相对路径
    if (QDir::isAbsolutePath(filePath)) {
        QDir currentDir(QDir::currentPath());
        QString relativePath = currentDir.relativeFilePath(filePath);
        
        // 确保路径包含ui_layouts
        if (!relativePath.contains(uiLayoutsPath)) {
            return QString("%1/%2").arg(uiLayoutsPath, fileName);
        }
        return relativePath;
    }
    
    // 如果是相对路径但不包含ui_layouts，添加ui_layouts前缀
    if (!filePath.contains(uiLayoutsPath)) {
        return QString("%1/%2").arg(uiLayoutsPath, fileName);
    }
    
    return filePath;
}

int main() {
    // 测试用例
    QString testCases[] = {
        "D:\\virtualMachine\\github\\20251203\\software-editor-04\\ui_layouts\\43214.ui",
        "D:\\virtualMachine\\github\\20251203\\software-editor-04\\1208.ui",
        "ui_layouts\\43214.ui",
        "43214.ui"
    };
    
    std::cout << "测试路径保存逻辑：" << std::endl;
    std::cout << "当前工作目录: " << QDir::currentPath().toStdString() << std::endl;
    std::cout << std::endl;
    
    for (const QString& testCase : testCases) {
        QString result = ensureUiLayoutsPath(testCase);
        std::cout << "输入: " << testCase.toStdString() << std::endl;
        std::cout << "输出: " << result.toStdString() << std::endl;
        std::cout << "绝对路径: " << toAbsolutePath(result).toStdString() << std::endl;
        std::cout << "---" << std::endl;
    }
    
    return 0;
}