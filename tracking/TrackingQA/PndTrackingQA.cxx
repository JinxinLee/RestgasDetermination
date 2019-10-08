/*
 * PndTrackingQA.cxx
 *
 *  Created on: Aug 23, 2013
 *      Author: stockman
 */

#include <PndTrackingQA.h>
#include "PndTrack.h"
#include "PndMCTrack.h"
#include "FairHit.h"
#include "FairMultiLinkedData_Interface.h"

ClassImp(PndTrackingQA);

PndTrackingQA::PndTrackingQA (TString trackBranchName, TString idealTrackName, Bool_t pndTrackData):
    	fTrackBranchName(trackBranchName), fIdealTrackName(idealTrackName), fPndTrackOrTrackCand(pndTrackData), fPossibleTrack(0), fCleanFunctor(kFALSE), fNGhosts(0), fNClones(0), fUseCorrectedSkewedHits(kFALSE), fRunTimeBased(kFALSE), fVerbose(0)
{
	if(fPossibleTrack == 0){
		std::cout << "-I- PndTrackingQA::PndTrackingQA no PossibleTrackFunctor given. Taking Standard!" << std::endl;
		if (trackBranchName == "MVDTrack" ){
			fPossibleTrack = new RiemannMvdSttGemFunctor();
		} else if (trackBranchName == "CombiTrackCand" ) {
			fPossibleTrack = new OnlySttFunctor();
		} else {
			fPossibleTrack = new StandardTrackFunctor();
		}
		fCleanFunctor = kTRUE;
	}
}

PndTrackingQA::PndTrackingQA (TString trackBranchName, TString idealTrackName, PndTrackFunctor* posTrack, Bool_t pndTrackData):
		fTrackBranchName(trackBranchName), fIdealTrackName(idealTrackName), fPndTrackOrTrackCand(pndTrackData), fPossibleTrack(posTrack), fCleanFunctor(kFALSE), fNGhosts(0), fNClones(0), fUseCorrectedSkewedHits(kFALSE), fRunTimeBased(kFALSE), fVerbose(0)
{
	if(fPossibleTrack == 0){
		std::cout << "-I- PndTrackingQA::PndTrackingQA no PossibleTrackFunctor given. Taking Standard!" << std::endl;
		if (trackBranchName == "MVDTrack" ){
			fPossibleTrack = new RiemannMvdSttGemFunctor();
		} else if (trackBranchName == "CombiTrackCand" ) {
			fPossibleTrack = new OnlySttFunctor();
		} else {
			fPossibleTrack = new StandardTrackFunctor();
		}
		fCleanFunctor = kTRUE;
	}
}

PndTrackingQA::~PndTrackingQA()
{
	if (fCleanFunctor)
		delete (fPossibleTrack);
}

void PndTrackingQA::Init()
{
	ioman = FairRootManager::Instance();
	if (!ioman) {
		std::cout << "-E- PndTrackingQualityTask::Init: "
				<< "RootManager not instantiated!" << std::endl;
		return;
	}

	fTrack = (TClonesArray*) ioman->GetObject(fTrackBranchName);
	fMCTrack = (TClonesArray*) ioman->GetObject("MCTrack");
	fIdealTrack = (TClonesArray*) ioman->GetObject(fIdealTrackName);

	// J.R The track cands below was added by me
	fIdealTrackCand = (TClonesArray*) ioman->GetObject("IdealTrackCand");
	fTrackCand = (TClonesArray*) ioman->GetObject("CombiTrackCand");


	if (fBranchNames.size() == 0){
		AddHitsBranchName("MVDHitsPixel");
		AddHitsBranchName("MVDHitsStrip");
		AddHitsBranchName("STTHit");
		AddHitsBranchName("GEMHit");
		AddHitsBranchName("FTSHit");

		//	if (FairRootManager::Instance()->GetBranchId("CorrectedSkewedHits")  > 0){
		//		AddHitsBranchName("CorrectedSkewedHits");
		//	}
	}

	if (fVerbose > 0){
		std::cout << "-I- PndTrackingQA::Init: PossibleTrackFunctor: ";
		fPossibleTrack->Print();
	}
}

