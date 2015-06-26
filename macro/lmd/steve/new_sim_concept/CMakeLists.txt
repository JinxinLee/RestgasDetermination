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

execute_process(COMMAND rootcint -f PndSimRecoParamsDict.cxx -c PndSimRecoParams.h PndSimRecoParamsReader.h LinkDef.h)

set(SRCS PndSimRecoParams.cxx PndSimRecoParamsReader.cxx PndSimRecoParamsDict.cxx)

include_directories(${INCLUDE_DIRECTORIES})

link_directories(${LINK_DIRECTORIES})

add_library(SimRecoParams SHARED ${SRCS})

#target_link_libraries(SimRecoParams boost)
