import pyautogui
import time
import os
from config import SCREENSHOT_CONFIG

def take_screenshot(filename):
    """
    拍摄屏幕截图
    :param filename: 截图文件名
    :return: 截图文件路径
    """
    if not SCREENSHOT_CONFIG["enabled"]:
        return None
    
    # 创建截图目录
    screenshot_dir = SCREENSHOT_CONFIG["directory"]
    if not os.path.exists(screenshot_dir):
        os.makedirs(screenshot_dir)
    
    # 生成截图文件路径
    file_path = os.path.join(screenshot_dir, f"{filename}.{SCREENSHOT_CONFIG['format']}")
    
    # 拍摄截图
    pyautogui.screenshot(file_path)
    return file_path

def wait_for_window(title, timeout=10):
    """
    等待窗口出现
    :param title: 窗口标题
    :param timeout: 超时时间（秒）
    :return: 窗口对象，如果超时则返回None
    """
    start_time = time.time()
    while time.time() - start_time < timeout:
        active_window = pyautogui.getActiveWindow()
        if active_window and title in active_window.title:
            return active_window
        time.sleep(0.5)
    return None

def wait_for_element(image_path, confidence=0.8, timeout=10):
    """
    等待元素出现
    :param image_path: 元素图像路径
    :param confidence: 匹配置信度
    :param timeout: 超时时间（秒）
    :return: 元素位置坐标，如果超时则返回None
    """
    start_time = time.time()
    while time.time() - start_time < timeout:
        try:
            location = pyautogui.locateCenterOnScreen(image_path, confidence=confidence)
            if location:
                return location
        except pyautogui.ImageNotFoundException:
            pass
        time.sleep(0.5)
    return None

def click_element(image_path, confidence=0.8, timeout=10):
    """
    点击元素
    :param image_path: 元素图像路径
    :param confidence: 匹配置信度
    :param timeout: 超时时间（秒）
    :return: 是否成功点击
    """
    location = wait_for_element(image_path, confidence, timeout)
    if location:
        pyautogui.click(location)
        return True
    return False

def type_in_element(image_path, text, confidence=0.8, timeout=10):
    """
    在元素中输入文本
    :param image_path: 元素图像路径
    :param text: 要输入的文本
    :param confidence: 匹配置信度
    :param timeout: 超时时间（秒）
    :return: 是否成功输入
    """
    location = wait_for_element(image_path, confidence, timeout)
    if location:
        pyautogui.click(location)
        pyautogui.typewrite(text)
        return True
    return False

def get_element_text(image_path, confidence=0.8, timeout=10):
    """
    获取元素文本
    :param image_path: 元素图像路径
    :param confidence: 匹配置信度
    :param timeout: 超时时间（秒）
    :return: 元素文本，如果失败则返回None
    """
    location = wait_for_element(image_path, confidence, timeout)
    if location:
        # 这里需要根据实际情况调整文本获取方法
        # 例如：使用OCR技术识别文本
        return None
    return None