void PndTrackingQA::AnalyseEvent(TClonesArray *recoTrackInfo)
{

	FillMapTrackQualifikation();

	if (fVerbose > 2){
		std::cout << "PndTrackingQA::AnalyseEvent() Track quality map before analysis: " << std::endl << std::endl;
		PrintTrackQualityMap(kTRUE);
	}

/* 	std::cout << " " << std::endl;
	std::cout << "Ideal track: " << std::endl;
	for (Int_t i = 0; i < fIdealTrack->GetEntriesFast(); i++){
		PndTrack *currentTrack = (PndTrack *) fIdealTrack->At(i);
		std::cout << "EntryNr(): " << currentTrack->GetEntryNr() << std::endl;
	}  */

	for (Int_t i = 0; i < fTrack->GetEntriesFast(); i++){
		if (fVerbose > 0){
			std::cout << "----------------------------------" << std::endl;
			std::cout << "Analyze Track: " << i << std::endl;
		}
		std::map<TString, FairMultiLinkedData> trackInfo;

		FairLink mostProbableTrackFairLink;
		Int_t mostProbableTrack;

		if (fPndTrackOrTrackCand){

			PndTrack* myTrack = (PndTrack*)fTrack->At(i);
			trackInfo = AnalyseTrackCand(myTrack->GetTrackCandPtr());

			if(fRunTimeBased){
				mostProbableTrackFairLink = AnalyseTrackInfoTimeBased(trackInfo, myTrack->GetEntryNr());
			}
			if(!fRunTimeBased){
				mostProbableTrack = AnalyseTrackInfo(trackInfo, i);
			}

		} else {
			PndTrackCand* myTrack;
			myTrack = (PndTrackCand*)fTrack->At(i);
			trackInfo = AnalyseTrackCand(myTrack);
			if(fRunTimeBased){

				mostProbableTrackFairLink = AnalyseTrackInfoTimeBased(trackInfo, (FairLink) myTrack->GetEntryNr());

			}
			if(!fRunTimeBased){
				mostProbableTrack = AnalyseTrackInfo(trackInfo, i);
			}
		}

		if (fVerbose > 1){
			std::cout << "PndTrackingQA::AnalyseEvent Analyse track: " << i << std::endl;
			std::cout << "mostProbableTrack " << mostProbableTrack << std::endl;
		}

		if(!fRunTimeBased){
			if (mostProbableTrack == -1) continue;}

		fTrackIdMCId[i] = mostProbableTrack;
		CalcEfficiencies(mostProbableTrack, trackInfo);

		if (fMapTrackQualification.count(mostProbableTrack) > 0)
			fMCTrackFound[mostProbableTrack]++;

		PndTrackingQualityRecoInfo recoinfo = GetRecoInfoFromRecoTrack(i, mostProbableTrack, mostProbableTrackFairLink);

		int nof_asso_mctracks = trackInfo["AllHits"].GetNLinks();
		recoinfo.SetNofMCTracks(nof_asso_mctracks);
		int size = recoTrackInfo->GetEntriesFast();

		new((*recoTrackInfo)[size]) PndTrackingQualityRecoInfo(recoinfo);

	}

	//Walter, get nr copious tracks
	for (std::map<Int_t, Int_t>::iterator iter = fMCTrackFound.begin(); iter != fMCTrackFound.end(); iter++){
		//std::cout<<"Copious track: "<<fMCTrackFound[iter->first]-1<<std::endl;
		if (fMCTrackFound[iter->first] > 1) {
			fNClones = fNClones + fMCTrackFound[iter->first]-1;
		}
	}

	if(!fRunTimeBased){
		for (std::map<Int_t, Int_t>::iterator iter = fMapTrackQualification.begin(); iter != fMapTrackQualification.end(); iter++) {

			if (iter->first > -1 && iter->second > 0) {

				PndMCTrack* mcTrack = (PndMCTrack*) fMCTrack->At(iter->first);

				if (fPndTrackOrTrackCand) {
					PndTrack* myTrack = (PndTrack*) fTrack->At(fMCIdTrackId[iter->first]);
					TVector3 mom(myTrack->GetParamFirst().GetPx(),
							myTrack->GetParamFirst().GetPy(),
							myTrack->GetParamFirst().GetPz());

					TVector3 McMom;
					if(mcTrack){
						TVector3 mcMom(mcTrack->GetMomentum());

						fMapPResolution[iter->first] = (mom.Mag() - mcMom.Mag());
						fMapP[iter->first] = mom;
						fMapPtResolution[iter->first] = (mom.Pt() - mcMom.Pt());
						fMapPt[iter->first] = mom.Pt();
						fMapPlResolution[iter->first] = (mom.Pz() - mcMom.Pz());
						fMapPl[iter->first] = mom.Pz();
						fMapPResolutionRel[iter->first] = (mom.Mag() - mcMom.Mag())
																														/ mcMom.Mag();
						fMapPtResolutionRel[iter->first] = (mom.Pt() - mcMom.Pt())
																														/ mcMom.Pt();
						fMapPlResolutionRel[iter->first] = (mom.Pz() - mcMom.Pz())
																														/ mcMom.Pz();

					}
				}
			}
		}
	}

	if(fRunTimeBased){

		for (std::map<FairLink, Int_t>::iterator iter = fTimeBasedMapTrackQualification.begin(); iter != fTimeBasedMapTrackQualification.end(); iter++) {

			if (iter->second > 0) {
				FairLink myFairLink = iter->first;

				PndMCTrack* mcTrack = (PndMCTrack*) FairRootManager::Instance()->GetCloneOfLinkData(myFairLink);

				if (fPndTrackOrTrackCand) {

					PndTrack* myTrack = (PndTrack*) FairRootManager::Instance()->GetCloneOfLinkData((FairLink) fTimeBasedMCIdTrackId[iter->first]);

					if (myTrack==nullptr) continue;

					TVector3 mom(myTrack->GetParamFirst().GetPx(),
							myTrack->GetParamFirst().GetPy(),
							myTrack->GetParamFirst().GetPz());
					TVector3 McMom(mcTrack->GetMomentum());

					fTimeBasedMapPResolution[iter->first] = (mom.Mag() - McMom.Mag());
					fTimeBasedMapP[iter->first] = mom;
					fTimeBasedMapPtResolution[iter->first] = (mom.Pt() - McMom.Pt());
					fTimeBasedMapPt[iter->first] = mom.Pt();
					fTimeBasedMapPlResolution[iter->first] = (mom.Pz() - McMom.Pz());
					fTimeBasedMapPl[iter->first] = mom.Pz();
					fTimeBasedMapPResolutionRel[iter->first] = (mom.Mag() - McMom.Mag())
																														/ McMom.Mag();
					fTimeBasedMapPtResolutionRel[iter->first] = (mom.Pt() - McMom.Pt())
																														/ McMom.Pt();
					fTimeBasedMapPlResolutionRel[iter->first] = (mom.Pz() - McMom.Pz())
																														/ McMom.Pz();
				}
			}
		}
	}

	if (fVerbose > 0)
		std::cout << "AnalyseEvent End" << std::endl;
}

FairMultiLinkedData PndTrackingQA::GetMCInfoForBranch(TString branchName, PndTrackCand* trackCand)
{
	FairMultiLinkedData result;
	result.SetInsertHistory(kFALSE);
	FairMultiLinkedData linksOfType = trackCand->GetLinksWithType(ioman->GetBranchId(branchName));

	for (int j = 0; j < linksOfType.GetNLinks(); j++){
		FairMultiLinkedData_Interface* linkData = (FairMultiLinkedData_Interface*)FairRootManager::Instance()->GetCloneOfLinkData(linksOfType.GetLink(j));
		if (linkData != 0){
			FairMultiLinkedData linkDataType = linkData->GetLinksWithType(FairRootManager::Instance()->GetBranchId("MCTrack"));
			linkDataType.SetAllWeights(1.);
			result.AddLinks(linkDataType);
			linkData->Delete();
		}
	}
	return result;
}

