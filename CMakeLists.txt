cmake_minimum_required(VERSION 3.10)

project(fmr)

set(CMAKE_CXX_COMPILER g++)
set(wxWidgets_CONFIG_OPTIONS --prefix=/usr/local)

find_package(wxWidgets REQUIRED base core)
include(${wxWidgets_USE_FILE})
include_directories(src)
set(wxWidgets_LIBRARIES ${wxWidgets_LIBRARIES})

add_subdirectory(src)

add_executable(fmr src/app.cpp)
target_link_libraries(fmr frame)
