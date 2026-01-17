# 修复 updateNavigationControls 函数
def fix_update_navigation_controls():
    file_path = r"d:\virtualMachine\github\0117\alphaL2\software-editor\src\uilayout\statemachineeditor_v2.cpp"
    
    with open(file_path, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # 查找需要替换的函数
    old_function = '''void WizardPreviewWidget::updateNavigationControls()
{
    if (!m_currentWizard) return;
    
    // 更新页面标题和描述
    QList<WizardPage*> pages = m_currentWizard->allPages();
    if (m_currentPageIndex < pages.size()) {
        WizardPage *currentPage = pages[m_currentPageIndex];
        if (currentPage) {
            m_pageTitleLabel->setText(currentPage->title);
            m_pageDescriptionLabel->setText(QString("当前页面: %1").arg(currentPage->title));
            
            // 添加到主布局（如果尚未添加）
            if (m_mainLayout->indexOf(m_pageTitleLabel) == -1) {
                m_mainLayout->insertWidget(0, m_pageTitleLabel);
                m_mainLayout->insertWidget(1, m_pageDescriptionLabel);
            }
        }
    }
    
    // 更新导航标签
    m_navigationLabel->setText(QString("第 %1 页 / 共 %2 页").arg(m_currentPageIndex + 1).arg(m_totalPages));
    
    m_prevButton->setEnabled(m_currentPageIndex > 0);
    m_nextButton->setEnabled(m_currentPageIndex < m_totalPages - 1);
    m_finishButton->setEnabled(m_currentPageIndex == m_totalPages - 1);
    
    // 显示导航标签
    m_prevButton->show();
    m_nextButton->show();
    m_finishButton->show();
    m_cancelButton->show();
    
    // 添加导航布局（如果尚未添加）
    if (m_mainLayout->indexOf(m_navigationLayout) == -1) {
        m_mainLayout->addLayout(m_navigationLayout);
    }
}'''
    
    new_function = '''void WizardPreviewWidget::updateNavigationControls()
{
    if (!m_currentWizard) return;
    
    // 更新页面标题和描述
    QList<WizardPage*> pages = m_currentWizard->allPages();
    if (m_currentPageIndex < pages.size()) {
        WizardPage *currentPage = pages[m_currentPageIndex];
        if (currentPage) {
            m_pageTitleLabel->setText(currentPage->title);
            m_pageDescriptionLabel->setText(QString("当前页面: %1").arg(currentPage->title));
        }
    }
    
    // 更新导航标签
    m_navigationLabel->setText(QString("第 %1 页 / 共 %2 页").arg(m_currentPageIndex + 1).arg(m_totalPages));
    
    m_prevButton->setEnabled(m_currentPageIndex > 0);
    m_nextButton->setEnabled(m_currentPageIndex < m_totalPages - 1);
    m_finishButton->setEnabled(m_currentPageIndex == m_totalPages - 1);
    
    // 显示导航标签
    m_prevButton->show();
    m_nextButton->show();
    m_finishButton->show();
    m_cancelButton->show();
}'''
    
    if old_function in content:
        content = content.replace(old_function, new_function)
        with open(file_path, 'w', encoding='utf-8') as f:
            f.write(content)
        print("✓ 成功修复 updateNavigationControls 函数")
    else:
        print("✗ 未找到需要替换的函数内容")

if __name__ == "__main__":
    fix_update_navigation_controls()
