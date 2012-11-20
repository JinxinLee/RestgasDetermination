/** CbmStsMapsHit
 *@author Michael Deveaux <m.deveaux@gsi.de>
 **Acknowledgements to M. Al-Turany, D. Bertini, G. Gaycken
 **Version beta 0.1 (02.02.2005)
 **/


#include "PndFtsHit.h"

#include "PndDetectorList.h"

#include <iostream>
#include "TMath.h"
/** Default constructor **/
PndFtsHit::PndFtsHit()
{
    Clear();
}


PndFtsHit::PndFtsHit(Int_t detID, Int_t tubeID, Int_t chamberID, Int_t layerID, Int_t skew, Int_t mcindex, TVector3& pos, TVector3& dpos, Double_t p, Double_t isochrone, Double_t isochroneError, Double_t chDep) : FairHit(detID, pos, dpos, mcindex)
{
  fTubeID = tubeID;
  fChamberID = chamberID;
  fLayerID = layerID;
  fSkew = skew;
  fPulse   = p;
  fIsochrone = isochrone;
  fIsochroneError = isochroneError;
  fDepCharge = chDep;

  SetLink(FairLink("FTSPoint", mcindex));

}

/** Public method Clear **/
void PndFtsHit::Clear()
{
  fTubeID = 0;
  fChamberID = 0;
  fLayerID = 0;
  fSkew = 0;
  fPulse   = 0;
  fIsochrone = 0.;
  fIsochroneError = 0.;
  fDepCharge = 0.;

} 


/** Destructor **/
PndFtsHit::~PndFtsHit()
{
}



ClassImp(PndFtsHit)
