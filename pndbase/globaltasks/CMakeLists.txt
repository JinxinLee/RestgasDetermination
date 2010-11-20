# Create a library called "" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CMAKE_SOURCE_DIR}/geobase 
${CMAKE_SOURCE_DIR}/base 
${CMAKE_SOURCE_DIR}/parbase 
${CMAKE_SOURCE_DIR}/field
${CMAKE_SOURCE_DIR}/pndbase/proxydict 
${CMAKE_SOURCE_DIR}/pndbase/gblenv 
${CMAKE_SOURCE_DIR}/pndbase/globaltasks
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(globaltasks_SRCS
PndGlobalEvtTask.cxx 
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${globaltasks_SRCS}" "${INCLUDE_DIRECTORIES}" globaltasks_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h globaltasks_HEADERS "${globaltasks_SRCS}")

set(globaltasks_LINKDEF  globaltasksLinkDef.h)
set(globaltasks_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/globaltasksDict.cxx) 

ROOT_GENERATE_DICTIONARY("${globaltasks_HEADERS}" "${globaltasks_LINKDEF}" "${globaltasks_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(globaltasks_SRCS ${globaltasks_SRCS} ${globaltasks_DICTIONARY})

add_library(GlobalTasks SHARED ${globaltasks_SRCS})
target_link_libraries(GlobalTasks ${ROOT_LIBRARIES})
set_target_properties(GlobalTasks PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS GlobalTasks DESTINATION ${CMAKE_BINARY_DIR}/lib)
