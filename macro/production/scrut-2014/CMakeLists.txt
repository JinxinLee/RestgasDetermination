# Create a library called "libScrut" 

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES} 
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/trackbase 
${CMAKE_SOURCE_DIR}/pnddata
${CMAKE_SOURCE_DIR}/pnddata/PidData
${CMAKE_SOURCE_DIR}/fastsim
${CMAKE_SOURCE_DIR}/analysis/rho
${CMAKE_SOURCE_DIR}/analysis/rho/RhoBase
${CMAKE_SOURCE_DIR}/analysis/rho/RhoSelector
${CMAKE_SOURCE_DIR}/analysis/rho/RhoHistogram
${CMAKE_SOURCE_DIR}/analysis/rho/RhoTools
${CMAKE_SOURCE_DIR}/macro/scrut
${CMAKE_SOURCE_DIR}/analysis/AnalysisTools
${CMAKE_SOURCE_DIR}/analysis/rho/RhoFitter
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
PndScrutAnaTask.cxx
)

set(LINKDEF scrutLinkDef.h)
set(LIBRARY_NAME Scrut)

set(DEPENDENCIES Base PndData Rho AnalysisTools TrkBase fastsim)

GENERATE_LIBRARY()


