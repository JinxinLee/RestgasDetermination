// Panda Headers ----------------------

// This Class' Header ------------------
#include "FitStatTask.h"

// C/C++ Headers ----------------------
#include <vector>
#include <string>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "PndMCTrack.h"
#include "Track.h"


// Class Member definitions -----------
#include "AbsTrackRep.h"
#include "statTree.h"


FitStatTask::FitStatTask()
  : FairTask("FitStat"), _persistence(kFALSE)
{
  stat = new statTreeMaker(std::string("statTree"),
						   std::string("statTree.root"));
  _trackBranchName = "TrackPostFit";
}


FitStatTask::~FitStatTask()
{
  delete stat;
}

InitStatus
FitStatTask::Init()
{
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("FitStatTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input collection
  _trackArray=(TClonesArray*) ioman->GetObject(_trackBranchName);
  
  if(_trackArray==0)
    {
      Error("FitStatTask::Init","Track-array not found!");
      return kERROR;
    }

  return kSUCCESS;
}


void
FitStatTask::Exec(Option_t* opt)
{

  std::vector<Track*> trackVec;
  for(int i=0;i<_trackArray->GetEntriesFast();i++) {
	trackVec.push_back((Track*)  _trackArray->At(i));
  }

  std::cout << "FitStatTask::addTracks ..." << std::endl;
  stat->addTracks(trackVec,0);
  std::cout << "FitStatTask::addTracks done" << std::endl;

  return;
}

ClassImp(FitStatTask)
