# Create a library called "libfsim" 

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES} 
${CMAKE_SOURCE_DIR}/trackbase 
${CMAKE_SOURCE_DIR}/pnddata
${CMAKE_SOURCE_DIR}/pnddata/PidData
${CMAKE_SOURCE_DIR}/passive
${CMAKE_SOURCE_DIR}/tpc
${CMAKE_SOURCE_DIR}/lhetrack
${CMAKE_SOURCE_DIR}/fsim
${CMAKE_SOURCE_DIR}/pid
${CMAKE_SOURCE_DIR}/rho
${CMAKE_SOURCE_DIR}/rho/RhoBase
${CMAKE_SOURCE_DIR}/rho/RhoSelector
${CMAKE_SOURCE_DIR}/PndTools/AnalysisTools
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${GLPK_LIB_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
PndFastSim.cxx
PndFsmTrack.cxx
PndFsmResponse.cxx
PndFsmAbsDet.cxx
#StrTok.cxx
PndFsmDetFactory.cxx
PndFsmEmcBarrel.cxx
PndFsmEmcBwCap.cxx
PndFsmEmcFwCap.cxx
PndFsmEmcFS.cxx
PndFsmStt.cxx
PndFsmTpc.cxx
PndFsmMdcFS.cxx
PndFsmMdcTS.cxx
PndFsmRich.cxx
PndFsmDrcDisc.cxx
PndFsmDrcBarrel.cxx
PndFsmEffTracker.cxx
PndFsmMvd.cxx
PndFsmMvd2.cxx
PndFsmTof.cxx
PndFsmSimpleTracker.cxx
PndFsmCmpDet.cxx
PndFsmCombiDet.cxx
PndFsmRandom.cxx
#PndSimpleAnalysis.cxx
#PndAnalysis.cxx
#PndListProvider.cxx
#PndListDefiner.cxx
#PndEventLoop.cxx
)

set(HEADERS
#ArgList.h
FsmDetTypes.h
PndFastSim.h
PndFsmTrack.h
PndFsmResponse.h
PndFsmAbsDet.h
#StrTok.h
PndFsmDetFactory.h
PndFsmEmcBarrel.h
PndFsmEmcBwCap.h
PndFsmEmcFwCap.h
PndFsmEmcFS.h
PndFsmStt.h
PndFsmTpc.h
PndFsmMdcFS.h
PndFsmMdcTS.h
PndFsmRich.h
PndFsmDrcDisc.h
PndFsmDrcBarrel.h
PndFsmEffTracker.h
PndFsmMvd.h
PndFsmMvd2.h
PndFsmTof.h
PndFsmSimpleTracker.h
PndFsmCmpDet.h
PndFsmCombiDet.h
PndFsmRandom.h
#PndSimpleAnalysis.h
#PndAnalysis.h
#PndListProvider.h
#PndListDefiner.h
#PndEventLoop.h
)

set(LINKDEF fsimLinkDef.h)
set(LIBRARY_NAME fsim)

set(DEPENDENCIES Base ParBase PndData AnalysisTools)

GENERATE_LIBRARY()
