#include "PndPidCorrelator.h"
#include "PndSciTHit.h"
#include "PndSciTPoint.h"

#include <cmath>

//_________________________________________________________________
Bool_t PndPidCorrelator::GetTofInfo(FairTrackParH* helix, PndPidCandidate* pidCand) {
  
  if (!fIdeal)
    {
      if ((helix->GetMomentum().Theta()*TMath::RadToDeg())<20.) return kFALSE; 
      if ((helix->GetMomentum().Theta()*TMath::RadToDeg())>150.) return kFALSE;
    }
  FairGeanePro *fProTof = new FairGeanePro();
  if (!fCorrErrorProp) fProTof->PropagateOnlyParameters(); 
  FairGeanePro *fProVertex = new FairGeanePro();
  if (!fCorrErrorProp) fProVertex->PropagateOnlyParameters();
  //---
  PndSciTHit *tofHit = NULL; 
  Int_t tofEntries = fTofHit->GetEntriesFast();
  Int_t tofIndex = -1;
  Float_t tofTof = 0., tofLength = -1000, tofGLength = -1000, tofLengthTemp = -1000;
  Float_t tofQuality = 1000000;
  
  Float_t chi2 = 0;
  TVector3 vertex(0., 0., 0.);
  TVector3 tofPos(0., 0., 0.);
  TVector3 momentum(0., 0., 0.);
  for (Int_t tt = 0; tt<tofEntries; tt++)
    {
      tofHit = (PndSciTHit*)fTofHit->At(tt);
      if ( fIdeal && ( ((PndSciTPoint*)fTofPoint->At(tofHit->GetRefIndex()))->GetTrackID() !=pidCand->GetMcIndex()) ) continue;
      tofHit->Position(tofPos);
    
      if (fGeanePro) // Overwrites vertex if Geane is used
	{ 
     
	  fProTof->SetPoint(tofPos);
	  fProTof->PropagateToPCA(1, 1);
	  FairTrackParH *fRes= new FairTrackParH();
	  Bool_t rc =  fProTof->Propagate(helix, fRes, fPidHyp*pidCand->GetCharge());	
	  if (!rc) continue;
      
	  vertex.SetXYZ(fRes->GetX(), fRes->GetY(), fRes->GetZ());
     
	  fProVertex->SetPoint(TVector3(0,0,0));
	  fProVertex->PropagateToPCA(1, -1);
	  FairTrackParH *fRes2= new FairTrackParH();
	  Bool_t rc2 =  fProVertex->Propagate(fRes, fRes2, fPidHyp*pidCand->GetCharge());
	  if (rc2) tofLengthTemp = fProVertex->GetLengthAtPCA();
	}
    
      Float_t dist = (tofPos-vertex).Mag2();
    
      if ( tofQuality > dist)
	{
	  tofIndex = tt;
	  tofQuality = dist;
	  tofTof = tofHit->GetTime();
	  tofLength = tofLengthTemp; // abs(phi * track->GetRadius() / TMath::Sin(track->GetMomentum().Theta()));
	}
      if (fDebugMode)
	{
	  Float_t ntuple[] = {vertex.X(), vertex.Y(), vertex.Z(), vertex.Phi(),
			      helix->GetMomentum().Mag(), helix->GetQ(), helix->GetMomentum().Theta(), helix->GetZ(),
			      tofPos.X(), tofPos.Y(), tofPos.Z(), tofPos.Phi(),
			      dist, vertex.DeltaPhi(tofPos), tofLength, tofGLength};
	  tofCorr->Fill(ntuple);
	}
    }
  
  if ( (tofQuality<fCorrPar->GetTofCut()) || (fIdeal && tofIndex!=-1) )
    {
      pidCand->SetTofQuality(tofQuality);
      pidCand->SetTofStopTime(tofTof);
      pidCand->SetTofTrackLength(tofLength);
      pidCand->SetTofIndex(tofIndex);
      if (tofLength>0.)
	{
	  // mass^2 = p^2 * ( 1/beta^2 - 1 )
	  Float_t mass2 = helix->GetMomentum().Mag()*helix->GetMomentum().Mag()*(30.*30.*tofTof*tofTof/tofLength/tofLength-1.);
	  pidCand->SetTofM2(mass2);
	}
    }
  
  return kTRUE;
}

ClassImp(PndPidCorrelator)
