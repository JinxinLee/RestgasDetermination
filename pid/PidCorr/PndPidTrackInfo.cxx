#include "PndPidCorrelator.h"
#include "PndPidCandidate.h"
#include "PndTrack.h"
#include "FairTrackParH.h"
#include "Fitter/PndVtxFitterParticle.h"
#include "FairRunAna.h"
#include "FairField.h"

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
    TVector3 momentum, pocaz;
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
    
    pocaz.SetXYZ(fRes->GetX(), fRes->GetY(), fRes->GetZ()); //cm ??
    momentum = fRes->GetMomentum();
    FairTrackParP *fParab = new FairTrackParP(fRes, TVector3(1.,0.,0.), TVector3(0.,1.,0.), ierr);
    Double_t globalCov[6][6];
    fParab->GetMARSCov(globalCov);
    
    Int_t ii,jj;
    TMatrixD err(6,6);
    for (ii=0;ii<6;ii++) for(jj=0;jj<6;jj++) err[ii][jj]=globalCov[ii][jj];
    
    TLorentzVector lv;
    lv.SetVectM(momentum, TDatabasePDG::Instance()->GetParticle(fPidHyp)->Mass()); // set mass hypothesis
    Float_t energy = lv.E();
    TMatrixD mat = covTool.GetConverted7(covTool.GetFitError(lv, err));
    
    pidCand->SetPosition(pocaz);
    pidCand->SetMomentum(momentum);
    pidCand->SetEnergy(energy);
    pidCand->SetCov7(mat);
    
    // Adding the helix parameters to the candidate (TFitParams)
    // It is nice to know at analysis stage
    Double_t pnt[3], Bf[3];
    pnt[0]=pocaz.X();
    pnt[1]=pocaz.Y();
    pnt[2]=pocaz.Z(); 
    FairRunAna::Instance()->GetField()->GetFieldValue(pnt, Bf); //[kGs]
    Double_t B = Bf[0]*Bf[0]+Bf[1]*Bf[1]+Bf[2]*Bf[2];
    Double_t helixparams[5];
    helixparams[0]=pocaz.Perp(); //D0
    helixparams[1]=fRes->GetPhi(); //phi0
    helixparams[2]=-0.299792458*B*charge/momentum.Perp(); //omega=rho=1/R[cm]=-2.998*B[kGs]*Q[e]/p_perp[GeV/c] 
    helixparams[3]=pocaz.Z(); //z0
    helixparams[4]=1/tan(fRes->GetLambda()); //lambda(fair)=theta; lambda(averey)=cot(theta)
    pidCand->SetHelixParams(helixparams);    
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
