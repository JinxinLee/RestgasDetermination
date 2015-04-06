# Create a library called "libRich" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES} 
  ${BASE_INCLUDE_DIRECTORIES} 
)

Set(INCLUDE_DIRECTORIES
  ${PANDAROOT_SOURCE_DIR}/pnddata
  ${PANDAROOT_SOURCE_DIR}/pnddata/RichData
  ${PANDAROOT_SOURCE_DIR}/rich
)

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

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
