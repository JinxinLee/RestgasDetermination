//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      pad-wise correction of Digi amplitudes
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Felix Boehmer    TUM            (original author)
//
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndTpcDigiAmpCorrectionTask.h"

// C/C++ Headers ----------------------
#include <iostream>
#include <fstream>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "TClonesArray.h"
#include "PndTpcDigi.h"
#include "PndTpcDigiPar.h"

#include "TError.h"


// Class Member definitions -----------

ClassImp(PndTpcDigiAmpCorrectionTask)


PndTpcDigiAmpCorrectionTask::PndTpcDigiAmpCorrectionTask()
:  FairTask("Digi Amplitude Correction"),
  fPersistence(kFALSE),
  fInputSet(kFALSE),
  fAsciiInput("")
  
{
  fDigiBranchName = "PndTpcDigi";
}



InitStatus
PndTpcDigiAmpCorrectionTask::Init()
{
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("PndTpcDigiAmpCorrectionTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input collection
  fDigiArr=(TClonesArray*) ioman->GetObject(fDigiBranchName);

  if(fDigiArr==0)
    {
      Error("PndTpcDigiAmpCorrectionTask::Init","Cluster-array not found!");
      return kERROR;
    }
  
  return kSUCCESS;
}


void
PndTpcDigiAmpCorrectionTask::SetParContainers() {
  
  std::cout<<"PndTpcDigiAmpCorrectionTask::SetParContainers"<<std::endl;
  std::cout.flush();

  // Get run and runtime database
  FairRun* run = FairRun::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");
  
  FairRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");
  
  // Get PndTpc digitisation parameter container
  fPar= (PndTpcDigiPar*) db->getContainer("PndTpcDigiPar");
  if (! fPar ) Fatal("SetParContainers", "PndTpcDigiPar not found");
}

void
PndTpcDigiAmpCorrectionTask::Exec(Option_t* opt)
{
  
  Int_t nd=fDigiArr->GetEntriesFast();
  
  for(unsigned int id=0; id<nd; id++) {
    PndTpcDigi* digi = (PndTpcDigi*)fDigiArr->At(id);
    double amp = digi->amp();
    int padID = digi->padId();
    if(padID<0) {
      std::cout<<"WARNING: PndTpcDigiAmpCorrectionTask::Exec()"
	       <<"   Invalid PadID - skipping digi"<<std::endl;
      continue;
    }
    double corr = 1.;
    if(fInputSet) 
      corr = fCorrMap[padID];
      
    std::cout<<"Amp before: "<<amp<<"; corr: "<<corr
	     <<"  setting amp: "<<amp*corr<<std::endl;
    digi->amp(amp*corr);
  }
      
  return;
}

void
PndTpcDigiAmpCorrectionTask::readFile() {
  
  ifstream instr;
  instr.open(fAsciiInput.Data());
  int padID;
  double corr;
  
  while(1) {
    instr>>padID>>corr;
    if(instr.eof())
      break;
    //std::cout<<padID<<"   "<<corr<<std::endl;
    if(padID < 0)  {
      Fatal("PndTpcDigiAmpCorrectionTask::readFile()",
	    "Invalid PadID. Aborting");
      return;
    }
    if(fCorrMap.find(padID) != fCorrMap.end()) {
      Fatal("PndTpcDigiAmpCorrectionTask::readFile()",
	    "Inconsistent Input file: ambiguous padID. Aborting");
      return;
    }
    fCorrMap[padID] = corr;
  }

  std::cout<<"PndTpcDigiAmpCorrectionTask::readFile() \n"
	   <<"   ...done. Correction map has "<<fCorrMap.size()
	   <<" entries."<<std::endl;
  
  return;

}


ClassImp(PndTpcDigiAmpCorrectionTask)
