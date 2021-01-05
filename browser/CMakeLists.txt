cmake_minimum_required(VERSION 3.16)
project(Browser)
set(Qt5_DIR "~/Qt/5.13.2/gcc_64/lib/cmake/Qt5")
find_package(Qt5 COMPONENTS Core Widgets Multimedia WebEngine WebEngineWidgets REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTOUIC ON)
SET(CMAKE_AUTORCC ON)
set(SOURCES
        src/main.cpp
        src/jimberx264browser.cpp
        src/jimberx264webview.cpp
        src/jimberx264webpage.cpp
        src/jxfileupload.cpp
        src/jxfiledownload.cpp
        src/jpopupwindow.cpp
        src/socketcommunication.h
        )

#target_include_directories(src)
include_directories(~/Qt/video/src/webenginewidgets/api)
add_executable(browser ${SOURCES})
target_link_libraries(browser Qt5::Core Qt5::Widgets Qt5::WebEngine Qt5::WebEngineWidgets)