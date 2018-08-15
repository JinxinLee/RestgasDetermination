#include "PndT0CandidateDetermination.h"

#include <iostream>

// Root includes
// framework includes
#include "FairRootManager.h"
#include "FairTimeStamp.h"



PndT0CandidateDetermination::PndT0CandidateDetermination(TString inputBranch) :
  FairTask("PndT0CandidateDeterminationTask")
{
  fPersistence = kTRUE;
  fRunContinuous = kTRUE;
  fOutBranchName = "T0Candidates";
  fInBranch = inputBranch;
  fBlockedTime = 4;
  fStartTime = -10;
}

PndT0CandidateDetermination::~PndT0CandidateDetermination()
{
}



// -----   Public method Init   --------------------------------------------
InitStatus PndT0CandidateDetermination::Init()
{
  FairRootManager* ioman = FairRootManager::Instance();

  if ( ! ioman )
    {
      std::cout << "-E- PndT0CandidateDetermination::Init: "
     << "RootManager not instantiated!" << std::endl;
      return kFATAL;
    }
 

  fInArray = (TClonesArray*)ioman->GetObject(fInBranch.Data());

  if ( ! fInArray ) 
    {
      std::cout << "-W- PndT0CandidateDetermination::Init: "
		<< "No input branch found!" << std::endl;
      return kERROR;
  }
  
  fOutArray = ioman->Register(fOutBranchName.Data(), "FairTimeStamp", "EventDetermination", fPersistence);

  return kSUCCESS;
}


// -----   Public method Exec   --------------------------------------------
void PndT0CandidateDetermination::Exec(Option_t*)
{
  // Declare some variables

  FairTimeStamp *inStamp;
  double time;

  // Reset output array
  fOutArray->Clear();
  if (fInArray->GetEntriesFast()==0) return;
  
  if(fRunContinuous==kFALSE){  // ---  for Event based Simulation     
    inStamp = (FairTimeStamp*) fInArray->At(0);         
    new ((*fOutArray)[fOutArray->GetEntries()]) FairTimeStamp(*(inStamp));   
  }
  else{  // for time based simulation or continuous read out

    for (int j = 0; j < fInArray->GetEntriesFast(); j++){
      inStamp = (FairTimeStamp*) fInArray->At(j);
      time = inStamp->GetTimeStamp();
      if (time >= fStartTime){  // check if timeStamp can trigger a new T0
	
	new ((*fOutArray)[fOutArray->GetEntries()]) FairTimeStamp(*(inStamp));   	
	fStartTime = time+fBlockedTime; // set new StartTime for potential T0s. 
      }

    }  
    
  }
}


void PndT0CandidateDetermination::FinishTask(){

  if(fRunContinuous==kTRUE){
    // read in next to last "event" and write out the missing data

    FairRootManager* ioman = FairRootManager::Instance();
    fInArray = (TClonesArray*)ioman->GetObject(fInBranch.Data());

    Exec("0");
    FairRootManager::Instance()->SetLastFill();

  } 
  //fOutArray->Delete();
}


ClassImp(PndT0CandidateDetermination);

