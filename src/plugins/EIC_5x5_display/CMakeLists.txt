cmake_minimum_required (VERSION 3.16)

project(EIC_5x5_display)


file(GLOB LIB_SRC_FILES *.cc)
file(GLOB HEADER_FILES *.h)

add_library(${PROJECT_NAME} SHARED)
target_sources(${PROJECT_NAME} PRIVATE ${LIB_SRC_FILES} ${HEADER_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/plugins)
target_link_directories(${PROJECT_NAME} PRIVATE ${CCDB_LIBDIR} /usr/lib64/mysql)
target_link_directories(${PROJECT_NAME} PRIVATE ${TRIDAS_HOME}/../build/tools)
target_link_libraries(${PROJECT_NAME} ptfile_reader DAQ FT TT HallD streamingreco_system CCDB_lib CCDB_sqlite mysqlclient)

install(TARGETS ${PROJECT_NAME} DESTINATION ${PLUGIN_OUTPUT_DIRECTORY})
