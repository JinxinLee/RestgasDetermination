//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class TrackReadTask
//      see TrackReadTask.h for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "TrackReadTask.h"

// C/C++ Headers ----------------------
#include <map>
#include <algorithm>
#include <string>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "PndMCTrack.h"
#include "Track.h"


// Class Member definitions -----------
#include "AliceHelixTrackRep.h"
#include "SLTrackRep.h"
#include "AbsTrackRep.h"
#include "FitParams.h"


TrackReadTask::TrackReadTask()
  : FairTask("TrackRead"), _persistence(kFALSE)
{
  _trackBranchName = "TrackPostFit";
}


TrackReadTask::~TrackReadTask()
{}

InitStatus
TrackReadTask::Init()
{
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("TrackReadTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input collection
  _trackArray=(TClonesArray*) ioman->GetObject(_trackBranchName);
  
  if(_trackArray==0)
    {
      Error("TrackReadTask::Init","Track-array not found!");
      return kERROR;
    }

  return kSUCCESS;
}


void
TrackReadTask::Exec(Option_t* opt)
{
  _trackArray->Print();
  Track* aTrack = (Track*)  _trackArray->At(0);
  
  if(aTrack)  {
	for(int i=0;i<aTrack->getNumReps();i++) {
	  AbsTrackRep* rep = (AbsTrackRep*)aTrack->getTrackRep(i);
	  std::cout << typeid(rep).name() << std::endl;
	  std::cout << i << "    AliceHelix "<< dynamic_cast<AliceHelixTrackRep*>(rep) << std::endl;
	  std::cout <<      "     SL "<< dynamic_cast<SLTrackRep*>(rep) << std::endl;
	  //	  rep->getState().Print();
	  TMatrixT<double> mat;
	  rep->getFitParams()->getStateFin(45,mat);
	  mat.Print();
	  rep->getFitParams()->getfStateFilt(45,mat);
	  mat.Print();
	}
  }
  
  return;
}

ClassImp(TrackReadTask)
