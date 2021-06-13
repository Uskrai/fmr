
find_package(wxWidgets REQUIRED COMPONENTS core)
include(${wxWidgets_USE_FILE})

add_library(fmr_wx INTERFACE)
target_link_libraries(fmr_wx INTERFACE ${wxWidgets_LIBRARIES})
target_include_directories(fmr_wx INTERFACE ../../include)

add_library(fmr_wx_archive 
    archive/find.cpp 
    archive/archive.cpp)
target_link_libraries(fmr_wx_archive PUBLIC fmr_wx)

target_link_libraries(fmr_wx INTERFACE fmr_wx_archive)

