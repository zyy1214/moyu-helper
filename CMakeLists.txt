cmake_minimum_required(VERSION 3.5)

project(moyu-helper VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Sql)
find_package(Qt6 REQUIRED COMPONENTS Network)
find_package(Qt6 REQUIRED COMPONENTS Charts)

set(PROJECT_SOURCES
        main.cpp
        recordwindow.cpp
        recordwindow.h
        recordwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(moyu-helper
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        record.h
        record.cpp
        resources.qrc
        uitools.h
        uitools.cpp
        data_storage.h
        data_storage.cpp
        loginwindow.h loginwindow.cpp
        network.h network.cpp
        welcomewindow.h welcomewindow.cpp welcomewindow.ui
        mainwindow.ui
        mainwindow.h mainwindow.cpp mainwindow.ui
        modwindow.cpp
        modwindow.h
        modwindow.ui
        mod.cpp
        mod.h
        calculate.cpp
        calculate.h
        chartwindow.cpp chartwindow.h chartwindow.ui
        qtchart.h
        themewindow.h themewindow.cpp themewindow.ui
        focuswindow.cpp focuswindow.h focuswindow.ui
        colorprovider.h colorprovider.cpp
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET moyu-helper APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(moyu-helper SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(moyu-helper
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(moyu-helper PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(moyu-helper PRIVATE Qt6::Sql)
target_link_libraries(moyu-helper PRIVATE Qt6::Network)
target_link_libraries(moyu-helper PRIVATE Qt6::Charts)

target_link_libraries(moyu-helper PRIVATE Shell32.lib Ole32.lib)


# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.moyu-helper)
endif()
set_target_properties(moyu-helper PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS moyu-helper
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(moyu-helper)
endif()
