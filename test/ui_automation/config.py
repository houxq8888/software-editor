# UI自动化测试配置文件

# 应用程序信息
APP_INFO = {
    "path": "d:/virtualMachine/github/software-editor/build/software-editor.exe",
    "executable_path": "d:/virtualMachine/github/software-editor/build/software-editor.exe",
    "window_title": "UI布局编辑器",
    "load_time": 5  # 应用程序加载时间（秒）
}

# UI元素坐标配置
# 注意：这些坐标需要根据实际UI布局进行调整
UI_ELEMENTS = {
    "main_window": {
        "title": "UI布局编辑器"
    },
    "buttons": {
        "ok_button": (500, 300),  # OK按钮坐标
        "cancel_button": (600, 300),  # Cancel按钮坐标
        "preview_button": (700, 300)  # 预览按钮坐标
    },
    "input_fields": {
        "text_input": (400, 150)  # 文本输入框坐标
    },
    "labels": {
        "title_label": (200, 100)  # 标题标签坐标
    },
    "menus": {
        "file_menu": (50, 20),
        "edit_menu": (100, 20),
        "help_menu": (150, 20)
    }
}

# 测试数据配置
TEST_DATA = {
    "text_input": "测试输入内容",
    "name_input": "测试姓名",
    "expected_result": "预期结果"
}

# 截图配置
SCREENSHOT_CONFIG = {
    "enabled": True,
    "directory": "screenshots",
    "format": "png"
}
