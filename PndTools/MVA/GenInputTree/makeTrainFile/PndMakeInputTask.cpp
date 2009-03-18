#include "PndMakeInputTask.h"

// Default constructor
PndMakeInputTask::PndMakeInputTask(){}

PndMakeInputTask::PndMakeInputTask(std::string treeName)
{
  // Init a container to hold the variables.
  p = tof = emc = stt = mvd = thetaC = 0.0;
  const char* name = treeName.c_str();
  dataTree = new TTree(name,"TreeTitle");
  
  // Define the tree branches
  dataTree->Branch("p"     , &p      ,"p/F");
  dataTree->Branch("tof"   , &tof    ,"tof/F");
  dataTree->Branch("emc"   , &emc    ,"emc/F");
  dataTree->Branch("stt"   , &stt    ,"stt/F");
  dataTree->Branch("mvd"   , &mvd    ,"mvd/F");
  dataTree->Branch("thetac", &thetaC ,"thetac/F");
}

// Default destructor
PndMakeInputTask::~PndMakeInputTask()
{
  std::cout << "\n\n Destructor.\n\n"<< std::endl;
  dataTree->Write();
  delete dataTree;
}

/*
 * Init call of the task inherited from the FairTask. Registers the
 * output of the track and get the inputs of the track. Input is
 * LhePidTrack Output is PndPidCand
 */
InitStatus PndMakeInputTask::Init()
{
  FairRootManager* ioman = FairRootManager::Instance();
  if (!ioman){
    std::cout << "-E- PndMakeInputTask::Init: "
	      << "RootManager not instantised!" << std::endl;
    return kFATAL;
  }
  
  fPidTrackCand = (TClonesArray*) ioman->GetObject("LhePidTrack");
  
  if (!fPidTrackCand) {
    std::cout << "-W- PndMakeInputTask::Init: "
	      << "No SamEvt array!" << std::endl;
    return kERROR;
  }
  
  std::cout << "-I- Finished Init of PndGpidTask." << std::endl;
  return kSUCCESS;
}

// Exec call of the task
void PndMakeInputTask::Exec(Option_t* opt)
{
  std::cout << "Number of available tracks " 
	    << fPidTrackCand->GetEntriesFast()
	    << std::endl;
  
  // Loop through the Tracks.  
  for (int k = 0; k < fPidTrackCand->GetEntriesFast(); k++){
    
    // Select the current track
    PndLhePidTrack *pid = (PndLhePidTrack *) fPidTrackCand->At(k);
    
    int skip = 0 ;
    
    std::cout << "<INFO:> Track number is "<< k << std::endl;
    
    // TOF 
    tof = pid->GetTof();
    if (tof <= 0) {skip = 1;}
    
    // EMC
    emc  = pid->GetEmcELoss()/pid->GetP();
    if (emc <= 0 ){skip = 1;}
    
    // STT      
    stt = pid->GetSttDEDX();
    if ( stt <= 0 || isnan(stt)){ skip = 1;}
    
    // MVD
    mvd = pid->GetMvdDEDX();
    if (mvd <= 0 ){ skip = 1;}
    
    //ThetaC
    thetaC = pid->GetDrcThetaC();
    if (thetaC <= 0 ){ skip = 1;}
    
    // P
    p = pid->GetMomentum().Mag();
    if (p <= 0 ){ skip = 1;}
    
    // Check if something is wrong, skip the track
    if (skip != 0 ){
      std::cerr << "\t<WARNING:> Skipping Track " 
		<< k << ".\n\tOne or more values are not valid numbers."
		<< std::endl;
      continue;
    }
    else{
      dataTree->Fill();
    }//End if
  }//End Tracks Loop

  //std::cout << "<INFO:> Einde van exec." << std::endl;
  //dataTree->Write();
}//End Exec
ClassImp(PndMakeInputTask);
