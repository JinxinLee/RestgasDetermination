# Create a library called "anatask" 

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES} 
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/trackbase 
${CMAKE_SOURCE_DIR}/pnddata
${CMAKE_SOURCE_DIR}/pnddata/PidData
${CMAKE_SOURCE_DIR}/detectors/passive
${CMAKE_SOURCE_DIR}/tpc
${CMAKE_SOURCE_DIR}/lhetrack
${CMAKE_SOURCE_DIR}/detectors/emc/EmcData
${CMAKE_SOURCE_DIR}/detectors/emc/EmcReco
${CMAKE_SOURCE_DIR}/fastsim
${CMAKE_SOURCE_DIR}/analysis/rho
${CMAKE_SOURCE_DIR}/analysis/rho/RhoBase
${CMAKE_SOURCE_DIR}/analysis/rho/RhoSelector
${CMAKE_SOURCE_DIR}/tutorials/anatask
${CMAKE_SOURCE_DIR}/analysis/AnalysisTools
${CMAKE_SOURCE_DIR}/genfit
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(anatask_SRCS
#PndAnalysis.cxx
#PndMicroWriter.cxx
PndJpsi2PiAna.cxx
PndSoftTriggerTask.cxx
PndTripleAnaTask.cxx
)

set(anatask_HEADERS
#PndAnalysis.h
#PndMicroWriter.h
PndJpsi2PiAna.h
PndSoftTriggerTask.h
PndTripleAnaTask.h
)



set(anatask_LINKDEF anataskLinkDef.h)
set(anatask_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/anataskDict.cxx) 

ROOT_GENERATE_DICTIONARY("${anatask_HEADERS}" "${anatask_LINKDEF}" "${anatask_DICTIONARY}" "${INCLUDE_DIRECTORIES}")


set(anatask_SRCS ${anatask_SRCS} ${anatask_DICTIONARY})

add_library(anatask SHARED ${anatask_SRCS})
target_link_libraries(anatask ${ROOT_LIBRARIES})
set_target_properties(anatask PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS anatask DESTINATION ${CMAKE_BINARY_DIR}/lib)
