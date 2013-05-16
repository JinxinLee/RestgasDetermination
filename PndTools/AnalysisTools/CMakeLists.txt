# Create a library called "AnalysisTools" 

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES} 
${CMAKE_SOURCE_DIR}/pndbase/PndStdUtils 
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/trackbase 
${CMAKE_SOURCE_DIR}/pnddata
${CMAKE_SOURCE_DIR}/pnddata/TrackData
${CMAKE_SOURCE_DIR}/pnddata/PidData
${CMAKE_SOURCE_DIR}/pid
${CMAKE_SOURCE_DIR}/pid/PidData
${CMAKE_SOURCE_DIR}/passive
${CMAKE_SOURCE_DIR}/tpc
${CMAKE_SOURCE_DIR}/lhetrack
${CMAKE_SOURCE_DIR}/emc/EmcData
${CMAKE_SOURCE_DIR}/emc/EmcReco
${CMAKE_SOURCE_DIR}/fsim
${CMAKE_SOURCE_DIR}/rho
${CMAKE_SOURCE_DIR}/rho/RhoBase
${CMAKE_SOURCE_DIR}/rho/RhoTools
${CMAKE_SOURCE_DIR}/rho/RhoSelector
${CMAKE_SOURCE_DIR}/PndTools/AnalysisTools
${CMAKE_SOURCE_DIR}/PndTools/AnalysisTools/Fitter
${CMAKE_SOURCE_DIR}/genfit
${GEANT3_INCLUDE_DIR}
${CMAKE_SOURCE_DIR}/geane
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/GeaneTrackRep
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/LSLtrackRep
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS 
#PndMicroWriter.cxx
#PndSimplePidInfo.cxx
#PndMicroCandidate.cxx
PndEventInfo.cxx
PndEventReader.cxx
PndPidListMaker.cxx
PndAnalysis.cxx
PndAnalysisCalcTools.cxx
PndSimpleAnalysis.cxx
PndAnalysisTask.cxx
PndMyAnalysisTask.cxx
PndAnaTask.cxx
PndListDefiner.cxx
PndListProvider.cxx
PndMcListConverter.cxx
PndAnaPidCombiner.cxx
PndAnaPidSelector.cxx
PndAnaContFact.cxx
PndAnaSelectorPar.cxx
PndMcTruthMatch.cxx
PndGoodTrackSelector.cxx
Fitter/PndVtxFitterParticle.cxx
Fitter/PndVtxFitter_Init.cxx
Fitter/PndVtxFitterError.cxx
Fitter/PndVtxFitter.cxx
Fitter/Pnd4CFitter.cxx
Fitter/PndKinVtxFitter.cxx
Fitter/PndKinFitter.cxx
Fitter/PndChiVtxFitter.cxx
Fitter/PndVtxPoca.cxx
Fitter/PndVtxPRG.cxx
StrTok.cxx
)

set(LINKDEF AnalysisToolsLinkDef.h)
set(LIBRARY_NAME AnalysisTools)

set(DEPENDENCIES Base Emc Geane ParBase Pid PndData TrkBase Rho)

GENERATE_LIBRARY()
