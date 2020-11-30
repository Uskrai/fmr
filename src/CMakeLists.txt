cmake_minimum_required(VERSION 3.10)

set(LINK SHARED)


add_library(file ${LINK} file.cpp)
target_link_libraries(file ${wxWidgets_LIBRARIES})

add_library(frame ${LINK} frame.cpp)
target_link_libraries(frame panel "${wxWidgets_LIBRARIES}")

add_library(panel ${LINK} panel.cpp)
target_link_libraries(panel reader "${wxWidgets_LIBRARIES}")

add_library(reader ${LINK} reader.cpp)
target_link_libraries(reader ${wxWidgets_LIBRARIES})

