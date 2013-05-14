//
// PndTrkHit.cxx
// 
// Class for pattern recognition hit
//
// authors: Lia Lavezzi - INFN Pavia (2012)
//

#include "PndTrkHit.h"

#include "PndDetectorList.h"

#include <iostream>

// ROOT
#include "TArc.h"
#include "TMarker.h"

using namespace std;

// hit equality
// Bool_t operator==(const PndTrkHit &hit1, const PndTrkHit &hit2) {
//   cout << "######################### operator <" << endl;
//   return  hit1.fDetectorID == hit2.fDetectorID && hit1.fHitID == hit2.fHitID;
// }

Bool_t PndTrkHit::operator==(const PndTrkHit &hit1) {
  return  fDetectorID == hit1.fDetectorID && fHitID == hit1.fHitID;
}

Bool_t PndTrkHit::operator<(const PndTrkHit &hit1) {
  return  fSortVariable < hit1.fSortVariable;
}

// // hit sorting
// Bool_t operator<(const PndTrkHit *hit1, const PndTrkHit *hit2) {
//   cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! operator <" << endl;
//   return  hit1->fSortVariable == hit2->fSortVariable;
// }




PndTrkHit::PndTrkHit() :  fHitID(-1), fDetectorID(-1), fUsed(0), fIRegion(-1), fSensorID(-1), fPosition(0., 0., 0.), fIsochrone(0), fSortVariable(-1) {}

PndTrkHit::PndTrkHit(Int_t hitID, Int_t detID, Bool_t used, Int_t iregion, Int_t sensorID, TVector3& pos, Double_t isochrone, Double_t sortvar) : fHitID(hitID), fDetectorID(detID), fUsed(used), fIRegion(iregion), fSensorID(sensorID), fPosition(pos.X(), pos.Y(), pos.Z()), fIsochrone(isochrone), fSortVariable(sortvar) {}

// PndTrkHit::PndTrkHit(Int_t hitID, Int_t detID, Bool_t used, Int_t iregion, Int_t sensorID, TVector3& pos) :  fHitID(hitID), fDetID(detID), fUsed(used), fIRegion(iregion), fSensorID(sensorID), fPosition(pos.X(), pos.Y(), pos.Z()), fIsochrone(0) {}

PndTrkHit::PndTrkHit(const PndTrkHit &hit) : fHitID(hit.fHitID), fDetectorID(hit.fDetectorID), fUsed(hit.fUsed), fIRegion(hit.fIRegion), fSensorID(hit.fSensorID), fPosition(hit.fPosition), fIsochrone(hit.fIsochrone), fSortVariable(hit.fSortVariable) {}

PndTrkHit::~PndTrkHit() {}

Double_t PndTrkHit::GetDistance(PndTrkHit *fromhit) { 
 TVector3 frompoint = fromhit->GetPosition();
 TVector3 position = GetPosition();
 return GetDistance(frompoint);
}

Double_t PndTrkHit::GetDistance(TVector3 frompoint) {
  TVector3 position = GetPosition();
  return (position - frompoint).Mag();
}

Double_t PndTrkHit::GetXYDistance(PndTrkHit *fromhit) { 
 TVector3 frompoint = fromhit->GetPosition();
 TVector3 position = GetPosition();
 return GetXYDistance(frompoint);
}

Double_t PndTrkHit::GetXYDistance(TVector3 frompoint) {
  TVector3 position = GetPosition();
  return (position - frompoint).Perp();
}


void PndTrkHit::Draw(Color_t color) {
  //  cout << "draw "<< fIRegion << endl;
  Int_t style = 1; 
  if(fUsed  && color == kBlack) {
    style = 0;
    color = kGray;
  }
  TMarker *mrk = NULL;
  TArc *arc = NULL;

  switch(fIRegion) {
  case 0: 
    {
      mrk = new TMarker(fPosition.X(), fPosition.Y(), 21);
      break;
    }
  case 1: 
    {
      mrk = new TMarker(fPosition.X(), fPosition.Y(), 25);
      break;
    }
  case 2: 
  case 6:
  case 3:
  case 7: 
    {
      arc = new TArc(fPosition.X(), fPosition.Y(), fIsochrone); // TUBERADIUS);
      break;
    }
  case 4:
  case 5: 
    {
      //      arc = new TArc(fPosition.X(), fPosition.Y(), TUBERADIUS); // CHECK skew
      mrk = new TMarker(fPosition.X(), fPosition.Y(), 3);
      break;
    }
  }

  if(mrk) {
    mrk->SetMarkerColor(color);
    mrk->Draw("SAME");
  }
  if(arc) {
    arc->SetFillColor(color);
    arc->SetLineColor(color);
    arc->SetFillStyle(style);
    arc->Draw("SAME");
  }


}

void PndTrkHit::Print() {

  cout << "###############################" << endl;
  cout << "hitID " << fHitID << " used " << fUsed << " iregion " << fIRegion << endl;
  cout << " detID " << fDetectorID << " isensor " << fSensorID << " isochrone " << fIsochrone << endl;
  fPosition.Print();

}


ClassImp(PndTrkHit)
