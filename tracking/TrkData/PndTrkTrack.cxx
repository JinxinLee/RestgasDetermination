//
// PndTrkTrack.cxx
// 
// 
// 
//
// authors: Lia Lavezzi - INFN Pavia (2012)
//

#include "PndTrkTrack.h"
#include "TArc.h"
#include <iostream>


using namespace std;

PndTrkTrack::PndTrkTrack() :  fRefHit(NULL), fCluster(PndTrkCluster()), fCenterX(0), fCenterY(0), fRadius(0), fTanL(0), fZ0(0), fCharge(0) {}

PndTrkTrack::PndTrkTrack(PndTrkCluster *cluster) :  fRefHit(NULL), fCluster(*cluster), fCenterX(0), fCenterY(0), fRadius(0), fTanL(0), fZ0(0), fCharge(0) {}

PndTrkTrack::PndTrkTrack(PndTrkCluster *cluster, double x, double y, double radius) : fRefHit(NULL), fCluster(*cluster), fCenterX(x), fCenterY(y), fRadius(radius), fTanL(0), fZ0(0), fCharge(0) {}

PndTrkTrack::PndTrkTrack(PndTrkHit *hit, PndTrkCluster *cluster, double x, double y, double radius) : fRefHit(hit), fCluster(*cluster), fCenterX(x), fCenterY(y), fRadius(radius), fTanL(0), fZ0(0), fCharge(0) {}

PndTrkTrack::PndTrkTrack(double x, double y, double radius) : fRefHit(NULL), fCluster(PndTrkCluster()), fCenterX(x), fCenterY(y), fRadius(radius), fTanL(0), fZ0(0), fCharge(0) {}


PndTrkTrack::~PndTrkTrack() {
  delete fRefHit;
}

Bool_t PndTrkTrack::operator==(PndTrkTrack track)  {
 int nofhits = GetCluster().GetNofHits();
  return nofhits == track.GetCluster().GetNofHits() && fRadius == track.GetRadius() && fCenterX == track.GetCenter().X() && fCenterY == track.GetCenter().Y(); // CHECK
}

PndTrackCand PndTrkTrack::ConvertToPndTrackCand() {
  PndTrackCand trkcand;
  for(int ihit = 0; ihit < fCluster.GetNofHits(); ihit++) {
    PndTrkHit *hit = fCluster.GetHit(ihit);
    int detid = hit->GetDetectorID();
    int hitid = hit->GetHitID();
    trkcand.AddHit(detid, hitid, ihit);
  }
  return trkcand;
}

PndTrack PndTrkTrack::ConvertToPndTrack() {

  ComputeCharge();
  
  // first
  TVector3 pos1, mom1;
  PndTrkHit *hit1 = fCluster.GetHit(0);
  mom1 = ComputeMomentumAtPosition(hit1->GetPosition(), pos1);

  // last
  TVector3 pos2, mom2;
  PndTrkHit *hit2 = fCluster.GetHit(fCluster.GetNofHits() - 1);
  mom2 = ComputeMomentumAtPosition(hit2->GetPosition(), pos2);
    
  TVector3 dj(1, 0, 0), dk(0, 0, 1);   // CHECK
  TVector3 dpos1(2., 2., 10.);         // CHECK
  TVector3 dpos2(2., 2., 10.);         // CHECK
  TVector3 dmom1(0.2, 0.2, 0.5);       // CHECK
  TVector3 dmom2(0.2, 0.2, 0.5);       // CHECK

  FairTrackParP firstpar(pos1, mom1,
			 dpos1, dmom1, fCharge,
			 pos1, dj, dk);
    
  FairTrackParP lastpar(pos2, mom2,
			dpos2, dmom2, fCharge,
			pos2, dj, dk);

  PndTrackCand trkcand = ConvertToPndTrackCand();
  PndTrack track(firstpar, lastpar, trkcand);
  if(mom1.Z() == -999) track.SetFlag(-1);
  return track;
}



// void OrderFrom(TVector3 point) {

//   TVector3 tmpposition = point;
//   double tmpdistance = 1000.;
//   for(int ihit = 0; ihit < fCluster.GetNofHits(); ihit++) {
//     PndTrkHit *hit = fCluster.GetHit(ihit);
//     TVector3 position = hit->GetPosition();
//     double distance = (position - point).Mag();
//     if(distance < tmpdistance) {
//       tmpposition = position;
//       tmpdistance = distance;
//     }
//   }



