# Create a library called "libPNDDATA" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES} 
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

set(SRCS
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

PidData/PndPidCandidate.cxx
PidData/PndPidProbability.cxx

FairRecoCandidate.cxx
)

set(HEADERS
PndStack.h 
PndMCTrack.h

DskData/PndDskCerenkov.h
DskData/PndDskParticle.h
DskData/PndDskTrackPoint.h
DskData/PndDskFLGHit.h

DrcData/PndDrcPDPoint.h 
DrcData/PndDrcBarPoint.h
DrcData/PndDrcHit.h
DrcData/PndDrcPDHit.h
DrcData/PndDrcDigi.h

GemData/PndGemMCPoint.h
GemData/PndGemDigi.h
GemData/PndGemHit.h

MdtData/PndMdtPoint.h
MdtData/PndMdtDigi.h
MdtData/PndMdtCluster.h
MdtData/PndMdtHit.h
MdtData/PndMdtTrk.h

SttData/PndSttGeomPoint.h
SttData/PndSttHelixHit.h
SttData/PndSttHit.h  
SttData/PndSttHitInfo.h  
SttData/PndSttPoint.h  
SttData/PndSttTrack.h

FtsData/PndFtsPoint.h
FtsData/PndFtsHit.h

HypData/PndHypCluster.h
HypData/PndHypDigi.h
HypData/PndHypDigiStrip.h
HypData/PndHypHit.h
HypData/PndHypHitInfo.h
HypData/PndHypPidCand.h
HypData/PndHypPoint.h
HypData/PndHypStrip.h

SdsData/PndSdsMCPoint.h
SdsData/PndSdsDigi.h
SdsData/PndSdsPixel.h
SdsData/PndSdsDigiPixel.h
SdsData/PndSdsStrip.h
SdsData/PndSdsDigiStrip.h
SdsData/PndSdsCluster.h
SdsData/PndSdsClusterPixel.h
SdsData/PndSdsClusterStrip.h
SdsData/PndSdsHit.h
#SdsData/PndSdsHitInfo.h
SdsData/PndSdsPidCand.h
SdsData/PndSdsRadDamHit.h
SdsData/PndSdsDigiPixelMCInfo.h

RichData/PndRichPoint.h
 
MvdData/PndMvdApvHit.h
MvdData/PndMvdPidCand.h
MvdData/PndMvdRadDamHit.h

SciTData/PndSciTPoint.h
SciTData/PndSciTHit.h

TrackData/PndTrackCandHit.h
TrackData/PndTrackCand.h
TrackData/PndTrack.h
TrackData/PndTrackID.h
#TrackData/PndVertex.h

PidData/PndPidCandidate.h
PidData/PndPidProbability.h

FairRecoCandidate.h
)

set(LINKDEF  PndDataLinkDef.h)
set(LIBRARY_NAME PndData)

set(DEPENDENCIES Base GeoBase TrkBase)

GENERATE_LIBRARY()
