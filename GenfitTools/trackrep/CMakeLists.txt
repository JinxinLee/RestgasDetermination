# Create a library called "libtrackrep" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CLHEP_INCLUDE_DIR}
${BASE_INCLUDE_DIRECTORIES} 
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/GeaneTrackRep
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/LSLtrackRep
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/RKTrackRep
${CMAKE_SOURCE_DIR}/genfit
${CMAKE_SOURCE_DIR}/trackbase
${CMAKE_SOURCE_DIR}/geane
${GEANT3_INCLUDE_DIR}
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})


set(SRCS 
GeaneTrackRep/GeaneTrackRep.cxx
LSLtrackRep/LSLTrackRep.cxx
LSLtrackRep/LSLEQM.cxx
LSLtrackRep/Nystrom.cxx
RKTrackRep/RKTrackRep.cxx
)

set(LINKDEF  trackrepLinkDef.h)
set(LIBRARY_NAME trackrep)

set(DEPENDENCIES Geane TrkBase)

GENERATE_LIBRARY()
