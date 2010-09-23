/////////////////////////////////////////////////////////////
//
//  PndEmcHit
//
//  Emc digitised hit
//
//  Created 14/08/06  by S.Spataro
//
/////////////////////////////////////////////////////////////// 

#include "PndEmcHit.h"
#include "PndDetectorList.h"
#include <iostream>

using namespace std;

// -----   Default constructor   -------------------------------------------
PndEmcHit::PndEmcHit() {}
// -------------------------------------------------------------------------

// -----   Constructor           -------------------------------------------
PndEmcHit::PndEmcHit(Int_t trackid, Int_t id, Float_t energy, Float_t time, Float_t X, Float_t Y, Float_t Z)
{
  fTime = time;
  fEnergy = energy;
  fRefIndex = trackid;
  fDetectorID = id;
  fX = X;  fY = Y;  fZ = Z;
  fDx = 0.;  fDy = 0.;  fDz = 0.;
  
}
// -----   Constructor           -------------------------------------------
PndEmcHit::PndEmcHit(Int_t trackid, Int_t id, Float_t energy, Float_t time, Float_t X, Float_t Y, Float_t Z, std::vector<Int_t> McList)
{
  fTime = time;
  fEnergy = energy;
  fRefIndex = trackid;
  fDetectorID = id;
  fX = X;  fY = Y;  fZ = Z;
  fDx = 0.;  fDy = 0.;  fDz = 0.;
  fMcList = McList;
  //SetLinks(FairMultiLinkedData("MCTrack", McList));
}
// -----   Constructor           -------------------------------------------
PndEmcHit::PndEmcHit(Int_t trackid, Int_t id, Float_t energy, Float_t time, Float_t X, Float_t Y, Float_t Z, std::vector<PndEmcPoint*> PointList)
{
  fTime = time;
  fEnergy = energy; 
  fRefIndex = trackid;
  fDetectorID = id;
  fX = X;  fY = Y;  fZ = Z;
  fDx = 0.;  fDy = 0.;  fDz = 0.;
  fPointList = PointList;
}

//Copy
PndEmcHit::PndEmcHit(const PndEmcHit &copy)
{
	fEnergy=copy.fEnergy;
	fTime=copy.fTime;
	fRefIndex=copy.fRefIndex;
	fDetectorID=copy.fDetectorID;
	fX=copy.fX;
	fY=copy.fY;
	fZ=copy.fZ;
	// PointList and McList not copied...
}
// -----   Destructor   ----------------------------------------------------
PndEmcHit::~PndEmcHit() {}
// -------------------------------------------------------------------------


// -----   Public method Print   -------------------------------------------
void PndEmcHit::Print(const Option_t* opt) const {
  cout << "EMC hit: cellid=" << GetDetectorID() << ", Energy=" << fEnergy;
  if (fRefIndex>0) cout << ", TrackID= " << fRefIndex;
//  cout << ", x=" << GetX() << ", y=" << GetY() << endl << flush; 
}
// -------------------------------------------------------------------------

Short_t PndEmcHit::GetXPad() const {
  // Return the X pad value for clusterization
  
  // Barrel EMC
  if (GetModule()==1 || GetModule()==2)
    return (GetCrystal()+(GetCopy()-1)*10);
  
  // Endcups and forward EMC
  if ((GetModule()==3) || (GetModule()==4) || (GetModule()==5))
    {
      if (GetCopy()==1) {  return -GetRow()+1;}
      if (GetCopy()==2) {  return -GetRow()+1; }
      if (GetCopy()==3) {  return  GetRow();}
      if (GetCopy()==4) {  return  GetRow();}
    }
  
  // Test EMC
  if (GetModule()==6)
    return (GetRow());
  
  return -1000; // failure
}

Short_t PndEmcHit::GetYPad() const {
  // Return the Y pad value for clusterization
  
  // Barrel EMC
  if (GetModule()==1)
    return (GetRow()+29);
  
  if (GetModule()==2)
    return (-GetRow()+30);
  
  // Endcups and forward EMC
  if ((GetModule()==3) || (GetModule()==4) || (GetModule()==5))
    {
      if (GetCopy()==1) {  return  GetCrystal();   }
      if (GetCopy()==2) {  return -GetCrystal()+1;     }
      if (GetCopy()==3) {  return -GetCrystal()+1; }
      if (GetCopy()==4) {  return  GetCrystal();       }
    } 

  // Test EMC
  if (GetModule()==6)
    return (GetCrystal());
  
  return -1000; // failure
}
ClassImp(PndEmcHit)
