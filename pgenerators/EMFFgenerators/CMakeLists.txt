# Create a library called "libPIPIGEN" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES} 
${CMAKE_SOURCE_DIR}/pgenerators/EMFFgenerators

)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(PIPIGEN_SRCS
PndPiPiGenerator.cxx
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${PIPIGEN_SRCS}" "${INCLUDE_DIRECTORIES}" PIPIGEN_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h PIPIGEN_HEADERS "${PIPIGEN_SRCS}")

set(PIPIGEN_LINKDEF  PIPIGENLinkDef.h)
set(PIPIGEN_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/PndPIPIGENDict.cxx) 

ROOT_GENERATE_DICTIONARY("${PIPIGEN_HEADERS}" "${PIPIGEN_LINKDEF}" "${PIPIGEN_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(PIPIGEN_SRCS 
  ${PIPIGEN_SRCS} 
  ${PIPIGEN_DICTIONARY}
mzfunctions.cxx
mzfunctions_pp_to_pipi_vandewi.cxx
ranlxd.cxx
ranlxs.cxx
)

add_library(PIPIGEN SHARED ${PIPIGEN_SRCS})
target_link_libraries(PIPIGEN ${ROOT_LIBRARIES})
set_target_properties(PIPIGEN PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS PIPIGEN DESTINATION ${CMAKE_BINARY_DIR}/lib)
