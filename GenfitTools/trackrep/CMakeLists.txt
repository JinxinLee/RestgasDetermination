# Create a library called "libtrackrep" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CLHEP_INCLUDE_DIR}
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/GeaneTrackRep
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/LSLtrackRep
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/RKTrackRep
${CMAKE_SOURCE_DIR}/genfit
${CMAKE_SOURCE_DIR}/trackbase
${CMAKE_SOURCE_DIR}/geane
${GEANT3_INCLUDE_DIR}
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})


set(TRACKREP_SRCS 
GeaneTrackRep/GeaneTrackRep.cxx
LSLtrackRep/LSLTrackRep.cxx
LSLtrackRep/LSLEQM.cxx
LSLtrackRep/Nystrom.cxx
RKTrackRep/RKTrackRep.cxx
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${TRACKREP_SRCS}" "${INCLUDE_DIRECTORIES}" TRACKREP_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h TRACKREP_HEADERS "${TRACKREP_SRCS}")


set(TRACKREP_LINKDEF  trackrepLinkDef.h)
set(TRACKREP_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/trackrepDict.cxx) 


ROOT_GENERATE_DICTIONARY("${TRACKREP_HEADERS}" "${TRACKREP_LINKDEF}" "${TRACKREP_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(TRACKREP_SRCS_ALL ${TRACKREP_SRCS} ${TRACKREP_DICTIONARY})

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DOLDVMC")

add_library(trackrep SHARED ${TRACKREP_SRCS_ALL})
target_link_libraries(trackrep ${ROOT_LIBRARIES})
set_target_properties(trackrep PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS trackrep DESTINATION ${CMAKE_BINARY_DIR}/lib)
