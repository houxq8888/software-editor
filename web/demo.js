// Software Editor 演示平台 JavaScript 功能

// 全局变量
let currentDoc = 'dev-plan';
let softwareStatus = 'unknown';

// 页面加载完成后初始化
document.addEventListener('DOMContentLoaded', function() {
    initializeDemo();
    loadSoftwareStatus();
    showDoc('dev-plan'); // 默认显示开发计划
});

// 初始化演示平台
function initializeDemo() {
    console.log('初始化 Software Editor 演示平台');
    
    // 显示欢迎消息
    showMessage('欢迎使用 Software Editor 综合演示平台', 'success');
    
    // 设置软件路径
    document.getElementById('exePath').textContent = 'build/software-editor.exe';
    
    // 初始化文档内容
    initializeDocs();
}

// 显示消息
function showMessage(message, type = 'info') {
    const messageArea = document.getElementById('message-area');
    const alertClass = {
        'success': 'alert-success',
        'error': 'alert-danger',
        'warning': 'alert-warning',
        'info': 'alert-info'
    }[type] || 'alert-info';
    
    const alertDiv = document.createElement('div');
    alertDiv.className = `alert ${alertClass} alert-dismissible fade show`;
    alertDiv.innerHTML = `
        ${message}
        <button type="button" class="btn-close" data-bs-dismiss="alert"></button>
    `;
    
    messageArea.appendChild(alertDiv);
    
    // 5秒后自动消失
    setTimeout(() => {
        if (alertDiv.parentNode) {
            alertDiv.remove();
        }
    }, 5000);
}

// 加载软件状态
function loadSoftwareStatus() {
    showLoading(true);
    
    fetch('/api/software-status')
        .then(response => response.json())
        .then(data => {
            updateSoftwareStatus(data);
            showLoading(false);
        })
        .catch(error => {
            console.error('加载软件状态失败:', error);
            updateSoftwareStatus({
                exists: false,
                size: '未知',
                modified: '未知',
                status: 'error'
            });
            showLoading(false);
            showMessage('无法获取软件状态信息', 'error');
        });
}

// 更新软件状态显示
function updateSoftwareStatus(data) {
    const statusElement = document.getElementById('softwareStatus');
    const exeStatusElement = document.getElementById('exeStatus');
    const exeSizeElement = document.getElementById('exeSize');
    const exeModifiedElement = document.getElementById('exeModified');
    
    if (data.exists) {
        statusElement.textContent = '就绪';
        statusElement.className = 'status-badge status-ready';
        exeStatusElement.textContent = '存在';
        exeStatusElement.className = 'status-badge status-ready';
        exeSizeElement.textContent = data.size || '-- MB';
        exeModifiedElement.textContent = data.modified || '--';
        softwareStatus = 'ready';
    } else {
        statusElement.textContent = '未找到';
        statusElement.className = 'status-badge status-error';
        exeStatusElement.textContent = '不存在';
        exeStatusElement.className = 'status-badge status-error';
        exeSizeElement.textContent = '-- MB';
        exeModifiedElement.textContent = '--';
        softwareStatus = 'error';
    }
}

// 显示/隐藏加载状态
function showLoading(show) {
    const loadingElement = document.getElementById('loading');
    loadingElement.style.display = show ? 'block' : 'none';
}

// 初始化文档内容
function initializeDocs() {
    // 设置文档导航激活状态
    const navLinks = document.querySelectorAll('.doc-nav .nav-link');
    navLinks.forEach(link => {
        link.addEventListener('click', function(e) {
            e.preventDefault();
            
            // 移除所有激活状态
            navLinks.forEach(l => l.classList.remove('active'));
            
            // 设置当前激活状态
            this.classList.add('active');
            
            // 获取文档类型
            const docType = this.textContent.trim();
            let docId = '';
            
            switch(docType) {
                case '开发计划':
                    docId = 'dev-plan';
                    break;
                case '详细实施计划':
                    docId = 'detailed-plan';
                    break;
                case '代码架构':
                    docId = 'architecture';
                    break;
                default:
                    docId = 'dev-plan';
            }
            
            showDoc(docId);
        });
    });
}

