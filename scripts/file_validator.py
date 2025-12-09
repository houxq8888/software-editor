#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
文件验证工具类
用于验证产品配置中的文件是否存在
"""

import os
import json
import logging
from datetime import datetime
from typing import Dict, List, Any, Optional

class FileValidator:
    """文件验证器类"""
    
    def __init__(self, base_path: str):
        """
        初始化文件验证器
        
        Args:
            base_path: 基础路径，通常是产品配置文件夹的路径
        """
        self.base_path = base_path
        self.logger = self._setup_logger()
    
    def _setup_logger(self) -> logging.Logger:
        """设置日志记录器"""
        logger = logging.getLogger('FileValidator')
        if not logger.handlers:
            handler = logging.StreamHandler()
            formatter = logging.Formatter(
                '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
            )
            handler.setFormatter(formatter)
            logger.addHandler(handler)
            logger.setLevel(logging.INFO)
        return logger
    
    def validate_product_config(self, config_path: str) -> Dict[str, Any]:
        """
        验证产品配置文件及其引用的所有文件
        
        Args:
            config_path: 产品配置文件的路径
            
        Returns:
            验证结果字典
        """
        try:
            # 加载产品配置文件
            with open(config_path, 'r', encoding='utf-8') as f:
                config = json.load(f)
            
            self.logger.info(f"开始验证产品配置: {config.get('productInfo', {}).get('name', 'Unknown')}")
            
            # 验证产品配置文件本身
            config_validation = self._validate_config_file(config_path)
            
            # 验证UI布局文件
            ui_validation = self._validate_ui_files(config)
            
            # 验证状态机文件
            state_machine_validation = self._validate_state_machines(config)
            
            # 验证资源文件
            resource_validation = self._validate_resources(config)
            
            # 汇总验证结果
            validation_result = {
                "timestamp": datetime.now().isoformat(),
                "productName": config.get("productInfo", {}).get("name", "Unknown"),
                "productId": config.get("productInfo", {}).get("uniqueId", "Unknown"),
                "configValidation": config_validation,
                "uiValidation": ui_validation,
                "stateMachineValidation": state_machine_validation,
                "resourceValidation": resource_validation,
                "overallStatus": self._determine_overall_status(
                    config_validation, ui_validation, state_machine_validation, resource_validation
                ),
                "missingFiles": self._collect_missing_files(
                    config_validation, ui_validation, state_machine_validation, resource_validation
                )
            }
            
            # 更新配置文件中的验证状态
            self._update_config_validation_status(config_path, validation_result)
            
            self.logger.info(f"验证完成，总体状态: {validation_result['overallStatus']}")
            
            return validation_result
            
        except Exception as e:
            self.logger.error(f"验证产品配置时发生错误: {str(e)}")
            return {
                "timestamp": datetime.now().isoformat(),
                "error": str(e),
                "overallStatus": "error"
            }
    
    def _validate_config_file(self, config_path: str) -> Dict[str, Any]:
        """验证产品配置文件本身"""
        exists = os.path.exists(config_path)
        return {
            "filePath": config_path,
            "exists": exists,
            "status": "valid" if exists else "missing"
        }
    
    def _validate_ui_files(self, config: Dict[str, Any]) -> Dict[str, Any]:
        """验证UI布局文件"""
        ui_layouts_path = os.path.join(
            self.base_path, 
            config.get("fileStructure", {}).get("uiLayoutsPath", "ui_layouts")
        )
        
        ui_files = config.get("uiFiles", [])
        validation_results = []
        missing_files = []
        
        for ui_file in ui_files:
            file_name = ui_file.get("fileName")
            file_path = os.path.join(ui_layouts_path, file_name)
            exists = os.path.exists(file_path)
            
            validation_result = {
                "name": ui_file.get("name"),
                "fileName": file_name,
                "filePath": file_path,
                "exists": exists,
                "status": "valid" if exists else "missing",
                "isMain": ui_file.get("isMain", False)
            }
            
            validation_results.append(validation_result)
            if not exists:
                missing_files.append(file_path)
        
        return {
            "uiLayoutsPath": ui_layouts_path,
            "totalFiles": len(ui_files),
            "validFiles": len([r for r in validation_results if r["exists"]]),
            "missingFiles": missing_files,
            "validationResults": validation_results,
            "status": "valid" if not missing_files else "incomplete"
        }
    
    def _validate_state_machines(self, config: Dict[str, Any]) -> Dict[str, Any]:
        """验证状态机文件"""
        state_machines_path = os.path.join(
            self.base_path,
            config.get("fileStructure", {}).get("stateMachinesPath", "state_machines")
        )
        
        state_machines = config.get("stateMachines", [])
        validation_results = []
        missing_files = []
        
        for state_machine in state_machines:
            file_name = state_machine.get("fileName")
            file_path = os.path.join(state_machines_path, file_name)
            exists = os.path.exists(file_path)
            
            validation_result = {
                "name": state_machine.get("name"),
                "fileName": file_name,
                "filePath": file_path,
                "exists": exists,
                "status": "valid" if exists else "missing"
            }
            
            validation_results.append(validation_result)
            if not exists:
                missing_files.append(file_path)
        
        return {
            "stateMachinesPath": state_machines_path,
            "totalFiles": len(state_machines),
            "validFiles": len([r for r in validation_results if r["exists"]]),
            "missingFiles": missing_files,
            "validationResults": validation_results,
            "status": "valid" if not missing_files else "incomplete"
        }
    
    def _validate_resources(self, config: Dict[str, Any]) -> Dict[str, Any]:
        """验证资源文件"""
        resources_path = os.path.join(
            self.base_path,
            config.get("fileStructure", {}).get("resourcesPath", "resources")
        )
        
        resources = config.get("resources", {})
        validation_results = []
        missing_files = []
        
        for resource_type, file_name in resources.items():
            if file_name:
                file_path = os.path.join(resources_path, file_name)
                exists = os.path.exists(file_path)
                
                validation_result = {
                    "type": resource_type,
                    "fileName": file_name,
                    "filePath": file_path,
                    "exists": exists,
                    "status": "valid" if exists else "missing"
                }
                
                validation_results.append(validation_result)
                if not exists:
                    missing_files.append(file_path)
        
        return {
            "resourcesPath": resources_path,
            "totalFiles": len(resources),
            "validFiles": len([r for r in validation_results if r["exists"]]),
            "missingFiles": missing_files,
            "validationResults": validation_results,
            "status": "valid" if not missing_files else "incomplete"
        }
    
    def _determine_overall_status(self, *validation_results: Dict[str, Any]) -> str:
        """确定总体验证状态"""
        statuses = [result.get("status", "unknown") for result in validation_results]
        
        if "error" in statuses:
            return "error"
        elif any(status == "incomplete" for status in statuses):
            return "incomplete"
        elif all(status == "valid" for status in statuses):
            return "valid"
        else:
            return "unknown"
    
    def _collect_missing_files(self, *validation_results: Dict[str, Any]) -> List[str]:
        """收集所有缺失的文件"""
        missing_files = []
        for result in validation_results:
            missing_files.extend(result.get("missingFiles", []))
        return missing_files
    
    def _update_config_validation_status(self, config_path: str, validation_result: Dict[str, Any]) -> None:
        """更新配置文件中的验证状态"""
        try:
            with open(config_path, 'r', encoding='utf-8') as f:
                config = json.load(f)
            
            # 更新验证信息
            config["validation"] = {
                "lastValidated": validation_result["timestamp"],
                "validationStatus": validation_result["overallStatus"],
                "missingFiles": validation_result["missingFiles"]
            }
            
            # 写回配置文件
            with open(config_path, 'w', encoding='utf-8') as f:
                json.dump(config, f, ensure_ascii=False, indent=2)
                
        except Exception as e:
            self.logger.warning(f"无法更新配置文件验证状态: {str(e)}")
    
    def generate_validation_report(self, validation_result: Dict[str, Any]) -> str:
        """生成验证报告"""
        report = []
        report.append("=" * 60)
        report.append("产品文件验证报告")
        report.append("=" * 60)
        report.append(f"产品名称: {validation_result.get('productName', 'Unknown')}")
        report.append(f"产品ID: {validation_result.get('productId', 'Unknown')}")
        report.append(f"验证时间: {validation_result.get('timestamp', 'Unknown')}")
        report.append(f"总体状态: {validation_result.get('overallStatus', 'Unknown')}")
        report.append("")
        
        # 配置文件验证结果
        config_validation = validation_result.get("configValidation", {})
        report.append("配置文件验证:")
        report.append(f"  文件路径: {config_validation.get('filePath', 'Unknown')}")
        report.append(f"  状态: {config_validation.get('status', 'Unknown')}")
        report.append("")
        
        # UI文件验证结果
        ui_validation = validation_result.get("uiValidation", {})
        report.append("UI布局文件验证:")
        report.append(f"  路径: {ui_validation.get('uiLayoutsPath', 'Unknown')}")
        report.append(f"  总数: {ui_validation.get('totalFiles', 0)}")
        report.append(f"  有效文件: {ui_validation.get('validFiles', 0)}")
        report.append(f"  缺失文件: {len(ui_validation.get('missingFiles', []))}")
        for result in ui_validation.get("validationResults", []):
            status_icon = "✓" if result.get("exists") else "✗"
            main_indicator = " (主文件)" if result.get("isMain") else ""
            report.append(f"  {status_icon} {result.get('name', 'Unknown')}{main_indicator}: {result.get('fileName', 'Unknown')}")
        report.append("")
        
        # 状态机文件验证结果
        state_machine_validation = validation_result.get("stateMachineValidation", {})
        report.append("状态机文件验证:")
        report.append(f"  路径: {state_machine_validation.get('stateMachinesPath', 'Unknown')}")
        report.append(f"  总数: {state_machine_validation.get('totalFiles', 0)}")
        report.append(f"  有效文件: {state_machine_validation.get('validFiles', 0)}")
        report.append(f"  缺失文件: {len(state_machine_validation.get('missingFiles', []))}")
        for result in state_machine_validation.get("validationResults", []):
            status_icon = "✓" if result.get("exists") else "✗"
            report.append(f"  {status_icon} {result.get('name', 'Unknown')}: {result.get('fileName', 'Unknown')}")
        report.append("")
        
        # 资源文件验证结果
        resource_validation = validation_result.get("resourceValidation", {})
        report.append("资源文件验证:")
        report.append(f"  路径: {resource_validation.get('resourcesPath', 'Unknown')}")
        report.append(f"  总数: {resource_validation.get('totalFiles', 0)}")
        report.append(f"  有效文件: {resource_validation.get('validFiles', 0)}")
        report.append(f"  缺失文件: {len(resource_validation.get('missingFiles', []))}")
        for result in resource_validation.get("validationResults", []):
            status_icon = "✓" if result.get("exists") else "✗"
            report.append(f"  {status_icon} {result.get('type', 'Unknown')}: {result.get('fileName', 'Unknown')}")
        report.append("")
        
        # 缺失文件列表
        missing_files = validation_result.get("missingFiles", [])
        if missing_files:
            report.append("缺失文件列表:")
            for file_path in missing_files:
                report.append(f"  - {file_path}")
        else:
            report.append("✓ 所有文件都存在，验证通过！")
        
        report.append("=" * 60)
        
        return "\n".join(report)


def main():
    """主函数，用于测试文件验证器"""
    # 测试文件验证器
    base_path = "product_configurations/product_12345"
    config_path = os.path.join(base_path, "product_config.json")
    
    validator = FileValidator(base_path)
    
    # 执行验证
    validation_result = validator.validate_product_config(config_path)
    
    # 生成并打印报告
    report = validator.generate_validation_report(validation_result)
    print(report)


if __name__ == "__main__":
    main()