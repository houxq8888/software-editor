#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
产品加载器
在软件启动时加载产品配置并执行文件验证
"""

import os
import sys
import json
import logging
from datetime import datetime
from typing import Dict, List, Any, Optional

# 添加脚本目录到路径，以便导入file_validator
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

from file_validator import FileValidator


class ProductLoader:
    """产品加载器类"""
    
    def __init__(self, configurations_path: str = "product_configurations"):
        """
        初始化产品加载器
        
        Args:
            configurations_path: 产品配置文件夹的路径
        """
        self.configurations_path = configurations_path
        self.logger = self._setup_logger()
        self.validator = None
        
    def _setup_logger(self) -> logging.Logger:
        """设置日志记录器"""
        logger = logging.getLogger('ProductLoader')
        if not logger.handlers:
            handler = logging.StreamHandler()
            formatter = logging.Formatter(
                '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
            )
            handler.setFormatter(formatter)
            logger.addHandler(handler)
            logger.setLevel(logging.INFO)
        return logger
    
    def discover_products(self) -> List[str]:
        """
        发现可用的产品配置
        
        Returns:
            产品配置文件的路径列表
        """
        product_configs = []
        
        if not os.path.exists(self.configurations_path):
            self.logger.warning(f"产品配置文件夹不存在: {self.configurations_path}")
            return product_configs
        
        try:
            # 遍历产品配置文件夹
            for item in os.listdir(self.configurations_path):
                product_dir = os.path.join(self.configurations_path, item)
                if os.path.isdir(product_dir):
                    config_file = os.path.join(product_dir, "product_config.json")
                    if os.path.exists(config_file):
                        product_configs.append(config_file)
                        self.logger.info(f"发现产品配置: {config_file}")
        
        except Exception as e:
            self.logger.error(f"发现产品配置时发生错误: {str(e)}")
        
        return product_configs
    
    def load_product(self, config_path: str) -> Dict[str, Any]:
        """
        加载产品配置并执行文件验证
        
        Args:
            config_path: 产品配置文件的路径
            
        Returns:
            加载结果字典
        """
        try:
            # 加载产品配置
            with open(config_path, 'r', encoding='utf-8') as f:
                config = json.load(f)
            
            product_info = config.get("productInfo", {})
            product_name = product_info.get("name", "Unknown")
            product_id = product_info.get("uniqueId", "Unknown")
            
            self.logger.info(f"开始加载产品: {product_name} ({product_id})")
            
            # 设置验证器的基础路径
            base_path = os.path.dirname(config_path)
            self.validator = FileValidator(base_path)
            
            # 执行文件验证
            validation_result = self.validator.validate_product_config(config_path)
            
            # 根据验证结果决定下一步操作
            overall_status = validation_result.get("overallStatus", "unknown")
            
            if overall_status == "valid":
                self.logger.info("所有文件验证通过，产品加载成功")
                return {
                    "status": "success",
                    "productInfo": product_info,
                    "config": config,
                    "validationResult": validation_result,
                    "message": "产品加载成功"
                }
            
            elif overall_status == "incomplete":
                missing_files = validation_result.get("missingFiles", [])
                self.logger.warning(f"文件验证不完整，缺失 {len(missing_files)} 个文件")
                
                return {
                    "status": "incomplete",
                    "productInfo": product_info,
                    "config": config,
                    "validationResult": validation_result,
                    "missingFiles": missing_files,
                    "message": f"产品加载不完整，缺失 {len(missing_files)} 个文件"
                }
            
            else:
                self.logger.error("文件验证失败")
                return {
                    "status": "error",
                    "productInfo": product_info,
                    "config": config,
                    "validationResult": validation_result,
                    "message": "产品加载失败"
                }
        
        except Exception as e:
            self.logger.error(f"加载产品时发生错误: {str(e)}")
            return {
                "status": "error",
                "message": f"加载产品时发生错误: {str(e)}"
            }
    
    def prompt_missing_files(self, missing_files: List[str]) -> None:
        """
        提示用户缺失的文件
        
        Args:
            missing_files: 缺失的文件路径列表
        """
        if not missing_files:
            return
        
        print("\n" + "=" * 60)
        print("⚠️  文件缺失警告")
        print("=" * 60)
        print("以下文件在验证过程中被发现缺失:")
        
        for i, file_path in enumerate(missing_files, 1):
            print(f"{i}. {file_path}")
        
        print("\n请确保这些文件存在，然后重新启动软件。")
        print("=" * 60)
    
    def start_application(self, product_config_path: str = None) -> bool:
        """
        启动应用程序的主流程
        
        Args:
            product_config_path: 指定的产品配置文件路径，如果为None则自动发现
            
        Returns:
            是否成功启动
        """
        self.logger.info("开始启动应用程序...")
        
        # 发现产品配置
        if product_config_path is None:
            product_configs = self.discover_products()
            if not product_configs:
                self.logger.error("未发现任何产品配置")
                return False
            
            # 使用第一个找到的产品配置
            product_config_path = product_configs[0]
        
        # 加载产品
        load_result = self.load_product(product_config_path)
        
        # 处理加载结果
        status = load_result.get("status")
        
        if status == "success":
            self.logger.info("应用程序启动成功")
            
            # 显示验证报告
            if self.validator:
                validation_result = load_result.get("validationResult", {})
                report = self.validator.generate_validation_report(validation_result)
                print("\n" + report)
            
            return True
        
        elif status == "incomplete":
            self.logger.warning("应用程序启动不完整")
            
            # 显示验证报告
            if self.validator:
                validation_result = load_result.get("validationResult", {})
                report = self.validator.generate_validation_report(validation_result)
                print("\n" + report)
            
            # 提示缺失文件
            missing_files = load_result.get("missingFiles", [])
            self.prompt_missing_files(missing_files)
            
            return False
        
        else:
            self.logger.error("应用程序启动失败")
            return False


def main():
    """主函数，演示产品加载流程"""
    # 创建产品加载器
    loader = ProductLoader("product_configurations")
    
    # 启动应用程序
    success = loader.start_application()
    
    if success:
        print("\n🎉 应用程序已成功启动！")
        print("所有文件验证通过，可以正常使用。")
    else:
        print("\n❌ 应用程序启动失败。")
        print("请检查缺失的文件并重新启动。")
    
    return success


if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)