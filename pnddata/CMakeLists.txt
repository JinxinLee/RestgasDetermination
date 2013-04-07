# Create a library called "libPNDDATA" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CMAKE_SOURCE_DIR}/fairtools
${CMAKE_SOURCE_DIR}/geobase 
${CMAKE_SOURCE_DIR}/base 
${CMAKE_SOURCE_DIR}/pnddata 
${CMAKE_SOURCE_DIR}/pnddata/TrackData
${CMAKE_SOURCE_DIR}/trackbase
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${CMAKE_SOURCE_DIR}/base
)
 
link_directories( ${LINK_DIRECTORIES})

set(PNDDATA_SRCS
PndStack.cxx 
PndMCTrack.cxx

DskData/PndDskCerenkov.cxx
DskData/PndDskParticle.cxx
DskData/PndDskTrackPoint.cxx
DskData/PndDskFLGHit.cxx

DrcData/PndDrcPDPoint.cxx 
DrcData/PndDrcBarPoint.cxx
DrcData/PndDrcHit.cxx
DrcData/PndDrcPDHit.cxx
DrcData/PndDrcDigi.cxx

GemData/PndGemMCPoint.cxx
GemData/PndGemDigi.cxx
GemData/PndGemHit.cxx

MdtData/PndMdtPoint.cxx
MdtData/PndMdtDigi.cxx
MdtData/PndMdtCluster.cxx
MdtData/PndMdtHit.cxx
MdtData/PndMdtTrk.cxx

SttData/PndSttGeomPoint.cxx
SttData/PndSttHelixHit.cxx
SttData/PndSttHit.cxx  
SttData/PndSttHitInfo.cxx  
SttData/PndSttPoint.cxx  
SttData/PndSttTrack.cxx

FtsData/PndFtsPoint.cxx
FtsData/PndFtsHit.cxx

HypData/PndHypCluster.cxx
HypData/PndHypDigi.cxx
HypData/PndHypDigiStrip.cxx
HypData/PndHypHit.cxx
HypData/PndHypHitInfo.cxx
HypData/PndHypPidCand.cxx
HypData/PndHypPoint.cxx

SdsData/PndSdsMCPoint.cxx
SdsData/PndSdsDigi.cxx
SdsData/PndSdsPixel.cxx
SdsData/PndSdsDigiPixel.cxx
SdsData/PndSdsStrip.cxx
SdsData/PndSdsDigiStrip.cxx
SdsData/PndSdsCluster.cxx
SdsData/PndSdsClusterPixel.cxx
SdsData/PndSdsClusterStrip.cxx
SdsData/PndSdsHit.cxx
#SdsData/PndSdsHitInfo.cxx
SdsData/PndSdsPidCand.cxx
SdsData/PndSdsRadDamHit.cxx
SdsData/PndSdsDigiPixelMCInfo.cxx

RichData/PndRichPoint.cxx
 
MvdData/PndMvdApvHit.cxx
MvdData/PndMvdPidCand.cxx
MvdData/PndMvdRadDamHit.cxx

SciTData/PndSciTPoint.cxx
SciTData/PndSciTHit.cxx

TrackData/PndTrackCandHit.cxx
TrackData/PndTrackCand.cxx
TrackData/PndTrack.cxx
TrackData/PndTrackID.cxx
#TrackData/PndVertex.cxx
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${PNDDATA_SRCS}" "${INCLUDE_DIRECTORIES}" PNDDATA_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h PNDDATA_HEADERS "${PNDDATA_SRCS}")
SET( PNDDATA_HEADERS ${PNDDATA_HEADERS} HypData/PndHypStrip.h PndDetectorList.h )

set(PNDDATA_LINKDEF  PndDataLinkDef.h)
set(PNDDATA_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/PndDataDict.cxx) 

ROOT_GENERATE_DICTIONARY("${PNDDATA_HEADERS}" "${PNDDATA_LINKDEF}" "${PNDDATA_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(PNDDATA_SRCS ${PNDDATA_SRCS} ${PNDDATA_DICTIONARY})

add_library(PndData SHARED ${PNDDATA_SRCS})
target_link_libraries(PndData ${ROOT_LIBRARIES})
set_target_properties(PndData PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS PndData DESTINATION ${CMAKE_BINARY_DIR}/lib)

