cmake_minimum_required(VERSION 3.16)
project(StateMachineTest)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找Qt库
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# 设置包含目录
include_directories(
    ${CMAKE_SOURCE_DIR}/../src
    ${CMAKE_SOURCE_DIR}/../src/uilayout
)

# 添加源文件
set(SOURCES
    statemachine_test.cpp
)

# 添加可执行文件
add_executable(statemachine_test ${SOURCES})

# 链接Qt库
target_link_libraries(statemachine_test Qt6::Core Qt6::Widgets)

# 设置输出目录
set_target_properties(statemachine_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 复制依赖文件（如果需要）
# 添加自定义命令来复制资源文件等