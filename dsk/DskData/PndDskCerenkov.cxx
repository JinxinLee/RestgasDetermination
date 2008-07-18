////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  PndDskCerenkov
//
//  MC data storage class for a Cerenkov photon
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "TMath.h"
#include "PndDskCerenkov.h"
#include <iostream>
using std::cout;
using std::endl;

ClassImp(PndDskCerenkov);


// Default constructor
// -------------------------------------------------------------------------------------------------
PndDskCerenkov::PndDskCerenkov() : CbmMCPoint()
{
}


// Standard constructor
// -------------------------------------------------------------------------------------------------
PndDskCerenkov::PndDskCerenkov(Int_t trackID, Int_t volID, TVector3 pos, TVector3 mom,
              Double_t tof, Double_t length, Double_t eLoss,
              Double_t energy, Int_t motherPdgCode,
              TVector3 primaryHitMomentum)
  : CbmMCPoint(trackID, volID, pos, mom, tof, length, eLoss)
{
  fCurPos = pos;
  fEnergy = energy;
  fMotherPdgCode = motherPdgCode;
  fPrimaryHitMomentum = primaryHitMomentum;

  // when we start, the projected way is zero
  fPWay = 0.;
  fDetNumber = 0;
  fDetType = 10;
}


// Standard destructor
// -------------------------------------------------------------------------------------------------
PndDskCerenkov::~PndDskCerenkov()
{
}


// Calculate projected way
// -------------------------------------------------------------------------------------------------
void
PndDskCerenkov::CalcPWay(TVector3 newPos)
{
  fPWay += TMath::Sqrt( TMath::Power(newPos.X()-fCurPos.X(),2)
                       +TMath::Power(newPos.Y()-fCurPos.Y(),2) );
  fCurPos = newPos;
}


// Set Values when Cerenkov gets detected
// -------------------------------------------------------------------------------------------------
void
PndDskCerenkov::SetFinalValues(Int_t detNumber, UShort_t detType, Double_t tof, Double_t length)
{
  fDetNumber = detNumber;
  fDetType = detType;
  fTime = tof;
  fLength = length;
}
