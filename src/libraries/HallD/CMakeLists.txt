cmake_minimum_required (VERSION 3.16)

project(HallD)


file(GLOB LIB_SRC_FILES *.cc )
file(GLOB HEADER_FILES *.h )

add_library(${PROJECT_NAME} SHARED)
target_sources(${PROJECT_NAME} PRIVATE ${LIB_SRC_FILES} ${HEADER_FILES})


install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

