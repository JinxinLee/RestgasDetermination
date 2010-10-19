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
  //  fElectronDEDXMean->SetParameters(-0.00872775, 0.460742, 15.2417);  // LHETRACK no cut
  fElectronDEDXMean->SetParameters(-0.0182822, 0.445096, 15.211);        // LHETRACK  cut
  // fElectronDEDXMean->SetParameters(0.0017421, 0.462762, 15.2373);      // STT + MVD
}

void PndPidSttAssociatorTask::MuonDEDXMeanFunction() {
  fMuonDEDXMean = new TF1("mumdedx","[0] * (1./x)**2 + [1] * TMath::Log(x) + [2]", 0.05, 1.5);
  //  fMuonDEDXMean->SetParameters(0.12946, 1.53651, 11.3369);     // LHETRACK no cut
  fMuonDEDXMean->SetParameters(0.13822, 1.58573, 11.2707);         // LHETRACK cut
  // fMuonDEDXMean->SetParameters(0.116726, 1.39845, 11.346);      // STT + MVD
}

void PndPidSttAssociatorTask::PionDEDXMeanFunction() {
  fPionDEDXMean = new TF1("pimdedx","[0] * (1./x)**2 + [1] * TMath::Log(x) + [2]", 0.05, 1.5);
  //  fPionDEDXMean->SetParameters(0.189858, 1.40889, 10.9577);   // LHETRACK no cut
  fPionDEDXMean->SetParameters(0.242217, 1.65481, 10.8469);       // LHETRACK cut
  // fPionDEDXMean->SetParameters(0.190932, 1.27728, 10.9579);    // STT + MVD
}

void PndPidSttAssociatorTask::KaonDEDXMeanFunction() {
  fKaonDEDXMean = new TF1("kmdedx","[0] * (1./x)**2 + [1] * TMath::Log(x) + [2]", 0.05, 1.5);
  //  fKaonDEDXMean->SetParameters(2.51063, 1.45319, 9.07914);    // LHETRACK no cut
  fKaonDEDXMean->SetParameters(3.85798, 6.03764, 7.68956);        // LHETRACK cut
  // fKaonDEDXMean->SetParameters(2.76168, 1.70816, 8.87211);     // STT + MVD
}

void PndPidSttAssociatorTask::ProtonDEDXMeanFunction() {
  fProtonDEDXMean = new TF1("pmdedx","[0] * (1./x)**2 + [1] * TMath::Log(x) + [2]", 0.05, 1.5);
  //  fProtonDEDXMean->SetParameters(8.70827, 1.0621, 8.41066);     // LHETRACK no cut
  fProtonDEDXMean->SetParameters(14.3353, 11.6538, 2.75004);        // LHETRACK cut
  // fProtonDEDXMean->SetParameters(15.1555, 13.2337, 1.55363);     // STT + MVD
}


// functions to parametrize the dedx_sigma as function of momentum ----------

void PndPidSttAssociatorTask::ElectronDEDXSigmaFunction() {
  // fElectronDEDXSigma = new TF1("esdedx","pol1(0)", 0.05, 1.5);
  // fElectronDEDXSigma->SetParameters(1.52002, -0.131659); // LHETRACK no cut
  // fElectronDEDXSigma->SetParameters(1.59458, -0.234973);  // STT + MVD

  fElectronDEDXSigma = new TF1("esdedx","pol4(0)", 0.05, 1.5);
  fElectronDEDXSigma->SetParameters(0.728067, 4.90579, -14.0316, 15.5579, -5.80294); // LHETRACK cut
}

void PndPidSttAssociatorTask::MuonDEDXSigmaFunction() {
  // fMuonDEDXSigma = new TF1("musdedx","pol1(0)", 0.05, 1.5);
  // fMuonDEDXSigma->SetParameters(1.68652,  -0.496635); // LHETRACK no cut
  // fMuonDEDXSigma->SetParameters(1.37778,  -0.224275); // STT + MVD
  fMuonDEDXSigma = new TF1("musdedx","pol4(0)", 0.05, 1.5);
  fMuonDEDXSigma->SetParameters(0.728067, 4.90579, -14.0316, 15.5579, -5.80294); // LHETRACK cut
}

void PndPidSttAssociatorTask::PionDEDXSigmaFunction() {
  // fPionDEDXSigma = new TF1("pisdedx","pol1(0)", 0.05, 1.5);
  // fPionDEDXSigma->SetParameters(1.17002,  -0.0297058); // LHETRACK no cut
  // fPionDEDXSigma->SetParameters(1.31605,  -0.176124);  // STT + MVD
  fPionDEDXSigma = new TF1("pisdedx","pol4(0)", 0.05, 1.5);
  fPionDEDXSigma->SetParameters(5.7251, -29.2116, 64.0694, -58.7152, 19.1475); // LHETRACK cut
}

void PndPidSttAssociatorTask::KaonDEDXSigmaFunction() {
  // fKaonDEDXSigma = new TF1("ksdedx","pol1(0)", 0.05, 1.5);
  // fKaonDEDXSigma->SetParameters(2.19952,  -0.786072); // LHETRAK no cut
  // fKaonDEDXSigma->SetParameters(2.30038,  -0.910882); // STT + MVD

  fKaonDEDXSigma = new TF1("ksdedx","pol4(0)", 0.05, 1.5);
  fKaonDEDXSigma->SetParameters(22.0008, -96.9636, 169.8, -130.666, 37.0564); // LHETRAK cut
}

void PndPidSttAssociatorTask::ProtonDEDXSigmaFunction() {
  // fProtonDEDXSigma = new TF1("psdedx","pol1(0)", 0.05, 1.5);
  //  fProtonDEDXSigma->SetParameters(4.76073,  -2.55297); // LHETRAK no cut 
  // fProtonDEDXSigma->SetParameters(6.84975,  -4.30958); // STT + MVD

  fProtonDEDXSigma = new TF1("psdedx","pol4(0)", 0.05, 1.5);
  fProtonDEDXSigma->SetParameters(98.9892, -392.273, 597.038, -403.846, 101.931); // LHETRAK cut
}

ClassImp(PndPidSttAssociatorTask)