std::map<TString, FairMultiLinkedData> PndTrackingQA::AnalyseTrackCand(PndTrackCand* trackCand)
{
	std::map<TString, FairMultiLinkedData> trackInfo;

	if (fVerbose > 2) {
		std::cout << "PndTrackingQualityData::AnalyseTrackCand: TrackInfo" << std::endl;
		std::cout << *trackCand << std::endl;
	}

	for (size_t branchIndex = 0; branchIndex < fBranchNames.size(); branchIndex++){
		trackInfo[fBranchNames[branchIndex]] = GetMCInfoForBranch(fBranchNames[branchIndex], trackCand);
		trackInfo["AllHits"].AddLinks(trackInfo[fBranchNames[branchIndex]]);
	}
	if (fVerbose > 0)
		PrintTrackInfo(trackInfo);
	return trackInfo;
}

Int_t PndTrackingQA::AnalyseTrackInfo(std::map<TString, FairMultiLinkedData>& trackInfo, Int_t trackId)
{

	Int_t mostProbableTrack = -1;	//the MCTrack most FairLinks of a TrackCand are pointing to
	if (fVerbose > 2){
		std::cout << "PndTrackingQA::AnalyseTrackInfo: NMCLinks: " << trackInfo["AllHits"].GetNLinks() << std::endl;
		PrintTrackDataSummary(trackInfo["AllHits"]);
		std::cout << "PndTrackingQA::AnalyseTrackInfo " << trackInfo["AllHits"] << std::endl;
	}

	std::vector<FairLink> sortedMCTracks = trackInfo["AllHits"].GetSortedMCTracks();

	if (sortedMCTracks.size() == 0) return mostProbableTrack; //returns -1; no MCTracks

	if (sortedMCTracks.size() == 1){
		mostProbableTrack = sortedMCTracks[0].GetIndex();
		if (fMCIdIdealTrackId.count(mostProbableTrack) == 0){
			std::cout << "-W- PndTrackingQA::AnalyseTrackInfo fMCIdIdealTrackId does not contain mostProbableTrack" << mostProbableTrack << std::endl;
			return -1;
		}
		if (!(fMCIdIdealTrackId[mostProbableTrack] < fIdealTrack->GetEntriesFast())){
			std::cout << "-W- PndTrackingQA::AnalyseTrackInfo fMCIdIdealTrackId the ideal track is not in fIdealTrack. mostProbableTrack:" << mostProbableTrack << " idealTrack " << fMCIdIdealTrackId[mostProbableTrack] << std::endl;
			return -1;
		}

		PndTrackCand* myIdealTrack = ((PndTrack*)fIdealTrack->At(fMCIdIdealTrackId[mostProbableTrack]))->GetTrackCandPtr();

		if (myIdealTrack!=nullptr){
			std::cout << "Track data passed on to SetSumOfAllValidMCHits(): " << myIdealTrack->GetPointerToLinks()->GetEntryNr() << std::endl;

			Int_t nMCHits = GetSumOfAllValidMCHits(myIdealTrack->GetPointerToLinks());		//get the number of hits which should be found

			if (nMCHits == sortedMCTracks[0].GetWeight()){
				fMapTrackQualification[sortedMCTracks[0].GetIndex()] = qualityNumbers::kFullyFound;
				fMCIdTrackId[mostProbableTrack] = trackId;
			} else {
				if (fMapTrackQualification[sortedMCTracks[0].GetIndex()] != qualityNumbers::kFullyFound){
					fMapTrackQualification[sortedMCTracks[0].GetIndex()] = qualityNumbers::kPartiallyFound;
					fMCIdTrackId[mostProbableTrack] = trackId;
				}
			}
		}
	} else {
		Int_t highestCount = sortedMCTracks[0].GetWeight();
		mostProbableTrack = sortedMCTracks[0].GetIndex();
		Int_t allCounts = 0;
		for (size_t i = 0; i < sortedMCTracks.size(); i++){
			allCounts += sortedMCTracks[i].GetWeight();
			//			if (trackInfo["AllHits"].GetLink(i).GetWeight() > highestCount){
			//				highestCount = trackInfo["AllHits"].GetLink(i).GetWeight();
			//				mostProbableTrack = trackInfo["AllHits"].GetLink(i).GetIndex();
			//			}
		}

		if ((Double_t)highestCount/(Double_t)allCounts > 0.7){
			if (fMapTrackQualification[mostProbableTrack] != qualityNumbers::kFullyFound
					&& fMapTrackQualification[mostProbableTrack] != qualityNumbers::kPartiallyFound){
				fMapTrackQualification[mostProbableTrack] = qualityNumbers::kSpuriousFound;
				fMCIdTrackId[mostProbableTrack] = trackId;
			}
		}
		else {
			fNGhosts++;
		}
	}

	if(!fRunTimeBased){

		if (fVerbose > 0){
			for (size_t j = 0; j < sortedMCTracks.size(); j++){
				FairLink myLink = sortedMCTracks[j];
				if (fMCIdIdealTrackId.count(myLink.GetIndex()) > 0){
					PndTrackCand* myIdealTrack = ((PndTrack*)fIdealTrack->At(fMCIdIdealTrackId[myLink.GetIndex()]))->GetTrackCandPtr();
					if (fVerbose > 1) std::cout << "Ideal Tracking: Track " << myLink.GetIndex() << ": ";
					if (fVerbose > 1) PrintTrackDataSummary(*myIdealTrack->GetPointerToLinks());
				} else {
					std::cout << "Ideal Tracking: Track " << myLink.GetIndex() << " not available" << std::endl;
				}

			}
			//		std::cout << "MostProbableTrack: " << mostProbableTrack << " Quality: " << fMapTrackQualification[mostProbableTrack] << std::endl;
			//		std::cout << std::endl;
		}
	}

	return mostProbableTrack;
}

