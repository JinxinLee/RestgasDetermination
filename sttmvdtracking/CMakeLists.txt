# Create a library called "libSttMvdTracking" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.


set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${GEANT3_INCLUDE_DIR}
${BASE_INCLUDE_DIRECTORIES} 
${SIMPATH}/include
${CMAKE_SOURCE_DIR}/pnddata 
${CMAKE_SOURCE_DIR}/pnddata/SttData
${CMAKE_SOURCE_DIR}/trackbase
${CMAKE_SOURCE_DIR}/pnddata/TrackData
${CMAKE_SOURCE_DIR}/PndTools/riemannfit
${CMAKE_SOURCE_DIR}/PndTools/generalTools
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/stt
${CMAKE_SOURCE_DIR}/sttmvdtracking
${CMAKE_SOURCE_DIR}/mvd/MvdTracking
${CMAKE_SOURCE_DIR}/pnddata/SdsData
${CMAKE_SOURCE_DIR}/pnddata/SciTData
${CMAKE_SOURCE_DIR}/gem 
${CMAKE_SOURCE_DIR}/pnddata/GemData 
${CMAKE_SOURCE_DIR}/geane 
)

include_directories( ${INCLUDE_DIRECTORIES})



set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
# PndSttMvdTracking.cxx
PndSttMvdGemTracking.cxx
PndSttMvdGemTrackingIdeal.cxx
#PndSttMvdV0PR.cxx
PndMixBackgroundEvents.cxx
#  PndSecondaryTrackFinder.cxx
PndLambdaIM.cxx
PndMvdSttGemRiemannTrackFinder.cxx
)

set(LINKDEF  SttMvdTrackingLinkDef.h)
set(LIBRARY_NAME SttMvdTracking)

set(DEPENDENCIES Base GeoBase ParBase PndData TrkBase Geane Gem Stt riemann)

GENERATE_LIBRARY()
