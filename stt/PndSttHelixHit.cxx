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
  : CbmHit(detID, pos, dpos, mcindex) 
{
  fHitIndex = hitindex;
  fEdep = edep;
}

// DONT USE THIS! add hl to make it OK
PndSttHelixHit::PndSttHelixHit(Int_t detID, 
			       TVector3& pos, TVector3& dpos, 
			       Int_t mcindex, Int_t hitindex, Double_t edep,
			       TVector3& cpos, TVector3& dcpos, 
			       Double_t isochrone,
			       Double_t isochroneError, TVector3 wireDir)			        
 : CbmHit(detID, pos, dpos, mcindex) 
{

  fHitIndex = hitindex;
  fEdep = edep;

  fXcen = cpos.X();  fYcen = cpos.Y();  fZcen = cpos.Z(); 
  fDxcen = dcpos.X();   fDycen = dcpos.Y();   fDzcen = dcpos.Z(); 
  fIsochrone = isochrone;
  fIsochroneError = isochroneError;
  fWireDirection = wireDir;
}


// this copies the PndSttHit to the PndSttHelixHit
void PndSttHelixHit::CopyHitToHelixHit(PndSttHit *aHit, Int_t hitindex)
{
//   CbmHit::SetPosition(TVector3(0,0,0));
//   CbmHit::SetPositionError(TVector3(0,0,0));
  CbmHit::SetDetectorID(aHit->GetDetectorID());
  CbmHit::SetRefIndex(aHit->GetRefIndex());

  fHitIndex = hitindex;
  // fEdep = aHit->GetEdep(); // CHECK uncomment!
  TVector3 centerPosition, centerPositionError;
  aHit->Position(centerPosition);
  aHit->PositionError(centerPositionError);

  fXcen = centerPosition.X(); fYcen = centerPosition.Y(); fZcen = centerPosition.Z(); 
  fDxcen = centerPositionError.X(); fDycen = centerPositionError.Y(); fDzcen = centerPositionError.Z(); 


  fIsochrone = aHit->GetIsochrone();
  fIsochroneError = aHit->GetIsochroneError();
  fWireDirection = aHit->GetWireDirection();
  fhl = 75.; // aHit->GetHalfLength(); // CHECK to be added
}



/** Public method Clear **/
void PndSttHelixHit::Clear() 
{
  fEdep = 0.;
  fHitIndex = 0;
}  

 
/** Destructor **/
PndSttHelixHit::~PndSttHelixHit() 
{
} 

void PndSttHelixHit::Print()
{
  // reco & center positions
  cout << "position " << GetX() << " " << GetY() << " " << GetZ() << endl;
  cout << "center position " << GetXcen()  << " " << GetYcen() << " " << GetZcen() << endl;

  // index
  cout << "mc, hit indexes " << GetRefIndex() << " " << GetHitIndex() << endl;

  // drift
  cout << "drift radius " << GetIsochrone() << endl;

  // wire
  cout << "wire dir " << GetWireDirection().X() << " " << GetWireDirection().Y() << " " << GetWireDirection().Z() << endl;
  cout << "wire half length " << GetHalfLength() << endl;
}



ClassImp(PndSttHelixHit)
