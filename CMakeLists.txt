cmake_minimum_required(VERSION 3.5)

project(mattermost-client LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

find_package(Qt5 COMPONENTS Network REQUIRED)
include_directories(${Qt5Network_INCLUDE_DIRS})

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

target_link_libraries(mattermost-client-qt PRIVATE Qt5::Widgets)
target_link_libraries(mattermost-client-qt PRIVATE Qt5::Network)
