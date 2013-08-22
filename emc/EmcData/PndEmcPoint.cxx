/////////////////////////////////////////////////////////////
//
//  PndEmcPoint
//
//  Geant point for Emc detector
//
//  Created 14/08/06  by S.Spataro
//
///////////////////////////////////////////////////////////////

#include "PndEmcPoint.h"
#include "PndDetectorList.h"
#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndEmcPoint::PndEmcPoint() : FairMCPoint(),
			     nModule(-1), nRow(-1),  nCrystal(-1), nCopy(-1)
{
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndEmcPoint::PndEmcPoint(Int_t trackID, Int_t detID, Int_t evtID, TVector3 pos,
                        TVector3 mom, Double_t tof, Double_t length,
			 Double_t eLoss, Short_t mod, Short_t row, Short_t crys, Short_t copy)
  : FairMCPoint(trackID, detID, pos, mom, tof, length, eLoss, evtID),
    nModule(mod), nRow(row), nCrystal(crys), nCopy(copy)
{
  SetLink(FairLink("MCTrack", trackID));
}
// -------------------------------------------------------------------------

// -----   Copy constructor   ------------------------------------------
PndEmcPoint::PndEmcPoint(const PndEmcPoint& point)
  :FairMCPoint(point.fTrackID, point.fDetectorID, TVector3(point.fX, point.fY, point.fZ), TVector3(point.fPx, point.fPy, point.fPz), 
	       point.fTime, point.fLength, point.fELoss, point.fEventId),
   nModule(point.nModule), nRow(point.nRow), nCrystal(point.nCrystal), nCopy(point.nCopy)

{ 
  SetLinks(point.GetLinks());
} 
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndEmcPoint::~PndEmcPoint() { }
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void PndEmcPoint::Print(const Option_t* opt) const {
  cout << "-I- PndEmcPoint: EMC Point for track " << fTrackID 
       << " in detector " << fDetectorID << endl;
  cout << "    Position (" << fX << ", " << fY << ", " << fZ
       << ") cm" << endl;
  cout << "    Momentum (" << fPx << ", " << fPy << ", " << fPz
       << ") GeV" << endl;
  cout << "    Time " << fTime << " ns,  Length " << fLength 
       << " cm,  Energy loss " << fELoss*1.0e06 << " keV" << endl;
}
// -------------------------------------------------------------------------

Short_t PndEmcPoint::GetXPad() const {
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
    return -(GetCrystal()-36); //the minus sign before the paranthesis is introduced since the geometry of FwEndCap gets rotated by 180 deg around the y-axis in PndEmc.cxx;
			       //this rotation was done in turn due to the way the geometry was defined in the geometry file of the forward end cap

  /*
   if (GetModule()==3 && GetCrystal()==999 && GetRow()==999)
    return GetCrystal();
  */
  
  // Test EMC
  if (GetModule()==6)
    return  GetRow();
  
  return -1000; // failure
}

Short_t PndEmcPoint::GetYPad() const {
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


ClassImp(PndEmcPoint)
