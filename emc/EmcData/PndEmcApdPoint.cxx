/////////////////////////////////////////////////////////////
//
//  PndEmcApdPoint
//
//  Geant point for EmcApd detector
//
//  Created 07/04/08  by S.Spataro
//
///////////////////////////////////////////////////////////////

#include "PndEmcApdPoint.h"

#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndEmcApdPoint::PndEmcApdPoint() : FairMCPoint() {
  fTrackID    = -1;
  fDetectorID = -1;
  fEventID    = -1;
  fX          = fY  = fZ =  0.;
  fPx         = fPy = fPz = 0.;
  fTime       =  0.;
  fLength     =  0.;
  fELoss      =  0.;
  nModule     = -1;
  nRow        = -1;
  nCrystal    = -1;
  nCopy       = -1;
  fFlag       = -10;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndEmcApdPoint::PndEmcApdPoint(Int_t trackID, Int_t detID, Int_t evtID, TVector3 pos,
                        TVector3 mom, Double_t tof, Double_t length,
			 Double_t eLoss, Short_t mod, Short_t row, Short_t crys, Short_t copy, Short_t flag) {
  fTrackID    = trackID;
  fDetectorID = detID; 
  fEventID    = evtID;
  fX          = pos.X();
  fY          = pos.Y();
  fZ          = pos.Z();
  fPx         = mom.Px();
  fPy         = mom.Py();
  fPz         = mom.Pz();
  fTime       = tof;
  fLength     = length;
  fELoss      = eLoss;
  nModule     = mod;
  nRow        = row;
  nCrystal    = crys;
  nCopy       = copy;
  fFlag       = flag;
}



// -----   Destructor   ----------------------------------------------------
PndEmcApdPoint::~PndEmcApdPoint() { }
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void PndEmcApdPoint::Print(const Option_t* opt) const {
  cout << "-I- PndEmcApdPoint: EMCAPD Point for track " << fTrackID 
       << " in detector " << fDetectorID << endl;
  cout << "    Position (" << fX << ", " << fY << ", " << fZ
       << ") cm" << endl;
  cout << "    Momentum (" << fPx << ", " << fPy << ", " << fPz
       << ") GeV" << endl;
  cout << "    Time " << fTime << " ns,  Length " << fLength 
       << " cm,  Energy loss " << fELoss*1.0e06 << " keV" << endl;
}
// -------------------------------------------------------------------------

Short_t PndEmcApdPoint::GetXPad() const {
  // Return the X pad value for clusterization
  
  // Barrel EMC
  if (GetModule()==1 || GetModule()==2)
    return (GetCrystal()+(GetCopy()-1)*10);
  
  // BwEndCap and forward EMC
  if ((GetModule()==4) || (GetModule()==5))
    {
      if (GetCopy()==1) {  return -GetRow()+1;}
      if (GetCopy()==2) {  return -GetRow()+1; }
      if (GetCopy()==3) {  return  GetRow();}
      if (GetCopy()==4) {  return  GetRow();}
    }

  // FwEndCap
  if (GetModule()==3)
    return -(GetCrystal()-36);
  
  /*
   if (GetModule()==3 && GetCrystal()==999 && GetRow()==999)
    return GetCrystal();
  */
  
  // Test EMC
  if (GetModule()==6)
    return  GetRow();
  
  return -1000; // failure
}

Short_t PndEmcApdPoint::GetYPad() const {
  // Return the Y pad value for clusterization
  
  // Barrel EMC
  if (GetModule()==1)
    return (GetRow()+29);
  
  if (GetModule()==2)
    return (-GetRow()+30);
  
  // BwEndCap and forward EMC
  if ((GetModule()==4) || (GetModule()==5))
    {
      if (GetCopy()==1) {  return  GetCrystal();   }
      if (GetCopy()==2) {  return -GetCrystal()+1;     }
      if (GetCopy()==3) {  return -GetCrystal()+1; }
      if (GetCopy()==4) {  return  GetCrystal();       }
    }

  // FwEndCap
  if (GetModule()==3)
    return GetRow()-37;
  
  /*
   if (GetModule()==3 && GetCrystal()==999 && GetRow()==999)
    return GetRow();  
  */
  
  // Test EMC
  if (GetModule()==6)
    return  GetCrystal();
  
  return -1000; // failure
}

// -------------------------------------------------------------------------


ClassImp(PndEmcApdPoint)
