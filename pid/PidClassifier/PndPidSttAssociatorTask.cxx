//
// PndPidSttAssociatorTask.cxx
//
// ============================================

#include "PndPidSttAssociatorTask.h"
#include "PndPidCandidate.h"
#include "PndPidProbability.h"

#include "FairRootManager.h"
#include <iostream>
#include <cmath>

using std::cout;
using std::endl;
using std::sqrt;
//___________________________________________________________
PndPidSttAssociatorTask::~PndPidSttAssociatorTask() {
  delete fElectronDEDXMean;
  delete fMuonDEDXMean;
  delete fPionDEDXMean;
  delete fKaonDEDXMean;
  delete fProtonDEDXMeanL;
  delete fProtonDEDXMeanH;
}

//___________________________________________________________
PndPidSttAssociatorTask::PndPidSttAssociatorTask() {
  fPidChargedProb = new TClonesArray("PndPidProbability");
  fDefaultHypo = kTRUE;
}

//___________________________________________________________
PndPidSttAssociatorTask::PndPidSttAssociatorTask(const char *name, const char *title):FairTask(name) 
{
  fPidChargedProb = new TClonesArray("PndPidProbability");
  fDefaultHypo = kTRUE;
}

//___________________________________________________________
InitStatus PndPidSttAssociatorTask::Init() {
  
  if(fVerbose > 0) cout << "PndPidSttAssociatorTask::Init()" << endl;

  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- PndPidSttAssociatorTask::Init: "
	 << "RootManager not instantiated!" << endl;
    return kFATAL;
  }
  
  fPidChargedCand = (TClonesArray *) ioman->GetObject("PidChargedCand");
  if ( ! fPidChargedCand) {
    std::cout << "-E- PndPidSttAssociatorTask::Init: No PidChargedCand array!" << std::endl;
    return kERROR;
  }
    
  // set up functions
  ElectronDEDXMeanFunction();
  MuonDEDXMeanFunction();
  PionDEDXMeanFunction();
  KaonDEDXMeanFunction();
  ProtonDEDXMeanFunctionL();
  ProtonDEDXMeanFunctionH();
  ElectronDEDXSigmaFunction();
  MuonDEDXSigmaFunction();
  PionDEDXSigmaFunction();
  KaonDEDXSigmaFunction();
  ProtonDEDXSigmaFunctionL();
  ProtonDEDXSigmaFunctionH();

  ioman->Register("PidAlgoStt","Pid", fPidChargedProb, kTRUE); 

  if(fDefaultHypo == kTRUE) std::cout << "-I- PndPidSttAssociatorTask: using default mass hypo bands (muon)" << endl;
  else std::cout << "-I- PndPidSttAssociatorTask: NOT using default mass hypo bands" << endl;

  std::cout << "-I- PndPidSttAssociatorTask::Init: Intialization successfull" << endl;
 
  return kSUCCESS;
}

void PndPidSttAssociatorTask::Exec(Option_t * option) {

  // cout << "PND PID STT ASSOCIATOR" << endl;

  fPidChargedProb->Clear();
  
  for(Int_t i = 0; i < fPidChargedCand->GetEntriesFast(); i++)
    {
      PndPidCandidate* pidCand = (PndPidCandidate*) fPidChargedCand->At(i);
      TClonesArray& pidRef = *fPidChargedProb;
      PndPidProbability* prob = new(pidRef[i]) PndPidProbability();// initializes with flat probabilities
      prob->SetIndex(i);

      // cout << "i " << i << " dedx " << pidCand->GetSttMeanDEDX() << endl;
      if (pidCand->GetSttMeanDEDX() == 0) continue;

      // CUT on momentum: if mom > 0.8 GeV/c
      // no way to decide what particle it is 
      if (pidCand->GetMomentum().Mag() < 0.2 ) continue;
      if (pidCand->GetMomentum().Mag() > 5.0 ) continue;

      DoPidMatch(pidCand,prob);
    }
  
}

