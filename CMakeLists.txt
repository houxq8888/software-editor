cmake_minimum_required(VERSION 3.20)
project(software-editor)
# 设置默认构建类型
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 设置C++标准以确保Qt5和Qt6兼容性
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 跨平台Qt版本检测和配置
message(STATUS "检测到操作系统: ${CMAKE_SYSTEM_NAME}")

find_package(Qt6 COMPONENTS Core Gui Widgets Xml Designer Network UiTools REQUIRED)


if (WIN32)
    # Windows平台使用Qt6
    message(STATUS "Windows平台，使用Qt6")
    add_compile_definitions(QT_DESIGNER_STATIC)
    
    # Windows特定包含目录
    include_directories(D:/Qt/6.9.1/mingw_64/include)
    include_directories(D:/Qt/6.9.1/mingw_64/include/QtWidgets/6.9.1)
    include_directories(D:/Qt/6.9.1/mingw_64/include/QtWidgets/6.9.1/QtWidgets)
    include_directories(D:/Qt/6.9.1/mingw_64/include/QtWidgets/6.9.1/QtWidgets/private)
    include_directories(D:/Qt/6.9.1/mingw_64/include/QtGui/6.9.1)
    include_directories(D:/Qt/6.9.1/mingw_64/include/QtGui/6.9.1/QtGui)
    include_directories(D:/Qt/6.9.1/mingw_64/include/QtGui/6.9.1/QtGui/private)
    include_directories(D:/Qt/6.9.1/mingw_64/include/QtCore/6.9.1)
    include_directories(D:/Qt/6.9.1/mingw_64/include/QtCore/6.9.1/QtCore)
    include_directories(D:/Qt/6.9.1/mingw_64/include/QtCore/6.9.1/QtCore/private)
    
else()
    # Linux/Unix平台优先尝试Qt5
    message(STATUS "Linux平台，使用Qt6")
    add_compile_definitions(QT_DESIGNER_STATIC)

    include_directories(/home/ubunu2204/Qt/6.10.1/gcc_64/include/QtWidgets/6.10.1/QtWidgets)
    include_directories(/home/ubunu2204/Qt/6.10.1/gcc_64/include/QtWidgets/6.10.1)
    include_directories(/home/ubunu2204/Qt/6.10.1/gcc_64/include/QtGui/6.10.1/QtGui)
    include_directories(/home/ubunu2204/Qt/6.10.1/gcc_64/include/QtGui/6.10.1)
    include_directories(/home/ubunu2204/Qt/6.10.1/gcc_64/include/QtCore/6.10.1/QtCore)
    include_directories(/home/ubunu2204/Qt/6.10.1/gcc_64/include/QtCore/6.10.1)
endif()

set(QT_VERSION 6)
set(QT_PREFIX Qt6)

include_directories(src)
include_directories(src/product)
include_directories(src/uilayout)
include_directories(src/package)
include_directories(src/designer)
include_directories(src/designer/qtgradienteditor)
include_directories(src/designer/findwidget)
include_directories(src/designer/deviceskin)
include_directories(${PROJECT_SOURCE_DIR}/include)


# 收集Designer模块所有源文件，排除designer目录下的main.cpp
file(GLOB DESIGNER_SOURCES
    src/designer/*.cpp
    src/designer/*.h
    src/designer/*.ui
    src/designer/*/*.cpp
    src/designer/*/*.h
)
list(REMOVE_ITEM DESIGNER_SOURCES "${PROJECT_SOURCE_DIR}/src/designer/main.cpp")

# Windows应用图标设置
if(WIN32)
    # 注意：要在Windows上设置EXE图标，需要使用ICO格式的图标文件
    # 并通过资源文件(.rc)将其添加到项目中
    # 目前main.cpp中已设置窗口图标，但EXE图标需要ICO格式文件
endif()

add_executable(software-editor
    src/main.cpp
    ${DESIGNER_SOURCES}
    resources.qrc
    
    # 产品配置编辑模块
    src/product/product_mainwindow.cpp
    src/product/product_mainwindow.h
    src/product/product_mainwindow.ui
    src/product/product.cpp
    src/product/product.h
    src/product/productconfigmanager.cpp
    src/product/productconfigmanager.h
    src/product/productstate.cpp
    src/product/productstate.h
    src/product/template.json
    
    # UI布局编辑器模块（可选保留，如果需要兼容旧功能）
    src/uilayout/eventactioneditor.cpp
    src/uilayout/eventactioneditor.h
    src/uilayout/eventactionmodel.cpp
    src/uilayout/eventactionmodel.h
    src/uilayout/uiinterfacemanager.cpp
    src/uilayout/uiinterfacemanager.h
    src/uilayout/uiinterface.cpp
    src/uilayout/uiinterface.h
    src/uilayout/layoutitem.cpp
    src/uilayout/layoutitem.h
    src/uilayout/statemachine.cpp
    src/uilayout/statemachine.h
    src/uilayout/statemachineeditor_v2.cpp
    src/uilayout/statemachineeditor_v2.h
    src/uilayout/statemachineruntime.cpp
    src/uilayout/statemachineruntime.h
    src/uilayout/uiflowstatemachine.cpp
    src/uilayout/uiflowstatemachine.h
    src/uilayout/logicsequencestatemachine.cpp
    src/uilayout/logicsequencestatemachine.h
    src/uilayout/statemachineintegrationmanager.cpp
    src/uilayout/statemachineintegrationmanager.h
    # 向导功能模块
    src/uilayout/wizard.cpp
    src/uilayout/wizard.h
    
    # 智能打包模块
    src/package/packagemanager.cpp
    src/package/packagemanager.h
    src/package/packageservice.cpp
    src/package/packageservice.h
    src/package/ipackageservice.h
    src/package/packageconfig.cpp
    src/package/packageconfig.h
)
if (WIN32)
    target_link_libraries(software-editor PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Designer
        Qt6::Network
        Qt6::UiTools
        D:/Qt/6.9.1/mingw_64/lib/libQt6DesignerComponents.a
        Qt6::Xml
        Qt6::OpenGLWidgets
    )
else()
    target_link_libraries(software-editor PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Designer
        Qt6::Network
        Qt6::UiTools
        /usr/lib/x86_64-linux-gnu/libzstd.so
        /home/ubunu2204/Qt/6.10.1/gcc_64/lib/libQt6DesignerComponents.so
        Qt6::Xml
        Qt6::OpenGLWidgets
    )
endif()

# Install configuration
install(TARGETS software-editor
    RUNTIME DESTINATION ${INSTALL_BINDIR}
)