# Create a library called "" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR}
${GEANT3_INCLUDE_DIR}
${CLHEP_INCLUDE_DIR}
${BASE_INCLUDE_DIRECTORIES} 
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
#${GLPK_LIB_DIR}
)

link_directories( ${LINK_DIRECTORIES})

set(SRCS
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
PndGemIdealPatternRecoTask.cxx
PndGemMagneticFieldVsTrackParameters.cxx
PndGemDigiWriteoutBuffer.cxx
PndGemDigiSorterTask.cxx
PndGemDigiRingSorter.cxx
PndGemFindHitsAna.cxx
)

set(LINKDEF  GemLinkDef.h)
set(LIBRARY_NAME Gem)

set(DEPENDENCIES Base ParBase PndData TrkBase Geane)

GENERATE_LIBRARY()
