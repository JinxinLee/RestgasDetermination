/** PndHypHitInfo source file
 *@author 
 ** Acknowledgements to M. Al-Turany, D. Bertini, G. Gaycken, Michael Deveaux <m.deveaux@gsi.de>
 ** Version beta 0.1 (02.02.2005)
 **/


#include "PndHypHitInfo.h"


/** Constructor **/
PndHypHitInfo::PndHypHitInfo() :
  fFileNumber(-1),
  fEventNumber(-1),
  fTrackID(-1),
  fPointID(-1),
  fNMerged(0),
  fIsFake(kFALSE)
{
}


/** Constructor with all parameters **/
PndHypHitInfo::PndHypHitInfo(Int_t fileNumber, Int_t eventNumber,
			     Int_t trackID, Int_t pointID,
			     Int_t nMerged, Bool_t isFake) :
  fFileNumber(fileNumber),
  fEventNumber(eventNumber),
  fTrackID(trackID),
  fPointID(pointID),
  fNMerged(nMerged),
  fIsFake(isFake)
{
}


/** Destructor **/
PndHypHitInfo::~PndHypHitInfo() { }


/** Public method Clear **/
void PndHypHitInfo::Clear() {
  fFileNumber  = -1;
  fEventNumber = -1;
  fTrackID     = -1;
  fPointID     = -1;
  fNMerged     =  0;
  fIsFake      = kFALSE;
}



ClassImp(PndHypHitInfo);