// If running time based the function AnalyseTrackInfo returns a FairLink
FairLink PndTrackingQA::AnalyseTrackInfoTimeBased(std::map<TString, FairMultiLinkedData>& trackInfo, FairLink trackId)
{

	FairLink mostProbableTrackFairLink= FairLink(-1,-1,-1,-1,1);	//the MCTrack most FairLinks of a TrackCand are pointing to
	if (fVerbose > 0){
		PrintTrackDataSummary(trackInfo["AllHits"]);
	}

	std::vector<FairLink> sortedMCTracks = trackInfo["AllHits"].GetSortedMCTracks();

	if (sortedMCTracks.size() == 0) {return mostProbableTrackFairLink;}

	if (sortedMCTracks.size() == 1){
		mostProbableTrackFairLink = sortedMCTracks[0];
		if (fTimeBasedMCIdIdealTrackId.count(mostProbableTrackFairLink) == 0){
			std::cout << "-W- PndTrackingQA::AnalyseTrackInfo fMCIdIdealTrackId does not contain mostProbableTrack" << mostProbableTrackFairLink << std::endl;
			mostProbableTrackFairLink= FairLink(-1,-1,-1,-1,1); // Set FairLink to "NULL" FairLink
		}

		FairLink myLink = (FairLink) fTimeBasedMCIdIdealTrackId[mostProbableTrackFairLink];

		for (Int_t i_IdealTrackArray = 0; i_IdealTrackArray < fIdealTrack->GetEntries(); i_IdealTrackArray++){

			PndTrack *currentTrack = (PndTrack *) fIdealTrack->At(i_IdealTrackArray);

			if(currentTrack!=nullptr){
				if(currentTrack->GetEntryNr().GetIndex()==myLink.GetIndex()){
					if(currentTrack->GetEntryNr().GetEntry()==myLink.GetEntry()&&currentTrack->GetEntryNr().GetWeight()==myLink.GetWeight()){

						PndTrack * idealTrack = (PndTrack *) fIdealTrack->At(i_IdealTrackArray);

						// TODO: The above loop over the ideal tracks should be replaced by the line below when the bug is fixed
						//PndTrack* idealTrack = (PndTrack*) ioman->GetCloneOfLinkData(myLink);
						//PndTrack* idealTrack = (PndTrack*) ioman->GetCloneOfLinkData(fTimeBasedMCIdIdealTrackId[mostProbableTrackFairLink]);

						if (idealTrack!=nullptr){

							PndTrackCand* myIdealTrack = (PndTrackCand*) idealTrack->GetTrackCandPtr();

							Int_t nMCHits = GetSumOfAllValidMCHits(myIdealTrack->GetPointerToLinks());		//get the number of hits which should be found

							if (nMCHits == sortedMCTracks[0].GetWeight()){
								fTimeBasedMapTrackQualification[sortedMCTracks[0]] = qualityNumbers::kFullyFound;
								fTimeBasedMCIdTrackId[sortedMCTracks[0]] = trackId;
							} else {
								if (fTimeBasedMapTrackQualification[sortedMCTracks[0]] != qualityNumbers::kFullyFound){
									fTimeBasedMapTrackQualification[sortedMCTracks[0]] = qualityNumbers::kPartiallyFound;
									fTimeBasedMCIdTrackId[sortedMCTracks[0]] = trackId;
								}
							}
							//idealTrack->Delete();
						}
					}
				}
			}
		}

	} else {
		Int_t highestCount = sortedMCTracks[0].GetWeight();
    mostProbableTrackFairLink = sortedMCTracks[0];
		Int_t allCounts = 0;
		for (size_t i = 0; i < sortedMCTracks.size(); i++){
			allCounts += sortedMCTracks[i].GetWeight();
			//			if (trackInfo["AllHits"].GetLink(i).GetWeight() > highestCount){
			//				highestCount = trackInfo["AllHits"].GetLink(i).GetWeight();
			//				mostProbableTrack = trackInfo["AllHits"].GetLink(i).GetIndex();
			//			}
		}

		if ((Double_t)highestCount/(Double_t)allCounts > 0.7){
			if (fTimeBasedMapTrackQualification[mostProbableTrackFairLink] != qualityNumbers::kFullyFound
					&& fTimeBasedMapTrackQualification[mostProbableTrackFairLink] != qualityNumbers::kPartiallyFound){
				fTimeBasedMapTrackQualification[mostProbableTrackFairLink] = qualityNumbers::kSpuriousFound;
				fTimeBasedMCIdTrackId[mostProbableTrackFairLink] = trackId;
			}
		}
		else {
			fNGhosts++;
		}
	}
	if (fVerbose > 0){
		for (size_t j = 0; j < sortedMCTracks.size(); j++){
			FairLink myLink = sortedMCTracks[j];
			if (fMCIdIdealTrackId.count(myLink.GetIndex()) > 0){

				PndTrack* idealTrack = (PndTrack*) ioman->GetCloneOfLinkData(mostProbableTrackFairLink);
				PndTrackCand* myIdealTrack = idealTrack->GetTrackCandPtr();

				if (fVerbose > 1) std::cout << "Ideal Tracking: Track " << myLink.GetIndex() << ": ";
				if (fVerbose > 1) PrintTrackDataSummary(*myIdealTrack->GetPointerToLinks());
			} else {
				std::cout << "Ideal Tracking: Track " << myLink.GetIndex() << " not available" << std::endl;
			}

		}
		//		std::cout << "MostProbableTrack: " << mostProbableTrack << " Quality: " << fMapTrackQualification[mostProbableTrack] << std::endl;
		//		std::cout << std::endl;
	}

	return mostProbableTrackFairLink;
}

