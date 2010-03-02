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

PndPidAssociatorTask::PndPidAssociatorTask()
  : FairTask("PndPidAssociatorTaskSTD")
{
  std::cout << "<INFO> Call Default task constructor. " 
	    << "(PndPidAssociatorTask)"<< std::endl;
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
  // Clean-up allocated stuff.
  fManager->Write();

  if(fManager) delete fManager;
  if(fPidChargedCand) delete fPidChargedCand;
  if(fPidNeutralCand) delete fPidNeutralCand;
  if(fPidChargedProb) delete fPidChargedProb;
  if(fPidNeutralProb) delete fPidNeutralProb;
  if(fMCTrack) delete  fMCTrack;
  if(knnCls) delete knnCls;
}

//___________________________________________________________
InitStatus PndPidAssociatorTask::Init()
{
  std::cout << "InitStatus PndPidAssociatorTask::Init()" << std::endl;

  fManager = FairRootManager::Instance();

  // Get charged candidates.
  fPidChargedCand = (TClonesArray *)fManager->GetObject("PidChargedCand");
  
  if ( !fPidChargedCand){
    std::cout << "-I- PndPidAssociatorTask::Init: No PidChargedCand there!" 
	      << std::endl;
    return kERROR;
  }
  // Get Neutral candidates.
  fPidNeutralCand = (TClonesArray *)fManager->GetObject("PidNeutralCand");
  
  if ( ! fPidNeutralCand){
    std::cout << "-I- PndPidAssociatorTask::Init: No PidNeutralCand there!"
	      << std::endl;
    return kERROR;
  }
  
  // Get track array.
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
void PndPidAssociatorTask::Exec(Option_t* option)
{
  std::cout << "<INFO> Call to Exec with " << option << std::endl;  
  if(fVerbose > 1){
    std::cout << "-I- Start PndPidAssociatorTask. "<<std::endl;
  }
  // Get the charged Candidates
  for(int i = 0; i < fPidChargedCand->GetEntriesFast(); i++){
    PndPidCandidate* pidcand = (PndPidCandidate*)fPidChargedCand->At(i);
    TClonesArray& pidRef = *fPidChargedProb;
    PndPidProbability* prob = new(pidRef[i]) PndPidProbability();// initializes with zeros
    if(fVerbose > 1){
      std::cout << "-I- PndPidAssociatorTask Ch BEFORE  "
		<< pidcand->GetLorentzVector().M() << std::endl;
    }
    
    DoPidMatch(*pidcand, *prob);
    
    if(fVerbose > 1){
      std::cout << "-I- PndPidAssociatorTask Ch AFTER "
		<< pidcand->GetLorentzVector().M() << std::endl;
    }
  }

  // Get the Neutral Candidates
  for(int i = 0; i < fPidNeutralCand->GetEntriesFast(); i++){
    PndPidCandidate* pidcand = (PndPidCandidate*)fPidNeutralCand->At(i);
    TClonesArray& pidRef = *fPidNeutralProb;
    PndPidProbability* prob = new(pidRef[i]) PndPidProbability();// initializes with zeros

    DoPidMatch(*pidcand, *prob);
  }
}

void PndPidAssociatorTask::DoPidMatch(PndPidCandidate& pidcand, 
				      PndPidProbability& prob)
{
  std::cout << "===================================================="<< std::endl;
  prob.SetElectronPdf(1.);
  std::cout << "Momentum " << (pidcand.GetMomentum()).Mag() << std::endl;
  std::cout << "GetEnergy " << pidcand.GetEnergy() << std::endl;
  std::cout << "EMC " << pidcand.GetEmcCalEnergy() << std::endl;
  std::cout << "EMC/P " 
	    << (pidcand.GetEmcCalEnergy())/((pidcand.GetMomentum()).Mag()) 
	    << std::endl;
  std::cout << "STT " << pidcand.GetSttMeanDEDX() << std::endl;
  std::cout << "MVD " << pidcand.GetMvdDEDX() << std::endl;
  std::cout << "DRC_TC " << pidcand.GetDrcThetaC() << std::endl;
  std::cout << "TPC " << pidcand.GetTpcMeanDEDX() << std::endl;
  std::cout << "===================================================="<< std::endl;
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
