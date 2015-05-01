// ----------------------------------------------------------------------------
// -----                  PndDskTrackPoint source file                    -----
// -----                 Created 27/03/09  by P. Koch                     -----
// ----------------------------------------------------------------------------

#include <iostream>
using std::cout;
using std::endl;

#include "PndDskTrackPoint.h"



// -----   Default constructor   ----------------------------------------------
PndDskTrackPoint::PndDskTrackPoint()
  : FairMCPoint()
{
}
// ----------------------------------------------------------------------------


// -----   Standard constructor   ---------------------------------------------
PndDskTrackPoint::PndDskTrackPoint(Int_t trackID, Int_t detectorID, TVector3 position, TVector3 momentum,
      Double_t time, Double_t length, Double_t eLoss)
    : FairMCPoint(trackID, detectorID, position, momentum, time, length, eLoss)
{
}
// ----------------------------------------------------------------------------


// -----   Destructor   -------------------------------------------------------
PndDskTrackPoint::~PndDskTrackPoint()
{
}
// ----------------------------------------------------------------------------


// -----   Public method Print   ----------------------------------------------
void
PndDskTrackPoint::Print(const Option_t* opt) const {
  (void)opt; // to remove "unused" warnings
  cout << "-I- PndDskTrackPoint: track" << fTrackID
       << " created in detector " << fDetectorID << endl;
  cout << "    at Position (" << fX << ", " << fY << ", " << fZ
       << ") cm" << endl;
  cout << "    with Momentum (" << fPx << ", " << fPy << ", " << fPz
       << ") eV" << endl;
  cout << "    at Time " << fTime << " ns" << endl;
}
// ----------------------------------------------------------------------------

ClassImp(PndDskTrackPoint)
