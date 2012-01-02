# Create a library called "" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${GEANT3_INCLUDE_DIR}
${CLHEP_INCLUDE_DIR}
${CMAKE_SOURCE_DIR}/fairtools
${CMAKE_SOURCE_DIR}/geobase 
${CMAKE_SOURCE_DIR}/parbase 
${CMAKE_SOURCE_DIR}/base 
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/pnddata 
${CMAKE_SOURCE_DIR}/pnddata/SciTData
${CMAKE_SOURCE_DIR}/scitil
${CMAKE_SOURCE_DIR}/genfit
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SCIT_SRCS
PndGeoSciT.cxx 
PndSciT.cxx 
PndGeoSciTPar.cxx 
PndSciTContFact.cxx
PndSciTHitProducerIdeal.cxx  
#PndSciTAnaIdeal.cxx
#PndSciTMassTrig.cxx
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${SCIT_SRCS}" "${INCLUDE_DIRECTORIES}" SCIT_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h SCIT_HEADERS "${SCIT_SRCS}")

set(SCIT_LINKDEF  SciTLinkDef.h)
set(SCIT_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/PndSciTDict.cxx) 

ROOT_GENERATE_DICTIONARY("${SCIT_HEADERS}" "${SCIT_LINKDEF}" "${SCIT_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(SCIT_SRCS ${SCIT_SRCS} ${SCIT_DICTIONARY})

add_library(SciT SHARED ${SCIT_SRCS})
target_link_libraries(SciT ${ROOT_LIBRARIES})
set_target_properties(SciT PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS SciT DESTINATION ${CMAKE_BINARY_DIR}/lib)
