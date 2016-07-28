#include "PndPidFtofAssociatorTask.h"
#include "PndPidCandidate.h"
#include "PndPidProbability.h"
#include "FairRootManager.h"
#include "TMath.h"
#include "TF1.h"
#include "Riostream.h"


//___________________________________________________________
PndPidFtofAssociatorTask::~PndPidFtofAssociatorTask() {
  //
  FairRootManager *fManager =FairRootManager::Instance();
  fManager->Write();
}

//___________________________________________________________
PndPidFtofAssociatorTask::PndPidFtofAssociatorTask() {
  //---
  fPidChargedProb = new TClonesArray("PndPidProbability");
}

//___________________________________________________________
PndPidFtofAssociatorTask::PndPidFtofAssociatorTask(const char *name, const char *title):FairTask(name) 
{
  //---
  fPidChargedProb = new TClonesArray("PndPidProbability");
}

//___________________________________________________________
InitStatus PndPidFtofAssociatorTask::Init() {
  
  std::cout << "InitStatus PndPidFtofAssociatorTask::Init()" << std::endl;
  
  FairRootManager *fManager =FairRootManager::Instance();	

  fPidChargedCand = (TClonesArray *)fManager->GetObject("PidChargedCand");
  if ( ! fPidChargedCand) {
    std::cout << "-I- PndPidFtofAssociatorTask::Init: No PndPidCandidate array PidChargedCand there!" << std::endl;
    return kERROR;
  }
    
  Register();
  
  std::cout << "-I- PndPidFtofAssociatorTask::Init: Success!" << std::endl;
  
  return kSUCCESS;
}

//______________________________________________________
void PndPidFtofAssociatorTask::SetParContainers() {
  //--
}
//______________________________________________________
void PndPidFtofAssociatorTask::Exec(Option_t * option) {
  if (fPidChargedProb->GetEntriesFast() != 0) fPidChargedProb->Clear();
  if(fVerbose>1) std::cout << "-I- Start PndPidFtofAssociatorTask. "<<std::endl;

  // Get the Candidates
  for(Int_t i=0; i<fPidChargedCand->GetEntriesFast(); i++)
    {
      PndPidCandidate* pidcand = (PndPidCandidate*)fPidChargedCand->At(i);
      TClonesArray& pidRef = *fPidChargedProb;
      PndPidProbability* prob = new(pidRef[i]) PndPidProbability();// initializes with zeros
      prob->SetIndex(i);
      if (pidcand->GetTofIndex()==-1) continue;
      DoPidMatch(pidcand,prob);
    }
 
}

void PndPidFtofAssociatorTask::DoPidMatch(PndPidCandidate* pidcand, PndPidProbability* prob)
{

  Double_t mom = pidcand->GetMomentum().Mag();
    
  // electron
  {
    Double_t mass   = 0.0005;
    Double_t center = mom / TMath::Sqrt(mom*mom + mass*mass);
    if (mom > 1) center = center - 0.; //electron OK
    Double_t sigma  = 0.004;
    prob->SetElectronPdf(GetPdf(pidcand->GetTofM2(),pidcand->GetMomentum().Mag(),center, sigma));
  }

  // muon
  {
    Double_t mass   = 0.106;
    Double_t center = mom / TMath::Sqrt(mom*mom + mass*mass);
    if (mom > 1) center = center - 0.1 + 0.1*mom; //tracking bug, later remove
    Double_t sigma  = 0.004 + 0.64*exp(-mom/0.11);
    prob->SetMuonPdf(GetPdf(pidcand->GetTofM2(),pidcand->GetMomentum().Mag(),center, sigma));
  }
  
  // pion
  {
    Double_t mass   = 0.140;
    Double_t center = mom / TMath::Sqrt(mom*mom + mass*mass);
    if (mom > 1) center = center - 0.1 + 0.1*mom; //tracking bug, later remove
    Double_t sigma  = 0.004 + 0.64*exp(-mom/0.11);
    prob->SetPionPdf(GetPdf(pidcand->GetTofM2(),pidcand->GetMomentum().Mag(),center, sigma));
  }
  
  // kaon
  {
    Double_t mass   = 0.494;
    Double_t center = mom / TMath::Sqrt(mom*mom + mass*mass);
    if (mom > 1) center = center - 0.39 + 0.37*mom; //tracking bug, later remove
    Double_t sigma  = 0.004 + 0.62*exp(-mom/0.21);
    prob->SetKaonPdf(GetPdf(pidcand->GetTofM2(),pidcand->GetMomentum().Mag(),center, sigma));
  }
  
  // proton
  {
    Double_t mass   = 0.938;
    Double_t center = mom / TMath::Sqrt(mom*mom + mass*mass);
    if (mom > 1) center = center - 0.12 + 0.12*mom; //tracking bug, later remove
    Double_t sigma  = 0.004 + 0.5*exp(-mom/0.13);
    prob->SetProtonPdf(GetPdf(pidcand->GetTofM2(),pidcand->GetMomentum().Mag(),center, sigma));
  }
}  

Double_t PndPidFtofAssociatorTask::GetPdf(Double_t mass2, Double_t mom, Double_t center, Double_t sigma)
{
  
  Double_t beta_c = mom / TMath::Sqrt(mom*mom + mass2);
  if (beta_c>0.)
    {
      TF1 *gausPdf = new TF1("gausPdf","gausn",0,1);
      gausPdf->SetParameter(0,1);
      gausPdf->SetParameter(1,center);
      gausPdf->SetParameter(2,sigma);
      Double_t val = gausPdf->Eval(beta_c);
      delete gausPdf;
      return val;
    }
  else
    {
      // FIXME: Don't write zeros to that pdf!
      return 0.;
    }
}

//_________________________________________________________________
void PndPidFtofAssociatorTask::Register() {
  //---
  FairRootManager::Instance()->
    Register("PidAlgoFtof","Pid", fPidChargedProb, kTRUE); 
}

//_________________________________________________________________
void PndPidFtofAssociatorTask::Finish() {
}

//_________________________________________________________________
void PndPidFtofAssociatorTask::Reset() {
  //---
}


ClassImp(PndPidFtofAssociatorTask)
