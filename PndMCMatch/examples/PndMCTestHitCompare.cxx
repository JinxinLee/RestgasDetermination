// -------------------------------------------------------------------------
// -----                PndMCTestHitCompare source file             -----
// -----                  Created 18/07/08  by T.Stockmanns        -----
// -------------------------------------------------------------------------
// libc includes
#include <iostream>

// Root includes
#include "TROOT.h"
#include "TClonesArray.h"

// framework includes
#include "FairRootManager.h"
#include "PndMCTestHitCompare.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairHit.h"
#include "FairMultiLinkedData.h"

#include "FairMCPoint.h"
#include "FairHit.h"
#include "PndMCEntry.h"

#include "PndDetectorList.h"


// -----   Default constructor   -------------------------------------------
PndMCTestHitCompare::PndMCTestHitCompare(TString hitBranchName, TString pointBranchName)
	: FairTask("Creates PndMC test"), fHits(0), fMCPoints(0), fMCMatch(0),
	  fHitBranchName(hitBranchName), fPointBranchName(pointBranchName)
{
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndMCTestHitCompare::~PndMCTestHitCompare()
{
}

// -----   Public method Init   --------------------------------------------
InitStatus PndMCTestHitCompare::Init()
{


//  fMCMatch->InitStage(kMCTrack, "", "MCTrack");



  FairRootManager* ioman = FairRootManager::Instance();
  	if (!ioman) {
  		std::cout << "-E- PndMCTestHitCompare::Init: "
  				<< "RootManager not instantiated!" << std::endl;
  		return kFATAL;
  	}

  	fMCMatch = (PndMCMatch*)ioman->GetObject("MCMatch");

  	fHits = (TClonesArray*)ioman->GetObject(fHitBranchName);
  	fMCPoints = (TClonesArray*)ioman->GetObject(fPointBranchName);

    fHistoDistZ = new TH1D("HistoDistZ","HistoDistZ", 400, -200, 200);
    fHistoDistR = new TH1D("HistoDistR","HistoDistR", 401, -1, 10);;
    fHistoDist = new TH1D("HistoDist","HistoDist", 401, -1, 200);;

	std::cout << "-I- PndMCTestHitCompare::Init: Initialization successfull" << std::endl;


  return kSUCCESS;
}


// -------------------------------------------------------------------------
void PndMCTestHitCompare::SetParContainers()
{
  // Get Base Container
//  FairRun* ana = FairRun::Instance();
//  FairRuntimeDb* rtdb=ana->GetRuntimeDb();

}


// -----   Public method Exec   --------------------------------------------
void PndMCTestHitCompare::Exec(Option_t* opt)
{
	//fMCMatch->CreateArtificialStage(kMCTrack, "", "");

	//PndMCResult myResult = fMCMatch->GetMCInfo(kMVDHitsStrip, kMVDPoint);
	//std::cout << myResult;
	FairRootManager* ioman = FairRootManager::Instance();
	for (int i = 0; i < fHits->GetEntries(); i++){
		//PndMCEntry myLinks = myResult.GetMCLink(i);
		FairHit* myHit = (FairHit*)fHits->At(i);
		PndMCEntry myLinks = fMCMatch->GetMCInfoSingle(FairLink(fHitBranchName, i), fPointBranchName);
		PndMCEntry myMCLinks = fMCMatch->GetMCInfoSingle(FairLink(fHitBranchName, i), "MCTrack");
		FairMultiLinkedData pointLinks = myLinks.GetLinksWithType(ioman->GetBranchId(fPointBranchName));

		if (pointLinks.GetNLinks() == 2 && myMCLinks.GetNLinks() == 1){

			FairMCPoint* myMCPoint1 = (FairMCPoint*)fMCPoints->At(myLinks.GetLink(0).GetIndex());
			FairMCPoint* myMCPoint2 = (FairMCPoint*)fMCPoints->At(myLinks.GetLink(1).GetIndex());

			TVector3 mcPoint1, mcPoint2;
			myMCPoint1->Position(mcPoint1);
			myMCPoint2->Position(mcPoint2);

			TVector3 temp = (mcPoint1 - mcPoint2);
			temp *= 0.5;

			TVector3 mcPos = mcPoint1;
			mcPos -= temp;

//			std::cout << "MCPos1: " << mcPoint1.x() << "/" << mcPoint1.y() << "/" << mcPoint1.z() << " " <<
//					mcPoint2.x() << "/" << mcPoint2.y() << "/" << mcPoint2.z() << " = " <<
//					mcPos.x() << "/" << mcPos.y() << "/" << mcPos.z() << std::endl;
			TVector3 hitPos;
			myHit->Position(hitPos);

			TVector3 result = hitPos;
			result -= mcPos;


//			std::cout << "HitPos: " << hitPos.x() << "/" << hitPos.y() << "/" << hitPos.z() << std::endl;
//			std::cout << "Difference: " << result.x() << "/" << result.y() << "/" << result.z() << " Mag: " << result.Mag();

			fHistoDistZ->Fill(result.z());
			fHistoDist->Fill(result.Mag());
			result.SetZ(0);
			fHistoDistR->Fill(result.Mag());
//			std::cout << " MagRPhi: " << result.Mag() << std::endl << std::endl;

		}

//		std::cout << fHitBranchName.Data() << " Hit: " << *myHit;
//
//		for (int j = 0; j < myLinks.GetNLinks(); j++){
//			if (myLinks.GetLink(j).GetType() == ioman->GetBranchId(fPointBranchName)){
//				std::cout << "MCPoint " << myLinks.GetLink(j).GetIndex() << std::endl;
//				FairMCPoint* myMCPoint = (FairMCPoint*)fMCPoints->At(myLinks.GetLink(j).GetIndex());
//				//myMCTrack->Print(myLinks.GetFairLink(j).GetIndex());
//				std::cout << fPointBranchName << " " << *myMCPoint;
//				std::cout << "--------------------------------" << std::endl;
//				if (myLinks.GetNLinks() == 2){
//
//					TVector3 pointPos;
//					myMCPoint->Position(pointPos);
//
//					TVector3 result = hitPos;
//
//				}
//			}
//		}
//		std::cout << std::endl;
	}
}

void PndMCTestHitCompare::Finish()
{
	fHistoDistZ->Write();
	fHistoDistR->Write();
	fHistoDist->Write();

	delete(fHistoDistZ);
	delete(fHistoDistR);
	delete(fHistoDist);
}


ClassImp(PndMCTestHitCompare);
