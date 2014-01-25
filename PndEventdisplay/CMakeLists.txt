# Create a library called "libPndEventDisplay" 
# 18.06.09  T.Stockmanns

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${GEANT3_INCLUDE_DIR}
${BASE_INCLUDE_DIRECTORIES} 
${CMAKE_SOURCE_DIR}/PndTools/generalTools 
${CMAKE_SOURCE_DIR}/PndTools/HelixPropagator
${CMAKE_SOURCE_DIR}/PndMCMatch
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/trackbase 
${CMAKE_SOURCE_DIR}/geane
${CMAKE_SOURCE_DIR}/genfit
${CMAKE_SOURCE_DIR}/lhetrack
${CMAKE_SOURCE_DIR}/eventdisplay
${CMAKE_SOURCE_DIR}/pnddata
${CMAKE_SOURCE_DIR}/pnddata/DrcData
${CMAKE_SOURCE_DIR}/pnddata/SttData
${CMAKE_SOURCE_DIR}/pnddata/TrackData
${CMAKE_SOURCE_DIR}/pnddata/MvdData
${CMAKE_SOURCE_DIR}/pnddata/SdsData
${CMAKE_SOURCE_DIR}/drc
${CMAKE_SOURCE_DIR}/mvd/MvdDigi
${CMAKE_SOURCE_DIR}/sds/SdsDigi
${CMAKE_SOURCE_DIR}/stt
${CMAKE_SOURCE_DIR}/PndEventdisplay
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
PndTrackCandDraw.cxx
PndTrackDraw.cxx
PndDrcAccuDigiPixelDraw.cxx
PndSttMCPointDraw.cxx
PndMvdDigiPixelDraw.cxx
PndSttIsochroneDraw.cxx
)

set(LINKDEF PndEventDisplayLinkDef.h)
set(LIBRARY_NAME PndEventDisplay)

set(DEPENDENCIES Base EventDisplay generalTools MCMatch Geane
ParBase PndData Sds Stt helixpropagator)

GENERATE_LIBRARY()
