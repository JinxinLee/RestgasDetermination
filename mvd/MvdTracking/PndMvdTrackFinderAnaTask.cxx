#include "PndMvdTrackFinderAnaTask.h"

#include <iostream>

// Root includes
#include "TROOT.h"
#include "TString.h"
#include "TClonesArray.h"
#include "TParticlePDG.h"

// framework includes
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"


// PndMvd includes
// #include "PndMvdGFTrackCand.h"




PndMvdTrackFinderAnaTask::PndMvdTrackFinderAnaTask() : FairTask("MVD Track Finder Ana"), fEventNr(0)
{
	fIdealTrackCandBranch = "MVDIdealTrackCand";
	fRiemannTrackCandBranch = "MVDRiemannTrackCand";
	histo = new TH1I("h1","h1",20,-10,10);
	hitsPerTrack = new TH1I("hitsPerTrack","hitsPerTrack", 11,0,11);
}

PndMvdTrackFinderAnaTask::~PndMvdTrackFinderAnaTask()
{
	delete(histo);
	delete(hitsPerTrack);
}

void PndMvdTrackFinderAnaTask::SetParContainers()
{
}

InitStatus PndMvdTrackFinderAnaTask::ReInit()
{

  InitStatus stat=kSUCCESS;
  return stat;
}

// -----   Public method Init   --------------------------------------------
InitStatus PndMvdTrackFinderAnaTask::Init()
{

  FairRootManager* ioman = FairRootManager::Instance();

  if ( ! ioman )
    {
      std::cout << "-E- PndMvdTrackFinderAna::Init: "
     << "RootManager not instantiated!" << std::endl;
      return kFATAL;
    }

  // Get input array
  fIdealTrackCandArray = (TClonesArray*) ioman->GetObject(fIdealTrackCandBranch);
  if ( !fIdealTrackCandArray){
    std::cout << "-W- PndMvdTrackFinderAnaTask::Init: " << "No IdealTrackCandArray!" << std::endl;
    return kERROR;
  }

  // Get input array
  fRiemannTrackCandArray = (TClonesArray*) ioman->GetObject(fRiemannTrackCandBranch);
  if ( !fRiemannTrackCandArray){
    std::cout << "-W- PndMvdTrackFinderAnaTask::Init: " << "No RiemannTrackCandArray!" << std::endl;
    return kERROR;
  }

  std::cout << "-I- PndMvdTrackFinderAnaTask: Initialisation successfull" << std::endl;
  return kSUCCESS;
}