void PndTrackingQA::FillMapTrackQualifikation()
{
	fMapTrackQualification.clear();
	fMapTrackMCStatus.clear();
	fMCIdIdealTrackId.clear();

	fTimeBasedMapTrackQualification.clear();
	fTimeBasedMapTrackMCStatus.clear();
	fTimeBasedMCIdIdealTrackId.clear();

	//std::cout << " FillMapTrackQualification: " << std::endl;
	//std::cout << " fIdealTrack.size() " << fIdealTrack->GetEntriesFast() << std::endl;
	for (int i = 0; i < fIdealTrack->GetEntriesFast(); i++){
		PndTrackCand* idealTrackCand = (PndTrackCand*)((PndTrack*)fIdealTrack->At(i))->GetTrackCandPtr();

		PndTrack* idealTrack = (PndTrack*)fIdealTrack->At(i);

		PndMCTrack* mcTrack;
		Bool_t primaryTrack;

		if(!fRunTimeBased){
			mcTrack = (PndMCTrack*)fMCTrack->At(idealTrackCand->getMcTrackId());
			fMCIdIdealTrackId[idealTrackCand->getMcTrackId()] = i;

			if(mcTrack){
				primaryTrack = (mcTrack->GetMotherID() < 0);}
		}
		if(fRunTimeBased){
			linksMCTrack = idealTrackCand->GetLinksWithType(ioman->GetBranchId("MCTrack"));
			// TODO: in the line below one can instead get the sorted MC tracks and fetch a clone of the one with higest weight, i.e. the one at index 0
			mcTrack = (PndMCTrack *)ioman->GetCloneOfLinkData(linksMCTrack.GetLink(0));
			primaryTrack = (mcTrack->GetMotherID() < 0);

			if(idealTrack->GetSortedMCTracks().size()>0){
				fTimeBasedMCIdIdealTrackId[idealTrack->GetSortedMCTracks()[0]] = idealTrack->GetLinksWithType(ioman->GetBranchId("IdealTrack")).GetLink(0);
			}

			if(idealTrack->GetSortedMCTracks().size()>1){
				std::cout << "Warning: ideal track has been created from several MC tracks!" << std::endl;
			}
		}

		Bool_t atLeastThreeHits = kFALSE;
		Int_t nHits = 0;
		//			for (int branchIndex = 0; branchIndex < fBranchNames.size(); branchIndex++){
		//				TString branchName = fBranchNames[branchIndex];
		//				nHits += GetNIdealHits(i, branchName);
		//			}
		nHits += GetNIdealHits(*(idealTrackCand->GetPointerToLinks()), "MVDHitsPixel");
		nHits += GetNIdealHits(*(idealTrackCand->GetPointerToLinks()), "MVDHitsStrip");
		nHits += GetNIdealHits(*(idealTrackCand->GetPointerToLinks()), "STTHit");
		nHits += GetNIdealHits(*(idealTrackCand->GetPointerToLinks()), "GEMHit");
		nHits += GetNIdealHits(*(idealTrackCand->GetPointerToLinks()), "FTSHit");

		if (nHits > 2) atLeastThreeHits = kTRUE;

		PndTrackCand* entry = ((PndTrack*)fIdealTrack->At(i))->GetTrackCandPtr();

		if(!fRunTimeBased){

			if (atLeastThreeHits) {

				if (primaryTrack) {
					fMapTrackQualification[idealTrackCand->getMcTrackId()] = qualityNumbers::kAtLeastThreePrim;
				} else {
					fMapTrackQualification[idealTrackCand->getMcTrackId()] = qualityNumbers::kAtLeastThreeSec;
				}
			}

			else if (primaryTrack){  //No hits for primary track in tracking detectors
				fMapTrackQualification[idealTrackCand->getMcTrackId()] = qualityNumbers::kLessThanThreePrim;
			}

			//PndTrackCand* entry = ((PndTrack*)fIdealTrack->At(i))->GetTrackCandPtr();
			if ((*fPossibleTrack)((FairMultiLinkedData*)entry->GetPointerToLinks(), primaryTrack))
			{
				if (primaryTrack) {
					fMapTrackQualification[idealTrackCand->getMcTrackId()] = qualityNumbers::kPossiblePrim;
				} else {
					fMapTrackQualification[idealTrackCand->getMcTrackId()] = qualityNumbers::kPossibleSec;
				}
			}
		}

		if(fRunTimeBased){

			if (atLeastThreeHits) {

				if (primaryTrack) {
					fTimeBasedMapTrackQualification[idealTrack->GetSortedMCTracks()[0]] = qualityNumbers::kAtLeastThreePrim;
				} else {
					fTimeBasedMapTrackQualification[idealTrack->GetSortedMCTracks()[0]] = qualityNumbers::kAtLeastThreeSec;
				}
			}

			else if (primaryTrack){  //No hits for primary track in tracking detectors
				fTimeBasedMapTrackQualification[idealTrack->GetSortedMCTracks()[0]] = qualityNumbers::kLessThanThreePrim;
			}

			//PndTrackCand* entry = ((PndTrack*)fIdealTrack->At(i))->GetTrackCandPtr();
			if ((*fPossibleTrack)((FairMultiLinkedData*)entry->GetPointerToLinks(), primaryTrack))
			{
				if (primaryTrack) {
					fTimeBasedMapTrackQualification[idealTrack->GetSortedMCTracks()[0]] = qualityNumbers::kPossiblePrim;
				} else {
					fTimeBasedMapTrackQualification[idealTrack->GetSortedMCTracks()[0]] = qualityNumbers::kPossibleSec;
				}
			}
		}

	}
	if (fVerbose > 1){
		std::cout << "-I- PndMCTestPatternRecoQuality::FillMapTrackQualifikation:" << std::endl;
		//		PrintTrackQualityMap();
	}

	fTimeBasedMapTrackMCStatus = fTimeBasedMapTrackQualification;
	fMapTrackMCStatus = fMapTrackQualification;
}

