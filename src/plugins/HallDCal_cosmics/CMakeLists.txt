cmake_minimum_required (VERSION 3.16)

project(HallDCal_cosmics)


file(GLOB LIB_SRC_FILES *.cc *.cpp *.c)
file(GLOB HEADER_FILES *.h *.hh *.hpp)

add_library(${PROJECT_NAME} SHARED)
target_sources(${PROJECT_NAME} PRIVATE ${LIB_SRC_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
target_link_libraries(${PROJECT_NAME} HallD)


install(TARGETS ${PROJECT_NAME} DESTINATION ${PLUGIN_OUTPUT_DIRECTORY})
