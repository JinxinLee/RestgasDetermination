# Create a library called "generalTools" which includes the source files given in
# the array .
# The header extension is already found. Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES} 
  ${BASE_INCLUDE_DIRECTORIES} 
  ${CLHEP_INCLUDE_DIR} 
)

Set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/pnddata 
  ${CMAKE_SOURCE_DIR}/pnddata/TrackData
  ${CMAKE_SOURCE_DIR}/pnddata/PidData
  ${CMAKE_SOURCE_DIR}/trackbase
  ${CMAKE_SOURCE_DIR}/PndTools/generalTools
)

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

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
