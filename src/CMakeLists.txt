cmake_minimum_required(VERSION 3.10)

set(LINK SHARED)

add_library(config ${LINK} config.cpp)
target_link_libraries(config ${wxWidgets_LIBRARIES})

set(wxWidgets_LIBRARIES ${wxWidgets_LIBRARIES} config)

add_library(traverser ${LINK} traverser.cpp)
target_link_libraries(traverser ${wxWidgets_LIBRARIES})

add_library(image ${LINK} reader/image.cpp)
target_link_libraries(image ${wxWidgets_LIBRARIES} traverser)

add_library(reader ${LINK} reader/reader.cpp)
target_link_libraries(reader image ${wxWidgets_LIBRARIES})

add_library(frame ${LINK} frame.cpp)
target_link_libraries(frame panel ${wxWidgets_LIBRARIES})

add_library(panel ${LINK} panel.cpp)
target_link_libraries(panel reader ${wxWidgets_LIBRARIES})