// Software Editor - 开发进度监控面板前端逻辑

class ProgressDashboard {
    constructor() {
        this.progressData = null;
        this.acceptanceData = null;
        this.charts = {};
        this.apiBaseUrl = 'http://localhost:5000/api';
        this.init();
    }

    async init() {
        await this.loadData();
        this.initCharts();
        this.renderDashboard();
        this.setupEventListeners();
        
        // 自动刷新数据（每30秒）
        setInterval(() => this.refreshData(), 30000);
    }

    async loadData() {
        try {
            this.progressData = await this.fetchProgressData();
            this.acceptanceData = await this.fetchAcceptanceData();
            this.updateLastUpdateTime();
        } catch (error) {
            console.error('数据加载失败:', error);
            this.showError('数据加载失败，请检查后端服务是否启动');
        }
    }

    async fetchProgressData() {
        const response = await fetch(`${this.apiBaseUrl}/progress`);
        if (!response.ok) {
            throw new Error(`HTTP error! status: ${response.status}`);
        }
        const result = await response.json();
        if (!result.success) {
            throw new Error(result.error);
        }
        return result.data;
    }

    async fetchAcceptanceData(week = null, day = null) {
        let url = `${this.apiBaseUrl}/acceptance`;
        const params = new URLSearchParams();
        if (week) params.append('week', week);
        if (day) params.append('day', day);
        if (params.toString()) url += '?' + params.toString();
        
        const response = await fetch(url);
        if (!response.ok) {
            throw new Error(`HTTP error! status: ${response.status}`);
        }
        const result = await response.json();
        if (!result.success) {
            throw new Error(result.error);
        }
        return result.data;
    }

    updateLastUpdateTime() {
        const now = new Date();
        const timeString = now.toLocaleTimeString('zh-CN');
        document.getElementById('lastUpdate').textContent = timeString;
    }

    initCharts() {
        // 总体进度环形图
        this.charts.overallProgressRing = new Chart(
            document.getElementById('overallProgressRing'),
            {
                type: 'doughnut',
                data: {
                    datasets: [{
                        data: [0, 100],
                        backgroundColor: ['#3498db', '#ecf0f1'],
                        borderWidth: 0
                    }]
                },
                options: {
                    cutout: '70%',
                    responsive: true,
                    maintainAspectRatio: true,
                    plugins: {
                        legend: { display: false },
                        tooltip: { enabled: false }
                    },
                    animation: {
                        animateScale: true,
                        animateRotate: true
                    }
                }
            }
        );

        // 每周进度趋势图
        this.charts.weeklyProgressChart = new Chart(
            document.getElementById('weeklyProgressChart'),
            {
                type: 'line',
                data: {
                    labels: [],
                    datasets: [{
                        label: '每周进度',
                        data: [],
                        borderColor: '#3498db',
                        backgroundColor: 'rgba(52, 152, 219, 0.1)',
                        borderWidth: 3,
                        fill: true,
                        tension: 0.4
                    }]
                },
                options: {
                    responsive: true,
                    plugins: {
                        legend: { display: false }
                    },
                    scales: {
                        y: {
                            beginAtZero: true,
                            max: 100,
                            ticks: {
                                callback: function(value) {
                                    return value + '%';
                                }
                            }
                        }
                    }
                }
            }
        );

        // 任务状态分布图
        this.charts.taskStatusChart = new Chart(
            document.getElementById('taskStatusChart'),
            {
                type: 'pie',
                data: {
                    labels: ['已完成', '进行中', '待处理'],
                    datasets: [{
                        data: [0, 0, 0],
                        backgroundColor: ['#2ecc71', '#f39c12', '#e74c3c'],
                        borderWidth: 0
                    }]
                },
                options: {
                    responsive: true,
                    plugins: {
                        legend: {
                            position: 'bottom'
                        }
                    }
                }
            }
        );
    }

    renderDashboard() {
        if (!this.progressData) return;

        // 更新总体进度
        const overallProgress = this.progressData.overall_progress || 0;
        document.getElementById('overallProgress').textContent = overallProgress.toFixed(1) + '%';
        
        // 更新环形图
        this.charts.overallProgressRing.data.datasets[0].data = [overallProgress, 100 - overallProgress];
        this.charts.overallProgressRing.update();

        // 更新统计卡片
        this.updateStatsCards();
        
        // 更新图表数据
        this.updateCharts();
        
        // 渲染任务列表
        this.renderTaskList();
        
        // 渲染验收结果
        this.renderAcceptanceResults();
    }

