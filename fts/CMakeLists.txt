# Create a library called "libFts" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${GEANT3_INCLUDE_DIR}
${BASE_INCLUDE_DIRECTORIES} 
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/pnddata
${CMAKE_SOURCE_DIR}/pnddata/FtsData
${CMAKE_SOURCE_DIR}/trackbase
${CMAKE_SOURCE_DIR}/pnddata/TrackData
${CMAKE_SOURCE_DIR}/fts
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(FTS_SRCS
PndGeoFtsPar.cxx
PndGeoFts.cxx
PndFtsDigiPar.cxx
PndFtsHitInfo.cxx
PndFts.cxx
PndFts2.cxx
PndFtsContFact.cxx
PndFtsMapCreator.cxx
PndFtsTube.cxx
PndFtsSingleStraw.cxx
PndFtsHitProducerRealFast.cxx
PndFtsHitProducerIdeal.cxx
PndFtsHitProducerRealFull.cxx
PndFtsTrackerIdeal.cxx
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${FTS_SRCS}" "${INCLUDE_DIRECTORIES}" FTS_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h FTS_HEADERS "${FTS_SRCS}")

set(FTS_LINKDEF  FtsLinkDef.h)
set(FTS_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/PndFtsDict.cxx) 

ROOT_GENERATE_DICTIONARY("${FTS_HEADERS}" "${FTS_LINKDEF}" "${FTS_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(FTS_SRCS ${FTS_SRCS} ${FTS_DICTIONARY})

add_library(Fts SHARED ${FTS_SRCS})
target_link_libraries(Fts ${ROOT_LIBRARIES})
set_target_properties(Fts PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS Fts DESTINATION ${CMAKE_BINARY_DIR}/lib)
