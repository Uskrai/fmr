
find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(fmr_fs_test fs_test.cpp)
target_link_libraries(fmr_fs_test GTest::GTest GTest::Main fmr_fs gmock)
target_include_directories(fmr_fs_test PRIVATE ../include)
gtest_discover_tests(fmr_fs_test)
