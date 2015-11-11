# Create a library called "buffers" which includes the source files given in
# the array .
# The header extension is already found. Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES}
#${GEANT3_INCLUDE_DIR} 
#${CLHEP_INCLUDE_DIR}
  ${BASE_INCLUDE_DIRECTORIES} 
)

Set(INCLUDE_DIRECTORIES
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
  ${CMAKE_SOURCE_DIR}/trackbase
  ${CMAKE_SOURCE_DIR}/PndTools/riemannfit
  ${CMAKE_SOURCE_DIR}/PndTools/buffers
)

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
#PndRingSorterT.cxx
#PndWriteoutBufferT.cxx
#PndWriteoutBuffer.cxx
#PndSorterTaskT.cxx

)

set(LINKDEF  buffersLinkDef.h)
set(LIBRARY_NAME buffers)

set(DEPENDENCIES Base generalTools PndData)

GENERATE_LIBRARY()