    updateStatsCards() {
        const tasks = this.progressData.tasks || [];
        
        const totalTasks = tasks.length;
        const completedTasks = tasks.filter(task => task.status === 'completed').length;
        const inProgressTasks = tasks.filter(task => task.status === 'in_progress').length;
        const pendingTasks = tasks.filter(task => task.status === 'pending').length;

        document.getElementById('totalTasks').textContent = totalTasks;
        document.getElementById('completedTasks').textContent = completedTasks;
        document.getElementById('inProgressTasks').textContent = inProgressTasks;
        document.getElementById('pendingTasks').textContent = pendingTasks;

        // 更新饼图数据
        this.charts.taskStatusChart.data.datasets[0].data = [
            completedTasks, inProgressTasks, pendingTasks
        ];
        this.charts.taskStatusChart.update();
    }

    updateCharts() {
        // 更新每周进度趋势图
        const weeklyProgress = this.progressData.weekly_progress || {};
        const weeks = Object.keys(weeklyProgress).sort();
        const progressData = weeks.map(week => weeklyProgress[week]);
        
        this.charts.weeklyProgressChart.data.labels = weeks.map(week => `第${week}周`);
        this.charts.weeklyProgressChart.data.datasets[0].data = progressData;
        this.charts.weeklyProgressChart.update();
    }

    renderTaskList() {
        const taskList = document.getElementById('taskList');
        const tasks = this.progressData.tasks || [];
        
        // 按周分组
        const tasksByWeek = this.groupTasksByWeek(tasks);
        
        let html = '';
        
        Object.keys(tasksByWeek).sort().forEach(week => {
            html += `
                <div class="week-marker">
                    <h6 class="fw-bold text-primary mb-3">第${week}周</h6>
                    <div class="task-week-container">
            `;
            
            tasksByWeek[week].forEach(task => {
                html += this.renderTaskItem(task);
            });
            
            html += `
                    </div>
                </div>
            `;
        });
        
        taskList.innerHTML = html;
    }

    renderTaskItem(task) {
        const statusClass = this.getStatusClass(task.status);
        const statusText = this.getStatusText(task.status);
        const progress = task.progress || 0;
        
        return `
            <div class="task-item ${statusClass}" data-task-id="${task.id}" data-status="${task.status}">
                <div class="d-flex justify-content-between align-items-start mb-2">
                    <div class="fw-bold">${task.name}</div>
                    <span class="status-badge status-${statusClass}">${statusText}</span>
                </div>
                <div class="mb-2">
                    <small class="text-muted">${task.description || ''}</small>
                </div>
                <div class="d-flex align-items-center">
                    <div class="progress flex-grow-1 me-3" style="height: 8px;">
                        <div class="progress-bar bg-${this.getProgressBarColor(progress)}" 
                             style="width: ${progress}%"></div>
                    </div>
                    <small class="text-muted">${progress}%</small>
                </div>
                ${this.renderTaskDetails(task)}
            </div>
        `;
    }

    renderTaskDetails(task) {
        if (!task.details) return '';
        
        return `
            <div class="mt-2">
                <button class="btn btn-sm btn-outline-secondary" 
                        type="button" 
                        onclick="toggleTaskDetails('${task.id}')">
                    <i class="fas fa-chevron-down"></i> 查看详情
                </button>
                <div id="task-details-${task.id}" class="task-details mt-2" style="display: none;">
                    <div class="small">
                        ${this.renderCheckItems(task.details.check_items || [])}
                    </div>
                </div>
            </div>
        `;
    }

    renderCheckItems(checkItems) {
        return checkItems.map(item => `
            <div class="mb-2">
                <strong>${item.category}</strong>
                <ul class="mb-0">
                    ${item.sub_items.map(subItem => `
                        <li>${subItem}</li>
                    `).join('')}
                </ul>
            </div>
        `).join('');
    }

