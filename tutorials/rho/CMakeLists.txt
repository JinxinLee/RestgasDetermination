# Create a library called "tutrhotask" 

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES} 
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/trackbase 
${CMAKE_SOURCE_DIR}/pnddata
${CMAKE_SOURCE_DIR}/pnddata/PidData
${CMAKE_SOURCE_DIR}/passive
${CMAKE_SOURCE_DIR}/tpc
${CMAKE_SOURCE_DIR}/lhetrack
${CMAKE_SOURCE_DIR}/emc/EmcData
${CMAKE_SOURCE_DIR}/emc/EmcReco
${CMAKE_SOURCE_DIR}/fsim
${CMAKE_SOURCE_DIR}/rho
${CMAKE_SOURCE_DIR}/rho/RhoBase
${CMAKE_SOURCE_DIR}/rho/RhoSelector
${CMAKE_SOURCE_DIR}/tutorials/rho
${CMAKE_SOURCE_DIR}/PndTools/AnalysisTools
${CMAKE_SOURCE_DIR}/PndTools/AnalysisTools/Fitter
${CMAKE_SOURCE_DIR}/genfit
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(tutrhotask_SRCS
PndTutAnaTask.cxx
)

set(tutrhotask_HEADERS
PndTutAnaTask.h
)



set(tutrhotask_LINKDEF tutrhoLinkDef.h)
set(tutrhotask_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/tutrhoDict.cxx) 

ROOT_GENERATE_DICTIONARY("${tutrhotask_HEADERS}" "${tutrhotask_LINKDEF}" "${tutrhotask_DICTIONARY}" "${INCLUDE_DIRECTORIES}")


set(tutrhotask_SRCS ${tutrhotask_SRCS} ${tutrhotask_DICTIONARY})

add_library(tutrhotask SHARED ${tutrhotask_SRCS})
target_link_libraries(tutrhotask ${ROOT_LIBRARIES})
set_target_properties(tutrhotask PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS tutrhotask DESTINATION ${CMAKE_BINARY_DIR}/lib)
