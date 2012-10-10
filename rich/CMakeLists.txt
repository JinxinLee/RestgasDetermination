# Create a library called "libRich" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
#put here all directories where header files are located
${PANDAROOT_SOURCE_DIR}/fairtools
${PANDAROOT_SOURCE_DIR}/base
${PANDAROOT_SOURCE_DIR}/pnddata
${PANDAROOT_SOURCE_DIR}/pnddata/RichData
${PANDAROOT_SOURCE_DIR}/parbase
${PANDAROOT_SOURCE_DIR}/geobase
${PANDAROOT_SOURCE_DIR}/rich
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
) 

link_directories( ${LINK_DIRECTORIES})

set(PNDRICH_SRCS
#Put here your sourcefiles
PndRich.cxx
PndRichContFact.cxx
PndRichGeo.cxx
PndRichGeoPar.cxx
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h PNDRICH_HEADERS "${PNDRICH_SRCS}")

set(PNDRICH_LINKDEF RichLinkDef.h)
set(PNDRICH_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/PndRichDict.cxx) 

ROOT_GENERATE_DICTIONARY("${PNDRICH_HEADERS}" "${PNDRICH_LINKDEF}" "${PNDRICH_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

set(PNDRICH_SRCS ${PNDRICH_SRCS} ${PNDRICH_DICTIONARY})

add_library(Rich SHARED ${PNDRICH_SRCS})
target_link_libraries(Rich ${ROOT_LIBRARIES})
set_target_properties(Rich PROPERTIES  ${FAIRROOT_LIBRARY_PROPERTIES})

################ install ###################
install(TARGETS Rich DESTINATION ${CMAKE_BINARY_DIR}/lib)
