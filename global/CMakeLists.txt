# Create a library called "" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR}
${GEANT3_INCLUDE_DIR}
${CLHEP_INCLUDE_DIR}
${CMAKE_SOURCE_DIR}/fairtools
${CMAKE_SOURCE_DIR}/geobase
${CMAKE_SOURCE_DIR}/parbase
${CMAKE_SOURCE_DIR}/base
${CMAKE_SOURCE_DIR}/field
${CMAKE_SOURCE_DIR}/passive
${CMAKE_SOURCE_DIR}/PndTools/generalTools
${CMAKE_SOURCE_DIR}/pnddata
${CMAKE_SOURCE_DIR}/mvd/MvdData
${CMAKE_SOURCE_DIR}/mvd/MvdTools
${CMAKE_SOURCE_DIR}/pnddata/MvdData
${CMAKE_SOURCE_DIR}/pnddata/SdsData
${CMAKE_SOURCE_DIR}/stt
${CMAKE_SOURCE_DIR}/stt/sttreco
${CMAKE_SOURCE_DIR}/pnddata/SttData
${CMAKE_SOURCE_DIR}/gem
${CMAKE_SOURCE_DIR}/pnddata/GemData
${CMAKE_SOURCE_DIR}/pnddata/TrackData
${CMAKE_SOURCE_DIR}/genfit
${CMAKE_SOURCE_DIR}/recotasks
${CMAKE_SOURCE_DIR}/trackbase
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/LSLtrackRep
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/GeaneTrackRep
${CMAKE_SOURCE_DIR}/GenfitTools/recohits
${CMAKE_SOURCE_DIR}/geane
${CMAKE_SOURCE_DIR}/global
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})


########################### create libGlobal ######################################
set(GLOBAL_SRCS
PndGlobalIdealTrackMerger.cxx
PndGlobalTrackMergerQA.cxx
PndGlobalTrackFitQA.cxx
PndMvdGemTrackFinderOnHits.cxx
#PndGlobalPrepareKalmanTracks.cxx
#PndGlobalKalmanTask.cxx
PndBarrelTrackFinder.cxx
PndBarrelTrackFinderQA.cxx
#PndFitMCCircle.cxx
)
if(RULE_CHECKER_FOUND)
  CHECK_RULES("${GLOBAL_SRCS}" "${INCLUDE_DIRECTORIES}" GLOBAL_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h GLOBAL_HEADERS "${GLOBAL_SRCS}")
# SET(GLOBAL_SRCS ${GLOBAL_SRCS} ${DIGI_SRCS})
# SET(GLOBAL_HEADERS ${GLOBAL_HEADERS} ${DIGI_HEADERS})

set(GLOBAL_LINKDEF  GlobalLinkDef.h)
set(GLOBAL_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/GlobalDict.cxx)

ROOT_GENERATE_DICTIONARY("${GLOBAL_HEADERS}" "${GLOBAL_LINKDEF}" "${GLOBAL_DICTIONARY}" "${INCLUDE_DIRECTORIES}")
SET(GLOBAL_SRCS ${GLOBAL_SRCS} ${GLOBAL_DICTIONARY})

add_library(Global SHARED ${GLOBAL_SRCS})
target_link_libraries(Global ${ROOT_LIBRARIES})
set_target_properties(Global PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

################################## install #####################################
install(TARGETS Global DESTINATION ${CMAKE_BINARY_DIR}/lib)
# install(TARGETS GlobalReco DESTINATION ${CMAKE_BINARY_DIR}/lib)
# install(TARGETS GlobalTrk DESTINATION ${CMAKE_BINARY_DIR}/lib)


