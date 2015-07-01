#include "PndPidDiscAssociatorTask.h"
#include "PndPidCandidate.h"
#include "PndPidProbability.h"
#include "FairRootManager.h"
#include "TMath.h"
#include "TF1.h"
#include "Riostream.h"


//___________________________________________________________
PndPidDiscAssociatorTask::~PndPidDiscAssociatorTask() {
  //
  FairRootManager *fManager =FairRootManager::Instance();
  fManager->Write();
}

//___________________________________________________________
PndPidDiscAssociatorTask::PndPidDiscAssociatorTask() {
  //---
  fPidChargedProb = new TClonesArray("PndPidProbability");
}

//___________________________________________________________
PndPidDiscAssociatorTask::PndPidDiscAssociatorTask(const char *name, const char *title):FairTask(name) 
{
  //---
  fPidChargedProb = new TClonesArray("PndPidProbability");
}

//___________________________________________________________
InitStatus PndPidDiscAssociatorTask::Init() {
  
  std::cout << "InitStatus PndPidDiscAssociatorTask::Init()" << std::endl;
  
  FairRootManager *fManager =FairRootManager::Instance();	

  fPidChargedCand = (TClonesArray *)fManager->GetObject("PidChargedCand");
  if ( ! fPidChargedCand) {
    std::cout << "-I- PndPidDiscAssociatorTask::Init: No PndPidCandidate array PidChargedCand there!" << std::endl;
    return kERROR;
  }
    
  Register();
  
  std::cout << "-I- PndPidDiscAssociatorTask::Init: Success!" << std::endl;
  
  return kSUCCESS;
}

//______________________________________________________
void PndPidDiscAssociatorTask::SetParContainers() {
  //--
}
//______________________________________________________
void PndPidDiscAssociatorTask::Exec(Option_t * option) {
  if (fPidChargedProb->GetEntriesFast() != 0) fPidChargedProb->Clear();
  if(fVerbose>1) std::cout << "-I- Start PndPidDiscAssociatorTask. "<<std::endl;

  // Get the Candidates
  for(Int_t i=0; i<fPidChargedCand->GetEntriesFast(); i++)
    {
      PndPidCandidate* pidcand = (PndPidCandidate*)fPidChargedCand->At(i);
      TClonesArray& pidRef = *fPidChargedProb;
      PndPidProbability* prob = new(pidRef[i]) PndPidProbability();// initializes with zeros
      prob->SetIndex(i);
      if (pidcand->GetDiscIndex()==-1) continue;
      DoPidMatch(pidcand,prob);
    }
 
}

void PndPidDiscAssociatorTask::DoPidMatch(PndPidCandidate* pidcand, PndPidProbability* prob)
{

  // electron
  {
    Double_t mass   = 0.0005;
    Double_t sigma  = 0.006;
    prob->SetElectronPdf(GetPdf(pidcand->GetDiscThetaC(),pidcand->GetMomentum().Mag(),mass, sigma));
  }

  // muon
  {
    Double_t mass   = 0.106;
    Double_t sigma  = 0.006;
    prob->SetMuonPdf(GetPdf(pidcand->GetDiscThetaC(),pidcand->GetMomentum().Mag(),mass, sigma));
  }
  
  // pion
  {
    Double_t mass   = 0.140;
    Double_t sigma  = 0.006;
    prob->SetPionPdf(GetPdf(pidcand->GetDiscThetaC(),pidcand->GetMomentum().Mag(),mass, sigma));
  }
  
  // kaon
  {
    Double_t mass   = 0.494;
    Double_t sigma  = 0.005;
    prob->SetKaonPdf(GetPdf(pidcand->GetDiscThetaC(),pidcand->GetMomentum().Mag(),mass, sigma));
  }
  
  // proton
  {
    Double_t mass   = 0.938;
    Double_t sigma  = 0.005;
    prob->SetProtonPdf(GetPdf(pidcand->GetDiscThetaC(),pidcand->GetMomentum().Mag(),mass, sigma));
  }
}  

Double_t PndPidDiscAssociatorTask::GetPdf(Double_t thetaC, Double_t mom, Double_t mass, Double_t sigma)
{
  
  Double_t beta = mom / TMath::Sqrt(mom*mom + mass*mass);
  if ( (beta>0.) && ((1./1.47/beta)<1.) )
    {
      Double_t center = 1./1.47/beta;
      TF1 *gausPdf = new TF1("gausPdf","gausn",0,1);
      gausPdf->SetParameter(0,1);
      gausPdf->SetParameter(1,center);
      gausPdf->SetParameter(2,sigma);
      Double_t val = gausPdf->Eval(TMath::Cos(thetaC));
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
void PndPidDiscAssociatorTask::Register() {
  //---
  FairRootManager::Instance()->
    Register("PidAlgoDisc","Pid", fPidChargedProb, kTRUE); 
}

//_________________________________________________________________
void PndPidDiscAssociatorTask::Finish() {
}

//_________________________________________________________________
void PndPidDiscAssociatorTask::Reset() {
  //---
}


ClassImp(PndPidDiscAssociatorTask)
