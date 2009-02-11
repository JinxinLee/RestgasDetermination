// -------------------------------------------------------------------------
// -----                  PndDetectorList.header file                  -----
// -----                 Created 11/02/09  by M. Al-Turany                -----
// -------------------------------------------------------------------------


/** Unique identifier for all Panda detector systems **/

#ifndef PNDDETECTORLIST_H
#define PNDDETECTORLIST_H 1

enum DetectorId {
    kDrc,kDsk,kEmc,kGem,kMdt,kMVD,kRpc,kStt,kTpc,kTof};
    
/** Unique identifier for all Panda Point and Hit types **/

enum fDetectorType {  
   kUnknown,kTpcPoint,kTpcCluster,kMVDPoint,kMVDHitsStrip,kMVDHitsPixel,kEmcCluster,kEmcBump,kSttPoint,kSttHit,kSttHelixHit};

#endif
