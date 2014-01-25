# Create a library called "tpcreco" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CLHEP_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES}
${CMAKE_SOURCE_DIR}/trackbase 
${CMAKE_SOURCE_DIR}/PndTools/HelixPropagator
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
PndHelixPropagator.cxx
)

set(LINKDEF  helixPropagatorLinkDef.h)
set(LIBRARY_NAME helixpropagator)

set(DEPENDENCIES Base)

GENERATE_LIBRARY()
