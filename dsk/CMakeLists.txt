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
${FAIRROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})

set(SRCS
DskMC/PndDsk.cxx
DskMC/PndGeoDskFLG.cxx
DskMC/PndDskFLGHitProducerIdeal.cxx
)

set(LINKDEF DskLinkDef.h)
set(LIBRARY_NAME Dsk)

set(DEPENDENCIES Base GeoBase ParBase PndData)

GENERATE_LIBRARY()
