// -------------------------------------------------------------------------
// -----                      PndSttTrack source file                  -----
// -----                  Created 28/03/06  by R. Castelijns           -----
// -------------------------------------------------------------------------

#include "PndSttTrack.h"

#include <iostream>

using std::cout;
using std::endl;
using std::map;

// -----   Default constructor   -------------------------------------------
PndSttTrack::PndSttTrack() 
{
    fPidHypo  = 0;
    fFlag     = 0;
    fChi2Long = 0.;
    fChi2Rad  = 0.;
    fDist     = 0.;
    fPhi      = 0.;
    fRad      = 0.;
    fTanL     = 0.;
    fZ        = 0.;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndSttTrack::~PndSttTrack() 
{}
// -------------------------------------------------------------------------


// -----   Public method Print   -------------------------------------------
void PndSttTrack::Print() 
{
  cout << " Reco track corresponding to track cand: " 
       << fTrackCandIndex  << endl;
  cout << " Parameters of the helix: " << endl;
  cout << fDist << " " << fPhi << " " << fRad << " " << fTanL << " " << fZ << endl;
  //  cout << "Chi2Long: " << fChi2Long << ", Chi2Rad: " << fChi2Rad << endl; 
  cout << "Quality flag " << fFlag << endl; // CHECK add number of helix hits
}
// -------------------------------------------------------------------------

void PndSttTrack::AddHelixHit(Int_t size, Int_t index, Int_t helixhitindex) 
{
  //  fHelixHits.Reset();
    fHelixHits.Set(size);
    fHelixHits[index] = helixhitindex;
}

ClassImp(PndSttTrack)
