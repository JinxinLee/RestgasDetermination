# Create a library called "libTracking" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.



set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR}
${BASE_INCLUDE_DIRECTORIES} 
${CMAKE_SOURCE_DIR}/pnddata 
${CMAKE_SOURCE_DIR}/pnddata/SttData
${CMAKE_SOURCE_DIR}/pnddata/TrackData
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/stt
${CMAKE_SOURCE_DIR}/pnddata/SdsData
${CMAKE_SOURCE_DIR}/PndTools/generalTools/
${CMAKE_SOURCE_DIR}/catracking
${CMAKE_SOURCE_DIR}/catracking/code
${CMAKE_SOURCE_DIR}/catracking/code/Performance
)

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES} )
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-pragmas  -DDO_TPCCATRACKER_EFF_PERFORMANCE -DINCLUDE_LR_EFF -DUSE_TIMERS -DPANDA_STT -O3")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3 -DDO_TPCCATRACKER_EFF_PERFORMANCE")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3")


set(HEADERS
code/Performance/PndCATrackMCPointContainer.h
PndCATracking.h
)

set(SRCS
PndCATracking.cxx
code/PndCAFunctionality.cxx
code/PndCAGBTrack.cxx
code/PndCAParam.cxx
code/PndCATrackParam.cxx
code/PndCAGBHit.cxx
code/PndCAGBTracker.cxx
code/PndCAParameters.cxx
code/PndCATrackParamVector.cxx
code/Performance/PndCAGlobalPerformance.cxx  
code/Performance/PndCAPerformanceBase.cxx
code/Performance/PndCAMCPoint.cxx            
code/Performance/PndCAPerformance.cxx
code/Performance/PndCAMCTrack.cxx            
code/Performance/PndCAMCVertex.cxx           
code/Performance/PndCATrackMCPointContainer.cxx 
code/Performance/PndCATrackPerformanceBase.cxx
)

set(LINKDEF  CATrackingLinkDef.h)
set(LIBRARY_NAME CATracking)

set(DEPENDENCIES PndData Stt Vc.a)

GENERATE_LIBRARY()
