# Create a library called "" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CMAKE_SOURCE_DIR}/geobase 
${CMAKE_SOURCE_DIR}/base 
${CMAKE_SOURCE_DIR}/parbase 
${CMAKE_SOURCE_DIR}/field
${CMAKE_SOURCE_DIR}/pnddata 
${CMAKE_SOURCE_DIR}/pnddata/RpcData
${CMAKE_SOURCE_DIR}/rpc
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(RPC_SRCS
PndRpcGeoPar.cxx
PndRpcContFact.cxx
PndRpcGeo.cxx
PndRpcDetector.cxx
PndRpcHitProducerIdeal.cxx
PndRpcHitProducer.cxx
PndRpcSimpleReco.cxx
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h RPC_HEADERS "${RPC_SRCS}")

set(RPC_LINKDEF  RpcLinkDef.h)
set(RPC_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/RpcDict.cxx) 

ROOT_GENERATE_DICTIONARY("${RPC_HEADERS}" "${RPC_LINKDEF}" "${RPC_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(RPC_SRCS ${RPC_SRCS} ${RPC_DICTIONARY})

add_library(Rpc SHARED ${RPC_SRCS})
target_link_libraries(Rpc ${ROOT_LIBRARIES})
set_target_properties(Rpc PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS Rpc DESTINATION ${CMAKE_BINARY_DIR}/lib)
