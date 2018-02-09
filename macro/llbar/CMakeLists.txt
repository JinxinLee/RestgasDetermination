# Create a library called "llbaranatask" 
#Following lines were added
#${CLHEP_INCLUDE_DIR} 
#${SIMPATH}/include 
#${FAIRROOT_LIBRARY_DIR}
set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CLHEP_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES} 
${SIMPATH}/include 
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/trackbase 
${CMAKE_SOURCE_DIR}/pnddata
${CMAKE_SOURCE_DIR}/pnddata/PidData
${CMAKE_SOURCE_DIR}/passive
${CMAKE_SOURCE_DIR}/tpc
${CMAKE_SOURCE_DIR}/lhetrack
${CMAKE_SOURCE_DIR}/emc/EmcData
${CMAKE_SOURCE_DIR}/emc/EmcReco
${CMAKE_SOURCE_DIR}/fastsim
${CMAKE_SOURCE_DIR}/analysis/rho
${CMAKE_SOURCE_DIR}/analysis/rho/RhoBase
${CMAKE_SOURCE_DIR}/analysis/rho/RhoSelector
${CMAKE_SOURCE_DIR}/analysis/rho/RhoFitter
${CMAKE_SOURCE_DIR}/analysis/AnalysisTools
${CMAKE_SOURCE_DIR}/macro/llbar
${CMAKE_SOURCE_DIR}/genfit
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
PndLLbarAnaTask.cxx
)

set(LINKDEF llbaranaLinkDef.h)
set(LIBRARY_NAME llbaranatask)

set(DEPENDENCIES Base PndData Rho AnalysisTools TrkBase)

GENERATE_LIBRARY()
