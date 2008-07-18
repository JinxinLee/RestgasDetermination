#include "PndMuoDigi.h"

// -----   Default constructor   -------------------------------------------
PndMuoDigi::PndMuoDigi() {fFlag = -1; }


PndMuoDigi::PndMuoDigi(Int_t trackID,Int_t eventID, Short_t sec, Short_t pla, Short_t strip, Short_t box, Short_t tub) {
  fTrackID =trackID;
  fEventID =eventID;
  nSector = sec;
  nPlane = pla;
  nStrip = strip;
  nBox = box;
  nTube = tub;
}



/** Public method Clear **/
void PndMuoDigi::Clear() {
  fFlag = -1;
}  

 
/** Destructor **/
PndMuoDigi::~PndMuoDigi() {} 

ClassImp(PndMuoDigi)
