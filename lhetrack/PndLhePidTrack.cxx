#include "PndLhePidTrack.h"

////////////////////////////////////////////////////////////////////////
//                                                                    //
// PndLhePidTrack  class - pid track  for the LHE                     //
//                                                                    //
////////////////////////////////////////////////////////////////////////

ClassImp(PndLhePidTrack)

//______________________________________________________________
PndLhePidTrack::PndLhePidTrack () {
  // Default constructor.  
  SetDefaults();
  fMvdELoss = 0;
  fMvdHitCount = 0;
  fSttELoss = 0;
  fSttHitCount = 0;
  fTofIndex   = -1;
  fTofDeltaPhi = -1000.;
  fTofDeltaZ = -1000.;
  fTofQuality = -1;
  fTofPathLength = 0.;
  fEmcIndex   = -1;
  fEmcDeltaPhi = -1000.;
  fEmcDeltaZ = -1000.;
  fEmcQuality = -1;
  fTimeOfFlight = 0.;
  fEmcELoss = -1.;
  fMdtIndex   = -1;
  fMdtDeltaPhi = -1000.;
  fMdtDeltaZ = -1000.;
  fMdtQuality = -1;
  fMdtModule = 0;
  fMdtLayerCount = 0;
  fMdtChi2 = -1.;
}

//______________________________________________________________
PndLhePidTrack::PndLhePidTrack (const PndTpcLheTrack& track) : PndTpcLheTrack(track){
  // Default constructor.
  fMvdELoss = 0;
  fMvdHitCount = 0;
  fSttELoss = 0;
  fSttHitCount = 0;
  fTofIndex   = -1;
  fTofDeltaPhi = -1000.;
  fTofDeltaZ = -1000.;
  fTofQuality = -1;
  fTofPathLength = 0.;
  fEmcIndex   = -1;
  fEmcDeltaPhi = -1000.;
  fEmcDeltaZ = -1000.;
  fEmcQuality = -1;
  fTimeOfFlight = 0.;
  fEmcELoss = -1.;
  fMdtIndex   = -1;
  fMdtDeltaPhi = -1000.;
  fMdtDeltaZ = -1000.;
  fMdtQuality = -1;
  fMdtModule = 0;
  fMdtLayerCount = 0;
  fMdtChi2 = -1.;
}

//______________________________________________________________
PndLhePidTrack::PndLhePidTrack (const PndLhePidTrack& track) : PndTpcLheTrack((PndTpcLheTrack)track)
{
  // Default constructor.
  fMvdELoss = track.fMvdELoss;
  fMvdHitCount = track.fMvdHitCount;
  fSttELoss = track.fSttELoss;
  fSttHitCount = track.fSttHitCount;
  fTofIndex   = track.fTofIndex;
  fTofDeltaPhi = track.fTofDeltaPhi;
  fTofDeltaZ = track.fTofDeltaZ;
  fTofQuality = track.fTofQuality;
  fTofPathLength = track.fTofPathLength;
  fEmcIndex      = track.fEmcIndex;
  fEmcDeltaPhi   = track.fEmcDeltaPhi;
  fEmcDeltaZ     = track.fEmcDeltaZ;
  fEmcQuality    = track.fEmcQuality;
  fTimeOfFlight  = track.fTimeOfFlight;
  fEmcELoss      = track.fEmcELoss;
  fMdtIndex      = track.fMdtIndex;
  fMdtDeltaPhi   = track.fMdtDeltaPhi;
  fMdtDeltaZ     = track.fMdtDeltaZ;
  fMdtQuality    = track.fMdtQuality;
  fMdtModule     = track.fMdtModule;
  fMdtLayerCount = track.fMdtLayerCount;
  fMdtChi2       = track.fMdtChi2;
}
//______________________________________________________________
PndLhePidTrack ::~PndLhePidTrack () {
  // Destructor.

  //  cout << " Destructor for LhePidTrack" << endl;
  //if (fRealHits) { //fRealHits->Delete();} //;
  delete fRealHits;
  fRealHits = NULL;
}




