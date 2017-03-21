#include "PndDetectorList.h"
#include "PndPidIdealFwdOnlyAssociatorTask.h"
#include "PndPidCandidate.h"
#include "PndPidProbability.h"
#include "PndMCTrack.h"

#include "FairRootManager.h"

#include "TDatabasePDG.h"


#include <cmath>


//___________________________________________________________
PndPidIdealFwdOnlyAssociatorTask::~PndPidIdealFwdOnlyAssociatorTask() {
  //
  FairRootManager *fManager =FairRootManager::Instance();
  fManager->Write();
}

//___________________________________________________________
PndPidIdealFwdOnlyAssociatorTask::PndPidIdealFwdOnlyAssociatorTask() {
  //---
  fNeutralBranchName="PidAlgoIdealFwdOnlyNeutral";
  fChargedBranchName="PidAlgoIdealFwdOnlyCharged";
  fPidChargedProb = new TClonesArray("PndPidProbability");
  fPidNeutralProb = new TClonesArray("PndPidProbability");
}

//___________________________________________________________
PndPidIdealFwdOnlyAssociatorTask::PndPidIdealFwdOnlyAssociatorTask(const char *name, const char *title)
:FairTask(name) {
  //---
  fNeutralBranchName="PidAlgoIdealFwdOnlyNeutral";
  fChargedBranchName="PidAlgoIdealFwdOnlyCharged";
  fPidChargedProb = new TClonesArray("PndPidProbability");
  fPidNeutralProb = new TClonesArray("PndPidProbability");
  SetTitle(title);
}

void PndPidIdealFwdOnlyAssociatorTask::SetToOnlyOne()
{
  fNeutralBranchName="PidNeutralProbability";
  fChargedBranchName="PidChargedProbability";
}


//___________________________________________________________
InitStatus PndPidIdealFwdOnlyAssociatorTask::Init() {
  
  //  cout << "InitStatus PndPidIdealFwdOnlyAssociatorTask::Init()" << endl;
  
  FairRootManager *fManager =FairRootManager::Instance();	
  
  // TODO: Am I allowed to write in these Arrays?
  fPidChargedCand = (TClonesArray *)fManager->GetObject("PidChargedCand");
  if ( ! fPidChargedCand) {
    std::cout << "-I- PndPidIdealFwdOnlyAssociatorTask::Init: No PndPidCandidate array PidChargedCand there!" << std::endl;
    return kERROR;
  }
  
  fPidNeutralCand = (TClonesArray *)fManager->GetObject("PidNeutralCand");
  if ( ! fPidNeutralCand) {
    std::cout << "-I- PndPidIdealFwdOnlyAssociatorTask::Init: No PndPidCandidate array PidNeutralCand there!" << std::endl;
    return kERROR;
  }
  
  fMCTrack = (TClonesArray*) fManager->GetObject("MCTrack");
  if ( ! fMCTrack) {
    std::cout << "-I- PndPidIdealFwdOnlyAssociatorTask::Init: No MC Track array there!" << std::endl;
    return kERROR;
  }
  
  Register();
  
  std::cout << "-I- PndPidIdealFwdOnlyAssociatorTask::Init: Success!" << std::endl;
  
  return kSUCCESS;
}

//______________________________________________________
void PndPidIdealFwdOnlyAssociatorTask::SetParContainers() {
  //--
}
//______________________________________________________
void PndPidIdealFwdOnlyAssociatorTask::Exec(Option_t *) {
  // CAUTION We use Monte-Carlo info here!
  
  if(fVerbose>1) std::cout << "-I- Start PndPidIdealFwdOnlyAssociatorTask. "<<std::endl;
  if (fPidChargedProb->GetEntriesFast() != 0) fPidChargedProb->Clear();
  if (fPidNeutralProb->GetEntriesFast() != 0) fPidNeutralProb->Clear();
  // Get the Candidates
  for(Int_t i=0; i<fPidChargedCand->GetEntriesFast(); i++){
    PndPidCandidate* pidcand = (PndPidCandidate*)fPidChargedCand->At(i);
    PndPidProbability* prob = new((*fPidChargedProb)[i]) PndPidProbability(1.,1.,1.,1.,1.,i);// initializes with equal probability
    if(fVerbose>1) std::cout<<"-I- PndPidIdealFwdOnlyAssociatorTask Charged BEFORE  "<< pidcand->GetLorentzVector().M()<<std::endl;;
    if(! IsForward(pidcand) ) continue;
    DoPidMatch(pidcand,prob);
    if(fVerbose>1) std::cout<<"-I- PndPidIdealFwdOnlyAssociatorTask Charged AFTER   "<< pidcand->GetLorentzVector().M()<<std::endl;;
  }
  for(Int_t i=0; i<fPidNeutralCand->GetEntriesFast(); i++){
    //PndPidCandidate* pidcand = (PndPidCandidate*)fPidNeutralCand->At(i); //[R.K. 01/2017] unused variable
    new((*fPidNeutralProb)[i]) PndPidProbability(1.,1.,1.,1.,1.,i);//PndPidProbability* prob =  //[R.K.03/2017] unused variable
    // initializes with eqaual pdf (no preference)
    //DoPidMatch(pidcand,prob); //TODO match idealy neutral cands, esp. when we have pi0s 
  }
  
}

void PndPidIdealFwdOnlyAssociatorTask::DoPidMatch(PndPidCandidate* pidcand, PndPidProbability* prob)
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
      prob->SetProtonPdf(1.);
      prob->SetKaonPdf(1.);
      prob->SetPionPdf(1.);
      prob->SetMuonPdf(1.);
      prob->SetElectronPdf(1.);
      break;
  }
  
}  


//_________________________________________________________________
Bool_t PndPidIdealFwdOnlyAssociatorTask::IsForward(PndPidCandidate* cand) {
  //--- 
  // Checks if the track is in 5-10 degree in y and x respectively
  // tan(10˚) and tan(5˚) in numbers, don't call trig. functions too often.
  TVector3 vec = cand->GetMomentum();
  if( 0.08748866 * vec.y() > vec.z()) return kFALSE;
  if( 0.17632698 * vec.x() > vec.z()) return kFALSE;
  return kTRUE;
}




//_________________________________________________________________
void PndPidIdealFwdOnlyAssociatorTask::Register() {
  //---
  FairRootManager::Instance()->
  Register(fChargedBranchName,"Pid", fPidChargedProb, kTRUE); 
  FairRootManager::Instance()->
  Register(fNeutralBranchName,"Pid", fPidNeutralProb, kTRUE);
}

//_________________________________________________________________
void PndPidIdealFwdOnlyAssociatorTask::Finish() {
  //---
  //  FairRootManager* ioman = FairRootManager::Instance();
  //  ioman->W
}
//_________________________________________________________________
void PndPidIdealFwdOnlyAssociatorTask::Reset() {
  //---
}


ClassImp(PndPidIdealFwdOnlyAssociatorTask)
