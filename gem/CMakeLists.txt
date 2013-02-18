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
${CMAKE_SOURCE_DIR}/pnddata
${CMAKE_SOURCE_DIR}/pnddata/GemData
${CMAKE_SOURCE_DIR}/pnddata/TrackData
${CMAKE_SOURCE_DIR}/passive
${CMAKE_SOURCE_DIR}/gem
${CMAKE_SOURCE_DIR}/genfit
${CMAKE_SOURCE_DIR}/recotasks
${CMAKE_SOURCE_DIR}/trackbase
${CMAKE_SOURCE_DIR}/GenfitTools/recohits
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/LSLtrackRep
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/GeaneTrackRep
${CMAKE_SOURCE_DIR}/PndMCMatch
${CMAKE_SOURCE_DIR}/geane
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})


########################### create libGem ######################################
set(GEM_SRCS
PndGemDetector.cxx
PndGemSmearingTask.cxx
PndGemContFact.cxx
PndGemGeoPar.cxx
PndGemDigiPar.cxx
PndGemDigitize.cxx
PndGemFindClusters.cxx
PndGemFindHits.cxx
PndGemMatchHits.cxx
PndGemStation.cxx
PndGemSensor.cxx
PndGemIdealHitProducer.cxx
PndGemTrackFinder.cxx
PndGemTrackFinderIdeal.cxx
PndGemTrackFinderOnHits.cxx
PndGemFindTracks.cxx
PndGemTrackFinderQA.cxx
#PndGemRecoHit.cxx
PndGemIdealPatternRecoTask.cxx
PndGemMagneticFieldVsTrackParameters.cxx
PndGemDigiWriteoutBuffer.cxx
PndGemDigiSorterTask.cxx
PndGemDigiRingSorter.cxx
PndGemFindHitsAna.cxx
)
if(RULE_CHECKER_FOUND)
  CHECK_RULES("${GEM_SRCS}" "${INCLUDE_DIRECTORIES}" GEM_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h GEM_HEADERS "${GEM_SRCS}")
# SET(GEM_SRCS ${GEM_SRCS} ${DIGI_SRCS})
# SET(GEM_HEADERS ${GEM_HEADERS} ${DIGI_HEADERS})

set(GEM_LINKDEF  GemLinkDef.h)
set(GEM_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/GemDict.cxx)

ROOT_GENERATE_DICTIONARY("${GEM_HEADERS}" "${GEM_LINKDEF}" "${GEM_DICTIONARY}" "${INCLUDE_DIRECTORIES}")
SET(GEM_SRCS ${GEM_SRCS} ${GEM_DICTIONARY})

add_library(Gem SHARED ${GEM_SRCS})
target_link_libraries(Gem ${ROOT_LIBRARIES})
set_target_properties(Gem PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

################################## install #####################################
install(TARGETS Gem DESTINATION ${CMAKE_BINARY_DIR}/lib)
# install(TARGETS GemReco DESTINATION ${CMAKE_BINARY_DIR}/lib)
# install(TARGETS GemTrk DESTINATION ${CMAKE_BINARY_DIR}/lib)


