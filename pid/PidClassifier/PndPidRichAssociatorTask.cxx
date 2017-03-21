#include "PndPidRichAssociatorTask.h"
#include "PndPidCandidate.h"
#include "PndPidProbability.h"
#include "FairRootManager.h"
#include "TMath.h"
#include "TF1.h"
#include "Riostream.h"
#include <cmath>

//___________________________________________________________
PndPidRichAssociatorTask::~PndPidRichAssociatorTask() {
  //
  FairRootManager *fManager =FairRootManager::Instance();
  fManager->Write();
}

//___________________________________________________________
PndPidRichAssociatorTask::PndPidRichAssociatorTask() {
  //---
  fPidChargedProb = new TClonesArray("PndPidProbability");
}

//___________________________________________________________
PndPidRichAssociatorTask::PndPidRichAssociatorTask(const char *name, const char *title):FairTask(name) 
{
  //---
  fPidChargedProb = new TClonesArray("PndPidProbability");
  SetTitle(title);
}

//___________________________________________________________
InitStatus PndPidRichAssociatorTask::Init() {
  
  std::cout << "InitStatus PndPidRichAssociatorTask::Init()" << std::endl;
  
  FairRootManager *fManager =FairRootManager::Instance();	

  fPidChargedCand = (TClonesArray *)fManager->GetObject("PidChargedCand");
  if ( ! fPidChargedCand) {
    std::cout << "-I- PndPidRichAssociatorTask::Init: No PndPidCandidate array PidChargedCand there!" << std::endl;
    return kERROR;
  }
    
  Register();
  
  std::cout << "-I- PndPidRichAssociatorTask::Init: Success!" << std::endl;

  return kSUCCESS;
}

//______________________________________________________
void PndPidRichAssociatorTask::SetParContainers() {
  //--
}
//______________________________________________________
void PndPidRichAssociatorTask::Exec(Option_t *) {
  if (fPidChargedProb->GetEntriesFast() != 0) fPidChargedProb->Clear();
  if(fVerbose>1) std::cout << "-I- Start PndPidRichAssociatorTask. "<<std::endl;

  // Get the Candidates
  for(Int_t i=0; i<fPidChargedCand->GetEntriesFast(); i++)
    {
      PndPidCandidate* pidcand = (PndPidCandidate*)fPidChargedCand->At(i);
      TClonesArray& pidRef = *fPidChargedProb;
      PndPidProbability* prob = new(pidRef[i]) PndPidProbability();// initializes with zeros
      prob->SetIndex(i);
      if (pidcand->GetRichIndex()==-1) continue;
      DoPidMatch(pidcand,prob);
    }
 
}

void PndPidRichAssociatorTask::DoPidMatch(PndPidCandidate* pidcand, PndPidProbability* prob)
{
   Float_t sigma = pidcand->GetRichThetaCErr();
////////////////////////////////////////////////////////////////////////////
  // electron
  {
    Float_t mass   = 0.0005;
    prob->SetElectronPdf(GetPdf(pidcand->GetRichThetaC(),pidcand->GetMomentum().Mag(),mass, sigma));
  }

  // muon
  {
    Float_t mass   = 0.106;
    prob->SetMuonPdf(GetPdf(pidcand->GetRichThetaC(),pidcand->GetMomentum().Mag(),mass, sigma));
  }
  
  // pion
  {
    Float_t mass   = 0.140;
    prob->SetPionPdf(GetPdf(pidcand->GetRichThetaC(),pidcand->GetMomentum().Mag(),mass, sigma));
  }
  
  // kaon
  {
    Float_t mass   = 0.494;
    prob->SetKaonPdf(GetPdf(pidcand->GetRichThetaC(),pidcand->GetMomentum().Mag(),mass, sigma));
  }
  
  // proton
  {
    Float_t mass   = 0.938;
    prob->SetProtonPdf(GetPdf(pidcand->GetRichThetaC(),pidcand->GetMomentum().Mag(),mass, sigma));
  }
}  

Double_t PndPidRichAssociatorTask::GetPdf(Double_t thetaC, Double_t mom, Double_t mass, Double_t sigma)
{
  Double_t beta = mom / TMath::Sqrt(mom*mom + mass*mass);
  if ( (beta>0.) )
    {
      Double_t center = thetaC;
      TF1 *gausPdf = new TF1("gausPdf","gausn",0,1);
      gausPdf->SetParameter(0,1);
      gausPdf->SetParameter(1,center);
      gausPdf->SetParameter(2,sigma);
      Double_t val = gausPdf->Eval(beta);
      delete gausPdf;
      return val;
    }
  else
    {
      // FIXME: Don't write Zeros to that pdf!
      return 0.;
    }
}

//_________________________________________________________________
void PndPidRichAssociatorTask::Register() {
  //---
  FairRootManager::Instance()->
    Register("PidAlgoRich","Pid", fPidChargedProb, kTRUE); 
}

//_________________________________________________________________
void PndPidRichAssociatorTask::Finish() {
}

//_________________________________________________________________
void PndPidRichAssociatorTask::Reset() {
  //---
}


ClassImp(PndPidRichAssociatorTask)
