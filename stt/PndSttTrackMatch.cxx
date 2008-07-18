// -------------------------------------------------------------------------
// -----                    PndSttTrackMatch source file               -----
// -----                  Created 30/03/06  by R. Castelijns           -----
// -------------------------------------------------------------------------


#include "PndSttTrackMatch.h"



// -----   Default constructor   -------------------------------------------
PndSttTrackMatch::PndSttTrackMatch() {
  fMCTrackID    = -1;
  fNofTrueHits  =  0;
  fNofWrongHits =  0;
  fNofFakeHits  =  0;
  fNofMCTracks  =  0;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndSttTrackMatch::PndSttTrackMatch(Int_t mcTrackID, Int_t nTrue,
				   Int_t nWrong, Int_t nFake, 
				   Int_t nTracks) {
  fMCTrackID    = mcTrackID;
  fNofTrueHits  = nTrue;
  fNofWrongHits = nWrong;
  fNofFakeHits  = nFake;
  fNofMCTracks  = nTracks;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndSttTrackMatch::~PndSttTrackMatch() {}
// -------------------------------------------------------------------------


ClassImp(PndSttTrackMatch)
