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
#${CMAKE_SOURCE_DIR}/pnddata/FtofData
${CMAKE_SOURCE_DIR}/ftof
${CMAKE_SOURCE_DIR}/genfit
#${CMAKE_SOURCE_DIR}/recotasks
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
PndGeoFtof.cxx 
PndFtof.cxx 
PndFtofPoint.cxx 
PndFtofHit.cxx 
PndGeoFtofPar.cxx 
PndFtofContFact.cxx
PndFtofHitProducerIdeal.cxx  
#PndFtofAnaIdeal.cxx
#PndFtofMassTrig.cxx
)

set(LINKDEF  FtofLinkDef.h)
set(LIBRARY_NAME Ftof)

set(DEPENDENCIES Base GeoBase ParBase PndData)

GENERATE_LIBRARY()
