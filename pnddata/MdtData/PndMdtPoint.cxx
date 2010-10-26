
#include <iostream>

using std::cout;
using std::endl;

#include "PndMdtPoint.h"
#include "PndDetectorList.h"

// -----   Default constructor   -------------------------------------------
PndMdtPoint::PndMdtPoint() : FairMCPoint()
{
  fTrackParentID = -999;
  fPDG           = -999;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndMdtPoint::PndMdtPoint( Int_t trackID, Int_t detID, TVector3 pos, TVector3 mom,
	     Double_t tof, Double_t length, Double_t eLoss, Int_t
			  MotherTrId, Int_t pdg, TVector3 pos_in, TVector3 mom_in) :  FairMCPoint(trackID, detID, pos, mom,tof, length, eLoss )
{
  fTrackParentID = MotherTrId;
  fPDG           = pdg;
  fPosIn         = pos_in;
  fMomIn         = mom_in;
  //SetLink(FairLink("MCTrack", trackID));
}


// -----   Destructor   ----------------------------------------------------
PndMdtPoint::~PndMdtPoint() { }
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void PndMdtPoint::Print(const Option_t* opt) const {
/*  cout << "-I- PndMdtPoint: MDT Point for track " << fTrackID << " in the detector " << fDetectorID << endl;
  cout << " in event " << fEventID+1 << " with parent track " << fTrackParentID 
       << " and its particle PDG " << fPDG << endl;
  cout << "    Position (" << fPos.X() << ", " << fPos.Y() << ", " << fPos.Z() << ", " << fPos.T()
       << ") cm^3*s" << endl;
  cout << "    Momentum (" << fMom.X() << ", " << fMom.Y() << ", " << fMom.Z() << ", " << fMom.T()
       << ") GeV^4" << endl;
*/
}
// -------------------------------------------------------------------------



ClassImp(PndMdtPoint)
