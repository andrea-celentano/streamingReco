cmake_minimum_required (VERSION 3.16)

project(PTFileSource)

find_package(JANA REQUIRED)

file(GLOB LIB_SRC_FILES *.cc)
file(GLOB HEADER_FILES *.h)


add_library(${PROJECT_NAME} SHARED)
target_sources(${PROJECT_NAME} PRIVATE ${LIB_SRC_FILES} ${HEADER_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
target_include_directories(${PROJECT_NAME} PRIVATE $ENV{CCDB_HOME}/include)
target_link_directories(${PROJECT_NAME} PRIVATE ${CCDB_LIBDIR} /usr/lib64/mysql)
target_link_directories(${PROJECT_NAME} PRIVATE ${TRIDAS_HOME}/../build/tools)
target_link_libraries(${PROJECT_NAME} ${JANA_LIBRARY} ptfile_reader DAQ FT TT streamingreco_system CCDB_lib CCDB_sqlite mysqlclient)

set_target_properties(${PROJECT_NAME} PROPERTIES INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib)

install(TARGETS ${PROJECT_NAME} DESTINATION ${PLUGIN_OUTPUT_DIRECTORY})
