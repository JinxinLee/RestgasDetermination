/////////////////////////////////////////////////////////////
//
//  CbmFtofPoint
//
//  Geant point for Forward tof  detector
//  created by A. Sanchez
//
///////////////////////////////////////////////////////////////

#include "PndFtofPoint.h"

#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndFtofPoint::PndFtofPoint() : FairMCPoint(),
			       fcharge(0), fmass(0),  fpdgCode(0), fdist(0), fEventID(-1), fDetName(),
			       fXin(0), fYin(0), fZin(0), 
			       fPxin(0), fPyin(0), fPzin(0),
			       fPLin(0), fPLout(0), 
			       fXout(0), fYout(0), fZout(0),
			       fPxout(0), fPyout(0), fPzout(0)
{
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndFtofPoint::PndFtofPoint(Int_t trackID, Int_t evtID,
			 Int_t detID, TString detName,
			 TVector3 posin,
			 TVector3 momin, 
			 TVector3 posout, 
			 TVector3 momout, 
			 Double_t tof, Double_t length,
			 Double_t eLoss,Double_t charge,
                         Double_t mass, 
			 Int_t pdgCode,Double_t dist, 
			 Double_t PLin, Double_t PLout)
  : FairMCPoint(trackID, detID, posin, momin, tof, length, eLoss), 
    fcharge(charge), fmass(mass),  fpdgCode(pdgCode), fdist(dist), fEventID(evtID), fDetName(detName),
    fXin(posin.X()), fYin(posin.Y()), fZin(posin.Z()), 
    fPxin(momin.X()), fPyin(momin.Py()), fPzin(momin.Pz()),
    fPLin(PLin), fPLout(PLout), 
    fXout(posout.X()), fYout(posout.Y()), fZout(posout.Z()),
    fPxout(momout.Px()), fPyout(momout.Py()), fPzout(momout.Pz())
{
}


// -----   Destructor   ----------------------------------------------------
PndFtofPoint::~PndFtofPoint() { }
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void PndFtofPoint::Print(const Option_t* opt) const {
  cout << "-I- PndFtofPoint: forward tof Point for track " 
       <<" in detector " << endl;
  cout << "    Position (" << fX << ", " << fY << ", " << fZ
       << ") cm" << endl;
  cout << "    Momentum (" << fPx << ", " << fPy << ", " << fPz
       << ") GeV" << endl;
 
}
// -------------------------------------------------------------------------



ClassImp(PndFtofPoint)
