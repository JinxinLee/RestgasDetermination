#include "PndPidCorrelator.h"
#include "PndDskParticle.h"

#include <cmath>
//_________________________________________________________________
Bool_t PndPidCorrelator::GetDskInfo(FairTrackParH* helix, PndPidCandidate* pidCand) {
  if (helix->GetZ()<165.) return kFALSE; // consider tracks only from last gem plane
  
  FairGeanePro *fProDsk = new FairGeanePro(); 
  if (!fCorrErrorProp) fProDsk->PropagateOnlyParameters();
  //---
  PndDskParticle *dskParticle = NULL;
  Int_t dskEntries = fDskParticle->GetEntriesFast();
  Int_t dskIndex = -1, dskPhot = 0;
  Float_t dskThetaC = -1000, dskThetaCErr = 0, dskGLength = -1000;
  Float_t dskQuality = 1000000;
  Float_t x_p = -1000;
  
  TVector3 vertex(0., 0., 0.);
  TVector3 dskPos(0., 0., 0.);
  TVector3 momentum(0., 0., 0.);

  if (fGeanePro) // Overwrites vertex if Geane is used
    {     
      fProDsk->PropagateToVolume("Plate",0,1);
      vertex.SetXYZ(-10000, -10000, -10000); // reset vertex
      FairTrackParH *fRes= new FairTrackParH();
      Bool_t rc =  fProDsk->Propagate(helix, fRes, fPidHyp*pidCand->GetCharge());
      if (!rc) return kFALSE;
      vertex.SetXYZ(fRes->GetX(), fRes->GetY(), fRes->GetZ());
      dskGLength = fProDsk->GetLengthAtPCA();
      x_p = fRes->GetMomentum().Mag();
    }
  
  for (Int_t dd = 0; dd<dskEntries; dd++)
    {
      dskParticle = (PndDskParticle*)fDskParticle->At(dd);
      if ( fIdeal && (dskParticle->GetTrackID() !=pidCand->GetMcIndex()) ) continue;
      dskParticle->Position(dskPos);
      
      Float_t dist = (vertex-dskPos).Mag2();    
      if ( dskQuality > dist)
	{
	  dskIndex = dd;
	  dskQuality = dist;
	  dskThetaC = dskParticle->GetThetaC();
	  //dskThetaCErr = dskParticle->GetErrThetaC();
	  dskPhot = 0; // ** to be filled **
	}
      if (fDebugMode)
	{
	  Float_t ntuple[] = {vertex.X(), vertex.Y(), vertex.Z(), vertex.Phi(),
			      helix->GetMomentum().Mag(), helix->GetQ(), helix->GetMomentum().Theta(), helix->GetZ(),
			      dskPos.X(), dskPos.Y(), dskPos.Z(), dskPos.Phi(), dist, dskParticle->GetThetaC(), 0., vertex.DeltaPhi(dskPos), dskGLength,
			      helix->GetX(), helix->GetY(), helix->GetZ(), x_p, pidCand->GetFitStatus()
};
	  dskCorr->Fill(ntuple);
	}
    }
  
  if ((dskQuality<fCorrPar->GetDskCut()) || (fIdeal && dskIndex!=-1))
    {
      pidCand->SetDiscQuality(dskQuality);
      pidCand->SetDiscThetaC(dskThetaC);
      //pidCand->SetDskThetaCErr(dskThetaCErr);
      pidCand->SetDiscNumberOfPhotons(dskPhot);
      pidCand->SetDiscIndex(dskIndex);
    }
  return kTRUE;
}


ClassImp(PndPidCorrelator)