// -----   Public method Exec   --------------------------------------------
void PndMvdTrackFinderAnaTask::Exec(Option_t* opt)
{

  // Reset output array
  unsigned int detID, hitID;
  int noghosts = 0;
  if ( ! fRiemannTrackCandArray )
    Fatal("Exec", "No RiemannTrackCandArray");
  if ( ! fIdealTrackCandArray )
    Fatal("Exec", "No IdealTrackCandArray");

  fHitMatrix.clear();
  std::map<int, std::vector<int> > hitsInTrack;

  std::cout << "<<<<<<<<<<<< Event " << fEventNr++ << " >>>>>>>>>>>>>>" << std::endl;
  for (int i = 0; i < fRiemannTrackCandArray->GetEntriesFast(); i++){
	  hitsInTrack.clear();
	  GFTrackCand* myTrackCand = (GFTrackCand*)(fRiemannTrackCandArray->At(i));
	  std::cout << "RiemannTrack" << i << ": ";
	  for (unsigned int j = 0; j < myTrackCand->getNHits(); j++){
		  myTrackCand->getHit(j, detID, hitID);
		  std::cout << detID << "/" << hitID << " ";
		  hitsInTrack[detID].push_back(hitID);
	  }
	  fHitMatrix.push_back(hitsInTrack);
	  std::cout << " Curv: " << myTrackCand->getCurv() << " Dip: " << myTrackCand->getDip() << std::endl;
	  //		    << " Inv: " << myTrackCand->inverted() << std::endl;
  }
  std::cout << "HitMatrix.size(): " << fHitMatrix.size() << std::endl;
 // for (int p = 0; p < fHitMatrix.size(); p++){
//	  std::cout << "hits " << p << ": " << fHitMatrix[p][1].size() << std::endl;
//  }

  for (int k = 0; k < fIdealTrackCandArray->GetEntriesFast(); k++){
	  std::map<int, std::vector<int> > IdealHitsInTrack;
  	  GFTrackCand* myIdealTrackCand = (GFTrackCand*)(fIdealTrackCandArray->At(k));
  	  std::cout << "IdealTrack" << k << ": ";
  	  for (unsigned int l = 0; l < myIdealTrackCand->getNHits(); l++){
  		  myIdealTrackCand->getHit(l, detID, hitID);
  		  std::cout << detID << "/" << hitID << " ";
  		  IdealHitsInTrack[detID].push_back(hitID);
  	  }
  	std::cout << " Curv: " << myIdealTrackCand->getCurv() << " Dip: " << myIdealTrackCand->getDip() << std::endl;
  		 // 	  << " Inv: " << myIdealTrackCand->inverted() << std::endl;
  	int foundHits = TrackIncluded(IdealHitsInTrack);
  	int idealHits = NHitsInTrack(IdealHitsInTrack);

  	std::cout << "TrackIncluded: " << foundHits << std::endl;
  	//if (myIdealGFTrackCand->getMCID() < 6)
  		hitsPerTrack->Fill(idealHits);
  	if (idealHits > 2){
/*	  	if (foundHits == NHitsInTrack(IdealHitsInTrack)){
	  		std::cout << "All Hits per Track found!" << std::endl;
	  		histo->Fill(1);
	  	}
	  	else if (foundHits < idealHits){
	  		std::cout << "Some Hits of Track found!" << std::endl;
	  		histo->Fill(2);
	  	}
	  	else if (foundHits > idealHits){
	  		std::cout << "More Hits of Track found!" << std::endl;
	  		histo->Fill(3);
	  	}

  	}
  	else
  		histo->Fill(0);*/
  		histo->Fill(idealHits - foundHits);
  		if (idealHits-foundHits > 2)
  			std::cout << "Many lost hits!" << std::endl;
  		if (foundHits < 3)
  			histo->Fill(-6);
  		noghosts++;
  	}
  	else histo->Fill(-10);
  }
  std::cout << "NoGhosts: " << noghosts;
  std::cout << " fHitMatrix.size() " << fHitMatrix.size() << " result: " << (fHitMatrix.size() - noghosts) << std::endl;
  if ((fHitMatrix.size()-noghosts) < 10)
	  histo->Fill(-8, fHitMatrix.size() - noghosts);
  TCanvas* c1 = new TCanvas();
  c1->Divide(2,1);
  c1->cd(1);
  histo->DrawCopy();
  c1->cd(2);
  hitsPerTrack->DrawCopy();
}

int PndMvdTrackFinderAnaTask::NHitsInTrack(std::map<int, std::vector<int> > cand)
{
	int result;
	result = cand[1].size();
	result += cand[2].size();
	return result;
}

int PndMvdTrackFinderAnaTask::TrackIncluded(std::map<int, std::vector<int> > cand)
{
	int result = 0;
	int oldresult = 0;

	for (unsigned int i = 0; i < fHitMatrix.size(); i++){
		//std::cout << "CheckAgreement to HitMatrixVector " << i << std::endl;
		result = CheckAgreement(cand, fHitMatrix[i]);
		//std::cout << "Result: " << result << std::endl;
		if (result > oldresult) oldresult = result;
	}
	return oldresult;
}


int PndMvdTrackFinderAnaTask::CheckAgreement(std::map<int, std::vector<int> > val1, std::map<int, std::vector<int> > val2)
{
	int result = 0;
	for (int i = 1; i < 3; i++){
		std::vector<int> det1 = val1[i];
		std::vector<int> det2 = val2[i];

		for (unsigned int j = 0; j < det1.size(); j++){
			for (unsigned int k = 0; k < det2.size(); k++){
				//std::cout << "Det1: " << det1[j] << " Det2: " << det2[k] << std::endl;
				if (det1[j] == det2[k]){
					result++;
					continue;
				}
			}
		}
	}
	return result;
}

ClassImp(PndMvdTrackFinderAnaTask);

