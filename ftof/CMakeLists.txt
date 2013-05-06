# Create a library called "" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${GEANT3_INCLUDE_DIR}
${CLHEP_INCLUDE_DIR}
${BASE_INCLUDE_DIRECTORIES} 
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/pnddata 
#${CMAKE_SOURCE_DIR}/pnddata/FtofData
${CMAKE_SOURCE_DIR}/ftof
${CMAKE_SOURCE_DIR}/genfit
#${CMAKE_SOURCE_DIR}/recotasks
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(FTOF_SRCS
PndGeoFtof.cxx 
PndFtof.cxx 
PndFtofPoint.cxx 
PndFtofHit.cxx 
PndGeoFtofPar.cxx 
PndFtofContFact.cxx
PndFtofHitProducerIdeal.cxx  
#PndFtofAnaIdeal.cxx
#PndFtofMassTrig.cxx
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${FTOF_SRCS}" "${INCLUDE_DIRECTORIES}" FTOF_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h FTOF_HEADERS "${FTOF_SRCS}")

set(FTOF_LINKDEF  FtofLinkDef.h)
set(FTOF_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/PndFtofDict.cxx) 

ROOT_GENERATE_DICTIONARY("${FTOF_HEADERS}" "${FTOF_LINKDEF}" "${FTOF_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(FTOF_SRCS ${FTOF_SRCS} ${FTOF_DICTIONARY})

add_library(Ftof SHARED ${FTOF_SRCS})
target_link_libraries(Ftof ${ROOT_LIBRARIES})
set_target_properties(Ftof PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS Ftof DESTINATION ${CMAKE_BINARY_DIR}/lib)
