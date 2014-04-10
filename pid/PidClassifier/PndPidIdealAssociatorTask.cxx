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
  if (fPidChargedProb->GetEntriesFast() != 0) fPidChargedProb->Clear();
  if (fPidNeutralProb->GetEntriesFast() != 0) fPidNeutralProb->Clear();
  // Get the Candidates
  for(Int_t i=0; i<fPidChargedCand->GetEntriesFast(); i++){
    PndPidCandidate* pidcand = (PndPidCandidate*)fPidChargedCand->At(i);
    PndPidProbability* prob = new((*fPidChargedProb)[i]) PndPidProbability(0.2,0.2,0.2,0.2,0.2,i);// initializes with equal probability
    if(fVerbose>1) std::cout<<"-I- PndPidIdealAssociatorTask Charged BEFORE  "<< pidcand->GetLorentzVector().M()<<std::endl;;
    DoPidMatch(pidcand,prob);
    if(fVerbose>1) std::cout<<"-I- PndPidIdealAssociatorTask Charged AFTER   "<< pidcand->GetLorentzVector().M()<<std::endl;;
  }
  for(Int_t i=0; i<fPidNeutralCand->GetEntriesFast(); i++){
    PndPidCandidate* pidcand = (PndPidCandidate*)fPidNeutralCand->At(i);
    PndPidProbability* prob = new((*fPidNeutralProb)[i]) PndPidProbability(0.2,0.2,0.2,0.2,0.2,i);// initializes with zeros
    //DoPidMatch(pidcand,prob); //TODO match idealy neutral cands, esp. when we have pi0s
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
      prob->SetProtonPdf(0.);
      prob->SetKaonPdf(0.);
      prob->SetPionPdf(0.);
      prob->SetMuonPdf(0.);
      prob->SetElectronPdf(1.);
      break;

    case -13: //muon+
    case 13: //muon-
      prob->SetProtonPdf(0.);
      prob->SetKaonPdf(0.);
      prob->SetPionPdf(0.);
      prob->SetMuonPdf(1.);
      prob->SetElectronPdf(0.);
      break;

    case 211: //pion+
    case -211: //pion-
      prob->SetProtonPdf(0.);
      prob->SetKaonPdf(0.);
      prob->SetPionPdf(1.);
      prob->SetMuonPdf(0.);
      prob->SetElectronPdf(0.);
      break;

    case -321: //Kaon+
    case 321: //Kaon-
      prob->SetProtonPdf(0.);
      prob->SetKaonPdf(1.);
      prob->SetPionPdf(0.);
      prob->SetMuonPdf(0.);
      prob->SetElectronPdf(0.);
      break;

    case -2212: //antiproton
    case 2212: //proton
      prob->SetProtonPdf(1.);
      prob->SetKaonPdf(0.);
      prob->SetPionPdf(0.);
      prob->SetMuonPdf(0.);
      prob->SetElectronPdf(0.);
      break;

    default:
      prob->SetProtonPdf(0.2);
      prob->SetKaonPdf(0.2);
      prob->SetPionPdf(0.2);
      prob->SetMuonPdf(0.2);
      prob->SetElectronPdf(0.2);
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
