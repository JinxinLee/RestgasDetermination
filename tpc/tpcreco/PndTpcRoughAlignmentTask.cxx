//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndRoughAlignmentTask
//      see PndRoughAlignmentTask.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Felix Boehmer
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndTpcRoughAlignmentTask.h"

// C/C++ Headers ----------------------
#include <iostream>


// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "PndTpcCluster.h"

// Class Member definitions -----------


PndTpcRoughAlignmentTask::PndTpcRoughAlignmentTask()
  : FairTask("TPC Rough Alignemt"), fPersistence(kTRUE)
{
  fClusterBranchName = "PndTpcCluster";
}


PndTpcRoughAlignmentTask::~PndTpcRoughAlignmentTask()
{
  ;
}

InitStatus
PndTpcRoughAlignmentTask::Init()
{
  
  
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();
  
  if(ioman==0)
    {
      Error("PndTpcRoughAlignmentTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input collection
  fClusterArray=(TClonesArray*) ioman->GetObject(fClusterBranchName);

  if(fClusterArray==0)
    {
      Error("PndTpcRoughAlignmentTask::Init","track-array not found!");
      return kERROR;
    }
 
  return kSUCCESS;
}


void
PndTpcRoughAlignmentTask::Exec(Option_t* opt)
{
  std::cout<<"PndTpcRoughAlignmentTask::Exec"<<std::endl;
  
  unsigned int ncls = fClusterArray->GetEntriesFast();
  
  //loop over clusters
  for(unsigned int icl=0; icl<ncls; icl++) {
    PndTpcCluster* cl = (PndTpcCluster*) (*fClusterArray)[icl];
    TVector3 pos = cl->pos();
    TVector3 newpos = pos+fShift;
    cl->SetPos(newpos);    

  }//end loop over clusters
  
  return;
}

  



ClassImp(PndTpcRoughAlignmentTask);
