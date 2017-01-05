/** CbmStsMapsHit 
 *@author Michael Deveaux <m.deveaux@gsi.de>
 **Acknowledgements to M. Al-Turany, D. Bertini, G. Gaycken
 **Version beta 0.1 (02.02.2005)
 **/


#include "PndSttHit.h"

#include "PndDetectorList.h"

#include <iostream>
#include "TMath.h"

using namespace std;

/** Default constructor **/
PndSttHit::PndSttHit() :
 FairHit(), fTubeID(0), fPulse(0), fIsochrone(0), fIsochroneError(0), fDepCharge(0)
{
    Clear();
}


PndSttHit::PndSttHit(Int_t detID, Int_t tubeID, Int_t mcindex, TVector3& pos, TVector3& dpos, Double_t p, Double_t isochrone, Double_t isochroneError, Double_t chDep) : FairHit(detID, pos, dpos, mcindex),
  fTubeID(tubeID), fPulse(p), fIsochrone(isochrone), fIsochroneError(isochroneError), fDepCharge(chDep)
{
  SetTimeStamp(p);

  SetLink(FairLink("STTPoint", mcindex));
  
}

/** Public method Clear **/
void PndSttHit::Clear() 
{
  fTubeID = 0;
  fPulse   = 0;
  fIsochrone = 0.;
  fIsochroneError = 0.;
  fDepCharge = 0.;

}  

 
/** Destructor **/
PndSttHit::~PndSttHit() 
{
} 


Double_t PndSttHit::ComputedEdx(PndTrack *track, Double_t tuberadius) {
  
  // cout << "tuberadius/isochrone " << tuberadius << " " << fIsochrone << endl; 

 Double_t distance = 2 * sqrt(tuberadius * tuberadius - fIsochrone * fIsochrone); // cm
//  cout << "tuberadius/isochrone/distance " << tuberadius << " " << fIsochrone << " " << distance << endl;

  TVector3 momentum = track->GetParamFirst().GetMomentum();
  //Double_t pt = momentum.Perp();
  //Double_t pl = momentum.Z();

  Double_t coslam = momentum.Perp()/momentum.Mag();

  distance = distance / coslam;    
  //   cout << "depcharge " << fDepCharge << endl;;
   Double_t dedx = 0.;
   if (distance != 0)  dedx = fDepCharge/(1000000 * distance);  // in arbitrary units
//   cout << "cosla/distance2/dedx " << coslam << " " << distance/coslam << " " << dedx << endl;

  return dedx;
}


ClassImp(PndSttHit)
