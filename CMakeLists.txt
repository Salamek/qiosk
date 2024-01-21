cmake_minimum_required(VERSION 3.5)

project(qiosk VERSION 1.4.3 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS WebEngineWidgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS WebSockets)

set(PROJECT_SOURCES
    src/barbutton.cpp src/barbutton.h
    src/barwidget.cpp src/barwidget.h
    src/configuration.cpp src/configuration.h
    src/main.cpp
    src/mainwindow.cpp src/mainwindow.h
    src/progressbarwidget.cpp src/progressbarwidget.h
    src/resettimer.cpp src/resettimer.h
    src/tools.cpp src/tools.h
    src/userinputeventfilter.cpp src/userinputeventfilter.h
    src/webpage.cpp src/webpage.h
    src/webview.cpp src/webview.h
    src/websocketcontrol.h src/websocketcontrol.cpp
    src/qiosk.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(qiosk
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET qiosk APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(qiosk SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(qiosk
            ${PROJECT_SOURCES}
        )
    endif()
endif()


target_link_libraries(qiosk PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::WebEngineWidgets Qt::WebSockets)

set_target_properties(qiosk PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS qiosk DESTINATION bin)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(qiosk)
endif()

