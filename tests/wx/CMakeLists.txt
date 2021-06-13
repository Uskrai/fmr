
find_package(wxWidgets REQUIRED)
include(${wxWidgets_USE_FILE})


add_executable(wx_archive_test archive_test.cpp)
target_link_libraries(wx_archive_test GTest::GTest GTest::Main gmock fmr_wx)


add_executable_to_test(wx_archive_test)

