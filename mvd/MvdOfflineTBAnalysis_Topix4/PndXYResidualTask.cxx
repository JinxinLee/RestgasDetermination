/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
// -------------------------------------------------------------------------
// -----                PndXYResidualTaskT source file             -----
// -------------------------------------------------------------------------

#include "PndXYResidualTask.h"

#include "FairLink.h"                   // for FairLink
#include "FairRootManager.h"            // for FairRootManager
#include "FairTimeStamp.h"              // for FairTimeStamp
#include "FairRunAna.h"

#include "PndSdsHit.h"

#include "Riosfwd.h"                    // for ostream
#include "TClass.h"                     // for TClass
#include "TClonesArray.h"               // for TClonesArray

#include <iostream>                     // for operator<<, cout, ostream, etc
#include <iomanip>
#include <vector>                       // for vector

InitStatus PndXYResidualTask::ReInit()
{
  return kSUCCESS;
}

// -----   Public method Init   --------------------------------------------
InitStatus PndXYResidualTask::Init()
{

  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    std::cout << "-E- PndXYResidualTaskT::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  // Create and register output array
  fHitArray = (TClonesArray*)FairRootManager::Instance()->GetObject(fBranchName);
  fTrackArray = (TClonesArray*)FairRootManager::Instance()->GetObject("MvdTrack");

  //if(fVerbose>1) { Info("Init","Registering this branch: %s/%s",fFolder.Data(),fOutputBranch.Data()); }
  //fOutputArray = ioman->Register(fOutputBranch, fInputArray->GetClass()->GetName(), fFolder, fPersistance);
  fHc0c0 = new TH2D("fHc0c0", "fHc0c0", 100, -0.1, 0.1, 100, -0.1, 0.1);
  fHc0c1 = new TH2D("fHc0c1", "fHc0c1", 100, -0.1, 0.1, 100, -0.1, 0.1);
  fHc0c2 = new TH2D("fHc0c2", "fHc0c2", 100, -0.1, 0.1, 100, -0.1, 0.1);
  fHc0c3 = new TH2D("fHc0c3", "fHc0c3", 100, -0.1, 0.1, 100, -0.1, 0.1);


  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndXYResidualTask::Exec(Option_t* opt)
{
	if (fTrackArray->GetEntriesFast() > 0){
		for (int i = 0; i < fTrackArray->GetEntriesFast(); i++){
			PndTrack* myTrack = (PndTrack*)fTrackArray->At(i);
			std::vector<PndTrackCandHit> hits = myTrack->GetTrackCandPtr()->GetSortedHits();
			for (int iHits = 0; iHits < hits.size(); iHits++){
				PndSdsHit* myHit = (PndSdsHit*)fHitArray->At(hits[iHits].GetIndex());
				std::cout << "HitValue: " << myHit->GetPosition().X() << "/" << myHit->GetPosition().Y()<< "/" << myHit->GetPosition().Z() << std::endl;

				TVector3 predicted = PropagateToZ(myTrack, myHit->GetZ());
				std::cout << "PredictedValue: " <<predicted.X() << "/" << predicted.Y()<< "/" << predicted.Z() << std::endl;
				TVector3 res = predicted - myHit->GetPosition();
				if (myHit->GetSensorID() == 0) fHc0c0->Fill(res.X(), res.Y());
				if (myHit->GetSensorID() == 1) fHc0c1->Fill(res.X(), res.Y());
				if (myHit->GetSensorID() == 2) fHc0c2->Fill(res.X(), res.Y());
				if (myHit->GetSensorID() == 3) fHc0c3->Fill(res.X(), res.Y());

			}
		}

	}

}

TVector3 PndXYResidualTask::PropagateToZ(PndTrack* aTrack, Double_t z){
	TVector3 origin = aTrack->GetParamFirst().GetOrigin();
	TVector3 dir = aTrack->GetParamFirst().GetMomentum();

	std::cout << "OriginValue: " <<origin.X() << "/" << origin.Y()<< "/" << origin.Z() << std::endl;
	std::cout << "DirValue: " <<dir.X() << "/" << dir.Y()<< "/" << dir.Z() << std::endl;

	Double_t t = (z - origin.Z())/dir.Z();

	return origin + t * dir;
}

// -------------------------------------------------------------------------

void PndXYResidualTask::FinishEvent()
{
 // fOutputArray->Delete();
}

void PndXYResidualTask::FinishTask()
{
	fHc0c0->Write();
	fHc0c1->Write();
	fHc0c2->Write();
	fHc0c3->Write();
}

ClassImp(PndXYResidualTask);
