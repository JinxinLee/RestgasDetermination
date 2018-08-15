# Create a library called "libfastsim"

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES}
  ${BASE_INCLUDE_DIRECTORIES}
)

Set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/trackbase
  ${CMAKE_SOURCE_DIR}/pnddata
  ${CMAKE_SOURCE_DIR}/pnddata/PidData
  ${CMAKE_SOURCE_DIR}/detectors/passive
  ${CMAKE_SOURCE_DIR}/fastsim
  ${CMAKE_SOURCE_DIR}/pid
  ${CMAKE_SOURCE_DIR}/analysis/rho
  ${CMAKE_SOURCE_DIR}/analysis/rho/RhoBase
  ${CMAKE_SOURCE_DIR}/analysis/rho/RhoSelector
  ${CMAKE_SOURCE_DIR}/analysis/AnalysisTools
)

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

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
PndFsmMdtPid.cxx
PndFsmEmcPid.cxx
PndFsmIdealPid.cxx
)



set(LINKDEF fastsimLinkDef.h)
set(LIBRARY_NAME fastsim)

set(DEPENDENCIES Base ParBase PndData AnalysisTools)

GENERATE_LIBRARY()