void PndPidSttAssociatorTask::DoPidMatch(PndPidCandidate* pidcand, PndPidProbability* prob)
{
  // measured quantities
  Double_t momentum = pidcand->GetMomentum().Mag();
  Double_t dedx     = pidcand->GetSttMeanDEDX();

  // parametrized quantities (gaussian)
  Double_t dedx_mean, dedx_sigma;

  // e
  dedx_mean  = fElectronDEDXMean->Eval(momentum);
  dedx_sigma = fElectronDEDXSigma->Eval(momentum);
  prob->SetElectronPdf(GetPdf(dedx, dedx_mean, dedx_sigma));
  //   cout << "e: " << pidcand->GetMomentum().Mag() 
  //        << " "   << pidcand->GetSttMeanDEDX()
  //        << " "   << dedx_mean 
  //        << " "   << dedx_sigma 
  //        << " "   << prob->GetElectronPdf() << endl;

  // mu
  dedx_mean  = fMuonDEDXMean->Eval(momentum);
  dedx_sigma = fMuonDEDXSigma->Eval(momentum);
  prob->SetMuonPdf(GetPdf(dedx, dedx_mean, dedx_sigma));
  //   cout << "mu: " << pidcand->GetMomentum().Mag() 
  //        << " "   << pidcand->GetSttMeanDEDX() 
  //        << " "   << dedx_mean 
  //        << " "   << dedx_sigma 
  //        << " "   << prob->GetMuonPdf() << endl;
 
  // pi 
  dedx_mean  = fPionDEDXMean->Eval(momentum);
  dedx_sigma = fPionDEDXSigma->Eval(momentum);
  prob->SetPionPdf(GetPdf(dedx, dedx_mean, dedx_sigma));
  //   cout << "pi: " << pidcand->GetMomentum().Mag() 
  //        << " "   << pidcand->GetSttMeanDEDX() 
  //        << " "   << dedx_mean 
  //        << " "   << dedx_sigma 
  //        << " "   << prob->GetPionPdf() << endl;
 
  // k
  dedx_mean  = fKaonDEDXMean->Eval(momentum);
  dedx_sigma = fKaonDEDXSigma->Eval(momentum);
  prob->SetKaonPdf(GetPdf(dedx, dedx_mean, dedx_sigma));
  //   cout << "k: " << pidcand->GetMomentum().Mag() 
  //        << " "   << pidcand->GetSttMeanDEDX() 
  //        << " "   << dedx_mean 
  //        << " "   << dedx_sigma 
  //        << " "   << prob->GetKaonPdf() << endl;
  
  // p
  if(momentum < 1.){
      dedx_mean  = fProtonDEDXMeanL->Eval(momentum);
      dedx_sigma = fProtonDEDXSigmaL->Eval(momentum);
      prob->SetProtonPdf(GetPdf(dedx, dedx_mean, dedx_sigma));
  }
  if(momentum > 1.){
      dedx_mean  = fProtonDEDXMeanH->Eval(momentum);
      dedx_sigma = fProtonDEDXSigmaH->Eval(momentum);
      prob->SetProtonPdf(GetPdf(dedx, dedx_mean, dedx_sigma));
  }
  //   cout << "p: " << pidcand->GetMomentum().Mag()
  //        << " "   << pidcand->GetSttMeanDEDX()
  //        << " "   << dedx_mean
  //        << " "   << dedx_sigma
  //        << " "   << prob->GetProtonPdf() << endl;

  //   cout << endl;


}  

Double_t PndPidSttAssociatorTask::GetPdf(Double_t dedx, Double_t mean, Double_t sigma)
{
  // gaussian of dedx_mean distribution @ a certain momentum
  TF1 *gausPdf = new TF1("gausPdf","gausn", 0, 100);       // CHECK the extremities
  gausPdf->SetParameters(1, mean, sigma);
  Double_t val = gausPdf->Eval(dedx);
  delete gausPdf;
  return val;
}


// functions to parametrize the dedx_mean as function of momentum -----------

void PndPidSttAssociatorTask::ElectronDEDXMeanFunction() {
  fElectronDEDXMean = new TF1("emdedx","[0] * (1./x)**2 + [1] * TMath::Log(x) + [2]", 0.2, 5.);

  if(fDefaultHypo == kTRUE) fElectronDEDXMean->SetParameters(-2.53435e-02,1.35409e-01,7.34210e+00);
  else  fElectronDEDXMean->SetParameters(-2.53435e-02,1.35409e-01,7.34210e+00);

}

void PndPidSttAssociatorTask::MuonDEDXMeanFunction() {
  fMuonDEDXMean = new TF1("mumdedx","[0] * (1./x)**2 + [1] * TMath::Log(x) + [2]", 0.2, 5.);
  if(fDefaultHypo == kTRUE) fMuonDEDXMean->SetParameters(8.84580e-02,7.70377e-01,5.38858e+00);
  else fMuonDEDXMean->SetParameters(8.84580e-02,7.70377e-01,5.38858e+00);

}

void PndPidSttAssociatorTask::PionDEDXMeanFunction() {
  fPionDEDXMean = new TF1("pimdedx","[0] * (1./x)**2 + [1] * TMath::Log(x) + [2]", 0.2, 5);
  if(fDefaultHypo == kTRUE) fPionDEDXMean->SetParameters(1.28760e-01,7.53150e-01,5.22487e+00);
  else fPionDEDXMean->SetParameters(1.28760e-01,7.53150e-01,5.22487e+00);

}

