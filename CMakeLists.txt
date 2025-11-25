cmake_minimum_required(VERSION 3.20)
project(software-editor)
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Core Gui Widgets Xml Test OpenGLWidgets REQUIRED)

include_directories(src)
include_directories(src/product)
include_directories(src/uilayout)
include_directories(src/package)

add_executable(software-editor
    src/main.cpp
    
    # 产品配置编辑模块
    src/product/mainwindow.cpp
    src/product/mainwindow.h
    src/product/mainwindow.ui
    src/product/product.cpp
    src/product/product.h
    src/product/productconfigmanager.cpp
    src/product/productconfigmanager.h
    src/product/productstate.cpp
    src/product/productstate.h
    src/product/template.json
    
    # UI布局编辑器模块
    src/uilayout/uilayoutwindow.cpp
    src/uilayout/uilayoutwindow.h
    src/uilayout/uilayoutwindow.ui
    src/uilayout/previewwindow.cpp
    src/uilayout/previewwindow.h
    src/uilayout/previewwindow.ui
    src/uilayout/editareawidget.cpp
    src/uilayout/editareawidget.h
    src/uilayout/customtreewidget.cpp
    src/uilayout/customtreewidget.h
    src/uilayout/eventactioneditor.cpp
    src/uilayout/eventactioneditor.h
    src/uilayout/eventactionmodel.cpp
    src/uilayout/eventactionmodel.h
    src/uilayout/uiinterfacemanager.cpp
    src/uilayout/uiinterfacemanager.h
    src/uilayout/uiinterface.cpp
    src/uilayout/uiinterface.h
    src/uilayout/layoutitem.h
    
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
)

# Test configuration (暂时移除)
# add_executable(test_menu test_menu.cpp uilayoutwindow.cpp editareawidget.cpp mainwindow.cpp product.cpp previewwindow.cpp customtreewidget.cpp)
# target_link_libraries(test_menu PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Xml Qt6::Test)
# Install configuration
install(TARGETS software-editor
    RUNTIME DESTINATION ${INSTALL_BINDIR}
)
