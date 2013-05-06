# Create a library called "libBase" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.


set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${GEANT3_INCLUDE_DIR}
${BASE_INCLUDE_DIRECTORIES} 
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/pnddata
${CMAKE_SOURCE_DIR}/geane
${CMAKE_SOURCE_DIR}/trackbase
${CMAKE_SOURCE_DIR}/plane
${CMAKE_SOURCE_DIR}/stt
${CMAKE_SOURCE_DIR}/genfit
${CMAKE_SOURCE_DIR}/GeaneEx
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(GEANEEX_SRCS
FairGeaneTrT.cxx
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h GEANEEX_HEADERS "${GEANEEX_SRCS}")

set(GEANEEX_LINKDEF GeaneExLinkDef.h)
set(GEANEEX_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/GeaneExDict.cxx) 

ROOT_GENERATE_DICTIONARY("${GEANEEX_HEADERS}" "${GEANEEX_LINKDEF}" "${GEANEEX_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(GEANEEX_SRCS ${GEANEEX_SRCS} ${GEANEEX_DICTIONARY})

add_library(GeaneEx SHARED ${GEANEEX_SRCS})
target_link_libraries(GeaneEx ${ROOT_LIBRARIES})
set_target_properties(GeaneEx PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS GeaneEx DESTINATION ${CMAKE_BINARY_DIR}/lib)
