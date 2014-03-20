# Create a library called "libScrut" 

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES} 
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/trackbase 
${CMAKE_SOURCE_DIR}/pnddata
${CMAKE_SOURCE_DIR}/pnddata/PidData
${CMAKE_SOURCE_DIR}/fsim
${CMAKE_SOURCE_DIR}/rho
${CMAKE_SOURCE_DIR}/rho/RhoBase
${CMAKE_SOURCE_DIR}/rho/RhoSelector
${CMAKE_SOURCE_DIR}/rho/RhoHistogram
${CMAKE_SOURCE_DIR}/rho/RhoTools
${CMAKE_SOURCE_DIR}/macro/scrut
${CMAKE_SOURCE_DIR}/PndTools/AnalysisTools
${CMAKE_SOURCE_DIR}/PndTools/AnalysisTools/Fitter
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

set(DEPENDENCIES Base PndData Rho AnalysisTools TrkBase fsim)

GENERATE_LIBRARY()


