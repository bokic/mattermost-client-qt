cmake_minimum_required(VERSION 3.5)

project(mattermost-client VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS
        Widgets
        Network
)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network)

add_executable(mattermost-client-qt
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    logindialog.cpp
    logindialog.h
    logindialog.ui
    qmattermostbackend.cpp
    qmattermostbackend.h
    resource.qrc
)

target_link_libraries(mattermost-client-qt PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(mattermost-client-qt PRIVATE Qt${QT_VERSION_MAJOR}::Network)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_project()
endif()
