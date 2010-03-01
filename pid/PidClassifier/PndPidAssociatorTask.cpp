/* ************************************
 *  Author: M. Babai (M.Babai@rug.nl) *
 *                                    *
 *  pid classifier                    *
 *                                    *
 * Created: 23-03-2010                *
 * Modified:                          *
 *                                    *
 * ************************************/
#include "PndPidAssociatorTask.h"

// Fair- & Panda-Root includes
#include "PndDetectorList.h"
#include "PndPidCandidate.h"
#include "PndPidProbability.h"
#include "PndMCTrack.h"


//Root Includes
#include "TDatabasePDG.h"

//standard C++ includes

PndPidAssociatorTask::PndPidAssociatorTask()
: FairTask("PidStandardTask") 
{
  std::cout << "<INFO> Call standard task constructor." << std::endl;
  // Init charged and neutral probab. containers.
  fPidChargedProb = new TClonesArray("PndPidProbability");
  fPidNeutralProb = new TClonesArray("PndPidProbability");
}

//___________________________________________________________
PndPidAssociatorTask::PndPidAssociatorTask(const char *name, const char *title)
: FairTask(name) 
{
  std::cout << title << std::endl;
  // Init charged and neutral probab. containers.
  fPidChargedProb = new TClonesArray("PndPidProbability");
  fPidNeutralProb = new TClonesArray("PndPidProbability");
}

//___________________________________________________________
PndPidAssociatorTask::~PndPidAssociatorTask()
{
  //
  //  FairRootManager *fManager =FairRootManager::Instance();
  fManager->Write();
  if(fManager){
    delete fManager;
  }
}

//___________________________________________________________
InitStatus PndPidAssociatorTask::Init()
{
  std::cout << "InitStatus PndPidAssociatorTask::Init()" << std::endl;
  
  //FairRootManager *fManager =FairRootManager::Instance();
  fManager = FairRootManager::Instance();

  // TODO: Am I allowed to write in these Arrays?
  fPidChargedCand = (TClonesArray *)fManager->GetObject("PidChargedCand");
  
  if ( !fPidChargedCand){
    std::cout << "-I- PndPidAssociatorTask::Init: No PidChargedCand there!" 
	      << std::endl;
    return kERROR;
  }

  fPidNeutralCand = (TClonesArray *)fManager->GetObject("PidNeutralCand");
  
  if ( ! fPidNeutralCand){
    std::cout << "-I- PndPidAssociatorTask::Init: No PidNeutralCand there!"
	      << std::endl;
    return kERROR;
  }
  
  fMCTrack = (TClonesArray*) fManager->GetObject("MCTrack");
  if ( !fMCTrack){
    std::cout << "-I- PndPidAssociatorTask::Init: No MC Track array there!" << std::endl;
    return kERROR;
  }
  
  Register();
  
  std::cout << "-I- PndPidAssociatorTask::Init: Success!" << std::endl;
  
  return kSUCCESS;
}

//______________________________________________________
void PndPidAssociatorTask::SetParContainers()
{}
//______________________________________________________
void PndPidAssociatorTask::Exec(Option_t * option)
{
  std::cout << "<INFO> Call to Exec with " << option << std::endl;  
  if(fVerbose > 1){
    std::cout << "-I- Start PndPidAssociatorTask. "<<std::endl;
  }
  // Get the Candidates
  for(int i = 0; i < fPidChargedCand->GetEntriesFast(); i++){
    PndPidCandidate* pidcand = (PndPidCandidate*)fPidChargedCand->At(i);
    TClonesArray& pidRef = *fPidChargedProb;
    PndPidProbability* prob = new(pidRef[i]) PndPidProbability();// initializes with zeros
    if(fVerbose > 1) std::cout << "-I- PndPidAssociatorTask Ch BEFORE  "
			      << pidcand->GetLorentzVector().M() << std::endl;
    DoPidMatch(pidcand,prob);
    
    if(fVerbose > 1) std::cout << "-I- PndPidAssociatorTask Ch AFTER "
			       << pidcand->GetLorentzVector().M() << std::endl;
  }

  for(int i = 0; i < fPidNeutralCand->GetEntriesFast(); i++){
    PndPidCandidate* pidcand = (PndPidCandidate*)fPidNeutralCand->At(i);
    TClonesArray& pidRef = *fPidNeutralProb;
    PndPidProbability* prob = new(pidRef[i]) PndPidProbability();// initializes with zeros

    DoPidMatch(pidcand,prob);
  }
}

void PndPidAssociatorTask::DoPidMatch(PndPidCandidate* pidcand, 
				      PndPidProbability* prob)
{
  // Cheating for each Pid Candidate.
  
  int mcid = pidcand->GetMcIndex();
  if(-1 == mcid)
    return; // no specified MC id... do nothing
  
  PndMCTrack *mctrack = (PndMCTrack*)fMCTrack->At(mcid);
  
  if( 0 == mctrack)
    return; // better do nothing on a null pointer

  int mcpdg = mctrack->GetPdgCode();
  
  TLorentzVector lv = pidcand->GetLorentzVector();
  Double_t mass = TDatabasePDG::Instance()->GetParticle(mcpdg)->Mass();
  lv.SetXYZM(lv.X(), lv.Y(), lv.Z(), mass);

  pidcand->SetLorentzVector(lv);
  
  // cheat around with the probabilities...
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
void PndPidAssociatorTask::Register()
{
  //---
  FairRootManager::Instance()->Register("PidChargedProbability","Pid",
					fPidChargedProb, kTRUE); 
  FairRootManager::Instance()->Register("PidNeutralProbability","Pid", 
					fPidNeutralProb, kTRUE);
}

//_________________________________________________________________
void PndPidAssociatorTask::Finish()
{
  //  FairRootManager* ioman = FairRootManager::Instance();
  //  ioman->W
}
//_________________________________________________________________
void PndPidAssociatorTask::Reset()
{}

ClassImp(PndPidAssociatorTask)
