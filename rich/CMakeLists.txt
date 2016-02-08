# Create a library called "libRich" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES}
  ${GEANT3_INCLUDE_DIR}
  ${BASE_INCLUDE_DIRECTORIES} 
  ${CLHEP_INCLUDE_DIR}
)

Set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/rich	
  ${CMAKE_SOURCE_DIR}/pnddata
  ${CMAKE_SOURCE_DIR}/pnddata/RichData
  ${CMAKE_SOURCE_DIR}/pnddata/PidData
  ${CMAKE_SOURCE_DIR}/PndTools/generalTools
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
PndRichReco.cxx
PndRichHitProducer.cxx
PndRichResolution.cxx
PndRichRecoTask.cxx
)

set(LINKDEF RichLinkDef.h)
set(LIBRARY_NAME Rich)

set(DEPENDENCIES Base PndData generalTools)

GENERATE_LIBRARY()
