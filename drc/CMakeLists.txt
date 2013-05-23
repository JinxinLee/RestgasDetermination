# Create a library called "" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES}
${CMAKE_SOURCE_DIR}/base
${CMAKE_SOURCE_DIR}/fairtools
${CMAKE_SOURCE_DIR}/field
${CMAKE_SOURCE_DIR}/pnddata 
${CMAKE_SOURCE_DIR}/drc/
${CMAKE_SOURCE_DIR}/drc/drcprop 
${CMAKE_SOURCE_DIR}/pnddata/DrcData
${CMAKE_SOURCE_DIR}/pnddata/SdsData
${CMAKE_SOURCE_DIR}/PndTools/generalTools 

)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(DRC_SRCS
PndGeoDrc.cxx 
PndDrc.cxx 
PndGeoDrcPar.cxx 
PndDrcContFact.cxx 
PndDrcHitProducerIdeal.cxx 
PndDrcHitProducerReal.cxx 
PndDrcDigiTask.cxx
PndDrcHitFinder.cxx
DrawHits.cxx 
PndDrcDigiPar.cxx)


if(RULE_CHECKER_FOUND)
  CHECK_RULES("${DRC_SRCS}" "${INCLUDE_DIRECTORIES}" DRC_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h DRC_HEADERS "${DRC_SRCS}")

set(DRC_LINKDEF  DrcLinkDef.h)
set(DRC_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/PndDrcDict.cxx) 

ROOT_GENERATE_DICTIONARY("${DRC_HEADERS}" "${DRC_LINKDEF}" "${DRC_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(DRC_SRCS ${DRC_SRCS} ${DRC_DICTIONARY})

add_library(Drc SHARED ${DRC_SRCS})
target_link_libraries(Drc ${ROOT_LIBRARIES})
set_target_properties(Drc PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS Drc DESTINATION ${CMAKE_BINARY_DIR}/lib)
