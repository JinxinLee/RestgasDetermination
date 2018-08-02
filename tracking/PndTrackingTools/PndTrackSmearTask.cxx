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
	FairRootManager* ioman = FairRootManager::Instance();
  for(TClonesArray* arr : fOutputTracks) {arr->Delete();}


  for (int i=0;i<fTracks.size();i++)
  {
    TClonesArray* trkArray=fTracks[i];
    TClonesArray* trkOutArray=fOutputTracks[i];
    TString trkbranchname = fInputTrackBranches[i];
    for (int j = 0; j < trkArray->GetEntriesFast(); j++)
    {
  		PndTrack* myTrack = (PndTrack*)trkArray->At(j);
      //PndTrackCand& myTrackCand=myTrack->GetTrackCand();
      PndTrackCand* myTrackCand=myTrack->GetPndTrackCandViaRef();
      new((*trkOutArray)[j]) PndTrack(
    		SmearTrackPar(myTrack->GetParamFirst()), SmearTrackPar(myTrack->GetParamLast()),
				myTrack->GetTrackCand(), myTrack->GetFlag(),
				myTrack->GetChi2(), myTrack->GetNDF(), myTrack->GetPidHypo(),
				j,
				FairRootManager::Instance()->GetBranchId(trkbranchname));    }
  }

}


FairTrackParP PndTrackSmearTask::SmearTrackPar(FairTrackParP par)
{
  TVector3 mom=par.GetMomentum();
  SmearMom(mom);
  Double_t Cov66[6][6];
  par.GetMARSCov(Cov66);
  SmearCov(Cov66);
  FairTrackParP result(par.GetPosition(), mom, Cov66,par.GetQ(), par.GetOrigin(), par.GetJVer(), par.GetKVer());
  return result;
}


void PndTrackSmearTask::SmearMom(TVector3 &vec)
{
  // gaussian smearing
  Double_t rannn=0.;
  rannn = gRandom->Gaus(vec.X(),fMomSigma.X());
  vec.SetX(rannn);

  rannn = gRandom->Gaus(vec.Y(),fMomSigma.Y());
  vec.SetY(rannn);

  rannn = gRandom->Gaus(vec.Z(),fMomSigma.Z());
  vec.SetZ(rannn);

  return;
}


void PndTrackSmearTask::SmearCov(Double_t Cov66[6][6])
{
  Cov66[3][3]+=fMomSigma.X()*fMomSigma.X();
  Cov66[4][4]+=fMomSigma.Y()*fMomSigma.Y();
  Cov66[5][5]+=fMomSigma.Z()*fMomSigma.Z();
  return;
}


void PndTrackSmearTask::FinishEvent() {
	//if(fTracks != 0) fTracks->Delete();
}

void PndTrackSmearTask::Finish() {
}

