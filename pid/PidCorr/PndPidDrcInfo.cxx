#include "PndPidCorrelator.h"
#include "PndDrcHit.h"
#include "PndDrcBarPoint.h"

#include <cmath>

//_________________________________________________________________
Bool_t PndPidCorrelator::GetDrcInfo(FairTrackParH* helix, PndPidCandidate* pidCand) {
  if (helix->GetZ()>120.) return kFALSE; // cut fwd endcap tracks
  FairGeanePro *fProDrc = new FairGeanePro();
  if (!fCorrErrorProp) fProDrc->PropagateOnlyParameters();
  //---
  PndDrcHit *drcHit = NULL;
  Int_t drcEntries = fDrcHit->GetEntriesFast();
  Int_t drcIndex = -1, drcPhot = 0;
  Float_t drcThetaC = -1000, drcThetaCErr = 0, drcGLength = -1000;
  Float_t drcQuality = 1000000;
  
  TVector3 vertex(0., 0., 0.);
  Float_t vertex_z = -1000;
  TVector3 drcPos(0., 0., 0.);
  TVector3 momentum(0., 0., 0.);

  if (fGeanePro) // Overwrites vertex if Geane is used
    {     
      fProDrc->PropagateToVolume("DrcBarSensor",0,1);
//      fProDrc->PropagateToVolume("BarrelDIRC",0,1);
      vertex.SetXYZ(-10000, -10000, -10000); // reset vertex
      FairTrackParH *fRes= new FairTrackParH();
      Bool_t rc =  fProDrc->Propagate(helix, fRes, fPidHyp*pidCand->GetCharge()); 	
      if (!rc) return kFALSE;
      vertex.SetXYZ(fRes->GetX(), fRes->GetY(), 0.);
      vertex_z = fRes->GetZ();
      drcGLength = fProDrc->GetLengthAtPCA();
      if (drcGLength>30.) return kFALSE;  // additional cut on extrapolation distance to avoid fake correlations
    }
  for (Int_t dd = 0; dd<drcEntries; dd++)
    {
      drcHit = (PndDrcHit*)fDrcHit->At(dd); 
      if ( fIdeal && ( ((PndDrcBarPoint*)fDrcPoint->At(drcHit->GetRefIndex()))->GetTrackID() !=pidCand->GetMcIndex()) ) continue;
      drcHit->Position(drcPos);
      
      Float_t dphi = vertex.DeltaPhi(drcPos);
      Float_t dist = dphi * dphi;
      if (drcQuality > dist)
	{
	  drcIndex = dd;
	  drcQuality = dist;
	  drcThetaC = drcHit->GetThetaC();
	  drcThetaCErr = drcHit->GetErrThetaC();
	  drcPhot = 0; // ** to be filled **
	}
      if (fDebugMode)
	{
	  Float_t ntuple[] = {static_cast<Float_t>(vertex.X()), static_cast<Float_t>(vertex.Y()), static_cast<Float_t>(vertex.Z()), static_cast<Float_t>(vertex.Phi()), 
			      static_cast<Float_t>(helix->GetMomentum().Mag()), static_cast<Float_t>(helix->GetQ()), static_cast<Float_t>(helix->GetMomentum().Theta()), static_cast<Float_t>(helix->GetZ()),
			      static_cast<Float_t>(drcPos.X()), static_cast<Float_t>(drcPos.Y()), static_cast<Float_t>(drcPos.Phi()),
			      dist, static_cast<Float_t>(drcHit->GetThetaC()), 0., static_cast<Float_t>(vertex.DeltaPhi(drcPos)), drcGLength, 
			      static_cast<Float_t>(pidCand->GetFitStatus())
	  };
	  // Float_t ntuple[] = {vertex.X(), vertex.Y(), vertex_z, vertex.Phi(),  
	  // 		      helix->GetMomentum().Mag(), helix->GetQ(), helix->GetMomentum().Theta(), helix->GetZ(),
	  // 		      drcPos.X(), drcPos.Y(), drcPos.Phi(), 
	  // 		      dist, drcHit->GetThetaC(), 0., vertex.DeltaPhi(drcPos), drcGLength,
	  // 		      pidCand->GetFitStatus()
	  // };
	  drcCorr->Fill(ntuple);
	}
    }
  if ((drcQuality<fCorrPar->GetDrcCut()) || (fIdeal && drcIndex!=-1))
    {
      pidCand->SetDrcQuality(drcQuality);
      pidCand->SetDrcThetaC(drcThetaC);
      pidCand->SetDrcThetaCErr(drcThetaCErr);
      pidCand->SetDrcNumberOfPhotons(drcPhot);
      pidCand->SetDrcIndex(drcIndex);
    }
  return kTRUE;
}

ClassImp(PndPidCorrelator)
