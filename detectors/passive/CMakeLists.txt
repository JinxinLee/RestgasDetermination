# Create a library called "libPassive" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES} 
  ${BASE_INCLUDE_DIRECTORIES} 
)

Set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/passive
)

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
PndCave.cxx            
PndGeoCave.cxx        
PndGeoMagnet.cxx      
PndGeoPassivePar.cxx  
PndGeoPipe.cxx        
PndGeoTarget.cxx      
PndMagnet.cxx         
PndPassiveContFact.cxx
PndPipe.cxx           
PndTarget.cxx 
)

set(LINKDEF  PassiveLinkDef.h)
set(LIBRARY_NAME Passive)

set(DEPENDENCIES Base GeoBase ParBase)

GENERATE_LIBRARY()
