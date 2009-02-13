// -------------------------------------------------------------------------
// -----                  PndDetectorList.header file                  -----
// -----                 Created 11/02/09  by M. Al-Turany                -----
// -------------------------------------------------------------------------


/** Unique identifier for all Panda detector systems **/

#ifndef PNDDETECTORLIST_H
#define PNDDETECTORLIST_H 1

enum DetectorId {
    kDCH,kDRC,kDSK,kEMC,kGEM,kMDT,kMVD,kRPC,kSTT,kTPC,kTOF,kHYP};
    
/** Unique identifier for all Panda Point and Hit types **/

enum fDetectorType {  
   kUnknown,kTpcPoint,kTpcCluster,kMVDPoint,kMVDHitsStrip,kMVDHitsPixel,kEmcCluster,kEmcBump,kSttPoint,kSttHit,kSttHelixHit};


enum SensorSide { kTOP, kBOTTOM };

#endif
