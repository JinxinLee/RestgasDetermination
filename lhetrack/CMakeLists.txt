# Create a library called "libLHETrack" 

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${GEANT3_INCLUDE_DIR}
${CMAKE_SOURCE_DIR}/geobase 
${CMAKE_SOURCE_DIR}/parbase 
${CMAKE_SOURCE_DIR}/base 
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
${CMAKE_SOURCE_DIR}/genfit
${CMAKE_SOURCE_DIR}/pnddata
${CMAKE_SOURCE_DIR}/pnddata/SttData
${CMAKE_SOURCE_DIR}/pnddata/TofData
${CMAKE_SOURCE_DIR}/pnddata/MdtData
${CMAKE_SOURCE_DIR}/pnddata/DrcData
${CMAKE_SOURCE_DIR}/pnddata/MvdData
${CMAKE_SOURCE_DIR}/stt/sttreco
${CMAKE_SOURCE_DIR}/geane
${CMAKE_SOURCE_DIR}/trackrep
${CMAKE_SOURCE_DIR}/lhetrack


)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(LHETrack_SRCS
PndTpcLheTrackFinder.cxx
PndLheTrackFinderIdeal.cxx
PndTpcLheSegments.cxx
PndTpcLheHit.cxx
PndTpcLheTrackCuts.cxx      
PndTpcLheCMPoint.cxx
PndTpcLheTrack.cxx          
PndTpcLheCMTrack.cxx        
PndTpcLheTrackFitter.cxx
PndLheHitsMaker.cxx      
PndTpcLhePoint.cxx
PndLhePidMaker.cxx
PndLhePidTrack.cxx
PndLheKalmanTask.cxx
PndLheTpcPlanarRecoHit.cxx
PndLheCorrPar.cxx
PndLheContFact.cxx
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${LHETrack_SRCS}" "${INCLUDE_DIRECTORIES}" LHETrack_RULES)
endif(RULE_CHECKER_FOUND)


set(LHETrack_HEADERS
PndTpcLheHit.h
PndTpcLheTrackCuts.h 
PndTpcLheTrack.h   
PndTpcLheCMPoint.h  
PndTpcLheTrackFinder.h
PndLheTrackFinderIdeal.h
PndTpcLheTrackFitter.h 
PndLheHitsMaker.h
PndTpcLheCMTrack.h   
PndTpcLheSegments.h 
PndTpcLhePoint.h 
PndLhePidMaker.h
PndLhePidTrack.h
PndLheKalmanTask.h
PndLheTpcPlanarRecoHit.h
PndLheCorrPar.h
PndLheContFact.h
)



set(LHETrack_LINKDEF LHETrackLinkDef.h)
set(LHETrack_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/LHETrackDict.cxx) 

ROOT_GENERATE_DICTIONARY("${LHETrack_HEADERS}" "${LHETrack_LINKDEF}" "${LHETrack_DICTIONARY}" "${INCLUDE_DIRECTORIES}")


set(LHETrack_SRCS ${LHETrack_SRCS} ${LHETrack_DICTIONARY})

add_library(LHETrack SHARED ${LHETrack_SRCS})
target_link_libraries(LHETrack ${ROOT_LIBRARIES})
set_target_properties(LHETrack PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS LHETrack DESTINATION ${CMAKE_BINARY_DIR}/lib)
