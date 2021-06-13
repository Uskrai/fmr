
add_library(nwd INTERFACE)


if (FMR_USE_STD_FILESYSTEM)
  target_include_directories(nwd INTERFACE ../include/fmr/nowide/std)
  target_compile_features(nwd INTERFACE cxx_std_17)
else()
  find_package(Boost REQUIRED COMPONENTS filesystem nowide)
  target_include_directories(nwd INTERFACE ../include/fmr/nowide/boost)
  target_link_libraries(nwd INTERFACE Boost::filesystem Boost::nowide)
endif()

add_library(fmr_fs
    fs/find.cpp
    fs/filesystem.cpp
)
target_link_libraries(fmr_fs PUBLIC nwd)
target_include_directories(fmr_fs PUBLIC ../include)

if (FMR_USE_WX_WIDGETS)
  add_subdirectory(wx)
endif()
