# Create a library called "anatask" 

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CMAKE_SOURCE_DIR}/geobase 
${CMAKE_SOURCE_DIR}/parbase 
${CMAKE_SOURCE_DIR}/base 
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/trackbase 
${CMAKE_SOURCE_DIR}/mcstack
${CMAKE_SOURCE_DIR}/passive
${CMAKE_SOURCE_DIR}/tpc
${CMAKE_SOURCE_DIR}/lhetrack
${CMAKE_SOURCE_DIR}/emc/EmcData
${CMAKE_SOURCE_DIR}/emc/EmcReco
${CMAKE_SOURCE_DIR}/fsim
${CMAKE_SOURCE_DIR}/rho
${CMAKE_SOURCE_DIR}/tutorials/anatask
${CMAKE_SOURCE_DIR}/genfit
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(anatask_SRCS
PndAnalysis.cxx
PndMicroWriter.cxx
)

set(anatask_HEADERS
PndAnalysis.h
PndMicroWriter.h
)



set(anatask_LINKDEF anataskLinkDef.h)
set(anatask_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/anataskDict.cxx) 

ROOT_GENERATE_DICTIONARY("${anatask_HEADERS}" "${anatask_LINKDEF}" "${anatask_DICTIONARY}" "${INCLUDE_DIRECTORIES}")


set(anatask_SRCS ${anatask_SRCS} ${anatask_DICTIONARY})

add_library(anatask SHARED ${anatask_SRCS})
target_link_libraries(anatask ${ROOT_LIBRARIES})
set_target_properties(anatask PROPERTIES ${CBMROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS anatask DESTINATION ${CMAKE_BINARY_DIR}/lib)
