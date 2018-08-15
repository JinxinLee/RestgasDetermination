# Create a library called "libBase" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.


set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${GEANT3_INCLUDE_DIR}
${BASE_INCLUDE_DIRECTORIES} 
${CMAKE_SOURCE_DIR}/detectors/emc/EmcData
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/pnddata
${CMAKE_SOURCE_DIR}/geane
${CMAKE_SOURCE_DIR}/trackbase
${CMAKE_SOURCE_DIR}/plane
${CMAKE_SOURCE_DIR}/detectors/stt
${CMAKE_SOURCE_DIR}/detectors/mvd
${CMAKE_SOURCE_DIR}/genfit
${CMAKE_SOURCE_DIR}/GeaneExEmc
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(GEANEEX_SRCS
FairGeaneTrEmc.cxx
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h GEANEEX_HEADERS "${GEANEEX_SRCS}")

set(GEANEEX_LINKDEF GeaneExEmcLinkDef.h)
set(GEANEEX_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/GeaneExEmcDict.cxx) 

ROOT_GENERATE_DICTIONARY("${GEANEEX_HEADERS}" "${GEANEEX_LINKDEF}" "${GEANEEX_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(GEANEEX_SRCS ${GEANEEX_SRCS} ${GEANEEX_DICTIONARY})

add_library(GeaneExEmc SHARED ${GEANEEX_SRCS})
target_link_libraries(GeaneExEmc ${ROOT_LIBRARIES})
set_target_properties(GeaneExEmc PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS GeaneExEmc DESTINATION ${CMAKE_BINARY_DIR}/lib)
