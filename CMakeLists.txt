cmake_minimum_required(VERSION 3.20)
project(software-editor)
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Core Gui Widgets Xml Test OpenGLWidgets REQUIRED)

include_directories(src)

add_executable(software-editor
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/mainwindow.ui
    src/product.cpp
    src/product.h
    src/productconfigmanager.cpp
    src/productconfigmanager.h
    src/uilayoutwindow.cpp
    src/uilayoutwindow.h
    src/uilayoutwindow.ui
    src/previewwindow.cpp
    src/previewwindow.h
    src/previewwindow.ui
    src/editareawidget.cpp
    src/editareawidget.h
    src/customtreewidget.cpp
    src/customtreewidget.h
    src/packagedialog.cpp
    src/packagedialog.h
    src/packageconfig.cpp
    src/packageconfig.h
    src/packagemanager.cpp
    src/packagemanager.h
    src/smartpackagedialog.cpp
    src/smartpackagedialog.h
    src/smartpackageconfig.cpp
    src/smartpackageconfig.h
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
