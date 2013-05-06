# Create a library called "libEvtGenDirect" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CLHEP_BASE_DIR}/include
${CLHEP_INCLUDE_DIR}
${BASE_INCLUDE_DIRECTORIES} 
${CMAKE_SOURCE_DIR}/pgenerators/EvtGen
${CMAKE_SOURCE_DIR}/pgenerators/EvtGenDirect
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(EVTGENDIRECT_SRCS
PndEvtGenDirect.cxx
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${EVTGENDIRECT_SRCS}" "${INCLUDE_DIRECTORIES}" EVTGENDIRECT_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h EVTGENDIRECT_HEADERS "${EVTGENDIRECT_SRCS}")

set(EVTGENDIRECT_LINKDEF  EvtGenDirectLinkDef.h)
set(EVTGENDIRECT_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/PndEvtGenDirectDict.cxx) 

ROOT_GENERATE_DICTIONARY("${EVTGENDIRECT_HEADERS}" "${EVTGENDIRECT_LINKDEF}" "${EVTGENDIRECT_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(EVTGENDIRECT_SRCS ${EVTGENDIRECT_SRCS} ${EVTGENDIRECT_DICTIONARY})

add_library(EvtGenDirect SHARED ${EVTGENDIRECT_SRCS})
target_link_libraries(EvtGenDirect ${ROOT_LIBRARIES})
set_target_properties(EvtGenDirect PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS EvtGenDirect DESTINATION ${CMAKE_BINARY_DIR}/lib)
