// ----------------------------------------------------------------------------
// -----                  PndDskCerenkov source file                      -----
// -----                 Created 21/03/08  by P. Koch                     -----
// ----------------------------------------------------------------------------

#include "TMath.h"
#include <iostream>
using std::cout;
using std::endl;

#include "PndDskCerenkov.h"



// -----   Default constructor   ----------------------------------------------
PndDskCerenkov::PndDskCerenkov()
  : FairMCPoint(),
    fEnergy(0.),
    fWavelength(0.), 
    fPWay(0.),
    fLastPos(TVector3()),
    fMotherTrackID(-1),
    fMotherPdgCode(-1),
    fMotherPdgName(TString()),
    fDetNumber(-1),
    fDetType(-1),
    fDetTime(0.),
    fDetMomentum(TVector3()),
    fNofReflections(0), 
    fPrimaryHitAngle(0.),
    fPrimaryAngleToCerenkov(0.),
    f5RefPosition(TVector3()),
    fTotalRefAngle(0.)
{
}
// ----------------------------------------------------------------------------



// -----   Standard constructor   ---------------------------------------------
PndDskCerenkov::PndDskCerenkov(Int_t trackID, Int_t detectorID,
        TVector3 position, TVector3 momentum, Double_t time,
        Double_t energy, Double_t wavelength,
        Int_t motherTrackID, Int_t motherPdgCode, TString motherPdgName)
  : FairMCPoint(trackID, detectorID, position, momentum, time, 0., 0.),
    fEnergy(energy),
    fWavelength(wavelength),   
    fPWay(0.), 
    fLastPos(position),
    fMotherTrackID(motherTrackID),
    fMotherPdgCode(motherPdgCode),
    fMotherPdgName(motherPdgName),
    fDetNumber(-1),
    fDetType(-1),
    fDetTime(0.),
    fDetMomentum(TVector3()),
    fNofReflections(0), 
    fPrimaryHitAngle(0.),
    fPrimaryAngleToCerenkov(0.),
    f5RefPosition(TVector3()),
    fTotalRefAngle(0.)
{
//   cout << "Added Cerenkov with wl = " << fWavelength << endl;
}
// ----------------------------------------------------------------------------

PndDskCerenkov::PndDskCerenkov(const PndDskCerenkov& cerenkov) :
  FairMCPoint(cerenkov.fTrackID, cerenkov.fDetectorID,  TVector3(cerenkov.fX, cerenkov.fY, cerenkov.fZ), TVector3(cerenkov.fPx, cerenkov.fPy, cerenkov.fPz), cerenkov.fTime, cerenkov.fLength, cerenkov.fELoss, cerenkov.fEventId),
  fEnergy(cerenkov.fEnergy),
  fWavelength(cerenkov.fWavelength), 
  fPWay(cerenkov.fPWay), 
  fLastPos(cerenkov.fLastPos),
  fMotherTrackID(cerenkov.fMotherTrackID),
  fMotherPdgCode(cerenkov.fMotherPdgCode),
  fMotherPdgName(cerenkov.fMotherPdgName),
  fDetNumber(cerenkov.fDetNumber),
  fDetType(cerenkov.fDetType),
  fDetTime(cerenkov.fDetTime),
  fDetMomentum(cerenkov.fDetMomentum),
  fNofReflections(cerenkov.fNofReflections),
  fPrimaryHitAngle(cerenkov.fPrimaryHitAngle),
  fPrimaryAngleToCerenkov(cerenkov.fPrimaryAngleToCerenkov),
  f5RefPosition(cerenkov.f5RefPosition),
  fTotalRefAngle(cerenkov.fTotalRefAngle)
{
}

// -----   Destructor   -------------------------------------------------------
PndDskCerenkov::~PndDskCerenkov()
{
}
// ----------------------------------------------------------------------------



// -----   Public method Print   ----------------------------------------------
void
PndDskCerenkov::Print(const Option_t* opt) const {
  cout << "-I- PndDskCerenkov: track" << fTrackID 
       << " created in detector " << fDetectorID << endl;
  cout << "    at Position (" << fX << ", " << fY << ", " << fZ
       << ") cm" << endl;
  cout << "    with Momentum (" << fPx << ", " << fPy << ", " << fPz
       << ") eV" << endl;
  cout << "    at Time " << fTime << " ns" << endl;
}
// ----------------------------------------------------------------------------



// -----   Public method SetFinalValues   -------------------------------------
void
PndDskCerenkov::SetFinalValues(Int_t detNumber, Short_t detType, Double_t detTime,
        TVector3 detMomentum, Double_t length,
        Double_t primaryHitAngle, Double_t primaryAngleToCerenkov)
{
  fDetNumber   = detNumber;
  fDetType     = detType;
  fDetTime     = detTime;
  fDetMomentum = detMomentum;
  fLength      = length;
  fPrimaryHitAngle        = primaryHitAngle;
  fPrimaryAngleToCerenkov = primaryAngleToCerenkov;
}
// ----------------------------------------------------------------------------



// -----   Public method AddPWay   -------------------------------------
void
PndDskCerenkov::AddPWay(TVector3 pos)
{
// printf("..... %11.6lf %11.6lf %11.6lf - %11.6lf %11.6lf %11.6lf    -> %11.6lf\n",
//   fLastPos.X(), fLastPos.Y(), fLastPos.Z(),
//   pos.X(), pos.Y(), pos.Z(),
//   TMath::Sqrt( (fLastPos.X()-pos.X())*(fLastPos.X()-pos.X())
//              + (fLastPos.Y()-pos.Y())*(fLastPos.Y()-pos.Y()) )
// );
  fPWay += TMath::Sqrt( (fLastPos.X()-pos.X())*(fLastPos.X()-pos.X())
                      + (fLastPos.Y()-pos.Y())*(fLastPos.Y()-pos.Y()) );
  fLastPos = pos;
}



ClassImp(PndDskCerenkov)