//Bool_t PndTrackingQA::PossibleTrack(FairMultiLinkedData& mcForward)
//{
//	Bool_t possibleTrack = kFALSE;
//
//	for (int i = 0; i < fBranchNames.size(); i++){
//		if (fBranchNames[i] == "MVDHitsPixel"){
//			possibleTrack = possibleTrack | (mcForward.GetLinksWithType(ioman->GetBranchId("MVDHitsPixel")).GetNLinks() +
//										     mcForward.GetLinksWithType(ioman->GetBranchId("MVDHitsStrip")).GetNLinks() > 3);
//
//		}
//		if (fBranchNames[i] == "STTHit"){
//			possibleTrack = possibleTrack | mcForward.GetLinksWithType(ioman->GetBranchId("STTHit")).GetNLinks() > 5;
//		}
//	}
//
//	return possibleTrack;
//}

Int_t PndTrackingQA::GetSumOfAllValidMCHits(FairMultiLinkedData* trackData)
{
	Int_t result = 0;
	for (size_t branchIndex = 0; branchIndex < fBranchNames.size(); branchIndex++){
		if (fBranchNames[branchIndex] == "GEMHit"){
			FairMultiLinkedData gemHits = trackData->GetLinksWithType(ioman->GetBranchId("GEMPoint"));
			result += gemHits.GetNLinks();
		} else {
			result += trackData->GetLinksWithType(ioman->GetBranchId(fBranchNames[branchIndex])).GetNLinks();
		}
	}
	return result;
}

void PndTrackingQA::CalcEfficiencies(Int_t mostProbableTrack, std::map<TString, FairMultiLinkedData>& trackInfo)
{
	if (mostProbableTrack < 0) return;
	if (fVerbose > 2)
		std::cout << "PndTrackingQA::CalcEfficiencies() for mostProbableTrack " << mostProbableTrack
		<<  " count " << fMCIdIdealTrackId.count(mostProbableTrack) << std::endl;
	for (size_t branchIndex = 0; branchIndex < fBranchNames.size(); branchIndex++){
		if (fMCIdIdealTrackId.count(mostProbableTrack) > 0){
			PndTrackCand* trackCand = ((PndTrack*)fIdealTrack->At(fMCIdIdealTrackId[mostProbableTrack]))->GetTrackCandPtr();
			if (trackCand == 0) return;
			Int_t nMcHits = GetNIdealHits(*trackCand->GetPointerToLinks(), fBranchNames[branchIndex]);
			FairMultiLinkedData foundHits = trackInfo[fBranchNames[branchIndex]];
			for (int i = 0; i < foundHits.GetNLinks(); i++){
				if (foundHits.GetLink(i).GetIndex() == mostProbableTrack){
					Double_t nFoundHits = foundHits.GetLink(i).GetWeight();
					if ((nFoundHits/nMcHits) > fMapEfficiencies[mostProbableTrack][fBranchNames[branchIndex]].first){
						std::pair<Double_t, Int_t> result(nFoundHits/nMcHits, nMcHits);
						fMapEfficiencies[mostProbableTrack][fBranchNames[branchIndex]]=result;
					}
				}
			}
		}
	}
}

void PndTrackingQA::CalcEfficienciesTimeBased(FairLink mostProbableTrackFairLink, std::map<TString, FairMultiLinkedData>& trackInfo)
{
	// TODO Make sure this function does not cause any problems, look at condition at the beginning of previous function
	for (size_t branchIndex = 0; branchIndex < fBranchNames.size(); branchIndex++){
		if (fTimeBasedMCIdIdealTrackId.count(mostProbableTrackFairLink) > 0){

			// Use the map between the FairLink of MC track and ideal track to get a clone of the link data
			PndTrackCand* trackCand = ((PndTrack*) ioman->GetCloneOfLinkData(fTimeBasedMCIdIdealTrackId[mostProbableTrackFairLink]))->GetTrackCandPtr();
			PndTrack* currentTrack = (PndTrack*) ioman->GetCloneOfLinkData(fTimeBasedMCIdIdealTrackId[mostProbableTrackFairLink]);

			if (trackCand == 0) return;

			Int_t nMcHits = GetNIdealHits(*trackCand->GetPointerToLinks(), fBranchNames[branchIndex]);
			FairMultiLinkedData foundHits = trackInfo[fBranchNames[branchIndex]];

			// For every branch of hits one wants to calculate the efficiency and place it in the pair
			// This is done in the line below
			Double_t nFoundHits=currentTrack->GetLinksWithType(ioman->GetBranchId(fBranchNames[branchIndex])).GetNLinks();
			std::pair<Double_t, Int_t> result(nFoundHits/nMcHits, nMcHits);
			fMapEfficienciesTimeBased[mostProbableTrackFairLink][fBranchNames[branchIndex]]=result;

			trackCand->Delete();
			currentTrack->Delete();


		}
	}
}

//Int_t PndTrackingQA::GetNIdealHits(Int_t trackId, TString branchName)
//{
//	PndMCEntry idealTrack = fIdealTrackCand.GetEntry(trackId);
//	return GetNIdealHits(idealTrack, branchName);
//
//}

// The function below works fine for both event and time based data
Int_t PndTrackingQA::GetNIdealHits(FairMultiLinkedData& track, TString branchName)
{
	Int_t numberGemHits = 0;
	if (branchName == "GEMHit"){
		numberGemHits = track.GetLinksWithType(ioman->GetBranchId("GEMPoint")).GetNLinks();
		return numberGemHits;
	}
	return track.GetLinksWithType(ioman->GetBranchId(branchName)).GetNLinks();
}

void PndTrackingQA::PrintTrackDataSummary(FairMultiLinkedData& trackData, Bool_t detailedInfo)
{
	if (detailedInfo == kTRUE) std::cout << std::endl;
	for (size_t branchIndex = 0; branchIndex < fBranchNames.size(); branchIndex++){
		TString branchName = fBranchNames[branchIndex];
		if (GetNIdealHits(trackData, branchName) > 0){
			std::cout << branchName << " " << GetNIdealHits(trackData, branchName);
			if (detailedInfo == kTRUE){
				std::cout << " : ";
				if (trackData.GetLinksWithType(ioman->GetBranchId(branchName)).GetNLinks() > 0)
					std::cout << trackData.GetLinksWithType(ioman->GetBranchId(branchName));
				std::cout << std::endl;
			} else {
				std::cout << " | ";
			}
		}
	}
	std::cout << std::endl;
}

