# Create a library called "libTutDec13" 

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES} 
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/trackbase 
${CMAKE_SOURCE_DIR}/pnddata
${CMAKE_SOURCE_DIR}/pnddata/PidData
${CMAKE_SOURCE_DIR}/analysis/rho
${CMAKE_SOURCE_DIR}/analysis/rho/RhoBase
${CMAKE_SOURCE_DIR}/analysis/rho/RhoSelector
${CMAKE_SOURCE_DIR}/analysis/rho/RhoHistogram
${CMAKE_SOURCE_DIR}/analysis/rho/RhoTools
${CMAKE_SOURCE_DIR}/tutorials/dec13
${CMAKE_SOURCE_DIR}/tutorials/dec13/example_D0toKpi
${CMAKE_SOURCE_DIR}/analysis/AnalysisTools
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
PndTutAnaTask.cxx
example_D0toKpi/PndTutAnaTaskD0.cxx
)

set(LINKDEF tutDec13LinkDef.h)
set(LIBRARY_NAME TutDec13)

set(DEPENDENCIES Base PndData Rho AnalysisTools TrkBase)

GENERATE_LIBRARY()


