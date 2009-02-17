# Create a library called "" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CMAKE_SOURCE_DIR}/geobase 
${CMAKE_SOURCE_DIR}/parbase 
${CMAKE_SOURCE_DIR}/base 
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/pnddata 
${CMAKE_SOURCE_DIR}/muo 
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(MUO_SRCS
PndGeoMuo.cxx 
PndMuo.cxx
PndGeoMuoPar.cxx 
PndMuoPoint.cxx 
PndMuoContFact.cxx 
PndMuoDigi.cxx
PndMuoDigiProducer.cxx
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h MUO_HEADERS "${MUO_SRCS}")

set(MUO_LINKDEF  MuoLinkDef.h)
set(MUO_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/PndMuoDict.cxx) 

ROOT_GENERATE_DICTIONARY("${MUO_HEADERS}" "${MUO_LINKDEF}" "${MUO_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(MUO_SRCS ${MUO_SRCS} ${MUO_DICTIONARY})

add_library(Muo SHARED ${MUO_SRCS})
target_link_libraries(Muo ${ROOT_LIBRARIES})
set_target_properties(Muo PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS Muo DESTINATION ${CMAKE_BINARY_DIR}/lib)
