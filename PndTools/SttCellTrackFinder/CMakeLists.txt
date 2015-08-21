# Create a library called "tpcreco" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CLHEP_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES} 
${SIMPATH}/include
${CMAKE_SOURCE_DIR}/pnddata
${CMAKE_SOURCE_DIR}/pnddata/SttData
${CMAKE_SOURCE_DIR}/pnddata/TrackData
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/passive
${CMAKE_SOURCE_DIR}/genfit
${CMAKE_SOURCE_DIR}/trackbase
${CMAKE_SOURCE_DIR}/stt
${CMAKE_SOURCE_DIR}/sttmvdtracking
${CMAKE_SOURCE_DIR}/PndTools/SttCellTrackFinder
${CMAKE_SOURCE_DIR}/PndTools/riemannfit
${CMAKE_SOURCE_DIR}/PndMCMatch
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
PndSttCellTrackFinderTask.cxx
PndSttCellTrackFinder.cxx
PndSttCellTrackFinderData.cxx
PndSttCellTrackletGenerator.cxx
PndSttHitCorrector.cxx
PndSttCellTrackFinderAnalysisTask.cxx
PndSttSkewedCombineTask.cxx
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${CELL_TRACKFINDER_SRCS}" "${INCLUDE_DIRECTORIES}" RULES)
endif(RULE_CHECKER_FOUND)

set(LINKDEF  PndSttCellTrackFinderLinkDef.h)
set(LIBRARY_NAME SttCellTrackFinder)

set(DEPENDENCIES Base PndData TrkBase Stt riemann)

GENERATE_LIBRARY()
