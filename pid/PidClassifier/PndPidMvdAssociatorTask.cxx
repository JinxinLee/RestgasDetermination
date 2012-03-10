#include "PndPidMvdAssociatorTask.h"
#include "PndPidCandidate.h"
#include "PndPidProbability.h"
#include "PndPidMvdPar.h"
#include "FairRootManager.h"
#include "TMath.h"
#include "TF1.h"
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
  
  mvdPara = new PndPidMvdPar();
  
  std::cout << "-I- PndPidMvdAssociatorTask::Init: Success!" << std::endl;
  
  return kSUCCESS;
}

//______________________________________________________
void PndPidMvdAssociatorTask::SetParContainers() {
  //--
}
//______________________________________________________
void PndPidMvdAssociatorTask::Exec(Option_t * option) {
  if (fPidChargedProb->GetEntriesFast() != 0) fPidChargedProb->Clear();
  if(fVerbose>1) std::cout << "-I- Start PndPidMvdAssociatorTask. "<<std::endl;

  // Get the Candidates
  for(Int_t i=0; i<fPidChargedCand->GetEntriesFast(); i++)
    {
      PndPidCandidate* pidcand = (PndPidCandidate*)fPidChargedCand->At(i);
      PndPidProbability* prob = new((*fPidChargedProb)[i]) PndPidProbability(0.2,0.2,0.2,0.2,0.2,i);// initializes with equal probability
      if (pidcand->GetMvdDEDX()==0) continue;
      DoPidMatch(pidcand,prob);
    }
 
}

void PndPidMvdAssociatorTask::DoPidMatch(PndPidCandidate* pidcand, PndPidProbability* prob)
{
  Float_t CanMpv, CanSigma;

  //Electron
  CanMpv=mvdPara->GetElectronMpv(pidcand->GetMomentum().Mag());
  CanSigma=mvdPara->GetElectronSigma(pidcand->GetMomentum().Mag());
  prob->SetElectronPdf(GetPdf(pidcand->GetMvdDEDX(), CanMpv, CanSigma));
  //cout << "ele:\t" << pidcand->GetMomentum().Mag() << "\t" << pidcand->GetMvdDEDX() << "\t" << CanMpv << "\t" << CanSigma << "\t" << prob->GetElectronPdf() << endl;
   
  //Proton
  CanMpv=mvdPara->GetProtonMpv(pidcand->GetMomentum().Mag());
  CanSigma=mvdPara->GetProtonSigma(pidcand->GetMomentum().Mag());
  prob->SetProtonPdf(GetPdf(pidcand->GetMvdDEDX(), CanMpv, CanSigma));
  //cout << "proton:\t" << pidcand->GetMomentum().Mag() << "\t" << pidcand->GetMvdDEDX() << "\t" << CanMpv << "\t" << CanSigma << "\t" << prob->GetProtonPdf() << endl;

  //Pion
  CanMpv=mvdPara->GetPionMpv(pidcand->GetMomentum().Mag());
  CanSigma=mvdPara->GetPionSigma(pidcand->GetMomentum().Mag());
  prob->SetPionPdf(GetPdf(pidcand->GetMvdDEDX(), CanMpv, CanSigma));
  //cout << "pion:\t" << pidcand->GetMomentum().Mag() << "\t" << pidcand->GetMvdDEDX() << "\t" << CanMpv << "\t" << CanSigma << "\t" << prob->GetPionPdf() << endl;

  //Muon
  CanMpv=mvdPara->GetMuonMpv(pidcand->GetMomentum().Mag());
  CanSigma=mvdPara->GetMuonSigma(pidcand->GetMomentum().Mag());
  prob->SetMuonPdf(GetPdf(pidcand->GetMvdDEDX(), CanMpv, CanSigma));
  //cout << "muon:\t" << pidcand->GetMomentum().Mag() << "\t" << pidcand->GetMvdDEDX() << "\t" << CanMpv << "\t" << CanSigma << "\t" << prob->GetMuonPdf() << endl;

  //Kaon
  CanMpv=mvdPara->GetKaonMpv(pidcand->GetMomentum().Mag());
  CanSigma=mvdPara->GetKaonSigma(pidcand->GetMomentum().Mag());
  prob->SetKaonPdf(GetPdf(pidcand->GetMvdDEDX(), CanMpv, CanSigma));
  //cout << "kaon:\t" << pidcand->GetMomentum().Mag() << "\t" << pidcand->GetMvdDEDX() << "\t" << CanMpv << "\t" << CanSigma << "\t" << prob->GetKaonPdf() << endl;
}  

Float_t PndPidMvdAssociatorTask::GetPdf(Float_t dedx, Float_t Mpv, Float_t Sigma)
{
  TF1 *landauPdf = new TF1("landauPdf","landaun",0,1);
  landauPdf->SetParameter(0,1);
  landauPdf->SetParameter(1,Mpv);
  landauPdf->SetParameter(2,Sigma);
  Double_t val = landauPdf->Eval(dedx);
  delete landauPdf;
  return val;
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
