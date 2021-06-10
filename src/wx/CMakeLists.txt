
find_package(wxWidgets REQUIRED)
include(${wxWidgets_USE_FILE})

add_library(fmr_wx_archive archive/find.cpp)

target_link_libraries(fmr_wx_archive ${wxWidgets_LIBRARIES})
target_include_directories(fmr_wx_archive PUBLIC ../../include)


