#include "Riostream.h"
#include <iomanip>

#include "PndLheHit.h"

ClassImp(PndLheHit)

//________________________________________________________________
  PndLheHit::PndLheHit() 
    :FairHit(),
     fUsed(kFALSE),
     fHitNumber(-1),
     fTrackID(-1)
{
}

//________________________________________________________________
PndLheHit::~PndLheHit() {
  // ---
}

//________________________________________________________________
void PndLheHit::Print() {

      cout << flush;
      cout << " " << fTrackID ;
      cout << " " << fDetectorID;
      cout << " " << fHitNumber;
      cout << " " << Int_t(fUsed) ;

      cout.setf(ios::right, ios::floatfield);
      cout.precision(3);
      cout << " " << setw(7) << fX;
      cout << " " << setw(7) << fY;
      cout << " " << setw(5) << fZ;
      cout.setf(ios::scientific);
      cout.precision(5);
      cout << " " << setw(10) << fDx;
      cout << " " << setw(10) << fDy;
      cout << " " << setw(10) << fDz;
      cout << endl;
      cout.setf(ios::right, ios::floatfield);

}
