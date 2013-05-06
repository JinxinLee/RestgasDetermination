# Create a library called "" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES} 
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/pnddata 
${CMAKE_SOURCE_DIR}/hypGe
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(HYPGE_SRCS
PndGeoHypGe.cxx 
PndHypGe.cxx 
PndGeoHypGePar.cxx 
PndHypGePoint.cxx 
PndHypGeHit.cxx 
PndHypGeHitProducerIdeal.cxx 
PndHypGeContFact.cxx 
PndHypGeReader.cxx 
GeCluster.cxx
PndHypGeIdealTrackFinderTask.cxx 
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h HYPGE_HEADERS "${HYPGE_SRCS}")

set(HYPGE_LINKDEF  HypGeLinkDef.h)
set(HYPGE_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/PndHypGeDict.cxx) 

ROOT_GENERATE_DICTIONARY("${HYPGE_HEADERS}" "${HYPGE_LINKDEF}" "${HYPGE_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(HYPGE_SRCS ${HYPGE_SRCS} ${HYPGE_DICTIONARY})

add_library(HypGe SHARED ${HYPGE_SRCS})
target_link_libraries(HypGe ${ROOT_LIBRARIES})
set_target_properties(HypGe PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

################ install ###################
install(TARGETS HypGe DESTINATION ${CMAKE_BINARY_DIR}/lib)
