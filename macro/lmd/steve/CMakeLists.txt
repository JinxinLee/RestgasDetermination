# Check if cmake has the required version
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0 FATAL_ERROR)

set(CMAKE_BUILD_TYPE Debug)

set(LINK_DIRECTORIES
$ENV{ROOT_LIBRARY_DIR}
$ENV{SIMPATH}/lib
)

set(INCLUDE_DIRECTORIES
$ENV{ROOT_INCLUDE_DIR}
$ENV{SIMPATH}/include
)

########################### create config reader lib ##################################

execute_process(COMMAND rootcint -f SimConfigDict.cxx -c PndLmdSimConfigReader.h LinkDef.h)

set(SRCS PndLmdSimConfigReader.cxx SimConfigDict.cxx)

include_directories(${INCLUDE_DIRECTORIES})

link_directories(${LINK_DIRECTORIES})

add_library(SimConfigReader SHARED ${SRCS})

#target_link_libraries(SimConfigReader boost)
