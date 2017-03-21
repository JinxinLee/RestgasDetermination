# Create a library called "PndForwardTrackFinder" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES} 
  ${CLHEP_INCLUDE_DIR} 
  ${BASE_INCLUDE_DIRECTORIES} 
)

Set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/pnddata
  ${CMAKE_SOURCE_DIR}/pnddata/SttData
  ${CMAKE_SOURCE_DIR}/pnddata/FtsData
  ${CMAKE_SOURCE_DIR}/pnddata/SdsData
  ${CMAKE_SOURCE_DIR}/pnddata/TrackData
  ${CMAKE_SOURCE_DIR}/stt
  ${CMAKE_SOURCE_DIR}/field 
  ${CMAKE_SOURCE_DIR}/fts 
  ${CMAKE_SOURCE_DIR}/passive
  ${CMAKE_SOURCE_DIR}/genfit
  ${CMAKE_SOURCE_DIR}/trackbase
  ${CMAKE_SOURCE_DIR}/PndTrackers/riemannfit
  ${CMAKE_SOURCE_DIR}/PndTrackers/SttCellTrackFinder
  ${CMAKE_SOURCE_DIR}/PndTrackers/PndForwardTrackFinder
  ${CMAKE_SOURCE_DIR}/PndTrackers/PndIdealTrackFinder
)

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
PndForwardTrackFinderTask.cxx
PndFtsCellTrackletGenerator.cxx
PndFtsLineApproximator.cxx
PndFtsExpandedTrackCand.cxx
PndFtsLineComparator.cxx
Tuple.cxx
PndLineApproximation.cxx
PndLine.cxx
PndPlane.cxx
PndTrackCombiner.cxx
PndLineCombination.cxx
PndAnalysisForwardTask.cxx
PndModuleCombiner.cxx
PndWayFollower.cxx
PndTrackCollection.cxx
)

set(LINKDEF PndForwardTrackFinderLinkDef.h)
set(LIBRARY_NAME PndForwardTrackFinder)

set(DEPENDENCIES Base PndData TrkBase Stt Fts)

GENERATE_LIBRARY()
