# Create a library called "libPmt"

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR}
${BASE_INCLUDE_DIRECTORIES}
${CMAKE_SOURCE_DIR}/trackbase
${CMAKE_SOURCE_DIR}/field
${CMAKE_SOURCE_DIR}/pnddata
${CMAKE_SOURCE_DIR}/rho
${CMAKE_SOURCE_DIR}/rho/RhoBase
${CMAKE_SOURCE_DIR}/rho/RhoMath
${CMAKE_SOURCE_DIR}/rho/RhoTools
${CMAKE_SOURCE_DIR}/PndTools/AnalysisTools
${CMAKE_SOURCE_DIR}/PndTools/AnalysisTools/Fitter
${CMAKE_SOURCE_DIR}/macro/analysistools/PmtTask
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})

set(SRCS
PndPmtPoormantracks.cxx
PndPmtTask.cxx
)

set(LINKDEF PmtLinkDef.h)
set(LIBRARY_NAME Pmt)

set(DEPENDENCIES Base PndData Rho TrkBase AnalysisTools)

GENERATE_LIBRARY()


