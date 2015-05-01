// ----------------------------------------------------------------------------
// -----                   PndDskFLGHit source file                     -----
// -----                  Created 21/03/08  by P. Koch                    -----
// ----------------------------------------------------------------------------

#include <iostream>
using std::cout;
using std::endl;

#include "PndDskFLGHit.h"



// -----   Default constructor   ----------------------------------------------
PndDskFLGHit::PndDskFLGHit()
  : FairMCPoint(),
    fPdgCode(-1),
    fPdgName(TString()),
    fEnergy(0.),
    fMass(0.),
    fMotherTrackID(-1),
    fMotherPdgCode(-1),
    fMotherPdgName(TString()),
    fExitPosition(TVector3()),
    fExitMomentum(TVector3()),
    fExitTime(0.),
    fExitEnergy(0.),
    fThetaC(0.),
    fAngIn(0.),
    fMom_cherenkov_photon(TVector3()),
    fNum_light_guide(-1),
    fNum_pixel(-1) 
{
}    
// ----------------------------------------------------------------------------



// -----   Standard constructor   ---------------------------------------------
PndDskFLGHit::PndDskFLGHit(Int_t trackID, Int_t detectorID,
        TVector3 position, TVector3 momentum, Double_t time,
        Int_t pdgCode, TString pdgName, Double_t energy,
        Int_t motherTrackID, Int_t motherPdgCode, TString motherPdgName, Double_t mass,
	Double_t angIn, Double_t thetaC, TVector3 mom_photon, Int_t num_light_guide, Int_t num_pixel)
  : FairMCPoint(trackID, detectorID, position, momentum, time, 0., 0.),
    fPdgCode(pdgCode),
    fPdgName(pdgName),
    fEnergy(energy),
    fMass(mass),
    fMotherTrackID(motherTrackID),
    fMotherPdgCode(motherPdgCode),
    fMotherPdgName(motherPdgName),
    fExitPosition(TVector3()),
    fExitMomentum(TVector3()),
    fExitTime(0.),
    fExitEnergy(0.),
    fThetaC(thetaC),
    fAngIn(angIn),
    fMom_cherenkov_photon(mom_photon),
    fNum_light_guide(num_light_guide),
    fNum_pixel(num_pixel)
{
}
// ----------------------------------------------------------------------------
PndDskFLGHit::PndDskFLGHit(Int_t trackID, Int_t detectorID,
	TVector3 position, TVector3 momentum, Double_t time,
        Double_t angIn, Double_t thetaC, TVector3 mom_photon, Int_t num_light_guide, Int_t num_pixel)
  : FairMCPoint(trackID, detectorID, position, momentum, time, 0., 0.),
     fPdgCode(-1),
    fPdgName(TString()),
    fEnergy(0.),
    fMass(0.),
    fMotherTrackID(-1),
    fMotherPdgCode(-1),
    fMotherPdgName(TString()),
    fExitPosition(TVector3()),
    fExitMomentum(TVector3()),
    fExitTime(0.),
    fExitEnergy(0.),
    fThetaC(thetaC),
    fAngIn(angIn),
    fMom_cherenkov_photon(mom_photon),
    fNum_light_guide(num_light_guide),
    fNum_pixel(num_pixel)
{
}


// -----   Destructor   -------------------------------------------------------
PndDskFLGHit::~PndDskFLGHit()
{
}
// ----------------------------------------------------------------------------



// -----   Public method Print   ----------------------------------------------
void
PndDskFLGHit::Print(const Option_t* opt) const {
  (void)opt; // to remove "unused" warnings
  cout << "-I- PndDskFLGHit: track" << fTrackID 
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
PndDskFLGHit::SetFinalValues(TVector3 exitPosition, TVector3 exitMomentum,
        Double_t exitTime, Double_t exitEnergy)
{
  fExitPosition = exitPosition;
  fExitMomentum = exitMomentum;
  fExitTime     = exitTime;
  fExitEnergy   = exitEnergy;
}
// ----------------------------------------------------------------------------



ClassImp(PndDskFLGHit)
