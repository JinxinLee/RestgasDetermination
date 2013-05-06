# Create a library called "libPlane" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${GEANT3_INCLUDE_DIR}
${BASE_INCLUDE_DIRECTORIES} 
${CMAKE_SOURCE_DIR}/pnddata 
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/plane
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(PLANE_SRCS
CbmGeoPlane.cxx    			
CbmPlanePoint.cxx 			
CbmPlane.cxx   			
CbmGeoPlanePar.cxx 			
CbmPlaneContFact.cxx 			
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h PLANE_HEADERS "${PLANE_SRCS}")

set(PLANE_LINKDEF  PlaneLinkDef.h)
set(PLANE_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/CbmPlaneDict.cxx) 

ROOT_GENERATE_DICTIONARY("${PLANE_HEADERS}" "${PLANE_LINKDEF}" "${PLANE_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(PLANE_SRCS ${PLANE_SRCS} ${PLANE_DICTIONARY})

add_library(Plane SHARED ${PLANE_SRCS})
target_link_libraries(Plane ${ROOT_LIBRARIES})
set_target_properties(Plane PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS Plane DESTINATION ${CMAKE_BINARY_DIR}/lib)
