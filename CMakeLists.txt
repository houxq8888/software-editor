cmake_minimum_required(VERSION 3.20)
project(software-editor)
set(CMAKE_BUILD_TYPE Debug)
add_compile_definitions(QT_DESIGNER_STATIC)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Core Gui Widgets Xml Test OpenGLWidgets Designer Network REQUIRED)

include_directories(src)
include_directories(src/product)
include_directories(src/uilayout)
include_directories(src/package)
include_directories(src/designer)
include_directories(src/designer/qtgradienteditor)
include_directories(src/designer/findwidget)
include_directories(src/designer/deviceskin)
include_directories(${PROJECT_SOURCE_DIR}/include)
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

# 收集Designer模块所有源文件，排除designer目录下的main.cpp
file(GLOB DESIGNER_SOURCES
    src/designer/*.cpp
    src/designer/*.h
    src/designer/*.ui
    src/designer/*/*.cpp
    src/designer/*/*.h
)
list(REMOVE_ITEM DESIGNER_SOURCES "${PROJECT_SOURCE_DIR}/src/designer/main.cpp")

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
    src/uilayout/statemachineeditor.cpp
    src/uilayout/statemachineeditor.h
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
    
    # 智能打包模块
    src/package/packagemanager.cpp
    src/package/packagemanager.h
    src/package/packageservice.cpp
    src/package/packageservice.h
    src/package/ipackageservice.h
    src/package/packageconfig.cpp
    src/package/packageconfig.h
)

target_link_libraries(software-editor PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Xml Qt6::OpenGLWidgets)

target_link_libraries(software-editor PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Designer
    Qt6::Network
    D:/Qt/6.9.1/mingw_64/lib/libQt6DesignerComponents.a
    Qt6::Xml
)

# Test configuration (暂时移除)
# add_executable(test_menu test_menu.cpp uilayoutwindow.cpp editareawidget.cpp mainwindow.cpp product.cpp previewwindow.cpp customtreewidget.cpp)
# target_link_libraries(test_menu PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Xml Qt6::Test)
# Install configuration
install(TARGETS software-editor
    RUNTIME DESTINATION ${INSTALL_BINDIR}
)