void PndPidSttAssociatorTask::KaonDEDXMeanFunction() {
  fKaonDEDXMean = new TF1("kmdedx","[0] * (1./x)**2 + [1] * TMath::Log(x) + [2]", 0.2, 5.);
  if(fDefaultHypo == kTRUE) fKaonDEDXMean->SetParameters(1.56819e+00,8.85371e-01,4.13682e+00);
  else fKaonDEDXMean->SetParameters(1.56819e+00,8.85371e-01,4.13682e+00);

}

void PndPidSttAssociatorTask::ProtonDEDXMeanFunctionH() {
  fProtonDEDXMeanH = new TF1("pmdedx","[0] * (1./x)**2 + [1] * TMath::Log(x) + [2]", 0.2, 5.);
 if(fDefaultHypo == kTRUE) fProtonDEDXMeanH->SetParameters(4.16818e+00,6.90343e-01,3.97309e+00);
 else fProtonDEDXMeanH->SetParameters(4.16818e+00,6.90343e-01,3.97309e+00);

}

void PndPidSttAssociatorTask::ProtonDEDXMeanFunctionL() {
  fProtonDEDXMeanL = new TF1("pmdedx","[0] * (1./x)**2 + [1] * TMath::Log(x) + [2]", 0.2, 5.);
 if(fDefaultHypo == kTRUE) fProtonDEDXMeanL->SetParameters(6.52530,5.82758,1.86702);
 else fProtonDEDXMeanL->SetParameters(6.52530,5.82758,1.86702);

}


// functions to parametrize the dedx_sigma as function of momentum ----------

void PndPidSttAssociatorTask::ElectronDEDXSigmaFunction() {
  fElectronDEDXSigma = new TF1("esdedx","[0] * (1./x)**2 + [1] * TMath::Log(x) + [2]", 0.2, 5.);
  if(fDefaultHypo == kTRUE) fElectronDEDXSigma->SetParameters(1.81859e-03,-1.01187e-02,6.76615e-01);
  else fElectronDEDXSigma->SetParameters(1.81859e-03,-1.01187e-02,6.76615e-01);

}

void PndPidSttAssociatorTask::MuonDEDXSigmaFunction() {
  fMuonDEDXSigma = new TF1("musdedx","[0] * (1./x)**2 + [1] * TMath::Log(x) + [2]", 0.2, 5.);
  if(fDefaultHypo == kTRUE) fMuonDEDXSigma->SetParameters(8.54877e-03,2.96974e-02,5.51581e-01);
  else fMuonDEDXSigma->SetParameters(8.54877e-03,2.96974e-02,5.51581e-01);

}

void PndPidSttAssociatorTask::PionDEDXSigmaFunction() {
  fPionDEDXSigma = new TF1("pisdedx","[0] * (1./x)**2 + [1] * TMath::Log(x) + [2]", 0.2, 5.);
  if(fDefaultHypo == kTRUE) fPionDEDXSigma->SetParameters(1.49778e-02,3.05617e-02,5.43355e-01);
  else fPionDEDXSigma->SetParameters(1.49778e-02,3.05617e-02,5.43355e-01);

}

void PndPidSttAssociatorTask::KaonDEDXSigmaFunction() {
  fKaonDEDXSigma = new TF1("ksdedx","[0] * (1./x)**2 + [1] * TMath::Log(x) + [2]", 0.2, 5.);
  if(fDefaultHypo == kTRUE) fKaonDEDXSigma->SetParameters(2.30775e-01,8.81697e-02,4.20122e-01);
  else  fKaonDEDXSigma->SetParameters(2.30775e-01,8.81697e-02,4.20122e-01);

}

void PndPidSttAssociatorTask::ProtonDEDXSigmaFunctionL() {
  fProtonDEDXSigmaL = new TF1("psdedx","[0] * (1./x)**2 + [1] * TMath::Log(x) + [2]", 0.2, 5.);
  if(fDefaultHypo == kTRUE) fProtonDEDXSigmaL->SetParameters(1.11867e+00,1.78621e+00,-1.01217e-01);
  else fProtonDEDXSigmaL->SetParameters(1.11867e+00,1.78621e+00,-1.01217e-01);

}
void PndPidSttAssociatorTask::ProtonDEDXSigmaFunctionH() {
  fProtonDEDXSigmaH = new TF1("psdedx","[0] * (1./x)**2 + [1] * TMath::Log(x) + [2]", 0.2, 5.);
  if(fDefaultHypo == kTRUE) fProtonDEDXSigmaH->SetParameters(5.17593e-01,6.68813e-02,4.59503e-01);
  else fProtonDEDXSigmaL->SetParameters(5.17593e-01,6.68813e-02,4.59503e-01);

}

ClassImp(PndPidSttAssociatorTask)
