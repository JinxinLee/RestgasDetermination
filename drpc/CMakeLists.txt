# Create a library called "" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${GEANT3_INCLUDE_DIR}
${CLHEP_INCLUDE_DIR}
${CMAKE_SOURCE_DIR}/fairtools
${CMAKE_SOURCE_DIR}/geobase 
${CMAKE_SOURCE_DIR}/parbase 
${CMAKE_SOURCE_DIR}/base 
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/pnddata 
${CMAKE_SOURCE_DIR}/pnddata/DrpcData
${CMAKE_SOURCE_DIR}/drpc
${CMAKE_SOURCE_DIR}/genfit
${CMAKE_SOURCE_DIR}/recotasks
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(DRPC_SRCS
PndDrpcAnalysisMaterial.cxx
PndDrpc.cxx
PndDrpcParam.cxx
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${DRPC_SRCS}" "${INCLUDE_DIRECTORIES}" DRPC_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h DRPC_HEADERS "${DRPC_SRCS}")

set(DRPC_LINKDEF  DrpcLinkDef.h)
set(DRPC_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/PndDrpcDict.cxx) 

ROOT_GENERATE_DICTIONARY("${DRPC_HEADERS}" "${DRPC_LINKDEF}" "${DRPC_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(DRPC_SRCS ${DRPC_SRCS} ${DRPC_DICTIONARY})

add_library(Drpc SHARED ${DRPC_SRCS})
target_link_libraries(Drpc ${ROOT_LIBRARIES})
set_target_properties(Drpc PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS Drpc DESTINATION ${CMAKE_BINARY_DIR}/lib)
