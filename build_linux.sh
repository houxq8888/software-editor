#!/bin/bash

# software-editor Linux构建脚本
# 此脚本用于在Linux系统上构建项目

echo "开始构建software-editor..."

# 创建构建目录
if [ ! -d "build_linux" ]; then
    mkdir build_linux
fi

cd build_linux

# 运行CMake配置
echo "运行CMake配置..."
cmake .. -DCMAKE_BUILD_TYPE=Debug

# 检查CMake配置结果
if [ $? -ne 0 ]; then
    echo "CMake配置失败！"
    exit 1
fi

# 编译项目
echo "开始编译..."
make -j$(nproc)

# 检查编译结果
if [ $? -ne 0 ]; then
    echo "编译失败！"
    exit 1
fi

echo "构建完成！"
echo ""
echo "可执行文件位置:"
echo "- 主程序: build_linux/software-editor"
echo "- 示例程序: build_linux/wizardexample"
echo ""
echo "运行程序:"
echo "./build_linux/software-editor"