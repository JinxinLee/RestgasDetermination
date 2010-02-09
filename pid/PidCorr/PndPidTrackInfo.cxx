#include "PndDetectorList.h"
#include "PndPidCorrelator.h"
#include "PndPidCandidate.h"
#include "PndTrack.h"
#include "PndTrackID.h"

#include "FairTrackParH.h"
#include "FairMCApplication.h"
#include "FairRunAna.h"
#include "FairRootManager.h"
#include "FairRuntimeDb.h"

#include "TObjArray.h"
#include "TVector3.h"
#include "TGeoMatrix.h"
#include "TGeoBBox.h"
#include "TGeoManager.h"

#include <cmath>

#include "PndPidCorrelator.h"

//_________________________________________________________________
Bool_t PndPidCorrelator::GetTrackInfo(PndTrack* track, PndPidCandidate* pidCand) 
{
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
  TVector3 momentum, vertex;
  Float_t energy = TMath::Sqrt(momentum.Mag2()+0.13957*0.13957); // Pion hypothesis
  
  if (fGeanePro) // Overwrites vertex if Geane is used
    {
      FairGeanePro *fPro0 = new FairGeanePro();
      FairTrackParH *fRes= new FairTrackParH();
      fPro0->SetPoint(TVector3(0,0,0));
      fPro0->PropagateToPCA(1, -1);
      Bool_t rc =  fPro0->Propagate(helix, fRes, -13*charge);	
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
      TMatrixD mat(7,7);
      Int_t ii,jj;
      for (ii=0;ii<6;ii++) for(jj=0;jj<6;jj++) mat[ii][jj]=globalCov[ii][jj];
      
      energy = TMath::Sqrt(fParab->GetMomentum().Mag2()+0.13957*0.13957);
      //Extend matrix for energy (with default pion hypothesis) -> Klaus Goetzen
      Double_t invE = 1./(energy);
      mat[0+3][3+3] = mat[3+3][0+3] =
	(fRes->GetX()*mat[0+3][0+3]+fRes->GetY()*mat[0+3][1+3]+fRes->GetZ()*mat[0+3][2+3])*invE;
      mat[1+3][3+3] = mat[3+3][1+3] =
	(fRes->GetX()*mat[0+3][1+3]+fRes->GetY()*mat[1+3][1+3]+fRes->GetZ()*mat[1+3][2+3])*invE;
      mat[2+3][3+3] = mat[3+3][2+3] =
	(fRes->GetX()*mat[0+3][2+3]+fRes->GetY()*mat[1+3][2+3]+fRes->GetZ()*mat[2+3][2+3])*invE;
      mat[3+3][3+3] =
	(fRes->GetX()*fRes->GetX()*mat[0+3][0+3]+fRes->GetY()*fRes->GetY()*mat[1+3][1+3]+fRes->GetZ()*fRes->GetZ()*mat[2+3][2+3]
	 +2.0*fRes->GetX()*fRes->GetY()*mat[0+3][1+3]
	 +2.0*fRes->GetX()*fRes->GetZ()*mat[0+3][2+3]
	 +2.0*fRes->GetY()*fRes->GetZ()*mat[1+3][2+3])*invE*invE;
      
      mat[3+3][4-4] = mat[4-4][3+3] =
	(fRes->GetX()*mat[0+3][4-4]+fRes->GetY()*mat[1+3][4-4]+fRes->GetZ()*mat[2+3][4-4])*invE;
      mat[3+3][5-4] = mat[5-4][3+3] =
	(fRes->GetX()*mat[0+3][5-4]+fRes->GetY()*mat[1+3][5-4]+fRes->GetZ()*mat[2+3][5-4])*invE;
      mat[3+3][6-4] = mat[6-4][3+3] =
	(fRes->GetX()*mat[0+3][6-4]+fRes->GetY()*mat[1+3][6-4]+fRes->GetZ()*mat[2+3][6-4])*invE;
      
      pidCand->SetCov7(mat);
    }
  else
    {
      std::cout << std::endl << "-E- PndPidCorrelator::GetTrackInfo :: NO GEANE - no propagation available" << std::endl;
    }
  
  pidCand->SetPosition(vertex);
  pidCand->SetMomentum(momentum);
  pidCand->SetEnergy(energy);
  pidCand->SetFirstHit(first);
  pidCand->SetLastHit(last);
  //pidCand->SetMcIndex(); // ??? dummy, for the moment
  
  pidCand->SetDegreesOfFreedom(track->GetNDF());
  pidCand->SetFitStatus(track->GetFlag());
  pidCand->SetChiSquared(track->GetChi2());
  
  return kTRUE;
}

ClassImp(PndPidCorrelator)
