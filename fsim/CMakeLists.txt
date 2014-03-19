# Create a library called "libfsim"

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR}
${BASE_INCLUDE_DIRECTORIES}
${CMAKE_SOURCE_DIR}/trackbase
${CMAKE_SOURCE_DIR}/pnddata
${CMAKE_SOURCE_DIR}/pnddata/PidData
${CMAKE_SOURCE_DIR}/passive
${CMAKE_SOURCE_DIR}/tpc
${CMAKE_SOURCE_DIR}/fsim
${CMAKE_SOURCE_DIR}/fsim/scrut
${CMAKE_SOURCE_DIR}/pid
${CMAKE_SOURCE_DIR}/rho
${CMAKE_SOURCE_DIR}/rho/RhoBase
${CMAKE_SOURCE_DIR}/rho/RhoSelector
${CMAKE_SOURCE_DIR}/PndTools/AnalysisTools
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})

set(SRCS
PndFastSim.cxx
PndFsmTrack.cxx
PndFsmResponse.cxx
PndFsmAbsDet.cxx
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
PndFsmMvdPid.cxx
PndFsmSttPid.cxx
PndFsmSimpleVtx.cxx
)



set(LINKDEF fsimLinkDef.h)
set(LIBRARY_NAME fsim)

set(DEPENDENCIES Base ParBase PndData AnalysisTools)

GENERATE_LIBRARY()
