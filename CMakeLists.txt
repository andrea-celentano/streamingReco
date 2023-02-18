cmake_minimum_required (VERSION 3.16)
cmake_policy(SET CMP0028 NEW)
cmake_policy(SET CMP0074 NEW)

project (streamingReco)

# Include JANA by default
find_package(JANA REQUIRED)

# Include ROOT by default
find_package(ROOT REQUIRED)

# Make C++17 a default
if(NOT "${CMAKE_CXX_STANDARD}")
    set(CMAKE_CXX_STANDARD 17)
endif()

# Enable -fPIC for all targets
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Embedd link paths for shared libraries, plugins, executables
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH True)

# Install to the top directory by default
if( ${CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT} )
    set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR} CACHE PATH "Install in top directory by default" FORCE)
endif()

# Default plugins installation directory is 'plugins'
if(NOT DEFINED PLUGIN_OUTPUT_DIRECTORY)
    set(PLUGIN_OUTPUT_DIRECTORY "${CMAKE_INSTALL_PREFIX}/plugins")
    message(STATUS "${CMAKE_PROJECT_NAME}: Set default PLUGIN_OUTPUT_DIRECTORY")
endif()
message(STATUS "${CMAKE_PROJECT_NAME}: PLUGIN_OUTPUT_DIRECTORY: ${PLUGIN_OUTPUT_DIRECTORY}")

# Default plugins static libraries installation directory is 'lib'
if(NOT DEFINED PLUGIN_LIBRARY_OUTPUT_DIRECTORY)
    set(PLUGIN_LIBRARY_OUTPUT_DIRECTORY "lib")
    message(STATUS "${CMAKE_PROJECT_NAME}: Set default PLUGIN_OUTPUT_DIRECTORY")
endif()
message(STATUS "${CMAKE_PROJECT_NAME}: PLUGIN_LIBRARY_OUTPUT_DIRECTORY: ${PLUGIN_LIBRARY_OUTPUT_DIRECTORY}")


# Check and print what JANA2 is used
find_package(JANA REQUIRED)
message(STATUS "${CMAKE_PROJECT_NAME}: JANA2 CMake   : ${JANA_DIR}")
message(STATUS "${CMAKE_PROJECT_NAME}: JANA2 includes: ${JANA_INCLUDE_DIR}")
message(STATUS "${CMAKE_PROJECT_NAME}: JANA2 library : ${JANA_LIBRARY}")

# TRIDAS_HOME points to install directory. Source is one directory up
# CCDB points to install and source directory which are the same
set(TRIDAS_HOME $ENV{TRIDAS_HOME})
set(CCDB_HOME $ENV{CCDB_HOME})
set (CCDB_INCLUDE_DIR ${CCDB_HOME}/include/)
set (CCDB_LIBDIR ${CCDB_HOME}/lib)

set(INCLUDE_DIRS, "")
list(APPEND INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/libraries ${CMAKE_SOURCE_DIR}/src/external)
list(APPEND INCLUDE_DIRS ${TRIDAS_HOME}/.. ${TRIDAS_HOME}/../build/packages ${TRIDAS_HOME}/../packages/DataFormats ${TRIDAS_HOME}/../packages/utility)
# list(APPEND INCLUDE_DIRS ${TRIDAS_HOME}/.. ${CMAKE_SOURCE_DIR}/src/external/TRIDAS)
list(APPEND INCLUDE_DIRS ${JANA_INCLUDE_DIR} )
list(APPEND INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} )
list(APPEND INCLUDE_DIRS ${CCDB_HOME}/cpp/src )


include_directories(${INCLUDE_DIRS})
add_compile_options(-Wno-deprecated -Wno-deprecated-declarations)

get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
message(STATUS "include directories:'")
foreach(dir ${dirs})
  message(STATUS "     '${dir}'")
endforeach()

add_subdirectory (src/libraries)
add_subdirectory (src/plugins)
# add_subdirectory (src/external/TRIDAS)
add_subdirectory (src/programs/streamingReco)



