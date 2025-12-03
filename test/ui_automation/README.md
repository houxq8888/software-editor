# UI自动化测试框架

这是一个使用Python和pyautogui库编写的UI自动化测试框架，用于黑盒测试UI交互逻辑问题。

## 目录结构
```Tab
ui_automation/
├── config.py          # 配置文件
├── utils.py           # 工具模块
├── test_ui.py         # 测试脚本
├── screenshots/       # 截图目录
└── README.md          # 说明文档
```

## 安装依赖
```
pip install pyautogui pytest
```

## 配置文件

在`config.py`中配置应用程序信息和UI元素坐标：

```python
APP_INFO = {
    "executable_path": "../../build/software-editor.exe",
    "window_title": "software-editor",
    "load_time": 5  # 应用程序加载时间（秒）
}

UI_ELEMENTS = {
    "buttons": {
        "ok_button": (100, 100),
        "cancel_button": (200, 100),
        "preview_button": (300, 100)
    },
    # 其他UI元素配置
}
```

## 运行测试

### 运行所有测试
```
pytest test_ui.py -v
```

### 运行特定测试
```
pytest test_ui.py::test_main_window_title -v
```

### 生成测试报告
```
pytest test_ui.py -v --html=report.html
```

## 注意事项

1. **坐标调整**：UI元素的坐标需要根据实际UI布局进行调整
2. **应用程序路径**：确保应用程序路径正确
3. **等待时间**：根据应用程序的实际加载时间调整等待时间
4. **截图功能**：截图功能默认开启，可以在`config.py`中关闭
5. **分辨率**：确保测试时的屏幕分辨率与配置的坐标一致

## 示例测试

```python
import pytest
import pyautogui
import time
from config import APP_INFO, UI_ELEMENTS
from utils import take_screenshot

def setup_module():
    """在模块开始前执行的设置"""
    print("启动应用程序...")
    os.startfile(APP_INFO["executable_path"])
    time.sleep(APP_INFO["load_time"])

def teardown_module():
    """在模块结束后执行的清理"""
    print("关闭应用程序...")
    pyautogui.hotkey("alt", "f4")
    time.sleep(2)

def test_ok_button_click():
    """测试OK按钮点击功能"""
    # 移动鼠标到OK按钮位置并点击
    ok_button_pos = UI_ELEMENTS["buttons"]["ok_button"]
    pyautogui.moveTo(ok_button_pos, duration=0.5)
    pyautogui.click()
    
    # 等待操作完成
    time.sleep(1)
    
    # 拍摄截图
    take_screenshot("ok_button_click")
TabTab
    # 验证操作结果
    # ...
```

## 常见问题

### 1. 应用程序无法启动
确保应用程序路径正确，并且应用程序已经编译完成。

### 2. 元素定位失败
检查UI元素的坐标是否正确，或者使用图像识别的方法定位元素。

### 3. 测试执行速度过快
在适当的位置添加`time.sleep()`等待时间。

### 4. 截图无法保存
确保截图目录存在，或者有足够的写入权限。

## 技术支持

如有问题，请联系开发团队。
