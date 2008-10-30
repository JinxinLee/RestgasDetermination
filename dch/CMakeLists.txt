# Create a library called "" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CLHEP_INCLUDE_DIR}
${GEANT3_INCLUDE_DIR}
${CMAKE_SOURCE_DIR}/geobase 
${CMAKE_SOURCE_DIR}/base 
${CMAKE_SOURCE_DIR}/parbase 
${CMAKE_SOURCE_DIR}/field
${CMAKE_SOURCE_DIR}/trackrep
${CMAKE_SOURCE_DIR}/trackbase
${CMAKE_SOURCE_DIR}/geane
${CMAKE_SOURCE_DIR}/genfit
${CMAKE_SOURCE_DIR}/mcstack 
${CMAKE_SOURCE_DIR}/dch
${CMAKE_SOURCE_DIR}/dch/DchTracking
${CMAKE_SOURCE_DIR}/dch/DchFitting
${CMAKE_SOURCE_DIR}/dch/DchData
${CMAKE_SOURCE_DIR}/stt
${CMAKE_SOURCE_DIR}/stt/sttreco
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(DCH_SRCS
PndDchGeoPar.cxx
PndDchDigiPar.cxx
PndDchDigiInf.cxx
PndDchContFact.cxx
PndDchGeo.cxx
PndDchDetector.cxx
PndDchStructure.cxx
PndDchDigiProducer.cxx
PndDchMapper.cxx
PndDchDrifter.cxx
PndDchIdealHitProducer.cxx
PndDchCylinderHitProducer.cxx
DchData/PndDchPoint.cxx
DchData/PndDchDigi.cxx
DchData/PndDchHit.cxx
DchData/PndDchCylinderHit.cxx
DchFitting/PndDchPreFitterTR.cxx
DchFitting/PndDchRecoHit.cxx
DchFitting/PndDchRecoHit2.cxx
DchFitting/PndDchSimpleFitter.cxx
DchFitting/PndDchPreFitter.cxx
DchFitting/PndDchPrepareKalmanTracks.cxx
DchFitting/PndDchPrepareKalmanTracks2.cxx
DchFitting/PndDchKalmanTask.cxx
DchFitting/PndDchKalmanTask2.cxx
DchFitting/PndDchKalmanQATask.cxx
DchTracking/PndDchTrack.cxx
DchTracking/PndDchTrackFinder.cxx
DchTracking/PndDchTrackFinderIdeal.cxx
DchTracking/PndDchTrackFinderIdealDigi.cxx
DchTracking/PndDchTrackFinderIdealCylHit.cxx
DchTracking/PndDchFindTracks.cxx
DchTracking/PndDchTrackMatch.cxx
DchTracking/PndDchMatchTracks.cxx
DchTracking/PndDchFindTracksQa.cxx
#DchTracking/PndDchChamber.cxx
#DchTracking/PndDchChamberTracklet.cxx
#DchTracking/PndDchTrackFinderR1.cxx
#DchTracking/PndDchTrackFinderR2.cxx
#DchTracking/PndDchTrackFinderR3.cxx
#DchTracking/PndDchProcedureT1.cxx
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${DCH_SRCS}" "${INCLUDE_DIRECTORIES}" DCH_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h DCH_HEADERS "${DCH_SRCS}")

set(DCH_LINKDEF  DchLinkDef.h)
set(DCH_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/DchDict.cxx) 

ROOT_GENERATE_DICTIONARY("${DCH_HEADERS}" "${DCH_LINKDEF}" "${DCH_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(DCH_SRCS ${DCH_SRCS} ${DCH_DICTIONARY})

add_library(Dch SHARED ${DCH_SRCS})
target_link_libraries(Dch ${ROOT_LIBRARIES})
set_target_properties(Dch PROPERTIES ${CBMROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS Dch DESTINATION ${CMAKE_BINARY_DIR}/lib)
