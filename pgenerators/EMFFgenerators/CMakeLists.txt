# Create a library called "libEMFFGEN" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES}
  ${BASE_INCLUDE_DIRECTORIES}
)

set(INCLUDE_DIRECTORIES
${CMAKE_SOURCE_DIR}/fairtools
${CMAKE_SOURCE_DIR}/pgenerators/EMFFgenerators
${BASE_INCLUDE_DIRECTORIES}
)

Include_Directories( ${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(EMFFGEN_SRCS
PndPiPiGenerator.cxx
PndLepLepGenerator.cxx
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${EMFFGEN_SRCS}" "${INCLUDE_DIRECTORIES}" EMFFGEN_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h EMFFGEN_HEADERS "${EMFFGEN_SRCS}")

set(EMFFGEN_LINKDEF  EMFFGENLinkDef.h)
set(EMFFGEN_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/PndEMFFGENDict.cxx) 

ROOT_GENERATE_DICTIONARY("${EMFFGEN_HEADERS}" "${EMFFGEN_LINKDEF}" "${EMFFGEN_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(EMFFGEN_SRCS 
  ${EMFFGEN_SRCS} 
  ${EMFFGEN_DICTIONARY}
mzfunctions.cxx
mzfunctions_pp_to_pipi_vandewi.cxx
mzfunctions_pp_to_leplep_vandewi.cxx
ranlxd.cxx
ranlxs.cxx
)

add_library(EMFFGEN SHARED ${EMFFGEN_SRCS})
target_link_libraries(EMFFGEN ${ROOT_LIBRARIES})
set_target_properties(EMFFGEN PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS EMFFGEN DESTINATION ${CMAKE_BINARY_DIR}/lib)
