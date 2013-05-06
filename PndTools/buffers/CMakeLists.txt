# Create a library called "buffers" which includes the source files given in
# the array .
# The header extension is already found. Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR}
${GEANT3_INCLUDE_DIR} 
${CLHEP_INCLUDE_DIR}
${BASE_INCLUDE_DIRECTORIES} 
${CMAKE_SOURCE_DIR}/field
${CMAKE_SOURCE_DIR}/pnddata
${CMAKE_SOURCE_DIR}/pnddata/MvdData
${CMAKE_SOURCE_DIR}/pnddata/SdsData
${CMAKE_SOURCE_DIR}/pnddata/TrackData
${CMAKE_SOURCE_DIR}/passive
${CMAKE_SOURCE_DIR}/PndTools/generalTools
${CMAKE_SOURCE_DIR}/PndTools/riemannfit
${CMAKE_SOURCE_DIR}/sds/SdsMC
${CMAKE_SOURCE_DIR}/sds/SdsTools
${CMAKE_SOURCE_DIR}/sds/SdsDigi
${CMAKE_SOURCE_DIR}/sds
${CMAKE_SOURCE_DIR}/genfit
${CMAKE_SOURCE_DIR}/recotasks
${CMAKE_SOURCE_DIR}/trackbase
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/GeaneTrackRep
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/LSLtrackRep
${CMAKE_SOURCE_DIR}/geane
${CMAKE_SOURCE_DIR}/tpc
${CMAKE_SOURCE_DIR}/PndTools/riemannfit
${CMAKE_SOURCE_DIR}/PndTools/buffers
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(BUFFERS_SRCS
#PndRingSorterT.cxx
#PndWriteoutBufferT.cxx
#PndWriteoutBuffer.cxx
#PndSorterTaskT.cxx

)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h BUFFERS_HEADERS "${BUFFERS_SRCS}")

set(BUFFERS_LINKDEF  buffersLinkDef.h)
set(BUFFERS_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/buffersDict.cxx) 

ROOT_GENERATE_DICTIONARY("${BUFFERS_HEADERS}" "${BUFFERS_LINKDEF}" "${BUFFERS_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(BUFFERS_SRCS ${BUFFERS_SRCS} ${BUFFERS_DICTIONARY})

add_library(buffers SHARED ${BUFFERS_SRCS})
target_link_libraries(buffers ${ROOT_LIBRARIES})
set_target_properties(buffers PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS buffers DESTINATION ${CMAKE_BINARY_DIR}/lib)
