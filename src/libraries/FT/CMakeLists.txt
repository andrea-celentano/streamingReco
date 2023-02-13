cmake_minimum_required (VERSION 3.16)

project(FT)

# n.b. this captures everything in the FT directory and the kmeans and hdbscandir subdirectories
file(GLOB LIB_SRC_FILES *.cc */*.cc)
file(GLOB HEADER_FILES *.h */*.h)

add_library(${PROJECT_NAME} SHARED)
target_sources(${PROJECT_NAME} PRIVATE ${LIB_SRC_FILES})

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

