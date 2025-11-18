cmake_minimum_required(VERSION 3.20)
project(software-editor)
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

add_executable(software-editor
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    product.cpp
    product.h
    uilayoutwindow.cpp
    uilayoutwindow.h
)

target_link_libraries(software-editor PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)

target_link_libraries(software-editor PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# Install configuration
install(TARGETS software-editor
    RUNTIME DESTINATION ${INSTALL_BINDIR}
)