void PndTrackingQA::PrintTrackDataSummaryCompare(FairMultiLinkedData& recoTrackData, FairMultiLinkedData& idealTrackData)
{
	for (size_t branchIndex = 0; branchIndex < fBranchNames.size(); branchIndex++){
		TString branchName = fBranchNames[branchIndex];
		std::cout << branchName << " " << GetNIdealHits(recoTrackData, branchName);
		std::cout << "/" << GetNIdealHits(idealTrackData, branchName);
		std::cout << " | ";
	}
	std::cout << std::endl;
}


void PndTrackingQA::PrintTrackQualityMap(Bool_t detailedInfo)
{
	if(!fRunTimeBased){
		for (std::map<Int_t, Int_t>::iterator iter = fMapTrackQualification.begin(); iter != fMapTrackQualification.end(); iter++){
			std::cout << "TrackID: " << iter->first << " MCQuality: "  << qualityNumbers::QualityNumberToString(fMapTrackMCStatus[iter->first]) << " Quality: ";
			if (iter->second < 0)
				std::cout << " NOT FOUND ";
			else
				std::cout << qualityNumbers::QualityNumberToString(iter->second);
			std::cout << " NFound: " << fMCTrackFound[iter->first];
			std::cout << " MCData: ";
			if (fMCIdIdealTrackId.count(iter->first) > 0){
				PndTrackCand* idealTrackCand = ((PndTrack*)fIdealTrack->At(fMCIdIdealTrackId[iter->first]))->GetTrackCandPtr();
				if (iter->second < 0)
					PrintTrackDataSummary(*idealTrackCand->GetPointerToLinks(), detailedInfo);
				else {
					PndTrackCand* recoTrackCand = ((PndTrack*)fTrack->At(fMCIdTrackId[iter->first]))->GetTrackCandPtr();
					if (recoTrackCand != 0 && idealTrackCand != 0)
						PrintTrackDataSummaryCompare(*recoTrackCand->GetPointerToLinks(), *idealTrackCand->GetPointerToLinks());
				}
			}
		}
	}

	std::cout << std::endl;

}

