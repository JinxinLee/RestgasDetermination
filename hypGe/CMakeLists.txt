# Create a library called "" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES} 
${SIMPATH}/include
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/pnddata 
${CMAKE_SOURCE_DIR}/hypGe
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
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

set(LINKDEF  HypGeLinkDef.h)
set(LIBRARY_NAME HypGe)

set(DEPENDENCIES Base GeoBase ParBase PndData)

GENERATE_LIBRARY()
