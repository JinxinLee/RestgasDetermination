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
PndMCTrackInfo.cxx

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

PndPersistencyTask.cxx
)


set(LINKDEF  PndDataLinkDef.h)
set(LIBRARY_NAME PndData)

set(DEPENDENCIES Base GeoBase TrkBase)

GENERATE_LIBRARY()
