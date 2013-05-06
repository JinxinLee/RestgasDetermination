# Create a library called "" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR}
${BASE_INCLUDE_DIRECTORIES} 
${CMAKE_SOURCE_DIR}/field
${CMAKE_SOURCE_DIR}/pnddata
${CMAKE_SOURCE_DIR}/pnddata/DskData
${CMAKE_SOURCE_DIR}/dsk
${CMAKE_SOURCE_DIR}/dsk/DskMC

)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})

set(DSK_SRCS
DskMC/PndDsk.cxx
DskMC/PndGeoDskFLG.cxx
DskMC/PndDskFLGHitProducerIdeal.cxx
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${DSK_SRCS}" "${INCLUDE_DIRECTORIES}" DSK_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h DSK_HEADERS "${DSK_SRCS}")

set(DSK_LINKDEF DskLinkDef.h)
set(DSK_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/PndDskDict.cxx)

ROOT_GENERATE_DICTIONARY("${DSK_HEADERS}" "${DSK_LINKDEF}" "${DSK_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(DSK_SRCS ${DSK_SRCS} ${DSK_DICTIONARY})

add_library(Dsk SHARED ${DSK_SRCS})
target_link_libraries(Dsk ${ROOT_LIBRARIES})
set_target_properties(Dsk PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})


################ install ###################
install(TARGETS Dsk DESTINATION ${CMAKE_BINARY_DIR}/lib)
