# Create a library called "libLHETrack" 

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES} 
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/trackbase 
${CMAKE_SOURCE_DIR}/pnddata
${CMAKE_SOURCE_DIR}/passive
${CMAKE_SOURCE_DIR}/tpc
${CMAKE_SOURCE_DIR}/lhetrack
${CMAKE_SOURCE_DIR}/fastsim
#${CMAKE_SOURCE_DIR}/pndexample
${CMAKE_SOURCE_DIR}/tutorials/charmtask


)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(charmtask_SRCS
PndJPsiExample.cxx
)

set(charmtask_HEADERS
PndJPsiExample.h
)



set(charmtask_LINKDEF charmtaskLinkDef.h)
set(charmtask_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/charmtaskDict.cxx) 

ROOT_GENERATE_DICTIONARY("${charmtask_HEADERS}" "${charmtask_LINKDEF}" "${charmtask_DICTIONARY}" "${INCLUDE_DIRECTORIES}")


set(charmtask_SRCS ${charmtask_SRCS} ${charmtask_DICTIONARY})

add_library(charmtask SHARED ${charmtask_SRCS})
target_link_libraries(charmtask ${ROOT_LIBRARIES})
set_target_properties(charmtask PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS charmtask DESTINATION ${CMAKE_BINARY_DIR}/lib)
