# Create a library called "libTracking" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

#set(GLPK_INCLUDE_DIR
#${SIMPATH}/basics/glpk/include
#${SIMPATH}/include
#)


set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR}
${GLPK_INCLUDE_DIR}
${BASE_INCLUDE_DIRECTORIES} 
${CMAKE_SOURCE_DIR}/pnddata 
${CMAKE_SOURCE_DIR}/pnddata/SttData
${CMAKE_SOURCE_DIR}/pnddata/SciTData
${CMAKE_SOURCE_DIR}/trackbase
${CMAKE_SOURCE_DIR}/pnddata/TrackData
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/stt
${CMAKE_SOURCE_DIR}/sttmvdtracking
${CMAKE_SOURCE_DIR}/mvd/MvdTracking
${CMAKE_SOURCE_DIR}/pnddata/SdsData
${CMAKE_SOURCE_DIR}/gem 
${CMAKE_SOURCE_DIR}/pnddata/GemData 
${CMAKE_SOURCE_DIR}/geane 
${CMAKE_SOURCE_DIR}/tracking
${CMAKE_SOURCE_DIR}/PndTools/generalTools/
${CMAKE_SOURCE_DIR}/tracking/TrkSecondary
${CMAKE_SOURCE_DIR}/tracking/TrkStructure
${CMAKE_SOURCE_DIR}/tracking/TrkAlgo
${CMAKE_SOURCE_DIR}/tracking/TrkData
)

include_directories( ${INCLUDE_DIRECTORIES})

#set(GLPK_LIB_DIR 
#${SIMPATH}/basics/glpk/lib
#${SIMPATH}/lib
#)

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${GLPK_LIB_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(TRACKING_SRCS
PndTrkBoundaryParStraws.cxx
PndTrkBoundaryParStraws2.cxx
PndTrkChi2Fits.cxx
PndTrkCleanup.cxx
PndTrkComparisonMCtruth.cxx
PndTrkCTFindTrackInXY.cxx
PndTrkCTFindTrackInXY2.cxx
PndTrkCTGeometryCalculations.cxx
PndTrkGlpkFits.cxx
PndTrkLegendreFits.cxx
PndTrkMergeSort.cxx
PndTrkSttConformalFilling.cxx
PndTrkPlotMacros.cxx
PndTrkPlotMacros2.cxx
PndTrkPrintouts.cxx
PndTrkTracking.cxx
PndTrkTracking2.cxx
PndTrkSttAdjacencies.cxx
#
TrkAlgo/PndTrkLegendreTransform.cxx
TrkAlgo/PndTrkConformalTransform.cxx
TrkAlgo/PndTrkTools.cxx
TrkAlgo/PndTrkClean.cxx
TrkAlgo/PndTrkFitter.cxx
#
TrkData/PndTrkHit.cxx
TrkData/PndTrkSkewHit.cxx
TrkData/PndTrkHitList.cxx
TrkData/PndTrkSdsHitList.cxx
TrkData/PndTrkSttHitList.cxx
TrkData/PndTrkCluster.cxx
TrkData/PndTrkClusterList.cxx
TrkData/PndTrkTrack.cxx
TrkData/PndTrkConformalHit.cxx
TrkData/PndTrkConformalHitList.cxx
#
TrkSecondary/PndTrkLegendreTask.cxx
#
TrkStructure/PndTrkQATask.cxx
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${TRACKING_SRCS}" "${INCLUDE_DIRECTORIES}" TRACKING_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h TRACKING_HEADERS "${TRACKING_SRCS}")

set(TRACKING_LINKDEF  TrackingLinkDef.h)
set(TRACKING_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/PndTrackingDict.cxx) 

ROOT_GENERATE_DICTIONARY("${TRACKING_HEADERS}" "${TRACKING_LINKDEF}" "${TRACKING_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(TRACKING_SRCS ${TRACKING_SRCS} ${TRACKING_DICTIONARY})

add_library(Tracking SHARED ${TRACKING_SRCS})
target_link_libraries(Tracking ${ROOT_LIBRARIES} ${GLPK_LIB_DIR}/libglpk.so)
set_target_properties(Tracking PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS Tracking DESTINATION ${CMAKE_BINARY_DIR}/lib)
