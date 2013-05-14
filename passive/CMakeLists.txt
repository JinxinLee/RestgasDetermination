# Create a library called "libPassive" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES} 
${CMAKE_SOURCE_DIR}/passive
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
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