void PndTrackingQA::PrintTrackMCStatusMap()
{
	std::cout << "PrintTrackMCStatusMap: " << std::endl;
	for (std::map<Int_t, Int_t>::iterator iter = fMapTrackMCStatus.begin(); iter != fMapTrackMCStatus.end(); iter++){
		std::cout << "TrackID: " << iter->first << " Quality: "  << qualityNumbers::QualityNumberToString(iter->second) << " Found: " << fMCTrackFound[iter->first];
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

// This function works both time based and event based
void PndTrackingQA::PrintTrackInfo(std::map<TString, FairMultiLinkedData> info)
{
	std::cout << "TrackInfo: (MC-ID/NHits) : ";
	for (std::map<TString, FairMultiLinkedData>::iterator iter = info.begin(); iter != info.end(); iter++){
		std::cout << iter->first;
		for (int i = 0; i < iter->second.GetNLinks(); i++){
			std::cout << " : (" << iter->second.GetLink(i).GetIndex() << "/" << iter->second.GetLink(i).GetWeight() << ")";
		}
		std::cout << " || ";
	}
	std::cout << std::endl;
}

// TODO If needed make a new function similar to the one below which can handle the time based data
Bool_t PndTrackingQA::IsBetterTrackExisting(Int_t& mcIndex,  int quality)
{
	if (fMapTrackQualification.count(mcIndex) == 1){
		if (fMapTrackQualification[mcIndex] > 0 && fMapTrackQualification[mcIndex] > quality) return true;
	}
	return false;
}

PndTrackingQualityRecoInfo PndTrackingQA::GetRecoInfoFromRecoTrack(Int_t trackId, Int_t mctrackId, FairLink mctrackFairLink)
{
	PndTrackingQualityRecoInfo recoinfo(trackId);

	// The mctrackFairLink in the time based reconstruction corresponds to the MC track with mctrackId in the event based reconstruction

	// CHECK this can be modified in the future, for now it is ok
	// mvd pix / mvd str / stt paral / stt skew / gem

	if (fVerbose > 2)
		std::cout << "PndTrackingQA::GetRecoInfoFromRecoTrack()" << std::endl;

	std::map<int, int> noftruehits;
	std::map<int, int> noffakehits;
	std::map<int, int> nofmissinghits;

	for (size_t branchIndex = 0; branchIndex < fBranchNames.size();
			branchIndex++) {
		noftruehits.clear();
		noffakehits.clear();
		nofmissinghits.clear();

		// get the reco track...
		PndTrack *track = (PndTrack*) fTrack->At(trackId);
		// .. and the track cand
		PndTrackCand* trackcand = track->GetTrackCandPtr();

		recoinfo.SetPositionFirst(track->GetParamFirst().GetPosition());
		recoinfo.SetMomentumFirst(track->GetParamFirst().GetMomentum());

		recoinfo.SetPositionLast(track->GetParamLast().GetPosition());
		recoinfo.SetMomentumLast(track->GetParamLast().GetMomentum());

		recoinfo.SetCharge(track->GetParamFirst().GetQ());
		recoinfo.SetFlag(track->GetFlag());

		// get links associated to the reco track
		FairMultiLinkedData ptrlink = *trackcand->GetPointerToLinks();
		// get links corresponding to the hits of the specific detector
		FairMultiLinkedData links = ptrlink.GetLinksWithType(
				ioman->GetBranchId(fBranchNames[branchIndex]));
		// get their number
		Int_t nHits = links.GetNLinks();
		if (fVerbose > 2){
			if (nHits > 0)
				std::cout << "----- reco track " << trackId << " (mc track " << mctrackId<< ") has " << nHits << " from " << fBranchNames[branchIndex] << std::endl;
		}
		// get mc track info from each hit
		for (int ihit = 0; ihit < nHits; ihit++) {
			FairLink link = links.GetLink(ihit);
			int assomctrack = -1;
			// std::cout << "ihit " << ihit << " " << link.GetIndex() << " " << link.GetType() << " " << link.GetWeight() << std::endl;
			FairHit * hit = (FairHit*) links.GetData(link);  // Need to use clone of link data
			if (!hit) {
				std::cout << "ihit " << ihit << " " << link
						<< " is FAKE" << std::endl;
				//    std::cout << "No Obj Hit" << std::endl;

				//
				if (noffakehits.count(branchIndex) > 0)
					noffakehits[branchIndex]++;
				else
					noffakehits[branchIndex] = 1; // if not there
				continue;
			}

			// get links of the hit
			FairMultiLinkedData hitlink = *hit->GetPointerToLinks();
			// get the links corresponding to the mc track associated to the hit
			FairMultiLinkedData mclinks = hitlink.GetLinksWithType(
					ioman->GetBranchId("MCTrack"));
			//  	  std::cout << "hit " << ihit  << " belongs to " << mclinks.GetNLinks() << " mc tracks" << std::endl;
			Bool_t isgood = kFALSE;
			FairMultiLinkedData mvdstrhits = links.GetLinksWithType(
					FairRootManager::Instance()->GetBranchId("MVDHitsStrip"));
			FairMultiLinkedData gemhits = links.GetLinksWithType(
					FairRootManager::Instance()->GetBranchId("GEMHit"));
			if ((gemhits.GetNLinks() > 0 || mvdstrhits.GetNLinks() > 0) && mclinks.GetNLinks() > 1) {
				isgood = kFALSE;
			} else {
				for (int imctrk = 0; imctrk < mclinks.GetNLinks(); imctrk++) {

					FairLink mclink = mclinks.GetLink(imctrk);
					assomctrack = mclink.GetIndex();
					// 	     std::cout << "imctrk " << imctrk << " " << mclink.GetIndex() << " " << mclink.GetType() << " " << mclink.GetWeight() << std::endl;
					//	  std::cout << "ihit " << ihit << " (hitid " << link.GetIndex() << ") belongs to MC track " << assomctrack << std::endl;
					// event based
					if(!fRunTimeBased){
						if (assomctrack == mctrackId)
							isgood = kTRUE;
					}
					if(fRunTimeBased){

						if (assomctrack == mctrackFairLink.GetIndex() && mclink.GetEntry() == mctrackFairLink.GetEntry())
							isgood = kTRUE;

					}
				}
			}

			// if true
			if (isgood == kTRUE) {
				// if true and already there
				if (noftruehits.count(branchIndex) > 0)
					noftruehits[branchIndex]++;
				else
					noftruehits[branchIndex] = 1; // if not there
			} else { // if not
				if (noffakehits.count(branchIndex) > 0)
					noffakehits[branchIndex]++;
				else
					noffakehits[branchIndex] = 1; // if not there
			}

		}

		if (fMCIdIdealTrackId.count(mctrackId) > 0) {
			int idealtrackid = fMCIdIdealTrackId[mctrackId];
			PndTrack *idealtrack = (PndTrack*) fIdealTrack->At(idealtrackid);
			PndTrackCand* idealtrackcand = idealtrack->GetTrackCandPtr();

			Int_t nMcHits = GetNIdealHits(*idealtrackcand->GetPointerToLinks(),
					fBranchNames[branchIndex]);

			nofmissinghits[branchIndex] = nMcHits - noftruehits[branchIndex];

			//      std::cout << "**** ideal track " << idealtrackid << " has " << nMcHits << " from " << fBranchNames[branchIndex] << std::endl;

			//}

			//    std::cout << "===> BRANCH " << fBranchNames[branchIndex] << " of track " << trackId << " (mc track " << mctrackId << ") has " << noftruehits[branchIndex] << " true, " << noffakehits[branchIndex] << " fake and " << nofmissinghits[branchIndex] << " missing hits" << std::endl;

			if (fBranchNames[branchIndex] == "MVDHitsPixel") {
				recoinfo.SetNofMvdPixelTrueHits(noftruehits[branchIndex]);
				recoinfo.SetNofMvdPixelFakeHits(noffakehits[branchIndex]);
				recoinfo.SetNofMvdPixelMissingHits(nofmissinghits[branchIndex]);
			} else if (fBranchNames[branchIndex] == "MVDHitsStrip") {
				recoinfo.SetNofMvdStripTrueHits(noftruehits[branchIndex]);
				recoinfo.SetNofMvdStripFakeHits(noffakehits[branchIndex]);
				recoinfo.SetNofMvdStripMissingHits(nofmissinghits[branchIndex]);
			} else if (fBranchNames[branchIndex] == "STTHit") {
				recoinfo.SetNofSttTrueHits(noftruehits[branchIndex]);
				recoinfo.SetNofSttFakeHits(noffakehits[branchIndex]);
				recoinfo.SetNofSttMissingHits(nofmissinghits[branchIndex]);
			} else if (fBranchNames[branchIndex] == "GEMHit") {
				recoinfo.SetNofGemTrueHits(noftruehits[branchIndex]);
				recoinfo.SetNofGemFakeHits(noffakehits[branchIndex]);
				recoinfo.SetNofGemMissingHits(nofmissinghits[branchIndex]);
			} else if (fBranchNames[branchIndex] == "FTSHit") {
				recoinfo.SetNofFtsTrueHits(noftruehits[branchIndex]);
				recoinfo.SetNofFtsFakeHits(noffakehits[branchIndex]);
				recoinfo.SetNofFtsMissingHits(nofmissinghits[branchIndex]);
			}

		}
	}

	recoinfo.SetMCTrackID(mctrackId);
	return recoinfo;
}

