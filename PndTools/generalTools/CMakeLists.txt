# Create a library called "generalTools" which includes the source files given in
# the array .
# The header extension is already found. Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CLHEP_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES} 
${SIMPATH}/include
${CMAKE_SOURCE_DIR}/pnddata 
${CMAKE_SOURCE_DIR}/pnddata/TrackData
${CMAKE_SOURCE_DIR}/trackbase
${CMAKE_SOURCE_DIR}/PndTools/generalTools
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
PndFileNameCreator.cxx
PndStringSeparator.cxx
PndGeoHandling.cxx
PndMcCloner.cxx
PndSensorNameContFact.cxx
PndSensorNamePar.cxx
PndBlackBoxTask.cxx
PndEventCounterTask.cxx
PndTrackArrayMerger.cxx
)

set(LINKDEF  generalToolsLinkDef.h)
set(LIBRARY_NAME generalTools)

set(DEPENDENCIES Base ParBase PndData)

GENERATE_LIBRARY()
