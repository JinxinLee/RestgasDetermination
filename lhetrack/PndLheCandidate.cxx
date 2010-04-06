#include "PndLheHit.h"
#include "PndLheCandidate.h"
#include "PndTrackCand.h"
#include "TClonesArray.h"

#include "Riostream.h"
#include <iomanip>
#include <map>

//using std::map;
using std::multimap;
using std::sort;


////////////////////////////////////////////////////////////////////////
//                                                                    //
// PndLheCandidate  class - track representation for the LHE              //
//                                                                    //
////////////////////////////////////////////////////////////////////////

ClassImp(PndLheCandidate)

//______________________________________________________________
PndLheCandidate::PndLheCandidate () {
  // Default constructor.

  SetDefaults();
}

//______________________________________________________________
PndLheCandidate::PndLheCandidate (Int_t tracknumber) {
  // the track number is set.
  
  //  cout << " PndLheCandidate (tracknumber)" << endl;

  SetDefaults();
  SetTrackNumber(tracknumber);
}

//______________________________________________________________
PndLheCandidate ::~PndLheCandidate () {
  // Destructor.

  //  cout << " Destructor for LheTrack" << endl;
  if (fRealHits) { //fRealHits->Delete();} //;
    delete fRealHits;
    fRealHits = NULL;
  }

}

//______________________________________________________________
Float_t PndLheCandidate ::ExtrapolateToZ(TVector3 *mom, TVector3 *vertex, const Float_t z) {
  // Function to extrapolate the momentum and vertex coordinates to a well determined z value
  
  // alpha = .2998 * magfield / 100 <- for obtain moment in GeV/c
  Double_t alpha = .2998 * .02 ;
  Double_t Q = double(TMath::Sign(1, GetCharge()));
  
  PndLhePoint hel = GetCircle();
  
  Double_t xc = hel.GetX();
  Double_t yc = hel.GetY();
  Double_t rad = hel.GetZ();
  
  TVector2 centre(xc, yc);
  Double_t fi0 = centre.Phi();    // 0 - 2Pi
  Double_t d0 = Q*(TMath::Sqrt(xc*xc + yc*yc) - hel.GetZ());
  
  TVector2 pivot(-xc, -yc);
  
  Double_t lam = GetTanDipAngle();
  if (lam==0) {
    cout << "-W-  PndLheCandidate ::ExtrapolateToZ: lam==0 - skipped track" << endl;
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
Float_t PndLheCandidate ::ExtrapolateToR(TVector3 *mom, TVector3 *vertex, const Float_t R) {
  
  if (TMath::Tan(GetMomentum().Theta()==0)) {
    cout << "-W-  PndLheCandidate ::ExtrapolateToR: theta==0 - skipped track" << endl;
    return -100000;
  }
  
  Float_t zproj = R / TMath::Tan(GetMomentum().Theta()) + GetZ0();

  return ExtrapolateToZ(mom, vertex, zproj);
}

//______________________________________________________________
void PndLheCandidate::SortHits()
{
  Int_t fVerbose = 0;
  TObjArray* lheList = GetRHits();
  if (fVerbose) cout << lheList->GetEntriesFast() << " " << GetNumberOfHits() << endl;
  
  multimap<Float_t, Int_t> fPointList; // hdistance, hit index
  multimap<Float_t, Int_t>::iterator iter;
  for (Int_t lh=0; lh < lheList->GetEntriesFast(); lh++)
    {
      PndLheHit* lhit = (PndLheHit*)lheList->At(lh);
      if (NULL==lhit) break; 

      if (fVerbose) cout << "before:\t" << lhit->GetX() << "\t" << lhit->GetY() << "\t" <<lhit->GetZ() << "\t" << TMath::Sqrt(lhit->GetX()*lhit->GetX()+lhit->GetY()*lhit->GetY()+lhit->GetZ()*lhit->GetZ()) << endl;
      fPointList.insert(pair<Float_t, Int_t>(TMath::Sqrt(lhit->GetX()*lhit->GetX()+lhit->GetY()*lhit->GetY()+lhit->GetZ()*lhit->GetZ()),lh));
    }
 
  TObjArray  *fNewHits = new TObjArray(0);
  for (iter=fPointList.begin(); iter!=fPointList.end(); ++iter)
    {
      PndLheHit* lhit = (PndLheHit*)lheList->At((*iter).second);
      if (fVerbose) cout << "after:\t" << lhit->GetX() << "\t" << lhit->GetY() << "\t" <<lhit->GetZ() << "\t" << TMath::Sqrt(lhit->GetX()*lhit->GetX()+lhit->GetY()*lhit->GetY()+lhit->GetZ()*lhit->GetZ()) << endl;
      fNewHits->AddLast(lhit);
    }
    
  fRealHits = fNewHits;
  return;
}

//______________________________________________________________
PndTrackCand* PndLheCandidate::GetTrackCand()
{
  PndTrackCand *trackCand = new PndTrackCand();
  TObjArray* lheList = GetRHits();
  
  Int_t detId = 0;
  for (Int_t ii=0; ii< lheList->GetEntriesFast(); ++ii)
    {
      PndLheHit* hit = (PndLheHit*)lheList->At(ii);
      if (NULL==hit) break;
      trackCand->AddHit(hit->GetDetectorID(), hit->GetRefIndex(), hit->GetX()*hit->GetX()+hit->GetY()*hit->GetY()+hit->GetZ()*hit->GetZ());
    }

  trackCand->Sort();
  return trackCand;
}

//___________________________________________________________
void PndLheCandidate ::SetDefaults() {
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
  fGood = kTRUE;

  fCorrTrackIds.Set(0);
  fMultTrackIds.Set(0);

}

//______________________________________________________________
void PndLheCandidate ::SetTrackNumber(Int_t number) {

  // Sets the tracknumber for track.
  fTrackNumber = number;
}

//______________________________________________________________
void PndLheCandidate::SetVertex(Double_t vx, Double_t vy, Double_t vz) {

   fVertex.SetX(vx);
   fVertex.SetY(vy);
   fVertex.SetZ(vz);
}

//______________________________________________________________
void PndLheCandidate::SetLastHit(Double_t vx, Double_t vy, Double_t vz) {

   fLastHit.SetX(vx);
   fLastHit.SetY(vy);
   fLastHit.SetZ(vz);
}

//______________________________________________________________
void PndLheCandidate::SetFirstHit(Double_t vx, Double_t vy, Double_t vz) {

   fFirstHit.SetX(vx);
   fFirstHit.SetY(vy);
   fFirstHit.SetZ(vz);
}
//______________________________________________________________
void PndLheCandidate::SetCircle(Double_t x, Double_t y, Double_t r) {

   fCircle.SetX(x);
   fCircle.SetY(y);
   fCircle.SetZ(r);
}

//______________________________________________________________
 void PndLheCandidate::AddHit(PndLheHit* point) {
  // Adds a hit to the track.

  fRealHits->AddLast(point);
  AddLink(point->GetLink(0));
  //  point->SetUsage(kTRUE);
}

//______________________________________________________________
void PndLheCandidate::PrintHits() {
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
void PndLheCandidate::Print() {
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

