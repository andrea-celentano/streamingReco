cmake_minimum_required (VERSION 3.16)

project(DAQ)


file(GLOB LIB_SRC_FILES *.cc )
file(GLOB HEADER_FILES *.h )

add_library(${PROJECT_NAME} SHARED)
target_sources(${PROJECT_NAME} PRIVATE ${LIB_SRC_FILES} ${HEADER_FILES})
target_link_directories(${PROJECT_NAME} PRIVATE ${TRIDAS_HOME}/../build/tools)
target_link_libraries(${PROJECT_NAME} ${JANA_LIBRARY} ptfile_reader FT)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

