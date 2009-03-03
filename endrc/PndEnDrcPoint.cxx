////////////////////////////////////////////////////////////////////////////////
//
//  PndEnDrcPoint
//
//  MonteCarlo data storage class
//
//  created 2008 D. Glazier, K. Foehl Edinburgh
//
////////////////////////////////////////////////////////////////////////////////

#include "PndEnDrcPoint.h"
#include <iostream>
using std::cout;
using std::endl;

ClassImp(PndEnDrcPoint)

// -----------------------------------------------------------------------------
PndEnDrcPoint::PndEnDrcPoint() : FairMCPoint()
{
}

// -----------------------------------------------------------------------------
PndEnDrcPoint::PndEnDrcPoint(Int_t trackID, Int_t detID, Int_t pdgCode, TVector3 pos, TVector3 mom, Double_t tof, Double_t length, Double_t eLoss, Double_t PhotE, Double_t FPz, Double_t Angle): FairMCPoint(trackID, detID, pos, mom, tof, length, eLoss)
{
  fPdgCode    = pdgCode;
  fPhotE  = PhotE;
  fFPz    = FPz;
  fAngle  = Angle;
}

// -----------------------------------------------------------------------------
PndEnDrcPoint::~PndEnDrcPoint()
{
}

// -----------------------------------------------------------------------------
void PndEnDrcPoint::Print(const Option_t *opt) const
{
  cout << "-I- PndEnDrcPoint: Information of the point" << endl;
//   cout << "-I- PndEnDrcPoint: Point for track " << fTrackID 
//        << " in detector " << fDetectorID << endl;
//   cout << "    Position (" << fX << ", " << fY << ", " << fZ
//        << ") cm" << endl;
//   cout << "    Momentum (" << fPx << ", " << fPy << ", " << fPz
//        << ") GeV" << endl;
//   cout << "    Time " << fTime << " ns,  Length " << fLength 
//        << " cm,  Energy loss " << fELoss*1.0e06 << " keV" << endl;
}
