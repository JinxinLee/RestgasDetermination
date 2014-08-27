# Create a library called "Rho" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES} 
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/trackbase 
${CMAKE_SOURCE_DIR}/pnddata # put FairRecoCandidate to fairbase at some point
${CMAKE_SOURCE_DIR}/pnddata/PidData # CAUTION we have a PANDA specific access: RhoGoodPhotonSelector auses PndPidCandidate
${CMAKE_SOURCE_DIR}/rho/RhoBase
${CMAKE_SOURCE_DIR}/rho/RhoMath
${CMAKE_SOURCE_DIR}/rho/RhoSelector
${CMAKE_SOURCE_DIR}/rho/RhoConstraints
${CMAKE_SOURCE_DIR}/rho/RhoTools
${CMAKE_SOURCE_DIR}/rho/RhoHistogram
${CMAKE_SOURCE_DIR}/PndTools/AnalysisTools
${CMAKE_SOURCE_DIR}/PndTools/AnalysisTools/Fitter
${CMAKE_SOURCE_DIR}/rho
${CMAKE_SOURCE_DIR}/softrig
)

include_directories( ${INCLUDE_DIRECTORIES})

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

set(DEPENDENCIES Base PndData Rho AnalysisTools TrkBase)

GENERATE_LIBRARY()
