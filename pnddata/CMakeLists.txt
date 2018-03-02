# Create a library called "libPNDDATA" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES} 
  ${BASE_INCLUDE_DIRECTORIES} 
)
Set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/pnddata 
  ${CMAKE_SOURCE_DIR}/pnddata/DrcData
  ${CMAKE_SOURCE_DIR}/pnddata/DskData
  ${CMAKE_SOURCE_DIR}/pnddata/DiscData
  ${CMAKE_SOURCE_DIR}/pnddata/FtsData
  ${CMAKE_SOURCE_DIR}/pnddata/GemData
  ${CMAKE_SOURCE_DIR}/pnddata/HypData
  ${CMAKE_SOURCE_DIR}/pnddata/HypGData
  ${CMAKE_SOURCE_DIR}/pnddata/MdtData
  ${CMAKE_SOURCE_DIR}/pnddata/MvdData
  ${CMAKE_SOURCE_DIR}/pnddata/PidData
  ${CMAKE_SOURCE_DIR}/pnddata/RichData
  ${CMAKE_SOURCE_DIR}/pnddata/SciTData
  ${CMAKE_SOURCE_DIR}/pnddata/SdsData
  ${CMAKE_SOURCE_DIR}/pnddata/SttData
  ${CMAKE_SOURCE_DIR}/pnddata/scitil
  ${CMAKE_SOURCE_DIR}/pnddata/TrackData
  ${CMAKE_SOURCE_DIR}/trackbase
)

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${BASE_LINK_DIRECTORIES}
${FAIRROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
PndStack.cxx 
PndMCTrack.cxx

DskData/PndDskCerenkov.cxx
DskData/PndDskParticle.cxx
DskData/PndDskTrackPoint.cxx
DskData/PndDskFLGHit.cxx

DiscData/PndDiscDigitizedHit.cxx
DiscData/PndDiscParticleMCPoint.cxx
DiscData/PndDiscSensorMCPoint.cxx
DiscData/PndDiscReconResult.cxx
DiscData/PndDiscPID.cxx

DiscData/PndDiscDigitizedHit.cxx
DiscData/PndDiscParticleMCPoint.cxx
DiscData/PndDiscSensorMCPoint.cxx
DiscData/PndDiscReconResult.cxx
DiscData/PndDiscPID.cxx

DrcData/PndDrcEVPoint.cxx
DrcData/PndDrcPDPoint.cxx 
DrcData/PndDrcBarPoint.cxx
DrcData/PndDrcHit.cxx
DrcData/PndDrcPDHit.cxx
DrcData/PndDrcDigi.cxx
DrcData/PndDrcLutNode.cxx
DrcData/PndDrcLutInfo.cxx
DrcData/PndDrcTrackInfo.cxx
DrcData/PndDrcPhotonInfo.cxx
DrcData/PndDrcAmbiguityInfo.cxx

GemData/PndGemMCPoint.cxx
GemData/PndGemDigi.cxx
GemData/PndGemCluster.cxx
GemData/PndGemHit.cxx

MdtData/PndMdtPoint.cxx
MdtData/PndMdtDigi.cxx
MdtData/PndMdtCluster.cxx
MdtData/PndMdtHit.cxx
MdtData/PndMdtTrk.cxx
MdtData/PndMdtID.cxx

SttData/PndSttGeomPoint.cxx
SttData/PndSttHelixHit.cxx
SttData/PndSttHit.cxx  
SttData/PndSttSkewedHit.cxx  
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
SdsData/PndSdsDigiTopix4.cxx
SdsData/PndSdsDigiTopix4Header.cxx
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

RichData/PndRichPhoton.cxx
RichData/PndRichPDPoint.cxx
RichData/PndRichBarPoint.cxx
RichData/PndRichDigi.cxx
RichData/PndRichHit.cxx
RichData/PndRichPDHit.cxx
RichData/PndRichMirrorSegment.cxx
 
MvdData/PndMvdApvHit.cxx
MvdData/PndMvdPidCand.cxx
MvdData/PndMvdRadDamHit.cxx

SciTData/PndSciTPoint.cxx
SciTData/PndSciTHit.cxx

TrackData/PndTrackCandHit.cxx
TrackData/PndTrackCand.cxx
TrackData/PndTrack.cxx
TrackData/PndSimpleTrack.cxx
TrackData/PndTrackID.cxx
#TrackData/PndVertex.cxx

PidData/PndPidCandidate.cxx
PidData/PndPidProbability.cxx
PidData/PndPidBremCorrected4Mom.cxx

PndRecoCandidate.cxx
PndPersistencyTask.cxx
)

set(HEADERS
PndStack.h 
PndMCTrack.h

DskData/PndDskCerenkov.h
DskData/PndDskParticle.h
DskData/PndDskTrackPoint.h
DskData/PndDskFLGHit.h

DiscData/PndDiscDigitizedHit.h
DiscData/PndDiscParticleMCPoint.h
DiscData/PndDiscSensorMCPoint.h
DiscData/PndDiscReconResult.h
DiscData/PndDiscPID.h

DrcData/PndDrcEVPoint.h 
DrcData/PndDrcPDPoint.h 
DrcData/PndDrcBarPoint.h
DrcData/PndDrcHit.h
DrcData/PndDrcPDHit.h
DrcData/PndDrcDigi.h
DrcData/PndDrcLutNode.h
DrcData/PndDrcLutInfo.h
DrcData/PndDrcTrackInfo.h
DrcData/PndDrcPhotonInfo.h
DrcData/PndDrcAmbiguityInfo.h

GemData/PndGemMCPoint.h
GemData/PndGemDigi.h
GemData/PndGemCluster.h
GemData/PndGemHit.h

MdtData/PndMdtPoint.h
MdtData/PndMdtDigi.h
MdtData/PndMdtCluster.h
MdtData/PndMdtHit.h
MdtData/PndMdtTrk.h
MdtData/PndMdtID.h

SttData/PndSttGeomPoint.h
SttData/PndSttHelixHit.h
SttData/PndSttHit.h  
SttData/PndSttSkewedHit.h  
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
SdsData/PndSdsDigiTopix4.h
SdsData/PndSdsDigiTopix4Header.h
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

RichData/PndRichPhoton.h
RichData/PndRichPDPoint.h
RichData/PndRichBarPoint.h
RichData/PndRichDigi.h
RichData/PndRichHit.h
RichData/PndRichPDHit.h
RichData/PndRichMirrorSegment.h
 
MvdData/PndMvdApvHit.h
MvdData/PndMvdPidCand.h
MvdData/PndMvdRadDamHit.h

SciTData/PndSciTPoint.h
SciTData/PndSciTHit.h

TrackData/PndTrackCandHit.h
TrackData/PndTrackCand.h
TrackData/PndTrack.h
TrackData/PndSimpleTrack.h
TrackData/PndTrackID.h
#TrackData/PndVertex.h

PidData/PndPidCandidate.h
PidData/PndPidProbability.h
PidData/PndPidBremCorrected4Mom.h

PndRecoCandidate.h
PndPersistencyTask.h
)

set(LINKDEF  PndDataLinkDef.h)
set(LIBRARY_NAME PndData)

set(DEPENDENCIES Base GeoBase TrkBase)

GENERATE_LIBRARY()
