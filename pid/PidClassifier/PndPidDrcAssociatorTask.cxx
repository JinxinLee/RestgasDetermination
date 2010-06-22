#include "PndPidDrcAssociatorTask.h"
#include "PndPidCandidate.h"
#include "PndPidProbability.h"
#include "FairRootManager.h"
#include "TMath.h"
#include "TF1.h"
#include "Riostream.h"


//___________________________________________________________
PndPidDrcAssociatorTask::~PndPidDrcAssociatorTask() {
  //
  FairRootManager *fManager =FairRootManager::Instance();
  fManager->Write();
}

//___________________________________________________________
PndPidDrcAssociatorTask::PndPidDrcAssociatorTask() {
  //---
  fPidChargedProb = new TClonesArray("PndPidProbability");
}

//___________________________________________________________
PndPidDrcAssociatorTask::PndPidDrcAssociatorTask(const char *name, const char *title):FairTask(name) 
{
  //---
  fPidChargedProb = new TClonesArray("PndPidProbability");
}

//___________________________________________________________
InitStatus PndPidDrcAssociatorTask::Init() {
  
  cout << "InitStatus PndPidDrcAssociatorTask::Init()" << endl;
  
  FairRootManager *fManager =FairRootManager::Instance();	

  fPidChargedCand = (TClonesArray *)fManager->GetObject("PidChargedCand");
  if ( ! fPidChargedCand) {
    std::cout << "-I- PndPidDrcAssociatorTask::Init: No PndPidCandidate array PidChargedCand there!" << std::endl;
    return kERROR;
  }
    
  Register();
  
  std::cout << "-I- PndPidDrcAssociatorTask::Init: Success!" << std::endl;
  
  return kSUCCESS;
}

//______________________________________________________
void PndPidDrcAssociatorTask::SetParContainers() {
  //--
}
//______________________________________________________
void PndPidDrcAssociatorTask::Exec(Option_t * option) {
  if (fPidChargedProb->GetEntriesFast() != 0) fPidChargedProb->Clear();
  if(fVerbose>1) std::cout << "-I- Start PndPidDrcAssociatorTask. "<<std::endl;

  // Get the Candidates
  for(Int_t i=0; i<fPidChargedCand->GetEntriesFast(); i++)
    {
      PndPidCandidate* pidcand = (PndPidCandidate*)fPidChargedCand->At(i);
      TClonesArray& pidRef = *fPidChargedProb;
      PndPidProbability* prob = new(pidRef[i]) PndPidProbability();// initializes with zeros
      prob->SetIndex(i);
      if (pidcand->GetDrcIndex()==-1) continue;
      DoPidMatch(pidcand,prob);
    }
 
}

void PndPidDrcAssociatorTask::DoPidMatch(PndPidCandidate* pidcand, PndPidProbability* prob)
{

  // electron
  {
    Float_t mass   = 0.0005;
    Float_t sigma  = 0.006;
    prob->SetElectronPdf(GetPdf(pidcand->GetDrcThetaC(),pidcand->GetMomentum().Mag(),mass, sigma));
  }

  // muon
  {
    Float_t mass   = 0.106;
    Float_t sigma  = 0.006;
    prob->SetMuonPdf(GetPdf(pidcand->GetDrcThetaC(),pidcand->GetMomentum().Mag(),mass, sigma));
  }
  
  // pion
  {
    Float_t mass   = 0.140;
    Float_t sigma  = 0.006;
    prob->SetPionPdf(GetPdf(pidcand->GetDrcThetaC(),pidcand->GetMomentum().Mag(),mass, sigma));
  }
  
  // kaon
  {
    Float_t mass   = 0.494;
    Float_t sigma  = 0.005;
    prob->SetKaonPdf(GetPdf(pidcand->GetDrcThetaC(),pidcand->GetMomentum().Mag(),mass, sigma));
  }
  
  // proton
  {
    Float_t mass   = 0.938;
    Float_t sigma  = 0.005;
    prob->SetProtonPdf(GetPdf(pidcand->GetDrcThetaC(),pidcand->GetMomentum().Mag(),mass, sigma));
  }
}  

Float_t PndPidDrcAssociatorTask::GetPdf(Float_t thetaC, Float_t mom, Float_t mass, Float_t sigma)
{
  
  Float_t beta = mom / TMath::Sqrt(mom*mom + mass*mass);
  if ( (beta>0.) && ((1./1.47/beta)<1.) )
    {
      Float_t center = 1./1.47/beta;
      TF1 *gausPdf = new TF1("gausPdf","gausn",0,1);
      gausPdf->SetParameter(0,1);
      gausPdf->SetParameter(1,center);
      gausPdf->SetParameter(2,sigma);
      return gausPdf->Eval(TMath::Cos(thetaC));
    }
  else
    {
      return 0.;
    }
}

//_________________________________________________________________
void PndPidDrcAssociatorTask::Register() {
  //---
  FairRootManager::Instance()->
    Register("PidAlgoDrc","Pid", fPidChargedProb, kTRUE); 
}

//_________________________________________________________________
void PndPidDrcAssociatorTask::Finish() {
}

//_________________________________________________________________
void PndPidDrcAssociatorTask::Reset() {
  //---
}


ClassImp(PndPidDrcAssociatorTask)
