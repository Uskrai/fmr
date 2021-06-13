enable_testing()

find_package(GTest REQUIRED)
include(GoogleTest)


macro(add_executable_to_test name)
  add_test(NAME ${name} COMMAND $<TARGET_FILE:${name}>)
endmacro()


add_executable(fmr_fs_test fs_test.cpp)
target_link_libraries(fmr_fs_test GTest::GTest GTest::Main fmr_fs gmock)
target_include_directories(fmr_fs_test PRIVATE ../include)

add_executable_to_test(fmr_fs_test)

if(FMR_USE_WX_WIDGETS)
  add_subdirectory(wx)
endif()
