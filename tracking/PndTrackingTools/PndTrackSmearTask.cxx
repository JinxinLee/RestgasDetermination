/*
 * PndTrackSmearTask.cpp
 *
 *  Created on: Jun 26, 2013
 *      Author: schumann
 */

// Root includes
#include "TROOT.h"
#include "TClonesArray.h"
#include "FairRootManager.h"
#include "PndTrack.h"
#include "PndTrackSmearTask.h"
#include "TRandom.h"

ClassImp(PndTrackSmearTask);


void PndTrackSmearTask::SetParContainers() {
}

InitStatus PndTrackSmearTask::Init() {

	FairRootManager* ioman = FairRootManager::Instance();

	if (!ioman) {
		std::cout << "-E- PndTrackSmearTask::Init: "
				<< "RootManager not instantiated!" << std::endl;
		return kFATAL;
	}

	// get track branches
  for(TString branchname : fInputTrackBranches)
  {
	  fTracks.push_back( (TClonesArray*) ioman->GetObject(branchname));
  }

  // create output branches
  for(TString branchname : fOutputTrackBranches)
  {
 	  fOutputTracks.push_back( ioman->Register(branchname, "PndTrack", "Tracks", GetPersistency()) );
  }

	return kSUCCESS;

}

void PndTrackSmearTask::Exec(Option_t *) {

	if (fVerbose > 1) {
		std::cout << "============= Begin PndTrackSmearTask::Exec" << std::endl;
		std::cout << std::endl;
	}
	//FairRootManager* ioman = FairRootManager::Instance();

  //for(TClonesArray* trkArray : fTracks)
  //{
  	//for (int i = 0; i < trkArray->GetEntriesFast(); i++)
    //{
  		//PndTrack* myTrack = (PndTrack*)trkArray->At(i);
      //TVector3 mom = myTrack->Get
    //}
  //}

}

void PndTrackSmearTask::SmearVector(TVector3 &vec, const TVector3 &sigma)
{
  // gaussian smearing
  Double_t rannn=0.;
  rannn = gRandom->Gaus(vec.X(),sigma.X());
  vec.SetX(rannn);

  rannn = gRandom->Gaus(vec.Y(),sigma.Y());
  vec.SetY(rannn);

  rannn = gRandom->Gaus(vec.Z(),sigma.Z());
  vec.SetZ(rannn);

  return;
}


void PndTrackSmearTask::FinishEvent() {
	//if(fTracks != 0) fTracks->Delete();
}

void PndTrackSmearTask::Finish() {
}

