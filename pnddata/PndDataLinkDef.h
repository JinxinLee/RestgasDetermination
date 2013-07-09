// $Id: ZdcLinkDef+;,v 1.1.1.1
//2005/06/23 07:14:26 dbertini Exp $

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class PndStack+;
#pragma link C++ class PndMCTrack+;

#pragma link C++ class PndDskCerenkov+;
#pragma link C++ class PndDskParticle+;
#pragma link C++ class PndDskTrackPoint+;
#pragma link C++ class PndDskFLGHit+;

#pragma link C++ class PndDrcEVPoint+;
#pragma link C++ class PndDrcPDPoint+;
#pragma link C++ class PndDrcBarPoint+;
#pragma link C++ class PndDrcHit+;
#pragma link C++ class PndDrcPDHit+;
#pragma link C++ class PndDrcDigi+;

//#pragma link C++ class PndDrpcPoint+;
//#pragma link C++ class PndDrpcPointLite+;

#pragma link C++ class PndGemMCPoint+;
#pragma link C++ class PndGemDigi+;
#pragma link C++ class PndGemHit+;

#pragma link C++ class  PndMdtTrk+;
#pragma link C++ class  PndMdtPoint+;
#pragma link C++ class  PndMdtDigi+;
#pragma link C++ class  PndMdtCluster+;
#pragma link C++ class  PndMdtHit+;

#pragma link C++ class  PndSttGeomPoint+;
#pragma link C++ class  PndSttHelixHit+;
#pragma link C++ class  PndSttHit+;
#pragma link C++ class  PndSttHitInfo+;
#pragma link C++ class  PndSttPoint+;
#pragma link C++ class  PndSttTrack+;

//#pragma link C++ class  PndTofHit+;
//#pragma link C++ class  PndTofPoint+;

//#pragma link C++ class  PndRpcPoint+;
//#pragma link C++ class  PndRpcHit+;

#pragma link C++ class  PndFtsPoint+;
#pragma link C++ class  PndFtsHit+;

#pragma link C++ class  PndHypCluster+;
#pragma link C++ class  PndHypDigi+;
#pragma link C++ class  PndHypDigiStrip+;
#pragma link C++ class  PndHypHit+;
#pragma link C++ class  PndHypHitInfo+;
#pragma link C++ class  PndHypPidCand+;
#pragma link C++ class  PndHypPoint+;
#pragma link C++ class  PndHypStrip+;

#pragma link C++ class  PndRichPoint+;

//#pragma link C++ class  PndSdsApvHit+;
#pragma link C++ class  PndSdsMCPoint+;
#pragma link C++ class  PndSdsDigi+;
#pragma link C++ class  PndSdsDigiPixel+;
#pragma link C++ class  PndSdsDigiStrip+;
#pragma link C++ class  PndSdsCluster+;
#pragma link C++ class  PndSdsClusterPixel+;
#pragma link C++ class  PndSdsClusterStrip+;
#pragma link C++ class  PndSdsHit+;
//#pragma link C++ class  PndSdsHitInfo+;
#pragma link C++ class  PndSdsPidCand+;
#pragma link C++ class  PndSdsPixel+;
#pragma link C++ class  PndSdsRadDamHit+;
#pragma link C++ class  PndSdsStrip+;
#pragma link C++ class  PndSdsDigiPixelMCInfo+;

#pragma link C++ class  PndMvdApvHit+;
#pragma link C++ class  PndMvdPidCand+;
//#pragma link C++ class  PndMvdPixel+;
#pragma link C++ class  PndMvdRadDamHit+;
//#pragma link C++ class  PndMvdStrip+;

#pragma link C++ class  PndSciTPoint+;
#pragma link C++ class  PndSciTHit+;

#pragma link C++ class PndTrackCand+;
#pragma link C++ class PndTrack+;
#pragma link C++ class PndTrackID+;
#pragma link C++ class PndTrackCandHit+;
//#pragma link C++ class PndVertex+;

#pragma link C++ class PndPidCandidate+;
#pragma link C++ class PndPidProbability+;

#pragma link C++ class FairRecoCandidate;
#pragma link C++ function operator << ( ostream &, const FairRecoCandidate & );

#endif

