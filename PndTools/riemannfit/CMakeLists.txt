# Create a library called "tpcreco" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CLHEP_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES} 
${CMAKE_SOURCE_DIR}/pnddata
${CMAKE_SOURCE_DIR}/pnddata/SdsData
${CMAKE_SOURCE_DIR}/pnddata/TrackData
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/passive
${CMAKE_SOURCE_DIR}/genfit
${CMAKE_SOURCE_DIR}/trackbase
${CMAKE_SOURCE_DIR}/PndTools/riemannfit
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(RIEMANN_SRCS
PndRiemannHit.cxx
PndRiemannTrack.cxx
PndRiemannTrackFinder.cxx
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${RIEMANN_SRCS}" "${INCLUDE_DIRECTORIES}" RIEMANN_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h RIEMANN_HEADERS "${RIEMANN_SRCS}")

set(RIEMANN_LINKDEF  riemannfitLinkDef.h)
set(RIEMANN_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/riemannDict.cxx) 

ROOT_GENERATE_DICTIONARY("${RIEMANN_HEADERS}" "${RIEMANN_LINKDEF}" "${RIEMANN_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(RIEMANN_SRCS ${RIEMANN_SRCS} ${RIEMANN_DICTIONARY})

add_library(riemann SHARED ${RIEMANN_SRCS})
target_link_libraries(riemann ${ROOT_LIBRARIES})
set_target_properties(riemann PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS riemann DESTINATION ${CMAKE_BINARY_DIR}/lib)
