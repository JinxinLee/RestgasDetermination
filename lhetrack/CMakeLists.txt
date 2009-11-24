# Create a library called "libLheTrack" 

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${GEANT3_INCLUDE_DIR}
${CMAKE_SOURCE_DIR}/geobase 
${CMAKE_SOURCE_DIR}/parbase 
${CMAKE_SOURCE_DIR}/base
${CMAKE_SOURCE_DIR}/pndbase/PndStdUtils
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/trackbase 
${CMAKE_SOURCE_DIR}/passive
${CMAKE_SOURCE_DIR}/tpc
${CMAKE_SOURCE_DIR}/tpc/tpcreco
${CMAKE_SOURCE_DIR}/mvd/MvdTools
${CMAKE_SOURCE_DIR}/mvd/MvdData
${CMAKE_SOURCE_DIR}/emc/EmcData
${CMAKE_SOURCE_DIR}/emc/EmcReco
${CMAKE_SOURCE_DIR}/emc/EmcTools
${CMAKE_SOURCE_DIR}/gem
${CMAKE_SOURCE_DIR}/genfit
${CMAKE_SOURCE_DIR}/pnddata
${CMAKE_SOURCE_DIR}/pnddata/SttData
${CMAKE_SOURCE_DIR}/pnddata/TofData
${CMAKE_SOURCE_DIR}/pnddata/MdtData
${CMAKE_SOURCE_DIR}/pnddata/DrcData
${CMAKE_SOURCE_DIR}/pnddata/MvdData
${CMAKE_SOURCE_DIR}/pnddata/GemData
${CMAKE_SOURCE_DIR}/pnddata/TrackData
${CMAKE_SOURCE_DIR}/stt/sttreco
${CMAKE_SOURCE_DIR}/geane
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/LSLtrackRep
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/GeaneTrackRep
${CMAKE_SOURCE_DIR}/GenfitTools/adapters
${CMAKE_SOURCE_DIR}/lhetrack


)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(LheTrack_SRCS
PndLheTrackFinder.cxx
PndLheTrackFinderIdeal.cxx
PndLheSegments.cxx
PndLheTrackCuts.cxx      
PndLheCMPoint.cxx
PndLheCandidate.cxx
PndLheCMCandidate.cxx        
PndLheTrackFitter.cxx
PndLheHitsMaker.cxx      
PndLhePoint.cxx
PndLheHit.cxx
PndLhePidMaker.cxx
PndLhePidTrack.cxx
PndLheKalmanTask.cxx
PndLheCorrPar.cxx
PndLheContFact.cxx
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${LheTrack_SRCS}" "${INCLUDE_DIRECTORIES}" LheTrack_RULES)
endif(RULE_CHECKER_FOUND)


set(LheTrack_HEADERS
PndLheTrackCuts.h 
PndLheCandidate.h   
PndLheCMPoint.h  
PndLheTrackFinder.h
PndLheTrackFinderIdeal.h
PndLheTrackFitter.h 
PndLheHitsMaker.h
PndLheCMCandidate.h   
PndLheSegments.h 
PndLhePoint.h 
PndLheHit.h
PndLhePidMaker.h
PndLhePidTrack.h
PndLheKalmanTask.h
PndLheCorrPar.h
PndLheContFact.h
)



set(LheTrack_LINKDEF LheTrackLinkDef.h)
set(LheTrack_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/LheTrackDict.cxx) 

ROOT_GENERATE_DICTIONARY("${LheTrack_HEADERS}" "${LheTrack_LINKDEF}" "${LheTrack_DICTIONARY}" "${INCLUDE_DIRECTORIES}")


set(LheTrack_SRCS ${LheTrack_SRCS} ${LheTrack_DICTIONARY})

add_library(LheTrack SHARED ${LheTrack_SRCS})
target_link_libraries(LheTrack ${ROOT_LIBRARIES})
set_target_properties(LheTrack PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS LheTrack DESTINATION ${CMAKE_BINARY_DIR}/lib)
