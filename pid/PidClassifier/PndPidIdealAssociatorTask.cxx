#include "PndDetectorList.h"
#include "PndPidIdealAssociatorTask.h"
#include "PndPidCandidate.h"
#include "PndPidProbability.h"
#include "PndMCTrack.h"

#include "FairRootManager.h"

#include "TDatabasePDG.h"


#include <cmath>


//___________________________________________________________
PndPidIdealAssociatorTask::~PndPidIdealAssociatorTask() {
  //
  FairRootManager *fManager =FairRootManager::Instance();
  fManager->Write();
}

//___________________________________________________________
PndPidIdealAssociatorTask::PndPidIdealAssociatorTask() {
  //---
  fNeutralBranchName="PidAlgoIdealNeutral";
  fChargedBranchName="PidAlgoIdealCharged";
  fPidChargedProb = new TClonesArray("PndPidProbability");
  fPidNeutralProb = new TClonesArray("PndPidProbability");
}

//___________________________________________________________
PndPidIdealAssociatorTask::PndPidIdealAssociatorTask(const char *name, const char *title)
:FairTask(name) {
  //---
  fNeutralBranchName="PidAlgoIdealNeutral";
  fChargedBranchName="PidAlgoIdealCharged";
  fPidChargedProb = new TClonesArray("PndPidProbability");
  fPidNeutralProb = new TClonesArray("PndPidProbability");
}

void PndPidIdealAssociatorTask::SetToOnlyOne()
{
  fNeutralBranchName="PidNeutralProbability";
  fChargedBranchName="PidChargedProbability";
}


//___________________________________________________________
InitStatus PndPidIdealAssociatorTask::Init() {
  
  //  cout << "InitStatus PndPidIdealAssociatorTask::Init()" << endl;
  
  FairRootManager *fManager =FairRootManager::Instance();	
  
  // TODO: Am I allowed to write in these Arrays?
  fPidChargedCand = (TClonesArray *)fManager->GetObject("PidChargedCand");
  if ( ! fPidChargedCand) {
    std::cout << "-I- PndPidIdealAssociatorTask::Init: No PndPidCandidate array PidChargedCand there!" << std::endl;
    return kERROR;
  }
  
  fPidNeutralCand = (TClonesArray *)fManager->GetObject("PidNeutralCand");
  if ( ! fPidNeutralCand) {
    std::cout << "-I- PndPidIdealAssociatorTask::Init: No PndPidCandidate array PidNeutralCand there!" << std::endl;
    return kERROR;
  }
  
  fMCTrack = (TClonesArray*) fManager->GetObject("MCTrack");
  if ( ! fMCTrack) {
    std::cout << "-I- PndPidIdealAssociatorTask::Init: No MC Track array there!" << std::endl;
    return kERROR;
  }
  
  Register();
  
  std::cout << "-I- PndPidIdealAssociatorTask::Init: Success!" << std::endl;
  
  return kSUCCESS;
}

//______________________________________________________
void PndPidIdealAssociatorTask::SetParContainers() {
  //--
}
//______________________________________________________
void PndPidIdealAssociatorTask::Exec(Option_t * option) {
  // CAUTION We use Monte-Carlo info here!
  
  if(fVerbose>1) std::cout << "-I- Start PndPidIdealAssociatorTask. "<<std::endl;
  fPidChargedProb->Clear();
  fPidNeutralProb->Clear();
  // Get the Candidates
  for(Int_t i=0; i<fPidChargedCand->GetEntriesFast(); i++){
    PndPidCandidate* pidcand = (PndPidCandidate*)fPidChargedCand->At(i);
    PndPidProbability* prob = new((*fPidChargedProb)[i]) PndPidProbability(0.,0.,0.,0.,0.,-1);// initializes with zeros
    prob->SetIndex(i);
    if(fVerbose>1) std::cout<<"-I- PndPidIdealAssociatorTask Charged BEFORE  "<< pidcand->GetLorentzVector().M()<<std::endl;;
    DoPidMatch(pidcand,prob);
    if(fVerbose>1) std::cout<<"-I- PndPidIdealAssociatorTask Charged AFTER   "<< pidcand->GetLorentzVector().M()<<std::endl;;
  }
  for(Int_t i=0; i<fPidNeutralCand->GetEntriesFast(); i++){
    PndPidCandidate* pidcand = (PndPidCandidate*)fPidNeutralCand->At(i);
    PndPidProbability* prob = new((*fPidNeutralProb)[i]) PndPidProbability(0.,0.,0.,0.,0.,-1);// initializes with zeros
    prob->SetIndex(i);
    DoPidMatch(pidcand,prob);
  }
  
}

void PndPidIdealAssociatorTask::DoPidMatch(PndPidCandidate* pidcand, PndPidProbability* prob)
{
  // Cheating for each Pid Candidate.
  
  Int_t mcid = pidcand->GetMcIndex();
  if(mcid<0) return; // no specified MC id... do nothing
  PndMCTrack *mctrack = (PndMCTrack*)fMCTrack->At(mcid);
  if( 0==mctrack) return; // better do nothing on a null pointer
  Int_t mcpdg = mctrack->GetPdgCode();
  
  // TLorentzVector lv = pidcand->GetLorentzVector();
  // Double_t mass = TDatabasePDG::Instance()->GetParticle(mcpdg)->Mass();
  // lv.SetXYZM(lv.X(), lv.Y(), lv.Z(), mass);
  //   pidcand->SetLorentzVector(lv);
  
  // chaet around with the probabilities...
  switch(mcpdg) {
    case -11: //positron
    case 11: //electron
      prob->SetElectronPdf(1.);
      break;
      
    case -13: //muon+
    case 13: //muon-
      prob->SetMuonPdf(1.);
      break;
      
    case 211: //pion+
    case -211: //pion-
      prob->SetPionPdf(1.);
      break;
      
    case -321: //Kaon+
    case 321: //Kaon-
      prob->SetKaonPdf(1.);
      break;
      
    case -2212: //antiproton
    case 2212: //proton
      prob->SetProtonPdf(1.);
      break;
      
    default:
      break;
  }
  
}  




//_________________________________________________________________
void PndPidIdealAssociatorTask::Register() {
  //---
  FairRootManager::Instance()->
  Register(fChargedBranchName,"Pid", fPidChargedProb, kTRUE); 
  FairRootManager::Instance()->
  Register(fNeutralBranchName,"Pid", fPidNeutralProb, kTRUE);
}

//_________________________________________________________________
void PndPidIdealAssociatorTask::Finish() {
  //---
  //  FairRootManager* ioman = FairRootManager::Instance();
  //  ioman->W
}
//_________________________________________________________________
void PndPidIdealAssociatorTask::Reset() {
  //---
}


ClassImp(PndPidIdealAssociatorTask)
