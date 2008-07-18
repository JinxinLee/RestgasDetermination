// -------------------------------------------------------------------------
// -----                    PndDchTrackMatch source file               -----
// -------------------------------------------------------------------------

// Pnd includes
#include "PndDchTrackMatch.h"



// -----   Default constructor   -------------------------------------------
PndDchTrackMatch::PndDchTrackMatch() {
  fMCTrackID     = -1;
  fRecTrackID    = -1;
  fNofTrueHorDs  =  0;
  fNofWrongHorDs =  0;
  fNofFakeHorDs  =  0;
  fNofMCTracks   =  0;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndDchTrackMatch::PndDchTrackMatch(Int_t mcTrackID, Int_t recTrackID, Int_t nTrue,
				   Int_t nWrong, Int_t nFake, 
				   Int_t nTracks) {
  fMCTrackID     = mcTrackID;
  fRecTrackID    = recTrackID;
  fNofTrueHorDs  = nTrue;
  fNofWrongHorDs = nWrong;
  fNofFakeHorDs  = nFake;
  fNofMCTracks   = nTracks;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndDchTrackMatch::~PndDchTrackMatch() {}
// -------------------------------------------------------------------------


ClassImp(PndDchTrackMatch)
