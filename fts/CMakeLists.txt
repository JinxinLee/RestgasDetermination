# Create a library called "libFts" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${GEANT3_INCLUDE_DIR}
${BASE_INCLUDE_DIRECTORIES} 
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/pnddata
${CMAKE_SOURCE_DIR}/pnddata/FtsData
${CMAKE_SOURCE_DIR}/trackbase
${CMAKE_SOURCE_DIR}/pnddata/TrackData
${CMAKE_SOURCE_DIR}/fts
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
PndGeoFtsPar.cxx
PndGeoFts.cxx
PndFtsDigiPar.cxx
PndFtsHitInfo.cxx
PndFts.cxx
PndFts2.cxx
PndFtsContFact.cxx
PndFtsMapCreator.cxx
PndFtsTube.cxx
PndFtsSingleStraw.cxx
PndFtsHitProducerRealFast.cxx
PndFtsHitProducerIdeal.cxx
PndFtsHitProducerRealFull.cxx
PndFtsTrackerIdeal.cxx
#PndFtsTrackerHough.cxx
#PndFtsMomentumQA.cxx
)

set(LINKDEF  FtsLinkDef.h)
set(LIBRARY_NAME Fts)

set(DEPENDENCIES Base GeoBase ParBase PndData TrkBase)

GENERATE_LIBRARY()
