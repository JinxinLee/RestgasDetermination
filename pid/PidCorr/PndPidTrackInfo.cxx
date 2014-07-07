#include "PndPidCorrelator.h"
#include "PndPidCandidate.h"
#include "PndTrack.h"
#include "FairTrackParH.h"
#include "PndAnaCovTool.h"
#include "FairRun.h"
#include "FairRunSim.h"
#include "FairRunAna.h"
#include "FairField.h"

#include "TVector3.h"
#include "TDatabasePDG.h"
#include <cmath>

//_________________________________________________________________
Bool_t PndPidCorrelator::GetTrackInfo(PndTrack* track, PndPidCandidate* pidCand)
{
  static PndAnaCovTool covTool;

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
  FairTrackParH *fRes;
  TVector3 momentum, startpos;

  if (fGeanePro && fBackPropagate) // Overwrites vertex if Geane is used and if backpropagation
    {
    FairTrackParH *helix = new FairTrackParH(&par, ierr);
    FairGeanePro *fPro0 = new FairGeanePro();
    fRes= new FairTrackParH();

    // track back to Origin
    // [ralfk: changed to propagate to z axis - 03/2011]
    //fPro0->SetPoint(TVector3(0,0,0));
    //fPro0->PropagateToPCA(1, -1);
    // Propagatetrack back to z Axis
    fPro0->PropagateToPCA(2, -1);// track back to z axis
    TVector3 ex1(0.,0.,-50.); // virtual wire, dimensions chosen arbitrarily
    TVector3 ex2(0.,0.,100.); // we expect fast decaying tracks to be close to that
    fPro0->SetWire(ex1,ex2);

    Bool_t rc =  fPro0->Propagate(helix, fRes, fPidHyp*charge);
    if (!rc)
    {
      std::cout << "-W- PndPidCorrelator::GetTrackInfo :: Failed backward propagation" << std::endl;
      if (fVerbose>0) helix->Print();
      return kFALSE;
    }
  }
  else
    {
      // If no backpropagation, use the first params
      fRes = new FairTrackParH(&par, ierr);
    }
  startpos.SetXYZ(fRes->GetX(), fRes->GetY(), fRes->GetZ()); //cm
  momentum = fRes->GetMomentum();

  // ********  this block replaces
  TVector3 di = momentum;
  di.SetMag(1.);
  TVector3 dj = di.Orthogonal();
  TVector3 dk = di.Cross(dj);
  FairTrackParP *fParab = new FairTrackParP(fRes, dj, dk, ierr);
  // ******* that line (recipe by Lia)
  //FairTrackParP *fParab = new FairTrackParP(fRes, TVector3(1.,0.,0.), TVector3(0.,1.,0.), ierr);
  // ******* that line (recipe by Lia)

  Double_t globalCov[6][6];
  fParab->GetMARSCov(globalCov);

  Int_t ii,jj;
  TMatrixD err(6,6);
  for (ii=0;ii<6;ii++) for(jj=0;jj<6;jj++) err[ii][jj]=globalCov[ii][jj];

  //err.Print();

  TLorentzVector lv;
  lv.SetVectM(momentum, TDatabasePDG::Instance()->GetParticle(fPidHyp)->Mass()); // set mass hypothesis
  Float_t energy = lv.E();
  TMatrixD mat = covTool.GetConverted7(covTool.GetFitError(lv, err));

  pidCand->SetPosition(startpos);
  pidCand->SetMomentum(momentum);
  pidCand->SetEnergy(energy);
  pidCand->SetCov7(mat);

  // Adding the helix parameters to the candidate (TFitParams)
  // It is nice to know at analysis stage
  //rho helix: (D0,Phi0,rho(omega),Z0,tan(dip))
  //fair helix: (q/p,lambda, phi, y_perp, z_perp)
  Double_t Q=fRes->GetQ();
  //if(0==Q) ??? break/return?;
  Double_t pnt[3], Bf[3];
  pnt[0]=startpos.X();
  pnt[1]=startpos.Y();
  pnt[2]=startpos.Z();
  if(FairRun::Instance()->IsAna()){
    FairRunAna::Instance()->GetField()->GetFieldValue(pnt, Bf); //[kGs]
  }else{
    FairRunSim::Instance()->GetField()->GetFieldValue(pnt, Bf); //[kGs]
  }
  //Double_t B = sqrt(Bf[0]*Bf[0]+Bf[1]*Bf[1]+Bf[2]*Bf[2]);
  Double_t B = Bf[2];
  Double_t qBc = -0.000299792458*B*Q;
  Double_t icL = 1. / cos(fRes->GetLambda()); // inverted for practical reasons (better to multiply than to divide)
  Double_t icLs = icL*icL;
  Double_t helixparams[5];
  //helixparams[0]=startpos.Perp(); //D0
  helixparams[0]=fRes->GetY_sc() ; //D0
  //helixparams[1]=fRes->GetPhi(); //phi0
  helixparams[1]=fRes->GetMomentum().Phi(); //phi0
  helixparams[2]=qBc/(fRes->GetMomentum().Perp()); //omega=rho=1/R[cm]=-2.998e-4*B[kGs]*Q[e]/p_perp[GeV/c]
  //helixparams[3]=startpos.Z(); //z0
  helixparams[3]=fRes->GetZ_sc()*icL; //z0
  helixparams[4]=tan(fRes->GetLambda()); //lambda(averey)=cot(theta)=tan(lambda(geane))
  pidCand->SetHelixParams(helixparams);
  Double_t fairhelixcov[15];
  fRes->GetCov(fairhelixcov);
  Double_t rhohelixcov[15];
  // in the poca to z axis yperp=D0, x_perp^2+z_perp^2 = z_perp/cos(Lambda)= Z0
  rhohelixcov[0]  = fairhelixcov[12];                    // sigma^2 D0
  rhohelixcov[1]  = fairhelixcov[10];                    // cov D0 - Phi0
  rhohelixcov[2]  = fairhelixcov[3]  * qBc * icL;        // cov D0 - rho
  rhohelixcov[3]  = fairhelixcov[13] * icL;              // cov D0 - Z0
  rhohelixcov[4]  = fairhelixcov[7]  * icLs;             // cov D0 - tan(dip)
  rhohelixcov[5]  = fairhelixcov[9];                     // sigma^2 Phi0
  rhohelixcov[6]  = fairhelixcov[2]  * qBc * icL;        // cov Phi0 - rho
  rhohelixcov[7]  = fairhelixcov[11] * icL;              // cov Phi0 - Z0
  rhohelixcov[8]  = fairhelixcov[6]  * icLs;             // cov Phi0 - tan(dip)
  rhohelixcov[9]  = fairhelixcov[0]  * qBc * qBc * icLs; // sigma^2 rho
  rhohelixcov[10] = fairhelixcov[4]  * qBc * icLs;       // cov rho - Z0
  rhohelixcov[11] = fairhelixcov[1]  * qBc * icL * icLs; // cov rho - tan(dip)
  rhohelixcov[12] = fairhelixcov[14] * icLs;             // sigma^2 Z0
  rhohelixcov[13] = fairhelixcov[8]  * icL * icLs;       //cov Z0 - tan(dip)
  rhohelixcov[14] = fairhelixcov[5]  * icLs * icLs;      // sigma^2 tan(dip) - from
  pidCand->SetHelixCov(rhohelixcov);

  pidCand->SetFirstHit(first);
  pidCand->SetLastHit(last);
  pidCand->SetDegreesOfFreedom(track->GetNDF());
  pidCand->SetFitStatus(track->GetFlag());
  pidCand->SetChiSquared(track->GetChi2());

  return kTRUE;
}

ClassImp(PndPidCorrelator)
