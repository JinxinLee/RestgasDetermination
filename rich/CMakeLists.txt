# Create a library called "libRich" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
#put here all directories where header files are located
${BASE_INCLUDE_DIRECTORIES} 
${PANDAROOT_SOURCE_DIR}/pnddata
${PANDAROOT_SOURCE_DIR}/pnddata/RichData
${PANDAROOT_SOURCE_DIR}/rich
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
) 

link_directories( ${LINK_DIRECTORIES})

set(SRCS
#Put here your sourcefiles
PndRich.cxx
PndRichContFact.cxx
PndRichGeo.cxx
PndRichGeoPar.cxx
)

set(LINKDEF RichLinkDef.h)
set(LIBRARY_NAME Rich)

set(DEPENDENCIES Base PndData GeoBase ParBase)

GENERATE_LIBRARY()
