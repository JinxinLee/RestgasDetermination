# Create a library called "" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES}
${CMAKE_SOURCE_DIR}/fairtools
${CMAKE_SOURCE_DIR}/field
${CMAKE_SOURCE_DIR}/pnddata 
${CMAKE_SOURCE_DIR}/drc/
${CMAKE_SOURCE_DIR}/pnddata/DrcData
${CMAKE_SOURCE_DIR}/PndTools/generalTools 

)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
PndGeoDrc.cxx 
PndDrc.cxx 
PndGeoDrcPar.cxx 
PndDrcContFact.cxx 
PndDrcHitProducerIdeal.cxx 
PndDrcHitProducerReal.cxx 
PndDrcDigiTask.cxx
PndDrcHitFinder.cxx
DrawHits.cxx 
PndDrcDigiPar.cxx
PndDrcLutFill.cxx
PndDrcLutReco.cxx
)

set(LINKDEF  DrcLinkDef.h)
set(LIBRARY_NAME Drc) 

set(DEPENDENCIES Base GeoBase ParBase PndData)

GENERATE_LIBRARY()
