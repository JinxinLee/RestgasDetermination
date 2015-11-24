# Create a library called "AnalysisTools"

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES}
  ${BASE_INCLUDE_DIRECTORIES}
  ${GEANT3_INCLUDE_DIR}
)

Set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/field
  ${CMAKE_SOURCE_DIR}/trackbase
  ${CMAKE_SOURCE_DIR}/pnddata
  ${CMAKE_SOURCE_DIR}/pnddata/TrackData
  ${CMAKE_SOURCE_DIR}/pnddata/PidData
  ${CMAKE_SOURCE_DIR}/rho
  ${CMAKE_SOURCE_DIR}/rho/RhoBase
  ${CMAKE_SOURCE_DIR}/rho/RhoTools
  ${CMAKE_SOURCE_DIR}/rho/RhoSelector
  ${CMAKE_SOURCE_DIR}/rho/RhoMath
  ${CMAKE_SOURCE_DIR}/rho/RhoHistogram
  ${CMAKE_SOURCE_DIR}/rho/RhoFitter
  ${CMAKE_SOURCE_DIR}/rho/DecayTreeFitter
  ${CMAKE_SOURCE_DIR}/PndTools/PndParticleFitters
  ${CMAKE_SOURCE_DIR}/PndTools/AnalysisTools
  #${CMAKE_SOURCE_DIR}/PndTools/AnalysisTools/Fitter
  #${CMAKE_SOURCE_DIR}/PndTools/AnalysisTools/DecayTreeFitter
  ${CMAKE_SOURCE_DIR}/softrig
  ${CMAKE_SOURCE_DIR}/geane
)

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})

set(SRCS
#PndMicroWriter.cxx
#PndSimplePidInfo.cxx
#PndMicroCandidate.cxx
PndEventInfo.cxx
#PndEventReader.cxx
PndPidListMaker.cxx
PndAnalysis.cxx
#PndAnalysisCalcTools.cxx
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
#PndMcTruthMatch.cxx
PndGoodTrackSelector.cxx
PndEventShape.cxx
#PndAnaCovTool.cxx
PndRhoTupleQA.cxx
PndParticleQATask.cxx
PndSimpleCombiner.cxx
PndSimpleCombinerTask.cxx
##Fitter/PndVtxFitterParticle.cxx
##Fitter/PndVtxFitter_Init.cxx
##Fitter/PndVtxFitterError.cxx
##Fitter/PndVtxFitter.cxx
#Fitter/Pnd4CFitter.cxx
#Fitter/PndDecayTreeFitter.cxx
#Fitter/PndKinVtxFitter.cxx
#Fitter/PndKinFitter.cxx
#Fitter/PndVtxPoca.cxx
#Fitter/PndVtxPRG.cxx
#Fitter/PndKalmanVtxFitter.cxx
StrTok.cxx
)

set(LINKDEF AnalysisToolsLinkDef.h)
set(LIBRARY_NAME AnalysisTools)

set(DEPENDENCIES Base PndData Rho RhoFitter ParticleFitters TrkBase Geane)

GENERATE_LIBRARY()
