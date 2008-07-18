# Create a library called "" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CMAKE_SOURCE_DIR}/geobase 
${CMAKE_SOURCE_DIR}/parbase 
${CMAKE_SOURCE_DIR}/base 
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/mcstack 
${CMAKE_SOURCE_DIR}/hyp
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(HYP_SRCS
PndGeoHyp.cxx 
PndHyp.cxx 
PndGeoHypPar.cxx 
PndHypPoint.cxx 
PndHypHit.cxx
PndHypHitProducerIdeal.cxx    
PndHypContFact.cxx 
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${HYP_SRCS}" "${INCLUDE_DIRECTORIES}" HYP_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h HYP_HEADERS "${HYP_SRCS}")

set(HYP_LINKDEF  HypLinkDef.h)
set(HYP_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/PndHypDict.cxx) 

ROOT_GENERATE_DICTIONARY("${HYP_HEADERS}" "${HYP_LINKDEF}" "${HYP_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(HYP_SRCS ${HYP_SRCS} ${HYP_DICTIONARY})

add_library(Hyp SHARED ${HYP_SRCS})
target_link_libraries(Hyp ${ROOT_LIBRARIES})
set_target_properties(Hyp PROPERTIES ${CBMROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS Hyp DESTINATION ${CMAKE_BINARY_DIR}/lib)
