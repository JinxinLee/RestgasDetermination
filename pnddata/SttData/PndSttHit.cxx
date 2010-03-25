/** CbmStsMapsHit 
 *@author Michael Deveaux <m.deveaux@gsi.de>
 **Acknowledgements to M. Al-Turany, D. Bertini, G. Gaycken
 **Version beta 0.1 (02.02.2005)
 **/


#include "PndSttHit.h"

#include <iostream>
#include "TMath.h"
/** Default constructor **/
PndSttHit::PndSttHit() 
{
    Clear();
}


PndSttHit::PndSttHit(Int_t detID, Int_t tubeID, Int_t mcindex, TVector3& pos, TVector3& dpos, Double_t p, Double_t isochrone, Double_t isochroneError, Double_t chDep) : FairHit(detID, pos, dpos, mcindex)
{
  fTubeID = tubeID;
  fPulse   = p;
  fIsochrone = isochrone;
  fIsochroneError = isochroneError;
  fDepCharge = chDep;
  
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



ClassImp(PndSttHit)
