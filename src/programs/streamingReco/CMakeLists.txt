cmake_minimum_required (VERSION 3.16)

project(streamingReco)

find_package(JANA REQUIRED)


file(GLOB LIB_SRC_FILES *.cc *.cpp *.c)
file(GLOB HEADER_FILES *.h *.hh *.hpp)


add_executable(${PROJECT_NAME} )
target_sources(${PROJECT_NAME} PRIVATE ${LIB_SRC_FILES})

# Ugghh! For some reason using ${JANA_LIBRARY}(=JANA::jana2) does not work. 
# At least the JANA library does not get linked. (The following does seem
# to add all of the dependencies like dl, pthread, root, ...)
# Resort to hardcoded path. 
target_link_libraries(${PROJECT_NAME} ${JANA_LIB})
target_link_directories(${PROJECT_NAME} PRIVATE /usr/clas12/streaming/JANA2/lib)

message( STATUS 'CCDB_HOME=$ENV{CCDB_HOME}')
target_link_directories(${PROJECT_NAME} PRIVATE $ENV{CCDB_HOME}/lib /usr/lib64/mysql )
target_link_libraries(${PROJECT_NAME} CCDB_lib CCDB_sqlite mysqlclient)

target_link_libraries(${PROJECT_NAME} FT DAQ TT HallD external_TRIDAS JANA streamingreco_system)





