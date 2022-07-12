cmake_minimum_required(VERSION 3.5)

project(qt-kiosk VERSION 1.0 LANGUAGES CXX)

include(GNUInstallDirs)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS WebEngineWidgets)

set(PROJECT_SOURCES
    barbutton.cpp barbutton.h
    barwidget.cpp barwidget.h
    configuration.cpp configuration.h
    main.cpp
    mainwindow.cpp mainwindow.h
    progressbarwidget.cpp progressbarwidget.h
    resettimer.cpp resettimer.h
    tools.cpp tools.h
    userinputeventfilter.cpp userinputeventfilter.h
    webpage.cpp webpage.h
    webview.cpp webview.h
    qt-kiosk.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(qt-kiosk
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET qt-kiosk APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(qt-kiosk SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(qt-kiosk
            ${PROJECT_SOURCES}
        )
    endif()
endif()


target_link_libraries(qt-kiosk PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::WebEngineWidgets)

set_target_properties(qt-kiosk PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

#install(TARGETS qt-kiosk ${INSTALL_TARGETS_DEFAULT_ARGS}
#        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
#        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
#)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(qt-kiosk)
endif()

#target_link_libraries(qt-kiosk PUBLIC
#    Qt::Core
#    Qt::Gui
#    Qt::WebEngineWidgets
#)


# Resources:
#set(qt-kiosk_resource_files
#    "back.png"
#    "back_disabled.png"
#    "forward.png"
#    "forward_disabled.png"
#    "home.png"
#    "home_disabled.png"
#    "icon.png"
#    "refresh.png"
#    "refresh_disabled.png"
#    "stop.png"
#    "stop_disabled.png"
#)