// 显示文档内容
function showDoc(docType) {
    currentDoc = docType;
    const docContentElement = document.getElementById('docContent');
    
    // 显示加载状态
    docContentElement.innerHTML = `
        <div class="text-center">
            <div class="spinner-border text-primary" role="status">
                <span class="visually-hidden">加载中...</span>
            </div>
            <p class="text-muted mt-2">正在加载文档内容...</p>
        </div>
    `;
    
    // 获取文档内容
    fetch(`/api/doc-content?type=${docType}`)
        .then(response => response.json())
        .then(data => {
            if (data.success && data.content) {
                // 使用 marked 解析 Markdown
                const htmlContent = marked.parse(data.content);
                docContentElement.innerHTML = htmlContent;
                
                // 添加一些样式优化
                const headings = docContentElement.querySelectorAll('h1, h2, h3, h4, h5, h6');
                headings.forEach(heading => {
                    heading.style.color = '#2c3e50';
                    heading.style.marginTop = '1.5em';
                    heading.style.marginBottom = '0.5em';
                });
                
                const lists = docContentElement.querySelectorAll('ul, ol');
                lists.forEach(list => {
                    list.style.paddingLeft = '2em';
                });
                
                const paragraphs = docContentElement.querySelectorAll('p');
                paragraphs.forEach(p => {
                    p.style.lineHeight = '1.6';
                });
                
            } else {
                docContentElement.innerHTML = `
                    <div class="text-center text-muted">
                        <i class="fas fa-exclamation-triangle fa-2x mb-3"></i>
                        <p>无法加载文档内容</p>
                        <small>${data.error || '未知错误'}</small>
                    </div>
                `;
            }
        })
        .catch(error => {
            console.error('加载文档失败:', error);
            docContentElement.innerHTML = `
                <div class="text-center text-muted">
                    <i class="fas fa-exclamation-triangle fa-2x mb-3"></i>
                    <p>文档加载失败</p>
                    <small>请检查网络连接或服务器状态</small>
                </div>
            `;
        });
}

// 启动软件
function openSoftware() {
    if (softwareStatus === 'error') {
        showMessage('软件文件不存在，无法启动', 'error');
        return;
    }
    
    showMessage('正在启动 Software Editor...', 'info');
    
    fetch('/api/launch-software', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        }
    })
    .then(response => response.json())
    .then(data => {
        if (data.success) {
            showMessage('软件启动成功！', 'success');
        } else {
            showMessage(`启动失败: ${data.error}`, 'error');
        }
    })
    .catch(error => {
        console.error('启动软件失败:', error);
        showMessage('启动软件时发生错误', 'error');
    });
}

// 显示演示指南
function showDemoGuide() {
    const guideContent = `
# Software Editor 演示指南

## 演示流程建议

### 1. 项目介绍 (5分钟)
- 展示项目背景和解决的问题
- 介绍目标用户群体
- 演示平台功能概览

### 2. 核心功能演示 (15分钟)
- 产品配置编辑功能
- UI布局设计器操作
- 智能打包流程展示

### 3. 技术架构说明 (5分钟)
- 模块化架构设计
- 技术栈选择理由
- 扩展性和维护性

### 4. 开发计划展示 (5分钟)
- 三阶段开发路线图
- 当前进度和里程碑
- 未来规划

### 5. 问答环节 (5-10分钟)
- 回答技术问题
- 讨论商业价值
- 收集反馈建议

## 演示技巧
- 保持演示节奏适中
- 重点突出核心价值
- 准备常见问题解答
- 展示实际应用场景
    `;
    
    const htmlContent = marked.parse(guideContent);
    
    // 创建模态框显示指南
    const modal = document.createElement('div');
    modal.className = 'modal fade';
    modal.id = 'demoGuideModal';
    modal.innerHTML = `
        <div class="modal-dialog modal-lg">
            <div class="modal-content">
                <div class="modal-header">
                    <h5 class="modal-title">
                        <i class="fas fa-book me-2"></i>演示指南
                    </h5>
                    <button type="button" class="btn-close" data-bs-dismiss="modal"></button>
                </div>
                <div class="modal-body">
                    <div style="max-height: 400px; overflow-y: auto;">
                        ${htmlContent}
                    </div>
                </div>
                <div class="modal-footer">
                    <button type="button" class="btn btn-secondary" data-bs-dismiss="modal">关闭</button>
                    <button type="button" class="btn btn-primary" onclick="printGuide()">
                        <i class="fas fa-print me-1"></i>打印指南
                    </button>
                </div>
            </div>
        </div>
    `;
    
    document.body.appendChild(modal);
    
    const modalInstance = new bootstrap.Modal(modal);
    modalInstance.show();
    
    // 模态框关闭后移除元素
    modal.addEventListener('hidden.bs.modal', function() {
        modal.remove();
    });
}

// 打印指南
function printGuide() {
    window.print();
}

// 刷新数据
function refreshData() {
    showMessage('正在刷新数据...', 'info');
    loadSoftwareStatus();
    showDoc(currentDoc);
    
    setTimeout(() => {
        showMessage('数据刷新完成', 'success');
    }, 1000);
}

// 键盘快捷键支持
document.addEventListener('keydown', function(e) {
    // Ctrl + R: 刷新数据
    if (e.ctrlKey && e.key === 'r') {
        e.preventDefault();
        refreshData();
    }
    
    // F1: 显示帮助
    if (e.key === 'F1') {
        e.preventDefault();
        showDemoGuide();
    }
    
    // F5: 启动软件
    if (e.key === 'F5') {
        e.preventDefault();
        openSoftware();
    }
});

// 错误处理
window.addEventListener('error', function(e) {
    console.error('JavaScript 错误:', e.error);
    showMessage('页面发生错误，请刷新页面重试', 'error');
});

// 页面可见性变化处理
document.addEventListener('visibilitychange', function() {
    if (!document.hidden) {
        // 页面重新可见时刷新数据
        refreshData();
    }
});