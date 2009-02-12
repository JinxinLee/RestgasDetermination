/** CbmStsMapsHit 
 *@author Michael Deveaux <m.deveaux@gsi.de>
 **Acknowledgements to M. Al-Turany, D. Bertini, G. Gaycken
 **Version beta 0.1 (02.02.2005)
 **/


#include "PndSttHit.h"

#include <iostream>
#include "TMath.h"
/** Default constructor **/
PndSttHit::PndSttHit() 
{
    Clear();
}


/** Standard constructor **/
PndSttHit::PndSttHit(Int_t detID, TVector3& pos, TVector3& dpos, 
		     Int_t index, Int_t flag, Double_t isochrone,
		     Double_t isochroneError, TVector3 wireDir) 
  : FairHit(detID, pos, dpos, index) 
{
  fIsochrone = isochrone;
  fIsochroneError = isochroneError;
  fRadial = TMath::Sqrt(pos.X() * pos.X() + pos.Y() * pos.Y());
  fWireDirection = wireDir;
  fAssigned = kFALSE;
  // stt1
  fXint = fX;
  fYint = fY;
  fZint = fZ;
}

PndSttHit::PndSttHit(Int_t detID, TVector3& pos, TVector3& dpos, Int_t index) 
  : FairHit(detID, pos, dpos, index) 
{
    Clear();
}

//PndSttHit::PndSttHit(Int_t trackID,Int_t eventID,Double_t p, Double_t rr, Double_t rt,TString nam,TVector3 center,TVector3 tubemax,TVector3 tubemin) {
PndSttHit::PndSttHit(Int_t detID, TVector3& pos, TVector3& dpos, Int_t index, Int_t trackID, Double_t p, Double_t rr, Double_t rt, Double_t isochroneError, TVector3 wireDir) 
  : FairHit(detID, pos, dpos, index){
  fPulse   = p;
  fRsim    = rr;
  fRtrue   = rt;
  //  fFlag    = 1;       // ??
  //  fnam     = nam;
  fTrackID = trackID;
  //  fEventID = eventID; // ??
  //  tube_c   = center;  // ??
  //  tube_max = tubemax; // ??
  //  tube_min = tubemin; // ??
  // stt2
  fIsochrone = rr;  // isochrone = simulated radius <----
  fIsochroneError = isochroneError;
  fRadial = sqrt(pos.X() * pos.X() + pos.Y() * pos.Y());
  fWireDirection = wireDir;
  fAssigned = kFALSE;
  fXint = fX;
  fYint = fY;
  fZint = fZ;
  
}

/** Public method Clear **/
void PndSttHit::Clear() 
{
  fIsochrone = 0.;
  fIsochroneError = 0.;
  fRadial = 0.;
  fWireDirection.SetXYZ(0., 0., 0.);
  fAssigned = kFALSE;
  fDepCharge = 0.;
  fELoss = 0.;
  fdEdx = 0.;
}  

 
/** Destructor **/
PndSttHit::~PndSttHit() 
{
} 



ClassImp(PndSttHit)
