// ----------------------------------------------------------------------------
// -----                   PndDskParticle source file                     -----
// -----                  Created 21/03/08  by P. Koch                    -----
// ----------------------------------------------------------------------------

#include <iostream>
using std::cout;
using std::endl;

#include "PndDskParticle.h"



// -----   Default constructor   ----------------------------------------------
PndDskParticle::PndDskParticle()
  : FairMCPoint(),
    fPdgCode(-1),
    fPdgName(TString()),
    fEnergy(0.),
    fMotherTrackID(-1),
    fMotherPdgCode(-1),
    fMotherPdgName(TString()),
    fExitPosition(TVector3()),
    fExitMomentum(TVector3()),
    fExitTime(0.),
    fExitEnergy(0.)
{
}    
// ----------------------------------------------------------------------------



// -----   Standard constructor   ---------------------------------------------
PndDskParticle::PndDskParticle(Int_t trackID, Int_t detectorID,
        TVector3 position, TVector3 momentum, Double_t time,
        Int_t pdgCode, TString pdgName, Double_t energy,
        Int_t motherTrackID, Int_t motherPdgCode, TString motherPdgName)
  : FairMCPoint(trackID, detectorID, position, momentum, time, 0., 0.),
    fPdgCode(pdgCode),
    fPdgName(pdgName),
    fEnergy(energy),
    fMotherTrackID(motherTrackID),
    fMotherPdgCode(motherPdgCode),
    fMotherPdgName(motherPdgName),
    fExitPosition(TVector3()),
    fExitMomentum(TVector3()),
    fExitTime(0.),
    fExitEnergy(0.)    
{
}
// ----------------------------------------------------------------------------



// -----   Destructor   -------------------------------------------------------
PndDskParticle::~PndDskParticle()
{
}
// ----------------------------------------------------------------------------



// -----   Public method Print   ----------------------------------------------
void
PndDskParticle::Print(const Option_t* opt) const {
  cout << "-I- PndDskParticle: track" << fTrackID 
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
PndDskParticle::SetFinalValues(TVector3 exitPosition, TVector3 exitMomentum,
        Double_t exitTime, Double_t exitEnergy)
{
  fExitPosition = exitPosition;
  fExitMomentum = exitMomentum;
  fExitTime     = exitTime;
  fExitEnergy   = exitEnergy;
}
// ----------------------------------------------------------------------------



ClassImp(PndDskParticle)
