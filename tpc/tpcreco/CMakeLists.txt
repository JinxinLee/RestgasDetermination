# Create a library called "tpcreco" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CLHEP_INCLUDE_DIR} 
${GEANT3_INCLUDE_DIR}
${CMAKE_SOURCE_DIR}/fairtools
${CMAKE_SOURCE_DIR}/geobase 
${CMAKE_SOURCE_DIR}/parbase 
${CMAKE_SOURCE_DIR}/base 
${CMAKE_SOURCE_DIR}/pnddata
${CMAKE_SOURCE_DIR}/pnddata/TrackData
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/passive
${CMAKE_SOURCE_DIR}/genfit
${CMAKE_SOURCE_DIR}/recotasks
${CMAKE_SOURCE_DIR}/trackbase
${CMAKE_SOURCE_DIR}/geane
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/LSLtrackRep
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/GeaneTrackRep2
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/GeaneTrackRep
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/RKTrackRep
${CMAKE_SOURCE_DIR}/GenfitTools/recohits
${CMAKE_SOURCE_DIR}/pnddata/SdsData
${CMAKE_SOURCE_DIR}/pnddata/GemData
${CMAKE_SOURCE_DIR}/tpc
${CMAKE_SOURCE_DIR}/tpc/tpcreco
${CMAKE_SOURCE_DIR}/PndTools/generalTools


)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(TPCRECO_SRCS
PndTpcConfMapRecoHit.cxx 
PndTpcConfMapFit.cxx 
PndTpcZSFit.cxx 
PndTpcConfTrackFinder.cxx 
#test/PndTpcRecoTester.cxx 
PndTpcPlanarRecoHit.cxx 
PndTpcTrackVisTask.cxx
PndTpcTrackInitTask.cxx
StdDiscriminantFcn.cxx
PndTpcRiemannHit.cxx
PndTpcRiemannTrack.cxx
PndTpcRiemannTrackFinder.cxx
PndTpcEvtDeconvTask.cxx
#PndTpcRiemannHough.cxx
PndTpcProximityHTCorrelator.cxx
PndTpcHelixHTCorrelator.cxx
PndTpcRiemannTTCorrelator.cxx
PndTpcDipTTCorrelator.cxx
PndTpcClustershapeCorrelator.cxx
PndTpcRiemannTrackingTask.cxx
PndTpcIdealTrackingTask.cxx
PndTpcIdealTrackingTask2.cxx
#PndTpcRiemannMCTask.cxx
PndTpcRiemannMCResidual.cxx
##############################
#PndTpcRecoDEdxTask.cxx
#PndTpcDEDXDiagnosticPoint.cxx
#PndTpcDEDXDiagnostics.cxx
#PndTpcDEDXDiagnosticsTask.cxx
#PndTpcAdvancedDxEvalPoint.cxx
#PndTpcSimpleDxEvalPoint.cxx
#PndTpcDXCalculator.cxx
PndTpcClusterTrack.cxx
PndTpcTFSInfo.cxx
PndTpcSPHit.cxx	
MatrixChi2.cxx
PndTpcdEdxTask.cxx
PndTpcdEdx.cxx
PndTpcGenfitTestTask.cxx
PndTpcTestPlanarHit.cxx
PndTpcMVDCorrelatorTask.cxx
PndTpcGEMCorrelatorTask.cxx
PndTpcRoughAlignmentTask.cxx
../DebugLogger.cxx
../dbgstream.cxx
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${TPCRECO_SRCS}" "${INCLUDE_DIRECTORIES}" TPCRECO_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h TPCRECO_HEADERS "${TPCRECO_SRCS}")

set(TPCRECO_LINKDEF  tpcrecoLinkDef.h)
set(TPCRECO_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/tpcrecoDict.cxx) 
ROOT_GENERATE_DICTIONARY("${TPCRECO_HEADERS}" "${TPCRECO_LINKDEF}" "${TPCRECO_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(TPCRECO_SRCS ${TPCRECO_SRCS} ${TPCRECO_DICTIONARY})

add_library(TpcReco SHARED ${TPCRECO_SRCS})
target_link_libraries(TpcReco ${ROOT_LIBRARIES})
set_target_properties(TpcReco PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS TpcReco DESTINATION ${CMAKE_BINARY_DIR}/lib)
