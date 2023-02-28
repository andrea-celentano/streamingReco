cmake_minimum_required (VERSION 3.16)

project(EIC_5x5)


file(GLOB LIB_SRC_FILES *.cc *.cpp *.c)
file(GLOB HEADER_FILES *.h *.hh *.hpp)

add_library(${PROJECT_NAME} SHARED)
target_sources(${PROJECT_NAME} PRIVATE ${LIB_SRC_FILES} ${HEADER_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/plugins)
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
target_link_directories(${PROJECT_NAME} PRIVATE ${TRIDAS_HOME}/../build/tools)
target_link_libraries(${PROJECT_NAME} DAQ HallD streamingreco_system TT FT )


install(TARGETS ${PROJECT_NAME} DESTINATION ${PLUGIN_OUTPUT_DIRECTORY})
