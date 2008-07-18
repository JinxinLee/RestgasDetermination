#include "Riostream.h"
#include <iomanip>

#include "PndTpcLheHit.h"

ClassImp(PndTpcLheHit)

//________________________________________________________________
PndTpcLheHit::PndTpcLheHit() {
  //---

  SetUsage(kFALSE);
  SetHitNumber(-1);
  SetTrackID(-1);
  SetDetectorId(kUnknown);

  SetX(0);
  SetY(0);
  SetZ(0);

  SetXerr(0);
  SetYerr(0);
  SetZerr(0);
  

}

//________________________________________________________________
PndTpcLheHit::PndTpcLheHit(PndTpcPoint *point) {

  SetUsage(kFALSE);
  SetHitNumber(-1);
  SetTrackID(-1);
  SetDetectorId(kUnknown);
  
  Float_t error = 0.0;
  SetXerr(error);
  SetYerr(error);
  SetZerr(error);
    
}

//________________________________________________________________
PndTpcLheHit::PndTpcLheHit(Double_t *x, Int_t stn) {
  // Constructor which takes the coodrinates and the station number.

  SetUsage(kFALSE);
  SetHitNumber(-1);

  SetTrackID(-1);
  SetDetectorId(kUnknown);
 
  SetX(x[0]);
  SetY(x[1]);
  SetZ(x[2]);

  SetXerr(0.);
  SetYerr(0.);
  SetZerr(0.);

}

//________________________________________________________________
PndTpcLheHit::~PndTpcLheHit() {
  // ---
}

//________________________________________________________________
void PndTpcLheHit::Print() {

      cout << flush;
      cout << " " << fTrackID ;
      cout << " " << fDetectorId;
      cout << " " << fHitNumber;
      cout << " " << Int_t(fUsed) ;

      cout.setf(ios::right, ios::floatfield);
      cout.precision(3);
      cout << " " << setw(7) << fCoord.X();
      cout << " " << setw(7) << fCoord.Y();
      cout << " " << setw(5) << fCoord.Z();
      cout.setf(ios::scientific);
      cout.precision(5);
      cout << " " << setw(10) << fError.X();
      cout << " " << setw(10) << fError.Y();
      cout << " " << setw(10) << fError.Z();
      cout << endl;
      cout.setf(ios::right, ios::floatfield);

}
