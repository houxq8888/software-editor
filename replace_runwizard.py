import re

# 读取原文件
with open(r'd:\virtualMachine\github\0117\alphaL2\software-editor\src\uilayout\statemachineeditor_v2.cpp', 'r', encoding='utf-8') as f:
    content = f.read()

# 定义新的runWizard函数
new_function = '''void StateMachineEditorV2::runWizard()
{
    qDebug() << "[DEBUG] runWizard() function started";
    
    // 检查关键指针
    qDebug() << "[DEBUG] m_uiFilesListWidget: " << m_uiFilesListWidget;
    qDebug() << "[DEBUG] m_wizardManager: " << m_wizardManager;
    
    if (!m_uiFilesListWidget) {
        qDebug() << "[ERROR] m_uiFilesListWidget is null";
        QMessageBox::critical(this, "Error", "UI file list widget not initialized");
        return;
    }
    
    if (!m_wizardManager) {
        qDebug() << "[ERROR] m_wizardManager is null";
        QMessageBox::critical(this, "Error", "Wizard manager not initialized");
        return;
    }
    
    if (!m_wizardPreviewWidget) {
        qDebug() << "[INFO] m_wizardPreviewWidget is null, creating a new one...";
        m_wizardPreviewWidget = new WizardPreviewWidget(this);
        qDebug() << "[INFO] m_wizardPreviewWidget created successfully";
    }
    
    // 1. 检查是否有可用的UI文件
    qDebug() << "[DEBUG] Checking UI files count: " << m_uiFilesListWidget->count();
    if (m_uiFilesListWidget->count() == 0) {
        QMessageBox::warning(this, "Run Wizard", "No available UI files. Please add UI files first.");
        return;
    }
    
    // 2. 收集所有UI文件信息并找到主界面
    QMap<QString, UIFilePreviewItem*> uiFilePathMap;
    QList<UIFilePreviewItem*> allUiItems;
    UIFilePreviewItem *mainUiItem = nullptr;
    
    for (int i = 0; i < m_uiFilesListWidget->count(); ++i) {
        QListWidgetItem *listItem = m_uiFilesListWidget->item(i);
        if (!listItem) continue;
        
        UIFilePreviewItem *uiItem = qobject_cast<UIFilePreviewItem*>(m_uiFilesListWidget->itemWidget(listItem));
        if (uiItem) {
            QString filePath = uiItem->filePath();
            uiFilePathMap[filePath] = uiItem;
            allUiItems.append(uiItem);
            
            if (uiItem->isMainInterface()) {
                mainUiItem = uiItem;
                qDebug() << "[DEBUG] Found main interface: " << uiItem->fileName() << " at " << filePath;
            }
        }
    }
    
    if (!mainUiItem) {
        QMessageBox::critical(this, "Run Wizard", "Please select a main interface as the first window of the program.\\n\\nYou can click the 'Set as Main Interface' button in the UI file list to set the main interface.");
        return;
    }
    
    // 3. 解析控件事件定义，构建UI间跳转映射
    QMap<QString, QString> uiJumpMap;
    QSet<QString> uiWithJumpEvents;
    
    qDebug() << "[DEBUG] Parsing control events. Total events: " << m_definedControlEvents.size();
    
    for (const QString &eventStr : m_definedControlEvents) {
        QStringList parts = eventStr.split("|");
        if (parts.size() >= 5) {
            QString sourceUiPath = parts[3].trimmed();
            QString targetUiPath = parts[4].trimmed();
            
            qDebug() << "[DEBUG] Jump event: " << sourceUiPath << " -> " << targetUiPath;
            
            if (!sourceUiPath.isEmpty() && !targetUiPath.isEmpty()) {
                uiJumpMap[sourceUiPath] = targetUiPath;
                uiWithJumpEvents.insert(sourceUiPath);
                uiWithJumpEvents.insert(targetUiPath);
            }
        }
    }
    
    // 4. 创建向导实例
    Wizard *wizard = m_wizardManager->createWizard("MainApplicationWizard", "Main Application Wizard");
    if (!wizard) {
        qDebug() << "[ERROR] Failed to create wizard";
        QMessageBox::critical(this, "Error", "Failed to create wizard instance");
        return;
    }
    m_wizardManager->setCurrentWizard(wizard);
    
    // 5. 为每个UI文件创建向导页面，并根据跳转关系构建页面链
    QMap<QString, QString> pageIdMap;
    QSet<QString> processedPages;
    QStringList pageOrder;
    
    // 从主界面开始
    QString currentUiPath = mainUiItem->filePath();
    QString mainPageId = "page_" + QString::number(qHash(currentUiPath));
    pageIdMap[currentUiPath] = mainPageId;
    processedPages.insert(currentUiPath);
    pageOrder.append(mainPageId);
    
    // 构建基于跳转关系的页面链
    int maxIterations = allUiItems.size() * 2;
    int iterations = 0;
    
    while (!uiJumpMap.isEmpty() && iterations < maxIterations) {
        iterations++;
        
        if (uiJumpMap.contains(currentUiPath)) {
            QString nextUiPath = uiJumpMap[currentUiPath];
            
            if (!processedPages.contains(nextUiPath)) {
                QString pageId = "page_" + QString::number(qHash(nextUiPath));
                pageIdMap[nextUiPath] = pageId;
                processedPages.insert(nextUiPath);
                pageOrder.append(pageId);
                
                qDebug() << "[DEBUG] Adding page from jump: " << nextUiPath << " -> " << pageId;
            }
            
            uiJumpMap.remove(currentUiPath);
            currentUiPath = nextUiPath;
        } else {
            break;
        }
    }
    
    // 6. 添加所有UI页面到向导
    for (UIFilePreviewItem *uiItem : allUiItems) {
        QString filePath = uiItem->filePath();
        QString pageId;
        
        if (!pageIdMap.contains(filePath)) {
            pageId = "page_" + QString::number(qHash(filePath));
            pageIdMap[filePath] = pageId;
            
            // 如果是孤立UI，添加到页面链末尾
            if (!uiWithJumpEvents.contains(filePath)) {
                pageOrder.append(pageId);
                qDebug() << "[DEBUG] Adding orphan UI page: " << uiItem->fileName() << " -> " << pageId;
            }
        } else {
            pageId = pageIdMap[filePath];
        }
        
        WizardPage page;
        page.pageId = pageId;
        page.title = uiItem->fileName();
        page.description = "Interface: " + uiItem->fileName();
        page.isStartPage = (uiItem == mainUiItem);
        page.isFinalPage = false;
        page.enableNextButton = true;
        page.enableBackButton = true;
        page.enableFinishButton = false;
        
        wizard->addPage(page);
        qDebug() << "[DEBUG] Added page: " << page.title << " (" << pageId << ")";
    }
    
    // 7. 设置页面间的前后关系
    for (int i = 0; i < pageOrder.size(); ++i) {
        QString currentPageId = pageOrder[i];
        WizardPage *currentPage = wizard->page(currentPageId);
        
        if (!currentPage) continue;
        
        if (i > 0) {
            currentPage->previousPageId = pageOrder[i-1];
            currentPage->enableBackButton = true;
        } else {
            currentPage->enableBackButton = false;
        }
        
        if (i < pageOrder.size() - 1) {
            currentPage->nextPageId = pageOrder[i+1];
            currentPage->isFinalPage = false;
            currentPage->enableNextButton = true;
            currentPage->enableFinishButton = false;
        } else {
            currentPage->isFinalPage = true;
            currentPage->enableNextButton = false;
            currentPage->enableFinishButton = true;
        }
    }
    
    // 8. 检查向导是否有足够的页面
    if (wizard->allPages().isEmpty()) {
        qDebug() << "[ERROR] Wizard has no pages";
        QMessageBox::critical(this, "Error", "Wizard not initialized, at least one page is required to run.");
        return;
    }
    
    // 9. 显示向导预览
    qDebug() << "[DEBUG] Loading wizard into preview widget with " << wizard->allPages().size() << " pages";
    
    m_wizardPreviewWidget->loadWizard(wizard);
    qDebug() << "[DEBUG] Wizard loaded into preview widget";
    
    m_wizardPreviewWidget->show();
    m_wizardPreviewWidget->raise();
    m_wizardPreviewWidget->activateWindow();
    
    QMessageBox::information(this, "Run Wizard", "Wizard started successfully!\\n\\nThe wizard preview window has popped up, you can experience the wizard flow in the new window.\\n\\nThe main interface is the start page, all UI interfaces have been organized according to the jump relationships defined in control events.\\n\\nTotal " << wizard->allPages().size() << " UI interfaces have been added to the wizard.");
    
    qDebug() << "[DEBUG] runWizard() function completed successfully with " << wizard->allPages().size() << " pages";
}
'''

# 使用正则表达式匹配并替换runWizard函数
# 正则表达式匹配从"void StateMachineEditorV2::runWizard()"开始到下一个函数或文件结束的内容
pattern = r'(void StateMachineEditorV2::runWizard\(\)[\s\S]*?^\})'
replacement = new_function

# 使用re.DOTALL和re.MULTILINE
new_content = re.sub(pattern, replacement, content, flags=re.MULTILINE | re.DOTALL)

# 写入修改后的内容
with open(r'd:\virtualMachine\github\0117\alphaL2\software-editor\src\uilayout\statemachineeditor_v2.cpp', 'w', encoding='utf-8') as f:
    f.write(new_content)

print("runWizard function replaced successfully!")
