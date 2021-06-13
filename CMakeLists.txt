
cmake_minimum_required(VERSION 3.20)

project(
  FMR
  VERSION 0.1
  DESCRIPTION "An Program to view image in a folder"
  LANGUAGES CXX
)

option(FMR_USE_WX_WIDGETS "Build wxWidgets dependent library")
option(FMR_USE_STD_FILESYSTEM "Build fmr using std::filesystem instead of boost::filesystem and boost::nowide")

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

  set(CMAKE_CXX_EXTENSIONS OFF)

  set_property(GLOBAL PROPERTY USE_FOLDERS ON)

  include(CTest)
  
  find_package(Doxygen)
endif()

add_subdirectory(src)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()


