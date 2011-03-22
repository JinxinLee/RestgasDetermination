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
  delete fProtonDEDXMean;
}

//___________________________________________________________
PndPidSttAssociatorTask::PndPidSttAssociatorTask() {
  fPidChargedProb = new TClonesArray("PndPidProbability");
}

//___________________________________________________________
PndPidSttAssociatorTask::PndPidSttAssociatorTask(const char *name, const char *title):FairTask(name) 
{
  fPidChargedProb = new TClonesArray("PndPidProbability");
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
  ProtonDEDXMeanFunction();
  ElectronDEDXSigmaFunction();
  MuonDEDXSigmaFunction();
  PionDEDXSigmaFunction();
  KaonDEDXSigmaFunction();
  ProtonDEDXSigmaFunction();

  ioman->Register("PidAlgoStt","Pid", fPidChargedProb, kTRUE); 

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
      PndPidProbability* prob = new(pidRef[i]) PndPidProbability();// initializes with zeros
      prob->SetIndex(i);

      // cout << "i " << i << " dedx " << pidCand->GetSttMeanDEDX() << endl;
      if (pidCand->GetSttMeanDEDX() == 0) continue;

      // CUT on momentum: if mom > 0.8 GeV/c
      // no way to decide what particle it is 
      if (pidCand->GetMomentum().Mag() > 0.8) continue; 

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
  dedx_mean  = fProtonDEDXMean->Eval(momentum);
  dedx_sigma = fProtonDEDXSigma->Eval(momentum);
  prob->SetProtonPdf(GetPdf(dedx, dedx_mean, dedx_sigma));
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
  fElectronDEDXMean = new TF1("emdedx","[0] * (1./x)**2 + [1] * TMath::Log(x) + [2]", 0.05, 1.5);
  fElectronDEDXMean->SetParameters(-0.0051344, 0.163231, 7.30319);
}

void PndPidSttAssociatorTask::MuonDEDXMeanFunction() {
  fMuonDEDXMean = new TF1("mumdedx","[0] * (1./x)**2 + [1] * TMath::Log(x) + [2]", 0.05, 1.5);
  fMuonDEDXMean->SetParameters(0.0737857, 0.643465, 5.42333);
}

void PndPidSttAssociatorTask::PionDEDXMeanFunction() {
  fPionDEDXMean = new TF1("pimdedx","[0] * (1./x)**2 + [1] * TMath::Log(x) + [2]", 0.05, 1.5);
  fPionDEDXMean->SetParameters(0.124558, 0.585125, 5.20178);
}

void PndPidSttAssociatorTask::KaonDEDXMeanFunction() {
  fKaonDEDXMean = new TF1("kmdedx","[0] * (1./x)**2 + [1] * TMath::Log(x) + [2]", 0.05, 1.5);
  fKaonDEDXMean->SetParameters(1.71069, 2.21786, 3.82597);
}

void PndPidSttAssociatorTask::ProtonDEDXMeanFunction() {
  fProtonDEDXMean = new TF1("pmdedx","[0] * (1./x)**2 + [1] * TMath::Log(x) + [2]", 0.05, 1.5);
  fProtonDEDXMean->SetParameters(4.26561, -0.152697, 4.03156);
}


// functions to parametrize the dedx_sigma as function of momentum ----------

void PndPidSttAssociatorTask::ElectronDEDXSigmaFunction() {
  fElectronDEDXSigma = new TF1("esdedx","[0] * (1./x)**2 + [1] * TMath::Log(x) + [2]", 0.05, 1.5);
  fElectronDEDXSigma->SetParameters(0.00428134, -0.00359152, 0.655438);
}

void PndPidSttAssociatorTask::MuonDEDXSigmaFunction() {
  fMuonDEDXSigma = new TF1("musdedx","[0] * (1./x)**2 + [1] * TMath::Log(x) + [2]", 0.05, 1.5);
  fMuonDEDXSigma->SetParameters(0.0193587, -0.0174329, 0.539616);
}

void PndPidSttAssociatorTask::PionDEDXSigmaFunction() {
  fPionDEDXSigma = new TF1("pisdedx","[0] * (1./x)**2 + [1] * TMath::Log(x) + [2]", 0.05, 1.5);
  fPionDEDXSigma->SetParameters(0.0395417, -0.0443629, 0.502671);
}

void PndPidSttAssociatorTask::KaonDEDXSigmaFunction() {
  fKaonDEDXSigma = new TF1("ksdedx","[0] * (1./x)**2 + [1] * TMath::Log(x) + [2]", 0.05, 1.5);
  fKaonDEDXSigma->SetParameters(0.260654, 0.385838, 0.307763);
}

void PndPidSttAssociatorTask::ProtonDEDXSigmaFunction() {
  fProtonDEDXSigma = new TF1("psdedx","[0] * (1./x)**2 + [1] * TMath::Log(x) + [2]", 0.05, 1.5);
  fProtonDEDXSigma->SetParameters(0.338277, -0.664876, 0.656844);
}

ClassImp(PndPidSttAssociatorTask)
