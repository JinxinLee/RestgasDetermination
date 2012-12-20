// -------------------------------------------------------------------------
// -----                PndMCTimebasedPatternRecoQuality source file             -----
// -----                  Created 18/07/08  by T.Stockmanns        -----
// -------------------------------------------------------------------------
// libc includes
#include <iostream>

// Root includes
#include "TROOT.h"
#include "TClonesArray.h"

// framework includes
#include "FairRootManager.h"
#include "PndMCTimebasedPatternRecoQuality.h"
#include "PndMCDataCrawler.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairHit.h"
#include "FairMultiLinkedData.h"
#include "FairEventHeader.h"

#include "PndSdsMCPoint.h"

#include "PndMCTrack.h"
#include "PndTrack.h"
#include "PndMCEntry.h"

// -----   Default constructor   -------------------------------------------
PndMCTimebasedPatternRecoQuality::PndMCTimebasedPatternRecoQuality() :
	FairTask("Creates PndMC test"), fEventNr(0) {
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndMCTimebasedPatternRecoQuality::~PndMCTimebasedPatternRecoQuality() {
	delete fMCCrawler;
}

// -----   Public method Init   --------------------------------------------
InitStatus PndMCTimebasedPatternRecoQuality::Init() {

	//  fMCMatch->InitStage(kMCTrack, "", "MCTrack");

	fPHisto = new TH1D("fPHisto", "Momentum Resolution", 1000, -10, 10);
	fPtHisto = new TH1D("fPtHisto", "Transverse Momentum Resolution", 1000, -10, 10);
	fQualyHisto = new TH1I("fQualyHisto", "Quality of Trackfinding", 11, -0.5, 10.5);
	fTimeHisto = new TH1D("fTimeHisto", "Time Resolution", 2000, -100, 100);

	FairRootManager* ioman = FairRootManager::Instance();
	if (!ioman) {
		std::cout << "-E- PndMCTimebasedPatternRecoQuality::Init: "
				<< "RootManager not instantiated!" << std::endl;
		return kFATAL;
	}

	fMCCrawler = new PndMCDataCrawler();
	fMCCrawler->Init();
	fMCCrawler->SetVerbose(0);
	fMCCrawler->SetStoreAllEndpoints();
	fMCCrawler->SetStoreIntermediate();

	fTrack = (TClonesArray*) ioman->GetObject("MVDTrack");
	fMCTrack = (TClonesArray*) ioman->GetObject("MCTrack");
	fTrackCand = (TClonesArray*) ioman->GetObject("MVDRiemannTrackCand");

	std::cout
			<< "-I- PndMCTimebasedPatternRecoQuality::Init: Initialization successfull"
			<< std::endl;

	FillMCTrackMap();

	return kSUCCESS;
}

void PndMCTimebasedPatternRecoQuality::FillMCTrackMap()
{
	FairRootManager* ioman = FairRootManager::Instance();

	for (int i = 0; i < ioman->GetInChain()->GetEntries() ; i++){
		FairLink tempLink(-1, i, "MVDPoint", 0);
		TClonesArray* myMVDPoints = (TClonesArray*)ioman->GetCloneOfTClonesArray(tempLink);
		if (myMVDPoints > 0) {
//			std::cout << "MVDPoints: " << myMVDPoints->GetEntries() << std::endl;
			for (int pointsIter = 0; pointsIter < myMVDPoints->GetEntries(); pointsIter++){
				PndSdsMCPoint* myPoint = (PndSdsMCPoint*)myMVDPoints->At(pointsIter);
				if (fMCTrackMap[myPoint->GetLink(0)] == 0){
					fMCTrackMap[myPoint->GetLink(0)] = new PndTrackCand();
					fMCTrackQualityMap[myPoint->GetLink(0)];
				}
				fMCTrackMap[myPoint->GetLink(0)]->AddHit(myPoint->GetLink(0), myPoint->GetLink(0).GetEntry());
			}
		}
	}
}

// -------------------------------------------------------------------------
void PndMCTimebasedPatternRecoQuality::SetParContainers() {
	// Get Base Container
	//  FairRun* ana = FairRun::Instance();
	//  FairRuntimeDb* rtdb=ana->GetRuntimeDb();

}

// -----   Public method Exec   --------------------------------------------
void PndMCTimebasedPatternRecoQuality::Exec(Option_t* opt) {
	//fMCMatch->CreateArtificialStage(kMCTrack, "", "");
	FairRootManager* ioman = FairRootManager::Instance();
	std::cout << "----- Event " << fEventNr << " ------" << std::endl;
	fEventNr++;


	for (int i = 0; i < fTrackCand->GetEntriesFast(); i++){
		PndTrack* myTrack = (PndTrack*)fTrack->At(i);
		FairMultiLinkedData myTrackLinks = *(FairMultiLinkedData*)myTrack;
		FairMultiLinkedData result = fMCCrawler->GetInfo(myTrackLinks, "MCTrack");
		std::cout << "Result MCMatch: " << result << std::endl;
		std::cout << "TrackMatches: " << result.GetLinksWithType(0) << std::endl;
		if (result.GetLinksWithType(0).GetNLinks() == 1){
			std::cout << "MVDPoints: " << result.GetLinksWithType(2).GetNLinks() << std::endl;
			if (result.GetLinksWithType(2).GetNLinks() > 2){
//				fQualyHisto->Fill(8);
				if (fMCTrackQualityMap[result.GetLinksWithType(0).GetLink(0)] < 8){
					fMCTrackQualityMap[result.GetLinksWithType(0).GetLink(0)] = 8;
				}
				std::cout << "GoodTrack found!" << std::endl;
				if (fMCTrackMap.count(result.GetLinksWithType(0).GetLink(0)) > 0){
					std::cout << "fMCTrackMap found for link " << result.GetLinksWithType(0).GetLink(0) << std::endl;
					std::cout << "TrackCand hits: " << result.GetLinksWithType(2).GetNLinks() << " MCHits: " << fMCTrackMap[result.GetLinksWithType(0).GetLink(0)]->GetNHits() << std::endl;
					if (result.GetLinksWithType(2).GetNLinks() == fMCTrackMap[result.GetLinksWithType(0).GetLink(0)]->GetNHits()){
				//		if (fMCTrackQualityMap[result.GetLinksWithType(0).GetLink(0)] < 0){
							fMCTrackQualityMap[result.GetLinksWithType(0).GetLink(0)] = 9;
				//		}
					}
					PndMCTrack* mcTrack = (PndMCTrack*)FairRootManager::Instance()->GetCloneOfLinkData(result.GetLinksWithType(0).GetLink(0));
					std::cout << "RecoTrackMomentum: " << myTrack->GetParamFirst().GetMomentum().Mag() << " MCMomentum: " << mcTrack->GetMomentum().Mag() << std::endl;
					fPHisto->Fill(myTrack->GetParamFirst().GetMomentum().Mag() - mcTrack->GetMomentum().Mag());
					fPtHisto->Fill(myTrack->GetParamFirst().GetMomentum().Pt() - mcTrack->GetMomentum().Pt());
					FairEventHeader* header = (FairEventHeader*)ioman->GetCloneOfLinkData(FairLink(-1, result.GetLinksWithType(2).GetLink(0).GetEntry(), "EventHeader.", -1));
					std::cout << "MCEventTime: " << header->GetEventTime() << " measured Time: " << myTrack->GetTimeStamp() << std::endl;
					fTimeHisto->Fill(header->GetEventTime() - myTrack->GetTimeStamp());
					delete mcTrack;
				}
			}
			else {
				fQualyHisto->Fill(5);
				std::cout << "GhostTrack found!" << std::endl;
			}
		}
		else if (result.GetLinksWithType(0).GetNLinks() == 2){
			std::map<FairLink, Int_t> mapSpurious;
			FairMultiLinkedData pixelPoints = result.GetLinksWithType(2);
			for (int j = 0; j < pixelPoints.GetNLinks(); j++){
				PndSdsMCPoint *mcPoint = (PndSdsMCPoint*)FairRootManager::Instance()->GetCloneOfLinkData(pixelPoints.GetLink(j));
				mapSpurious[mcPoint->GetLink(0)]++;
				delete mcPoint;
			}
			bool atLeastThree = false;
			bool justOne = false;
			Int_t whichTrack = -1, trackIter = 0;
			for (std::map<FairLink, Int_t>::const_iterator iter = mapSpurious.begin(); iter != mapSpurious.end(); iter++, trackIter++){
				std::cout << "Link: " << iter->first << " count: " << iter->second << std::endl;
				if (iter->second > 2){
					atLeastThree = true;
					whichTrack = trackIter;
				}
				else if (iter->second == 1){
					justOne = true;
				}
			}
			std::cout << "WhichTrack: " << whichTrack << std::endl;
			if (atLeastThree && justOne){
				if (fMCTrackQualityMap[result.GetLinksWithType(0).GetLink(whichTrack)] < 7){
					fMCTrackQualityMap[result.GetLinksWithType(0).GetLink(whichTrack)] = 7;
				}
				std::cout << "SpuriousTrack found!" << std::endl;
			}
			else {
				fQualyHisto->Fill(5);
				std::cout << "GhostTrack found!" << std::endl;
			}
		}
		else if (result.GetLinksWithType(0).GetNLinks() > 2){
			fQualyHisto->Fill(5);
			std::cout << "GhostTrack found!" << std::endl;

		}
	}



//	double dP = 0, P = 0;
//	double dPt = 0, Pt = 0;
//	myResult.RemoveType(-1);															//get rid of artificial hit at (0,0,0)
//
//	for (int i = 0; i < myResult.GetNEntries(); i++){
//		if (myResult.GetEntry(i).GetNLinks() > 1){										//if the reco track has hits from different MC tracks it is regarded as ghost --> field 5
//			std::cout << myResult.GetEntry(i) << " No Match! Fill 5!" << std::endl;
//			fQualyHisto->Fill(5);
//		}
//	}
//	for (int i = 0; i < fMCTrack->GetEntries() && i < forward1.GetNEntries(); i++) {
//		PndMCTrack* myTrack = (PndMCTrack*)fMCTrack->At(i);
//		if (myTrack->GetMotherID() < 0 && forward1.GetEntry(i).GetNLinks() > 2) {		//only primaries and only tracks with more than 2 hits
//			std::cout << "Forward: " << i << ": " << forward1.GetEntry(i) << std::endl;
//			fQualyHisto->Fill(0);
//			for (int j = 0; j < myResult.GetNEntries(); j++) {
//
//				if (myResult.GetEntry(j).GetNLinks() == 1) {							//if only hits from one MC track are in the found track
//					std::cout << "Result " << j << ": " << myResult.GetEntry(j) << std::endl;
//					if (myResult.GetEntry(j).GetLink(0).GetIndex() == i){
//						std::cout << "NLinks " << forward1.GetEntry(i).GetNLinks() << " Weight: " << myResult.GetEntry(j).GetLink(0).GetWeight() << std::endl;
//						if (forward1.GetEntry(i).GetNLinks() == myResult.GetEntry(j).GetLink(0).GetWeight()) {
//							fQualyHisto->Fill(9);										//if all hits in the track are found fill index 9
//							std::cout << "Fill 9" << std::endl;
//							break;
//						} else {
//							fQualyHisto->Fill(8);										//if not all hits in the track are found fill index 8
//							std::cout << "Fill 8" << std::endl;
//							break;
//						}
//					}
//				}
//				else if (myResult.GetEntry(j).GetNLinks() == 2){						//test of spurious hits, if you have hits from two different MC Tracks in your reco track
//					Float_t weight1 = myResult.GetEntry(j).GetLink(0).GetWeight();
//					Float_t weight2 = myResult.GetEntry(j).GetLink(1).GetWeight();
//					if ((weight1 > 2 && weight2 == 1) || (weight1 == 1 && weight2 > 2)){		//if you have at least three hits from one MC track and only one hit from a different track
//						int mcIndex = -1;
//						if (weight1 > 2){
//							mcIndex = myResult.GetEntry(j).GetLink(0).GetIndex();
//						}
//						else{
//							mcIndex = myResult.GetEntry(j).GetLink(1).GetIndex();
//						}
//						if (mcIndex == i){
//							fQualyHisto->Fill(7);										//fill index 7
//							std::cout << "Fill 7" << std::endl;
//							break;
//						}
//					}
//				}
//			}
//		}
//	}
//	for (int i = 0; i < myResult.GetNEntries(); i++) {									//momentum match does not work at the moment because the MC indices are rearrange in the recent code
//		PndMCEntry myLinks = myResult.GetMCLink(i);
//		myLinks.RemoveType(-1);
//		PndTrack* myTrack = (PndTrack*) fTrack->At(i);
//		PndTrackCand* myTrackCand = (PndTrackCand*) fTrackCand->At(
//				myTrack->GetLink(0).GetIndex());
//		if (fVerbose > 2)std::cout << "TrackMatch for Track " << i << std::endl;
//		//myTrackCand->Print();
//		if (fVerbose > 2)myTrack->GetParamFirst().Print();
//		P = dP = myTrack->GetParamFirst().GetMomentum().Mag();
//		Pt = dPt = myTrack->GetParamFirst().GetMomentum().Pt();
//		//myTrackCand->Print();
//
//		if (fVerbose > 2)std::cout << "Belongs to: " << std::endl;
//
//		for (int j = 0; j < myLinks.GetNLinks(); j++) {
//			if (myLinks.GetLink(j).GetType() == ioman->GetBranchId("MCTrack")) {
//				if (fVerbose > 2)std::cout << "MCTrack " << myLinks.GetLink(j).GetIndex()
//						<< std::endl;
//				if (myLinks.GetLink(j).GetIndex() < fMCTrack->GetEntries()) {
//					PndMCTrack* myMCTrack = (PndMCTrack*) fMCTrack->At(
//							myLinks.GetLink(j).GetIndex());
//					if (fVerbose > 2)std::cout << "P: " << myMCTrack->Get4Momentum().Px() << " "
//							<< myMCTrack->Get4Momentum().Py() << " "
//							<< myMCTrack->Get4Momentum().Pz() << " Pabs "
//							<< myMCTrack->Get4Momentum().P() << " PID: "
//							<< myMCTrack->GetPdgCode() << std::endl;
//					if (fVerbose > 2)std::cout << "--------------------------------"
//							<< std::endl;
//					dP -= myMCTrack->Get4Momentum().P();
//					dPt -= myMCTrack->Get4Momentum().Pt();
//					fPHisto->Fill((dP / P));
//					fPtHisto->Fill((dPt / Pt));
//				} else
//					std::cout << "Index out of bounds: Index : "
//							<< myLinks.GetLink(j).GetIndex() << " Bounds: "
//							<< fMCTrack->GetEntries() << std::endl;
//			}
//		}
//		if (fVerbose > 2)std::cout << std::endl;
//	}
}

void PndMCTimebasedPatternRecoQuality::Finish() {
	for (std::map<FairLink, Int_t>::const_iterator iter = fMCTrackQualityMap.begin(); iter != fMCTrackQualityMap.end(); iter++){
			PndMCTrack* myTrack = (PndMCTrack*)FairRootManager::Instance()->GetCloneOfLinkData(iter->first);
			std::cout << iter->first << " qualy: " << iter->second << " NHits: " << fMCTrackMap[iter->first]->GetNHits() << " Primary: " << myTrack->GetMotherID() << std::endl;
			if (iter->second > 0)
				fQualyHisto->Fill(iter->second);
			if (fMCTrackMap[iter->first]->GetNHits() > 3 || (fMCTrackMap[iter->first]->GetNHits() == 3 && myTrack->GetMotherID() < 0)){
				fQualyHisto->Fill(0);
			}
//		}
	}
	fPHisto->Write();
	fPtHisto->Write();
	fQualyHisto->Write();
	fTimeHisto->Write();
	std::cout << "fQualyHisto: NPossible Tracks " << fQualyHisto->GetBinContent(1)
			  << " Ghosts: "    << fQualyHisto->GetBinContent(6)
			  << " Spurious: "  << fQualyHisto->GetBinContent(8)
			  << " Found: " 	<< fQualyHisto->GetBinContent(9)
			  << " FullyReco: "	<< fQualyHisto->GetBinContent(10) << std::endl;
}

ClassImp( PndMCTimebasedPatternRecoQuality);
