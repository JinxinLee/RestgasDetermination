#include "PndLheHit.h"
#include "PndTpcLheTrack.h"
#include "TrackCand.h"
#include "TClonesArray.h"

#include "Riostream.h"
#include <iomanip>
#include <map>

//using std::map;
using std::vector;
using std::sort;


////////////////////////////////////////////////////////////////////////
//                                                                    //
// PndTpcLheTrack  class - track representation for the LHE              //
//                                                                    //
////////////////////////////////////////////////////////////////////////

ClassImp(PndTpcLheTrack)

//______________________________________________________________
PndTpcLheTrack::PndTpcLheTrack () {
  // Default constructor.

  SetDefaults();
}

//______________________________________________________________
PndTpcLheTrack::PndTpcLheTrack (Int_t tracknumber) {
  // the track number is set.
  
  //  cout << " PndTpcLheTrack (tracknumber)" << endl;

  SetDefaults();
  SetTrackNumber(tracknumber);
}

//______________________________________________________________
PndTpcLheTrack ::~PndTpcLheTrack () {
  // Destructor.

  //  cout << " Destructor for LheTrack" << endl;
  if (fRealHits) { //fRealHits->Delete();} //;
    delete fRealHits;
    fRealHits = NULL;
  }

}

//______________________________________________________________
Float_t PndTpcLheTrack ::ExtrapolateToZ(TVector3 *mom, TVector3 *vertex, const Float_t z) {
  // Function to extrapolate the momentum and vertex coordinates to a well determined z value
  
  // alpha = .2998 * magfield / 100 <- for obtain moment in GeV/c
  Double_t alpha = .2998 * .02 ;
  Double_t Q = double(TMath::Sign(1, GetCharge()));
  
  PndTpcLhePoint hel = GetCircle();
  
  Double_t xc = hel.GetX();
  Double_t yc = hel.GetY();
  Double_t rad = hel.GetZ();
  
  TVector2 centre(xc, yc);
  Double_t fi0 = centre.Phi();    // 0 - 2Pi
  Double_t d0 = Q*(TMath::Sqrt(xc*xc + yc*yc) - hel.GetZ());
  
  TVector2 pivot(-xc, -yc);
  
  Double_t lam = GetTanDipAngle();
  if (lam==0) {
    cout << "-W-  PndTpcLheTrack ::ExtrapolateToZ: lam==0 - skipped track" << endl;
    return -100000;
  }
  
  // now suppose that all tracks 
  Double_t z0 = GetZ0();
  
  Double_t sign = GetMomentum().Z() / (Q*alpha*rad*lam);
  
  Double_t phi = - (z - z0) / (rad*lam*sign); // calulates phi from Z
  //Double_t phi = - (z - z0) * / (rad*lam); // calulates phi from Z

  Double_t x = d0*TMath::Cos(fi0) + rad *(TMath::Cos(fi0) - TMath::Cos(fi0 + phi));   
  Double_t y = d0*TMath::Sin(fi0) + rad *(TMath::Sin(fi0) - TMath::Sin(fi0 + phi));   
  //Double_t z = z0 - rad*lam*(phi)*TMath::Sign(-1.,phi); 
  
  Double_t px = -Q*alpha*rad*TMath::Sin(fi0 + phi);
  Double_t py =  Q*alpha*rad*TMath::Cos(fi0 + phi);
  // Double_t pz =  Q*alpha*rad*lam*TMath::Sign(-1.,phi);
  Double_t pz =  -alpha*rad*lam;

  vertex->SetX(x);
  vertex->SetY(y);
  vertex->SetZ(z);
  
  mom->SetX(px);
  mom->SetY(py);
  mom->SetZ(pz);
  
  return phi;
}

//______________________________________________________________
Float_t PndTpcLheTrack ::ExtrapolateToR(TVector3 *mom, TVector3 *vertex, const Float_t R) {
  
  if (TMath::Tan(GetMomentum().Theta()==0)) {
    cout << "-W-  PndTpcLheTrack ::ExtrapolateToR: theta==0 - skipped track" << endl;
    return -100000;
  }
  
  Float_t zproj = R / TMath::Tan(GetMomentum().Theta()) + GetZ0();

  return ExtrapolateToZ(mom, vertex, zproj);
}

