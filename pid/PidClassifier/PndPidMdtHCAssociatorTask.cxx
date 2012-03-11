#include "PndPidMdtHCAssociatorTask.h"
#include "PndPidCandidate.h"
#include "PndPidProbability.h"
#include "FairRootManager.h"
#include "TMath.h"
#include "Riostream.h"


//___________________________________________________________
PndPidMdtHCAssociatorTask::~PndPidMdtHCAssociatorTask() {
  //
  FairRootManager *fManager =FairRootManager::Instance();
  fManager->Write();
}

//___________________________________________________________
PndPidMdtHCAssociatorTask::PndPidMdtHCAssociatorTask() {
  //---
  fPidChargedProb = new TClonesArray("PndPidProbability");
}

//___________________________________________________________
PndPidMdtHCAssociatorTask::PndPidMdtHCAssociatorTask(const char *name, const char *title):FairTask(name) 
{
  //---
  fPidChargedProb = new TClonesArray("PndPidProbability");
}

//___________________________________________________________
InitStatus PndPidMdtHCAssociatorTask::Init() {
  
  cout << "InitStatus PndPidMdtHCAssociatorTask::Init()" << endl;
  
  FairRootManager *fManager =FairRootManager::Instance();	

  fPidChargedCand = (TClonesArray *)fManager->GetObject("PidChargedCand");
  if ( ! fPidChargedCand) {
    std::cout << "-I- PndPidMdtHCAssociatorTask::Init: No PndPidCandidate array PidChargedCand there!" << std::endl;
    return kERROR;
  }
    
  Register();
  
  std::cout << "-I- PndPidMdtHCAssociatorTask::Init: Success!" << std::endl;
  
  return kSUCCESS;
}

//______________________________________________________
void PndPidMdtHCAssociatorTask::SetParContainers() {
  //--
}
//______________________________________________________
void PndPidMdtHCAssociatorTask::Exec(Option_t * option) {
  if (fPidChargedProb->GetEntriesFast() != 0) fPidChargedProb->Delete();
  if(fVerbose>1) std::cout << "-I- Start PndPidMdtHCAssociatorTask. "<<std::endl;

  // Get the Candidates
  for(Int_t i=0; i<fPidChargedCand->GetEntriesFast(); i++)
    {
      PndPidCandidate* pidcand = (PndPidCandidate*)fPidChargedCand->At(i);
      TClonesArray& pidRef = *fPidChargedProb;
      PndPidProbability* prob = new(pidRef[i]) PndPidProbability();// initializes with flat probabilities
      prob->SetIndex(i);
      if (pidcand->GetMuoIndex()==-1) continue;
     
      
      DoPidMatch(pidcand,prob);
    }
 
}

void PndPidMdtHCAssociatorTask::DoPidMatch(PndPidCandidate* pidcand, PndPidProbability* prob)
{
  if (pidcand->GetMuoIron()==0.) return;
  
  Double_t iron_thr = 0, mom_min = 0, mom_max = 0;
  if ((pidcand->GetMuoModule()==-1) || (pidcand->GetMuoModule()==2) )
    {
      iron_thr = 60.;
      mom_min = 0.4;
      mom_max = 1.1;
    }
  else if (pidcand->GetMuoModule()==1) 
    {
      iron_thr = 40.;
      mom_min = 0.2;
      mom_max = 0.8;
    }
  else
    {
      return;
    }
  
  if (pidcand->GetMuoMomentumIn() < mom_min) return;
  
  if (pidcand->GetMuoMomentumIn() > mom_max)
    {
      if (pidcand->GetMuoIron()>iron_thr)
	{
    // FIXME: Don't write zeros to that pdf!
	  prob->SetElectronPdf(0);
	  prob->SetMuonPdf(1);
	  prob->SetPionPdf(0);
	  prob->SetKaonPdf(0);
	  prob->SetProtonPdf(0);
	  return;
	}
      else
	{
    // FIXME: Don't write zeros to that pdf!
	  prob->SetElectronPdf(1);
	  prob->SetMuonPdf(0);
	  prob->SetPionPdf(1);
	  prob->SetKaonPdf(1);
	  prob->SetProtonPdf(1);
	  return;
	}
    }  // end of momentum > mom_max
  else
    {
      if (pidcand->GetMuoIron() > (pidcand->GetMuoMomentumIn()*iron_thr/(mom_max-mom_min) - iron_thr* mom_min/(mom_max-mom_min)) )
	{
    // FIXME: Don't write zeros to that pdf!
	  prob->SetElectronPdf(0);
	  prob->SetMuonPdf(1);
	  prob->SetPionPdf(0);
	  prob->SetKaonPdf(0);
	  prob->SetProtonPdf(0);
	  return;
	}
      else
	{
    // FIXME: Don't write zeros to that pdf!
	  prob->SetElectronPdf(1);
	  prob->SetMuonPdf(0);
	  prob->SetPionPdf(1);
	  prob->SetKaonPdf(1);
	  prob->SetProtonPdf(1);
	  return;
	}
    } // enf of mom_mim < momentum < mom_max 
 
}

//_________________________________________________________________
void PndPidMdtHCAssociatorTask::Register() {
  //---
  FairRootManager::Instance()->
    Register("PidAlgoMdtHardCuts","Pid", fPidChargedProb, kTRUE); 
}

//_________________________________________________________________
void PndPidMdtHCAssociatorTask::Finish() {
}

//_________________________________________________________________
void PndPidMdtHCAssociatorTask::Reset() {
  //---
}


ClassImp(PndPidMdtHCAssociatorTask)