    renderAcceptanceResults() {
        const container = document.getElementById('acceptanceResults');
        const results = this.acceptanceData.acceptance_results || [];
        
        if (results.length === 0) {
            container.innerHTML = '<p class="text-muted">暂无验收检查结果</p>';
            return;
        }
        
        let html = '<div class="row">';
        
        results.forEach(result => {
            const score = result.score || 0;
            const statusClass = score >= 80 ? 'text-success' : score >= 60 ? 'text-warning' : 'text-danger';
            
            html += `
                <div class="col-md-6 col-lg-4 mb-3">
                    <div class="card h-100">
                        <div class="card-body">
                            <h6 class="card-title">${result.task_name}</h6>
                            <div class="d-flex justify-content-between align-items-center mb-2">
                                <span class="badge bg-primary">${result.week}周${result.day}天</span>
                                <span class="fw-bold ${statusClass}">${score}分</span>
                            </div>
                            <div class="progress mb-2" style="height: 6px;">
                                <div class="progress-bar ${score >= 80 ? 'bg-success' : score >= 60 ? 'bg-warning' : 'bg-danger'}" 
                                     style="width: ${score}%"></div>
                            </div>
                            <small class="text-muted">${result.acceptance_standard}</small>
                        </div>
                    </div>
                </div>
            `;
        });
        
        html += '</div>';
        container.innerHTML = html;
    }

    groupTasksByWeek(tasks) {
        const grouped = {};
        
        tasks.forEach(task => {
            const week = task.week || '1';
            if (!grouped[week]) {
                grouped[week] = [];
            }
            grouped[week].push(task);
        });
        
        return grouped;
    }

    getStatusClass(status) {
        switch (status) {
            case 'completed': return 'completed';
            case 'in_progress': return 'in-progress';
            default: return 'pending';
        }
    }

    getStatusText(status) {
        switch (status) {
            case 'completed': return '已完成';
            case 'in_progress': return '进行中';
            default: return '待处理';
        }
    }

    getProgressBarColor(progress) {
        if (progress >= 80) return 'success';
        if (progress >= 50) return 'warning';
        return 'danger';
    }

    async refreshData() {
        this.showLoading();
        try {
            const response = await fetch(`${this.apiBaseUrl}/refresh`);
            if (!response.ok) {
                throw new Error(`HTTP error! status: ${response.status}`);
            }
            const result = await response.json();
            if (!result.success) {
                throw new Error(result.error);
            }
            
            this.progressData = result.data.progress;
            this.acceptanceData = result.data.acceptance;
            this.renderDashboard();
            this.hideLoading();
            this.showSuccess('数据刷新成功');
        } catch (error) {
            console.error('刷新数据失败:', error);
            this.hideLoading();
            this.showError('刷新数据失败');
        }
    }
    
    // 显示加载状态
    showLoading() {
        document.getElementById('loading').style.display = 'block';
    }
    
    // 隐藏加载状态
    hideLoading() {
        document.getElementById('loading').style.display = 'none';
    }
    
    // 显示成功消息
    showSuccess(message) {
        this.showMessage(message, 'success');
    }
    
    // 显示错误消息
    showError(message) {
        this.showMessage(message, 'danger');
    }
    
    // 显示消息
    showMessage(message, type) {
        const alertDiv = document.createElement('div');
        alertDiv.className = `alert alert-${type} alert-dismissible fade show`;
        alertDiv.innerHTML = `
            ${message}
            <button type="button" class="btn-close" data-bs-dismiss="alert"></button>
        `;
        
        const container = document.querySelector('.container-fluid');
        if (container) {
            container.insertBefore(alertDiv, container.firstChild);
            
            // 3秒后自动消失
            setTimeout(() => {
                if (alertDiv.parentNode) {
                    alertDiv.remove();
                }
            }, 3000);
        }
    }

    setupEventListeners() {
        // 设置键盘快捷键
        document.addEventListener('keydown', (e) => {
            if (e.ctrlKey && e.key === 'r') {
                e.preventDefault();
                this.refreshData();
            }
        });
    }

    showToast(message, type = 'info') {
        // 简单的toast通知实现
        const toast = document.createElement('div');
        toast.className = `alert alert-${type} alert-dismissible fade show position-fixed`;
        toast.style.cssText = 'top: 20px; right: 20px; z-index: 1050; min-width: 300px;';
        toast.innerHTML = `
            ${message}
            <button type="button" class="btn-close" data-bs-dismiss="alert"></button>
        `;
        
        document.body.appendChild(toast);
        
        // 自动移除
        setTimeout(() => {
            if (toast.parentNode) {
                toast.parentNode.removeChild(toast);
            }
        }, 3000);
    }

