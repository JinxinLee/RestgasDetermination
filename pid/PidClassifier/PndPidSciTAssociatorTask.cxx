#include "PndPidSciTAssociatorTask.h"
#include "PndPidCandidate.h"
#include "PndPidProbability.h"
#include "FairRootManager.h"
#include "TMath.h"
#include "TF1.h"
#include "Riostream.h"

//___________________________________________________________
PndPidSciTAssociatorTask::~PndPidSciTAssociatorTask() {
  //
  FairRootManager *fManager =FairRootManager::Instance();
  fManager->Write();
}

//___________________________________________________________
PndPidSciTAssociatorTask::PndPidSciTAssociatorTask() {
  //---
  fPidChargedProb = new TClonesArray("PndPidProbability");
}

//___________________________________________________________
PndPidSciTAssociatorTask::PndPidSciTAssociatorTask(const char *name, const char *title):FairTask(name) 
{
  //---
  fPidChargedProb = new TClonesArray("PndPidProbability");
}

//___________________________________________________________
InitStatus PndPidSciTAssociatorTask::Init() {
  
  std::cout << "InitStatus PndPidSciTAssociatorTask::Init()" << std::endl;
  
  FairRootManager *fManager =FairRootManager::Instance();	

  fPidChargedCand = (TClonesArray *)fManager->GetObject("PidChargedCand");
  if ( ! fPidChargedCand) {
    std::cout << "-I- PndPidSciTAssociatorTask::Init: No PndPidCandidate array PidChargedCand there!" << std::endl;
    return kERROR;
  }
    
  Register();
  
  std::cout << "-I- PndPidSciTAssociatorTask::Init: Success!" << std::endl;
  
  return kSUCCESS;
}

//______________________________________________________
void PndPidSciTAssociatorTask::SetParContainers() {
  //--
}
//______________________________________________________
void PndPidSciTAssociatorTask::Exec(Option_t * option) {
  if (fPidChargedProb->GetEntriesFast() != 0) fPidChargedProb->Clear();
  if(fVerbose>1) std::cout << "-I- Start PndPidSciTAssociatorTask. "<<std::endl;

  PndPidCandidate *pidcand;
  PndPidProbability *prob;

  // Get the Candidates
  for(Int_t i=0; i<fPidChargedCand->GetEntriesFast(); i++)
    {
      pidcand = (PndPidCandidate*)fPidChargedCand->At(i);
      prob = new((*fPidChargedProb)[i]) PndPidProbability(0.2,0.2,0.2,0.2,0.2,i);// initializes with equal probability
      if (pidcand->GetTofIndex()==-1) continue;
      if ((pidcand->GetMomentum().Theta()*TMath::RadToDeg())<20.) continue; // pid runs only in the scitil region, with theta >20°
      DoPidMatch(pidcand,prob);
    }
}

void PndPidSciTAssociatorTask::DoPidMatch(PndPidCandidate* pidcand, PndPidProbability* prob)
{
  //parametrizise the sigma e.g. in dependenc of the transvers momentum
  Double_t sigma=0.11;

  TF1 *tofResolution = new TF1("tofResolution","[0]/x**4+[1]");

  Double_t mass = 0.140; // mass in GeV/c. Pion Mass as start Value

  Double_t mom =pidcand->GetMomentum().Mag();
  Double_t length = pidcand->GetTofTrackLength();
  Double_t tof = pidcand->GetTofStopTime();


  // electron 
  mass   = 0.000511;
  tofResolution->SetParameters(0.0000153,0.0765);
  sigma = tofResolution->Eval(mom); 
  prob->SetElectronPdf(GetPdf(mom,length,tof,mass,sigma));

  // muon
  mass   = 0.106;
  tofResolution->SetParameters(0.0000222,0.0768); 
  sigma = tofResolution->Eval(mom); 
  prob->SetMuonPdf(GetPdf(mom,length,tof,mass,sigma));
  
  // pion 
  mass   = 0.140;
  tofResolution->SetParameters(0.0000225,0.0764);
  sigma = tofResolution->Eval(mom);
  prob->SetPionPdf(GetPdf(mom,length,tof,mass,sigma));
   
  // kaon
  mass   = 0.494;
  tofResolution->SetParameters(0.0003,0.0767); 
  prob->SetKaonPdf(GetPdf(mom,length,tof,mass,sigma));

  // proton
  mass   = 0.938;
  tofResolution->SetParameters(0.00124,0.0787);
  sigma = tofResolution->Eval(mom);
  prob->SetProtonPdf(GetPdf(mom,length,tof,mass,sigma));

}  

Double_t PndPidSciTAssociatorTask::GetPdf(Double_t mom, Double_t length, Double_t tof, Double_t mass, Double_t sigma)
{
  // if (  ) only treat possible particles? e.g. with a minimum  tranverse momentum?
  Double_t tofCalc = length/(TMath::C()*100/1000000000)*sqrt(mass*mass/mom/mom+1);
  
  TF1 *gausPdf = new TF1("gausPdf","gausn",0,1);
  gausPdf->SetParameter(0,1);
  gausPdf->SetParameter(1,tofCalc);
  gausPdf->SetParameter(2,sigma);
  Double_t val = gausPdf->Eval(tof);
  delete gausPdf;
  return val;
}

//_________________________________________________________________
void PndPidSciTAssociatorTask::Register() {
  //---
  FairRootManager::Instance()->
    Register("PidAlgoSciT","Pid", fPidChargedProb, kTRUE); 
}

//_________________________________________________________________
void PndPidSciTAssociatorTask::Finish() {
}

//_________________________________________________________________
void PndPidSciTAssociatorTask::Reset() {
  //---
}


ClassImp(PndPidSciTAssociatorTask)
