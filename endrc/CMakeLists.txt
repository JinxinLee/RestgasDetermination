# Create a library called "" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR}
${CMAKE_SOURCE_DIR}/geobase
${CMAKE_SOURCE_DIR}/base
${CMAKE_SOURCE_DIR}/parbase
${CMAKE_SOURCE_DIR}/field
${CMAKE_SOURCE_DIR}/pnddata
${CMAKE_SOURCE_DIR}/endrc
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(DDRC_SRCS
PndEnDrc.cxx
PndGeoEnDrc.cxx
PndEnDrcPoint.cxx
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h DDRC_HEADERS "${DDRC_SRCS}")

set(DDRC_LINKDEF EnDrcLinkDef.h)
set(DDRC_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/PndEnDrcDict.cxx) 

ROOT_GENERATE_DICTIONARY("${DDRC_HEADERS}" "${DDRC_LINKDEF}" "${DDRC_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(DDRC_SRCS ${DDRC_SRCS} ${DDRC_DICTIONARY})

add_library(EnDrc SHARED ${DDRC_SRCS})
target_link_libraries(EnDrc ${ROOT_LIBRARIES})
set_target_properties(EnDrc PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES} )

################ install ###################
install(TARGETS EnDrc DESTINATION ${CMAKE_BINARY_DIR}/lib)