void PndTrkTrack::ComputeCharge() { // CHECK!!!

  /**
   // consider the centers
   Int_t nleft, nright = 0;

   // first
   PndTrkHit *hit1 = fCluster.GetHit(0);
   // last
   PndTrkHit *hit2 = fCluster.GetHit(fCluster->GetNofHits() - 1);

   // vector from 1st to last hit
   TVector3 firsttolast = hit2->GetPosition() - hit1->GetPosition();

   for(int ihit = 1; ihit < fCluster.GetNofHits() - 1; ihit++) {
   PndTrkHit *hit = fCluster.GetHit(ihit);

   // vector from 1st to this hit
   TVector3 position = hit->GetPosition() - hit1->GetPosition();

   double crossz = position.Cross(firsttolast).Z();
      
   // if  crossz > 0  hits stays 'on the right' (counterclockwise) --> negative
   // otherwise it stays 'on the left' (clockwise) --> positive
   if (crossz > 0.) nright++;
   else nleft++;
   }

   if(nright > nleft) fCharge = -1;
   else fCharge = 1;
  **/

  // consider the poca on track
  Int_t nmore = 0, nless = 0;

  // first
  PndTrkHit *hit1 = (PndTrkHit*) fCluster.GetHit(0);
  PndTrkTools tools;
  TVector3 position1 = tools.ComputePocaToPointOnCircle3(hit1->GetPosition().X(), hit1->GetPosition().Y(), fCenterX, fCenterY, fRadius);
      
  // vector from 1st to last hit
  TVector3 direction1 = position1 - TVector3(fCenterX, fCenterY, 0.);

  double tmpphi = 0.;

  for(int ihit = 1; ihit < fCluster.GetNofHits(); ihit++) {
    PndTrkHit *hit = fCluster.GetHit(ihit);
    TVector3 position = tools.ComputePocaToPointOnCircle3(hit->GetPosition().X(), hit->GetPosition().Y(), fCenterX, fCenterY, fRadius);
      
    // vector from 1st to this hit
    TVector3 direction = position - TVector3(fCenterX, fCenterY, 0.);
    double phi = TMath::ACos(direction.Dot(direction1)/(direction.Mag() * direction1.Mag()));
    if(ihit > 1) phi >= tmpphi ? nmore++ : nless++;
    tmpphi = phi;
    // cout << "phi " << phi * TMath::RadToDeg() << " " << hit->GetHitID() << " " << hit->GetDetectorID() << endl;
  }
  
  if(nmore > nless) fCharge = -1;
  else fCharge = 1;

  //  cout << "fCharge " << fCharge << " " << nmore << " " << nless << endl;

}


TVector3 PndTrkTrack::ComputeMomentumAtPosition(TVector3 position, TVector3 &newposition) {
  TVector3 momentum(-999, -999, -999);
  newposition = position;
  if(fCharge == 0) {
    // cout << "ComputeMomentumAtPos: CHARGE = 0!" << endl;
    return momentum;
  }

  TVector2 center(fCenterX, fCenterY);
  TVector2 pos = TVector2(position.X(), position.Y());
  TVector2 myrad = center - pos;
  double distance = TMath::Abs(myrad.Mod() - fRadius);
  if(distance > 0.5) {
    // cout << "ComputeMomentumAtPosition: POINT NOT ON THE TRACK " << distance << endl;
    // return momentum;  // CHECK
    // alternative
    // CHECK
    PndTrkTools tools;
    newposition = tools.ComputePocaToPointOnCircle3(pos.X(), pos.Y(), fCenterX, fCenterY, fRadius);
    myrad = center - pos;
    distance = TMath::Abs(myrad.Mod() - fRadius);
    //   if(distance > 0.5) {
    //       cout << "ComputeMomentumAtPosition: AGAIN POINT NOT ON THE TRACK " << distance << endl;
    //       return momentum;  
    //     }
  }


  Double_t rotx, roty;
  rotx = - fCharge * myrad.Y();
  roty = fCharge * myrad.X();


  //  cout << "COMPUTE MOMENTUM " << endl;
  Double_t pt = 0.006 * fRadius;

  //  cout << "pt " << pt << endl;
  Double_t pl = -999;
  if(fTanL != -999) pl = pt * fTanL;
  //  cout << "pl " << pl << " tanl " << fTanL << endl;

  Double_t ptot = TMath::Sqrt(pt * pt + pl * pl);

  momentum.SetX(rotx); // CHECK magnitude?
  momentum.SetY(roty); // CHECK magnitude?
  momentum.SetZ(0.);
  momentum.SetMag(pt);
  momentum.SetZ(pl);
  return momentum;
}

Double_t PndTrkTrack::ComputePhi(TVector3 hit) 
{
  TVector3 center(fCenterX, fCenterY, 0.);
  TVector3 fromcentertohit = hit - center;
  
  // I want the positive phi angle from x axis
  // in range [0, 360[.
  // I use TVector3::Phi() [fromcentertohit.Phi()]:
  // x   y      Phi        use!
  // -------------------------
  // +   +      0/90       phi
  // -   +     90/180      phi
  // -   -   -180/-90    phi + 360
  // +   -    -90/0      phi + 360
  
  double phi = fromcentertohit.Phi();
  if(fromcentertohit.Y() < 0) phi += (2 * TMath::Pi());
  // cout << "final phi in rad " << phi << endl;
  return phi * TMath::RadToDeg();
}


// =======================================================================================
void PndTrkTrack::Draw(Color_t color) {
  TArc *track = new TArc(fCenterX, fCenterY, fRadius);
  track->SetFillStyle(0);
  track->SetLineColor(color);
  track->Draw("SAME");
}

void PndTrkTrack::LightUp() {
  Draw(kYellow);
  fCluster.LightUp();
}

ClassImp(PndTrkTrack)
 
