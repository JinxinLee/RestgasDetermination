# Create a library called "libResults" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CLHEP_BASE_DIR}/include
${CLHEP_INCLUDE_DIR}
${BASE_INCLUDE_DIRECTORIES} 
${CMAKE_SOURCE_DIR}/pgenerators/PndFlukaGen
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(PNDFLUKAGEN_SRCS
FlukaResults.cpp
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${PNDFLUKAGEN_SRCS}" "${INCLUDE_DIRECTORIES}" PNDFLUKAGEN_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cpp *.h PNDFLUKAGEN_HEADERS "${PNDFLUKAGEN_SRCS}")

set(PNDFLUKAGEN_LINKDEF FlukaResultsLinkDef.h)
set(PNDFLUKAGEN_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/FlukaResultsDict.cxx) 

ROOT_GENERATE_DICTIONARY("${PNDFLUKAGEN_HEADERS}" "${PNDFLUKAGEN_LINKDEF}" "${PNDFLUKAGEN_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(PNDFLUKAGEN_SRCS ${PNDFLUKAGEN_SRCS} ${PNDFLUKAGEN_DICTIONARY})

add_library(FlukaResults SHARED ${PNDFLUKAGEN_SRCS})
target_link_libraries(FlukaResults ${ROOT_LIBRARIES})
set_target_properties(FlukaResults PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS FlukaResults DESTINATION ${CMAKE_BINARY_DIR}/lib)