    showError(message) {
        this.showToast(message, 'danger');
    }
}

// 全局函数
function refreshData() {
    if (window.dashboard) {
        window.dashboard.refreshData();
    }
}

async function generateReport(format = 'json') {
    // 生成报告功能
    if (window.dashboard && window.dashboard.progressData && window.dashboard.acceptanceData) {
        const reportData = {
            timestamp: new Date().toISOString(),
            progress: window.dashboard.progressData,
            acceptance: window.dashboard.acceptanceData,
            summary: {
                overall_progress: window.dashboard.progressData.overall_progress || 0,
                current_week: window.dashboard.progressData.current_week || 3,
                total_tasks: window.dashboard.progressData.total_tasks || 0,
                completed_tasks: window.dashboard.progressData.completed_tasks || 0,
                in_progress_tasks: window.dashboard.progressData.in_progress_tasks || 0,
                pending_tasks: window.dashboard.progressData.pending_tasks || 0,
                acceptance_score: window.dashboard.acceptanceData.overall_score || 0,
                passed_tasks: window.dashboard.acceptanceData.passed_tasks || 0,
                total_acceptance_tasks: window.dashboard.acceptanceData.total_tasks || 0
            }
        };
        
        if (format === 'json') {
            // JSON格式报告
            const dataStr = JSON.stringify(reportData, null, 2);
            const blob = new Blob([dataStr], { type: 'application/json' });
            const url = URL.createObjectURL(blob);
            const a = document.createElement('a');
            a.href = url;
            a.download = `progress_report_${new Date().toISOString().split('T')[0]}.json`;
            a.click();
            URL.revokeObjectURL(url);
            
            window.dashboard.showToast('JSON报告已生成', 'success');
        } else if (format === 'word') {
            // Word格式报告
            try {
                window.dashboard.showLoading();
                const response = await fetch(`${window.dashboard.apiBaseUrl}/generate-word-report`, {
                    method: 'POST',
                    headers: {
                        'Content-Type': 'application/json',
                    },
                    body: JSON.stringify(reportData)
                });
                
                if (!response.ok) {
                    throw new Error(`HTTP error! status: ${response.status}`);
                }
                
                const result = await response.json();
                if (result.success && result.download_url) {
                    // 下载Word文档
                    const a = document.createElement('a');
                    a.href = result.download_url;
                    a.download = `progress_report_${new Date().toISOString().split('T')[0]}.docx`;
                    a.click();
                    
                    window.dashboard.showToast('Word报告已生成', 'success');
                } else {
                    throw new Error(result.error || '生成Word报告失败');
                }
            } catch (error) {
                console.error('生成Word报告失败:', error);
                window.dashboard.showError('生成Word报告失败');
            } finally {
                window.dashboard.hideLoading();
            }
        }
    } else {
        if (window.dashboard) {
            window.dashboard.showError('无法生成报告：数据未加载完成');
        }
    }
}

function filterTasks(status) {
    const taskItems = document.querySelectorAll('.task-item');
    const buttons = document.querySelectorAll('.btn-group .btn');
    
    // 更新按钮状态
    buttons.forEach(btn => btn.classList.remove('active'));
    event.target.classList.add('active');
    
    // 过滤任务
    taskItems.forEach(item => {
        if (status === 'all' || item.dataset.status === status) {
            item.style.display = 'block';
        } else {
            item.style.display = 'none';
        }
    });
}

function toggleTaskDetails(taskId) {
    const details = document.getElementById(`task-details-${taskId}`);
    const button = event.target;
    
    if (details.style.display === 'none') {
        details.style.display = 'block';
        button.innerHTML = '<i class="fas fa-chevron-up"></i> 隐藏详情';
    } else {
        details.style.display = 'none';
        button.innerHTML = '<i class="fas fa-chevron-down"></i> 查看详情';
    }
}

// 初始化仪表板
document.addEventListener('DOMContentLoaded', () => {
    window.dashboard = new ProgressDashboard();
});