/////////////////////////////////////////////////////////////
// PndTofPoint
//
// Class for hit description for RpcTof
//
/////////////////////////////////////////////////////////////
#include <iostream>

using std::cout;
using std::endl;

#include "PndRpcPoint.h"

// -----   Default constructor   -------------------------------------------
PndRpcPoint::PndRpcPoint() : CbmMCPoint() {}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndRpcPoint::PndRpcPoint(Int_t trackID, Int_t detID, TVector3 pos,
 TVector3 mom, Double_t tof, Double_t length, Double_t eLoss, Int_t 
eventID, Double_t t1, Double_t t2, TVector3 pos_sm)
  : CbmMCPoint(trackID, detID, pos, mom, tof, length, eLoss) {
    fX  = pos.X();  //cm
    fY  = pos.Y();  //cm
    fZ  = pos.Z();  //cm
    //smeared position
    fX_sm  = pos_sm.X();  
    fY_sm  = pos_sm.Y();  
    fZ_sm  = pos_sm.Z();  
    //momentum
    fPx = mom.Px();
    fPy = mom.Py();
    fPz = mom.Pz();
    fDetectorID = detID;
    ftrackID = trackID;
    fTime  =  tof;
    fELoss = eLoss;
    fEventID  = eventID;
    fLength = length;   
    ft1=t1;
    ft2=t2;
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndRpcPoint::~PndRpcPoint() { }
// -------------------------------------------------------------------------



// -----   Public method Print   -------------------------------------------
void PndRpcPoint::Print(const Option_t* opt) const {
  cout << "-I- RpcTofPoint: Tof Point for track " << fTrackID 
       << " in module " << fDetectorID << endl;
  cout << "    Position (" << fX << ", " << fY << ", " << fZ
       << ") cm" << endl;
  cout << "    Position_smear (" << fX_sm << ", " << fY_sm << ", " << 
     fZ_sm << ") cm" << endl;
  cout << "    Momentum (" << fPx << ", " << fPy << ", " << fPz
       << ") GeV" << endl;
  cout << "    Time " << fTime << " ns,  Length " << fLength 
       << " cm,  Energy loss " << fELoss*1.0e06 << " keV" << endl;
  cout<< " Time1 " << ft1 << " Time2 " << ft2 << "ns" << endl; 
}
// -------------------------------------------------------------------------



ClassImp(PndRpcPoint)
