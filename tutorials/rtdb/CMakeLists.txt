# Create a library called "rtdbtut" 

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES} 
${CMAKE_SOURCE_DIR}/tutorials/rtdb
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(rtdbtut_SRCS
PndTutPar.cxx
PndTutContFact.cxx
PndTutAccessRTDBTask.cxx
)

set(rtdbtut_HEADERS
PndTutPar.h
PndTutContFact.h
PndTutAccessRTDBTask.h
)



set(rtdbtut_LINKDEF rtdbtutLinkDef.h)
set(rtdbtut_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/rtdbtutDict.cxx) 

ROOT_GENERATE_DICTIONARY("${rtdbtut_HEADERS}" "${rtdbtut_LINKDEF}" "${rtdbtut_DICTIONARY}" "${INCLUDE_DIRECTORIES}")


set(rtdbtut_SRCS ${rtdbtut_SRCS} ${rtdbtut_DICTIONARY})

add_library(rtdbtut SHARED ${rtdbtut_SRCS})
target_link_libraries(rtdbtut ${ROOT_LIBRARIES})
set_target_properties(rtdbtut PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS rtdbtut DESTINATION ${CMAKE_BINARY_DIR}/lib)
