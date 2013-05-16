# Create a library called "" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${GEANT3_INCLUDE_DIR}
${CLHEP_INCLUDE_DIR}
${BASE_INCLUDE_DIRECTORIES} 
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/pnddata 
${CMAKE_SOURCE_DIR}/pnddata/SciTData
${CMAKE_SOURCE_DIR}/scitil
${CMAKE_SOURCE_DIR}/genfit
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
PndGeoSciT.cxx 
PndSciT.cxx 
PndGeoSciTPar.cxx 
PndSciTContFact.cxx
PndSciTHitProducerIdeal.cxx  
#PndSciTAnaIdeal.cxx
#PndSciTMassTrig.cxx
)

set(LINKDEF  SciTLinkDef.h)
set(LIBRARY_NAME SciT)

set(DEPENDENCIES Base GeoBase ParBase PndData AnalysisTools)

GENERATE_LIBRARY()
