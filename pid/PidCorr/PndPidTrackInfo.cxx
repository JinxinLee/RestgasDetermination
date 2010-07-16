#include "PndPidCorrelator.h"
#include "PndPidCandidate.h"
#include "PndTrack.h"
#include "FairTrackParH.h"
#include "Fitter/PndVtxFitterParticle.h"

#include "TVector3.h"
#include "TDatabasePDG.h"
#include <cmath>

//_________________________________________________________________
Bool_t PndPidCorrelator::GetTrackInfo(PndTrack* track, PndPidCandidate* pidCand) 
{
  static PndVtxFitterParticle covTool;
  
  Int_t charge =   TMath::Sign(1, track->GetParamFirst().GetQ());
  pidCand->SetCharge(charge);
     
  TVector3 first(track->GetParamFirst().GetX(),
		 track->GetParamFirst().GetY(),
		 track->GetParamFirst().GetZ());
  TVector3 last(track->GetParamLast().GetX(),
		track->GetParamLast().GetY(),
		track->GetParamLast().GetZ());
  FairTrackParP par = track->GetParamFirst();
  Int_t ierr = 0;
  FairTrackParH *helix = new FairTrackParH(&par, ierr);
  
  if (fGeanePro) // Overwrites vertex if Geane is used
    { 
      TVector3 momentum, vertex;
      FairGeanePro *fPro0 = new FairGeanePro();
      FairTrackParH *fRes= new FairTrackParH();
      fPro0->SetPoint(TVector3(0,0,0));
      fPro0->PropagateToPCA(1, -1);
      Bool_t rc =  fPro0->Propagate(helix, fRes, fPidHyp*charge);	
      if (!rc)
	{
	  std::cout << "-W- PndPidCorrelator::GetTrackInfo :: Failed backward propagation" << std::endl;
	  if (fVerbose>0) helix->Print();
	  return kFALSE;
	}
      
      vertex.SetXYZ(fRes->GetX(), fRes->GetY(), fRes->GetZ());
      momentum = fRes->GetMomentum();
      FairTrackParP *fParab = new FairTrackParP(fRes, TVector3(1.,0.,0.), TVector3(0.,1.,0.), ierr);
      Double_t globalCov[6][6];
      fParab->GetMARSCov(globalCov);
      
      Int_t ii,jj;
      TMatrixD err(6,6);
      for (ii=0;ii<6;ii++) for(jj=0;jj<6;jj++) err[ii][jj]=globalCov[ii][jj];

      TLorentzVector lv;
      lv.SetVectM(momentum, TDatabasePDG::Instance()->GetParticle(fPidHyp)->Mass()); // set mass hypothesis
      Float_t energy = lv.M2();
      TMatrixD mat = covTool.GetConverted7(covTool.GetFitError(lv, err));
      
      pidCand->SetPosition(vertex);
      pidCand->SetMomentum(momentum);
      pidCand->SetEnergy(energy);
      pidCand->SetCov7(mat);
    }
  else
    {
      std::cout << std::endl << "-E- PndPidCorrelator::GetTrackInfo :: NO GEANE - no propagation available" << std::endl;
    }
  
  pidCand->SetFirstHit(first);
  pidCand->SetLastHit(last);
  pidCand->SetDegreesOfFreedom(track->GetNDF());
  pidCand->SetFitStatus(track->GetFlag());
  pidCand->SetChiSquared(track->GetChi2());
  
  return kTRUE;
}

ClassImp(PndPidCorrelator)
