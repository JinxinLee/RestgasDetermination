#include "PndPidMvdAssociatorTask.h"
#include "PndPidCandidate.h"
#include "PndPidProbability.h"
#include "PndPidMvdPar.h"
#include "FairRootManager.h"
#include "TMath.h"
#include "Riostream.h"


//___________________________________________________________
PndPidMvdAssociatorTask::~PndPidMvdAssociatorTask() {
  //
  FairRootManager *fManager =FairRootManager::Instance();
  fManager->Write();
}

//___________________________________________________________
PndPidMvdAssociatorTask::PndPidMvdAssociatorTask() {
  //---
  fPidChargedProb = new TClonesArray("PndPidProbability");
}

//___________________________________________________________
PndPidMvdAssociatorTask::PndPidMvdAssociatorTask(const char *name, const char *title):FairTask(name) 
{
  //---
  fPidChargedProb = new TClonesArray("PndPidProbability");
}

//___________________________________________________________
InitStatus PndPidMvdAssociatorTask::Init() {
  
  cout << "InitStatus PndPidMvdAssociatorTask::Init()" << endl;
  
  FairRootManager *fManager =FairRootManager::Instance();	

  fPidChargedCand = (TClonesArray *)fManager->GetObject("PidChargedCand");
  if ( ! fPidChargedCand) {
    std::cout << "-I- PndPidMvdAssociatorTask::Init: No PndPidCandidate array PidChargedCand there!" << std::endl;
    return kERROR;
  }
    
  Register();
  
  std::cout << "-I- PndPidMvdAssociatorTask::Init: Success!" << std::endl;
  
  return kSUCCESS;
}

//______________________________________________________
void PndPidMvdAssociatorTask::SetParContainers() {
  //--
}
//______________________________________________________
void PndPidMvdAssociatorTask::Exec(Option_t * option) {
  
  if(fVerbose>1) std::cout << "-I- Start PndPidMvdAssociatorTask. "<<std::endl;

  // Get the Candidates
  for(Int_t i=0; i<fPidChargedCand->GetEntriesFast(); i++)
    {
      PndPidCandidate* pidcand = (PndPidCandidate*)fPidChargedCand->At(i);
      TClonesArray& pidRef = *fPidChargedProb;
      PndPidProbability* prob = new(pidRef[i]) PndPidProbability();// initializes with zeros
      DoPidMatch(pidcand,prob);
    }
 
}

void PndPidMvdAssociatorTask::DoPidMatch(PndPidCandidate* pidcand, PndPidProbability* prob)
{

  PndPidMvdPar *mvdPara = new PndPidMvdPar();
  Float_t CanMpv, CanSigma;
  //Electron
  CanMpv=mvdPara->GetElectronMpv(pidcand->GetMomentum().Mag());
  CanSigma=mvdPara->GetElectronSigma(pidcand->GetMomentum().Mag());
  prob->SetElectronPdf(GetPdf(pidcand->GetMvdDEDX(), CanMpv, CanSigma));

  //Proton
  CanMpv=mvdPara->GetProtonMpv(pidcand->GetMomentum().Mag());
  CanSigma=mvdPara->GetProtonSigma(pidcand->GetMomentum().Mag());
  prob->SetProtonPdf(GetPdf(pidcand->GetMvdDEDX(), CanMpv, CanSigma));

  //Pion
  CanMpv=mvdPara->GetPionMpv(pidcand->GetMomentum().Mag());
  CanSigma=mvdPara->GetPionSigma(pidcand->GetMomentum().Mag());
  prob->SetPionPdf(GetPdf(pidcand->GetMvdDEDX(), CanMpv, CanSigma));

  //Muon
  CanMpv=mvdPara->GetMuonMpv(pidcand->GetMomentum().Mag());
  CanSigma=mvdPara->GetMuonSigma(pidcand->GetMomentum().Mag());
  prob->SetMuonPdf(GetPdf(pidcand->GetMvdDEDX(), CanMpv, CanSigma));

  //Kaon
  CanMpv=mvdPara->GetKaonMpv(pidcand->GetMomentum().Mag());
  CanSigma=mvdPara->GetKaonSigma(pidcand->GetMomentum().Mag());
  prob->SetKaonPdf(GetPdf(pidcand->GetMvdDEDX(), CanMpv, CanSigma));
  
}  
Float_t PndPidMvdAssociatorTask::GetPdf(Float_t dedx, Float_t Mpv, Float_t Sigma)
{
  return TMath::Landau(dedx,Mpv,Sigma,kTRUE);
}



//_________________________________________________________________
void PndPidMvdAssociatorTask::Register() {
  //---
  FairRootManager::Instance()->
    Register("PidAlgoMvd","Pid", fPidChargedProb, kTRUE); 
}

//_________________________________________________________________
void PndPidMvdAssociatorTask::Finish() {
}

//_________________________________________________________________
void PndPidMvdAssociatorTask::Reset() {
  //---
}


ClassImp(PndPidMvdAssociatorTask)
