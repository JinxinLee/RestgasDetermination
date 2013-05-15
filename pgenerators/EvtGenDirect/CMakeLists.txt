# Create a library called "libEvtGenDirect" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CLHEP_BASE_DIR}/include
${CLHEP_INCLUDE_DIR}
${BASE_INCLUDE_DIRECTORIES} 
${CMAKE_SOURCE_DIR}/pgenerators/EvtGen
${CMAKE_SOURCE_DIR}/pgenerators/EvtGenDirect
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${PYTHIA6_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
PndEvtGenDirect.cxx
)

set(LINKDEF  EvtGenDirectLinkDef.h)
set(LIBRARY_NAME EvtGenDirect)

set(DEPENDENCIES EvtGen)

GENERATE_LIBRARY()
