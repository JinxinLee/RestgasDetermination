/** CbmstsMapsHitInfo source file
 *@author Michael Deveaux <m.deveaux@gsi.de>
 ** Acknowledgements to M. Al-Turany, D. Bertini, G. Gaycken
 ** Version beta 0.1 (02.02.2005)
 **/


#include "PndSttHitInfo.h"


/** Constructor **/
PndSttHitInfo::PndSttHitInfo() :
  fFileNumber(-1), fEventNumber(-1), fTrackID(-1), fPointID(-1), fNMerged(0), fIsFake(kFALSE)
{
}


/** Constructor with all parameters **/
PndSttHitInfo::PndSttHitInfo(Int_t fileNumber, Int_t eventNumber,
			     Int_t trackID, Int_t pointID,
			     Int_t nMerged, Bool_t isFake) :
  fFileNumber(fileNumber), fEventNumber(eventNumber), fTrackID(trackID), fPointID(pointID), fNMerged(nMerged), fIsFake(isFake)
{
}


/** Destructor **/
PndSttHitInfo::~PndSttHitInfo() { }


/** Public method Clear **/
void PndSttHitInfo::Clear() {
  fFileNumber  = -1;
  fEventNumber = -1;
  fTrackID     = -1;
  fPointID     = -1;
  fNMerged     =  0;
  fIsFake      = kFALSE;
}



ClassImp(PndSttHitInfo);
