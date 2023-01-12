cmake_minimum_required (VERSION 3.16)

project(DAQ)


file(GLOB LIB_SRC_FILES *.cc *.cpp *.c)
file(GLOB HEADER_FILES *.h *.hh *.hpp)

add_library(${PROJECT_NAME} SHARED)
target_sources(${PROJECT_NAME} PRIVATE ${LIB_SRC_FILES})


