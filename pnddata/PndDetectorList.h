// -------------------------------------------------------------------------
// -----                  PndDetectorList.header file                  -----
// -----                 Created 11/02/09  by M. Al-Turany                -----
// -------------------------------------------------------------------------


/** Unique identifier for all Panda detector systems **/

#ifndef PNDDETECTORLIST_H
#define PNDDETECTORLIST_H 1

enum DetectorId {
    kDCH,kDRC,kDSK,kEMC,kGEM,kLUMI,kMDT,kMVD,kRPC,kSTT,kTPC,kTOF,kHYPG,kHYP};
    
/** Unique identifier for all Panda Point and Hit types **/

enum fDetectorType {  
   kUnknown, kMCTrack,
   kTpcPoint, kTpcCluster,
   kMVDPoint, kMVDDigiStrip, kMVDDigiPixel, kMVDClusterPixel, kMVDClusterStrip, kMVDHitsStrip, kMVDHitsPixel,
   kEmcPoint, kEmcHit, kEmcDigi, kEmcCluster, kEmcBump, kEmcRecoHit,
   kSttPoint, kSttHit, kSttHelixHit, kSttTrackCand, kSttTrack,
   kGemPoint, kGemDigi, kGemHit,
   kDchPoint, kDchDigi, kDchHit,
   kMdtPoint, kMdtHit,
   kLheHit, kLheCand, kLheTrack,
   kTrackCand, kTrack,
   kPidChargedCandidate, kPidNeutralCandidate
};



enum SensorSide { kTOP, kBOTTOM };

#endif
