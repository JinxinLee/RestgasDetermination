// -----                      PndSttHelixHit source file               -----
// -----                                                               -----
// -------------------------------------------------------------------------

#include "PndSttHelixHit.h"

#include <iostream>
#include "TMath.h"
#include "TVector3.h"
#include "PndSttHit.h"

using namespace std;

/** Default constructor **/
PndSttHelixHit::PndSttHelixHit() 
{
    Clear();
}

/** Standard constructor **/
// DONT USE THIS! delete it!
PndSttHelixHit::PndSttHelixHit(Int_t detID, 
			       TVector3& pos, TVector3& dpos,
			       Int_t mcindex, Int_t hitindex, Double_t edep)
  : FairHit(detID, pos, dpos, mcindex) 
{
  fHitIndex = hitindex;
  fELoss = edep;
}

// DONT USE THIS! add halflength to make it OK
PndSttHelixHit::PndSttHelixHit(Int_t detID, 
			       TVector3& pos, TVector3& dpos, 
			       Int_t mcindex, Int_t hitindex, Double_t edep,
			       Double_t isochrone,
			       Double_t isochroneError)		        
 : FairHit(detID, pos, dpos, mcindex) 
{

  fHitIndex = hitindex;
  fELoss = edep;
  fIsochrone = isochrone;
  fIsochroneError = isochroneError;
}


// this copies the PndSttHit to the PndSttHelixHit
void PndSttHelixHit::CopyHitToHelixHit(PndSttHit *aHit, Int_t hitindex)
{
//   FairHit::SetPosition(TVector3(0,0,0));
//   FairHit::SetPositionError(TVector3(0,0,0));
  FairHit::SetDetectorID(aHit->GetDetectorID());
  FairHit::SetRefIndex(aHit->GetRefIndex());

  fHitIndex = hitindex;
  fELoss = aHit->GetEnergyLoss();
  fDepCharge = aHit->GetDepCharge();
  //  fdEdx = aHit->GetdEdx();
  

  fIsochrone      = aHit->GetIsochrone();
  fIsochroneError = aHit->GetIsochroneError();

  fTubeID         = aHit->GetTubeID(); // CHECK added
}



/** Public method Clear **/
void PndSttHelixHit::Clear() 
{
  fHitIndex = 0;
  fDepCharge = 0.; 
  fdEdx = 0.;      
  fELoss = 0.;   

  fIsochrone = 0.; 
  fIsochroneError = 0.;
 
}  

 
/** Destructor **/
PndSttHelixHit::~PndSttHelixHit() 
{
} 

void PndSttHelixHit::Print()
{
  // reco & center positions
  cout << "position " << GetX() << " " << GetY() << " " << GetZ() << endl;
 
  // index
  cout << "mc, hit indexes " << GetRefIndex() << " " << GetHitIndex() << endl;

  // drift
  cout << "drift radius " << GetIsochrone() << endl;

}



ClassImp(PndSttHelixHit)
