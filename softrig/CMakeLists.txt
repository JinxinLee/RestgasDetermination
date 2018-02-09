# Create a library called "Rho" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES} 
  ${BASE_INCLUDE_DIRECTORIES} 
)

Set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/field 
  ${CMAKE_SOURCE_DIR}/trackbase 
  ${CMAKE_SOURCE_DIR}/pnddata # put FairRecoCandidate to fairbase at some point
  ${CMAKE_SOURCE_DIR}/pnddata/PidData # CAUTION we have a PANDA specific access: RhoGoodPhotonSelector auses PndPidCandidate
  ${CMAKE_SOURCE_DIR}/analysis/rho/RhoBase
  ${CMAKE_SOURCE_DIR}/analysis/rho/RhoMath
  ${CMAKE_SOURCE_DIR}/analysis/rho/RhoSelector
  ${CMAKE_SOURCE_DIR}/analysis/rho/RhoConstraints
  ${CMAKE_SOURCE_DIR}/analysis/rho/RhoTools
  ${CMAKE_SOURCE_DIR}/analysis/rho/RhoFitter
  ${CMAKE_SOURCE_DIR}/analysis/rho/RhoHistogram
  ${CMAKE_SOURCE_DIR}/analysis/AnalysisTools
  ${CMAKE_SOURCE_DIR}/analysis/rho
  ${CMAKE_SOURCE_DIR}/softrig
)

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS 
PndSoftTriggerTask.cxx
PndSoftTriggerLine.cxx
PndOnlineFilterInfo.cxx
PndAnaWithTrigger.cxx
)

set(LINKDEF  SofTrig_LinkDef.h)
set(LIBRARY_NAME SofTrig)

set(DEPENDENCIES Base PndData Rho AnalysisTools TrkBase TMVA)

GENERATE_LIBRARY()