//______________________________________________________________
TrackCand* PndTpcLheTrack::GetTrackCand()
{
  Int_t fVerbose = 0;
  TrackCand *trackCand = new TrackCand();
  TObjArray* lheList = GetRHits();
  if (fVerbose) cout << lheList->GetEntriesFast() << " " << GetNumberOfHits() << endl;
  
  map<Float_t, Int_t> fPointList;
  Int_t hit_counter = 0;
  vector<Float_t> hit_dist;
  for (Int_t lh=0; lh < lheList->GetEntriesFast(); lh++)
    {
      PndLheHit* lhit = (PndLheHit*)lheList->At(lh);
      if (NULL==lhit) break; 

      if (fVerbose) cout << "before:\t" << lhit->GetX() << "\t" << lhit->GetY() << "\t" <<lhit->GetZ() << endl;
      hit_dist.push_back(TMath::Sqrt(lhit->GetX()*lhit->GetX()+lhit->GetY()*lhit->GetY()+lhit->GetZ()*lhit->GetZ()));
      fPointList[TMath::Sqrt(lhit->GetX()*lhit->GetX()+lhit->GetY()*lhit->GetY()+lhit->GetZ()*lhit->GetZ())] = lh;
    }
  
  sort(hit_dist.begin(), hit_dist.end());
  
  Int_t detId = 0;
  for (Int_t ii=0; ii< hit_dist.size(); ++ii)
    {
      PndLheHit* lhit = (PndLheHit*)lheList->At(fPointList[hit_dist[ii]]);
      if (fVerbose) cout << "after:\t" << lhit->GetX() << "\t" << lhit->GetY() << "\t" <<lhit->GetZ() << endl;
      trackCand->addHit(lhit->GetDetectorID(), lhit->GetRefIndex());
      if (GetRadius()>0.)  
	{
	  trackCand->setCurv(1./GetRadius());
	}
      else
	{
	  return 0;
	}
      
      trackCand->setDip(TMath::ATan(GetTanDipAngle()));
      trackCand->setInverted(true);
    }
  hit_dist.clear();
  
  return trackCand;
   
}

//___________________________________________________________
void PndTpcLheTrack ::SetDefaults() {
  // Default setup for the track.

  //  fRealHits = new TRefArray();
  fRealHits = new TObjArray(0);

  fTrackNumber = -1;

  SetPid(0);

  ComesFromMainVertex(kFALSE);

  fP.SetX(0.);
  fP.SetY(0.);
  fP.SetZ(0.);

  fVertex.SetX(0.);
  fVertex.SetY(0.);
  fVertex.SetZ(0.);
  
  fLastHit.SetX(0.);
  fLastHit.SetY(0.);
  fLastHit.SetZ(0.);

  fFirstHit.SetX(0.);
  fFirstHit.SetY(0.);
  fFirstHit.SetZ(0.);

  fQ = 0;
  fZ0 = 0.;
  fZ0Err = 0.;
  fDipChi2 = 0.;
  fTanDipAngle = 0.;
  fTanDipAngleErr = 0.;
  fMvdHits = 0;
  fTpcHits = 0;
  fGemHits = 0;
  fFitTrackIndex = -1;
  fGood = kTRUE;

  fCorrTrackIds.Set(0);
  fMultTrackIds.Set(0);

}

//______________________________________________________________
void PndTpcLheTrack ::SetTrackNumber(Int_t number) {

  // Sets the tracknumber for track.
  fTrackNumber = number;
}

//______________________________________________________________
void PndTpcLheTrack::SetVertex(Double_t vx, Double_t vy, Double_t vz) {

   fVertex.SetX(vx);
   fVertex.SetY(vy);
   fVertex.SetZ(vz);
}

//______________________________________________________________
void PndTpcLheTrack::SetLastHit(Double_t vx, Double_t vy, Double_t vz) {

   fLastHit.SetX(vx);
   fLastHit.SetY(vy);
   fLastHit.SetZ(vz);
}

//______________________________________________________________
void PndTpcLheTrack::SetFirstHit(Double_t vx, Double_t vy, Double_t vz) {

   fFirstHit.SetX(vx);
   fFirstHit.SetY(vy);
   fFirstHit.SetZ(vz);
}
//______________________________________________________________
void PndTpcLheTrack::SetCircle(Double_t x, Double_t y, Double_t r) {

   fCircle.SetX(x);
   fCircle.SetY(y);
   fCircle.SetZ(r);
}

//______________________________________________________________
 void PndTpcLheTrack::AddHit(PndLheHit* point) {
  // Adds a hit to the track.

  fRealHits->AddLast(point);
  //  point->SetUsage(kTRUE);
}

//______________________________________________________________
void PndTpcLheTrack::PrintHits() {
  //

    Int_t nhit = fRealHits->GetEntriesFast();

    PndLheHit * ghit = NULL;

    for (Int_t j=0; j < nhit; j++) {
      ghit = (PndLheHit *) fRealHits->At(j);
      cout << " " << ghit->GetTrackID();
    }
    cout << endl;

}

//______________________________________________________________
void PndTpcLheTrack::Print() {
  //

    Int_t nhit = fRealHits->GetEntriesFast();

    cout << "\n Track " << fTrackNumber << "  nhits " << nhit;
    cout << " pdg "  <<  fPid;
    cout << " primary "  << fFromMainVertex << endl;

    cout << " vertex XYZ: " << setw(5) << fVertex.GetX();
    cout << " " << setw(5) << fVertex.GetY();
    cout << " " << setw(5) << fVertex.GetZ();

    cout << " Pxyz: ";
    cout.precision(3);
    cout << " " << setw(7) <<  fP.X();
    cout << " " << setw(7) <<  fP.Y();
    cout << " " << setw(7) <<  fP.Z();
    cout << endl;

    PndLheHit * ghit = NULL;

    for (Int_t j=0; j<nhit; j++) {
      ghit = (PndLheHit *) fRealHits->At(j);
      ghit->Print();
    }

}

