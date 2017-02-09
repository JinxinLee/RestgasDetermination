/*
 * PndTrackingQualityAnalysis.cxx
 *
 *  Created on: Aug 23, 2013
 *      Author: stockman
 */

#include "PndTrackingQualityAnalysis.h"

#include "PndTrack.h"
#include "PndMCTrack.h"
#include "PndTrackingQualityData.h"

ClassImp(PndTrackingQualityAnalysis);

PndTrackingQualityAnalysis::PndTrackingQualityAnalysis (TString trackBranchName, Bool_t pndTrackData):
	fTrackBranchName(trackBranchName), fPndTrackOrTrackCand(pndTrackData), fPossibleTrack(0), fNGhosts(0), fUseCorrectedSkewedHits(kFALSE), fVerbose(0)
{
	if(fPossibleTrack == 0){
		std::cout << "-I- PndTrackingQualityAnalysis::PndTrackingQualityAnalysis no PossibleTrackFunctor given. Taking Standard!" << std::endl;
		if (trackBranchName == "MVDTrack" ){
			fPossibleTrack = new RiemannMvdSttGemFunctor();
		} else if (trackBranchName == "CombiTrackCand" ) {
			fPossibleTrack = new OnlySttFunctor();
		} else {
			fPossibleTrack = new StandardTrackFunctor();
		}
	}
}

PndTrackingQualityAnalysis::PndTrackingQualityAnalysis (TString trackBranchName, PndTrackFunctor* posTrack, Bool_t pndTrackData):
	fTrackBranchName(trackBranchName), fPndTrackOrTrackCand(pndTrackData), fPossibleTrack(posTrack), fNGhosts(0), fUseCorrectedSkewedHits(kFALSE), fVerbose(0)
{
	if(fPossibleTrack == 0){
		std::cout << "-I- PndTrackingQualityAnalysis::PndTrackingQualityAnalysis no PossibleTrackFunctor given. Taking Standard!" << std::endl;
		if (trackBranchName == "MVDTrack" ){
			fPossibleTrack = new RiemannMvdSttGemFunctor();
		} else if (trackBranchName == "CombiTrackCand" ) {
			fPossibleTrack = new OnlySttFunctor();
		} else {
			fPossibleTrack = new StandardTrackFunctor();
		}
	}
}

PndTrackingQualityAnalysis::~PndTrackingQualityAnalysis()
{
	delete (fPossibleTrack);
}

void PndTrackingQualityAnalysis::Init()
{
	ioman = FairRootManager::Instance();
	if (!ioman) {
		std::cout << "-E- PndTrackingQualityTask::Init: "
				<< "RootManager not instantiated!" << std::endl;
		return;
	}

	fMCMatch = (PndMCMatch*) ioman->GetObject("MCMatch");

	if (!fMCMatch) {
		std::cout << "-E- PndTrackingQualityTask::Init: "
				<< "MCMatch not instantiated!" << std::endl;
		return;
	}

	fTrack = (TClonesArray*) ioman->GetObject(fTrackBranchName);
	fMCTrack = (TClonesArray*) ioman->GetObject("MCTrack");

	if (fBranchNames.size() == 0){
		AddHitsBranchName("MVDHitsPixel");
		AddHitsBranchName("MVDHitsStrip");
		AddHitsBranchName("STTHit");
		AddHitsBranchName("GEMHit");

		if (FairRootManager::Instance()->GetBranchId("CorrectedSkewedHits")  > 0){
			AddHitsBranchName("CorrectedSkewedHits");
		}
	}
	std::cout << "-I- PndTRackingQualityAnalysis::Init: PossibleTrackFunctor: ";
	fPossibleTrack->Print();
}

void PndTrackingQualityAnalysis::AnalyseEvent()
{

	fMCMatch->CreateArtificialStage("MCTrack");
//	fMCMatch->RemoveStage("GEMHit");
	fIdealTracksData = fMCMatch->GetMCInfo("MCTrack", "FTSHit");							//ideal pattern reconstruction for mvd. Replace MVDHitsStrip with the branch name of your detector
	fIdealTracksData.RemoveType(ioman->GetBranchId("MVDStripClusterCand"));
	fIdealTracksData.RemoveType(ioman->GetBranchId("MCTrack"));

	FillMapTrackQualifikation();
	std::cout << "PndTrackingQualityAnalysis::AnalyseEvent() Track quality map before analysis: " << std::endl;
	PrintTrackQualityMap(kTRUE);

	for (Int_t i = 0; i < fTrack->GetEntriesFast(); i++){

		PndTrackingQualityData trackQAData(fBranchNames);
		trackQAData.Init();
		trackQAData.SetVerbose(fVerbose);
		std::map<TString, FairMultiLinkedData> trackInfo;

		if (fPndTrackOrTrackCand){
			PndTrack* myTrack = (PndTrack*)fTrack->At(i);
			trackInfo = trackQAData.AnalyseTrack(myTrack);
		} else {
			PndTrackCand* myTrack = (PndTrackCand*)fTrack->At(i);
			trackInfo = trackQAData.AnalyseTrackCand(myTrack);
		}
		if (fVerbose > 1){
			std::cout << "PndTrackingQualityAnalysis::AnalyseEvent Analyse track: " << i << std::endl;
		}
		Int_t mostProbableTrack = AnalyseTrackInfo(trackInfo, i);

		if (mostProbableTrack == -1) continue;

		fTrackIdMCId[i] = mostProbableTrack;
		CalcEfficiencies(mostProbableTrack, trackInfo);

		fMCTrackFound[mostProbableTrack]++;
	}

	for (std::map<Int_t, Int_t>::iterator iter = fMapTrackQualification.begin();
			iter != fMapTrackQualification.end(); iter++) {


		if (iter->first > -1 && iter->second > 0) {
			PndMCTrack* mcTrack = (PndMCTrack*) fMCTrack->At(iter->first);

			if (fPndTrackOrTrackCand) {
				PndTrack* myTrack = (PndTrack*) fTrack->At(fMCIdTrackId[iter->first]);
				TVector3 mom(myTrack->GetParamFirst().GetPx(),
						myTrack->GetParamFirst().GetPy(),
						myTrack->GetParamFirst().GetPz());
				TVector3 McMom(mcTrack->GetMomentum());

				fMapPResolution[iter->first] = (mom.Mag() - McMom.Mag());
				fMapP[iter->first] = mom;
				fMapPtResolution[iter->first] = (mom.Pt() - McMom.Pt());
				fMapPt[iter->first] = mom.Pt();
				fMapPResolutionRel[iter->first] = (mom.Mag() - McMom.Mag())
						/ McMom.Mag();
				fMapPtResolutionRel[iter->first] = (mom.Pt() - McMom.Pt())
						/ McMom.Pt();
			}
		}

	}

}

Int_t PndTrackingQualityAnalysis::AnalyseTrackInfo(std::map<TString, FairMultiLinkedData>& trackInfo, Int_t trackId)
{

	Int_t mostProbableTrack = -1;
	if (fVerbose > 0)
		std::cout << "PndTrackingQualityAnalysis::AnalyseTrackInfo: TrackInfo: " << trackInfo["AllHits"].GetNLinks() << std::endl;
	PrintTrackDataSummary(trackInfo["AllHits"]);

	if (trackInfo["AllHits"].GetNLinks() == 1){
		mostProbableTrack = trackInfo["AllHits"].GetLink(0).GetIndex();
		PndMCEntry myEntry = fIdealTracksData.GetEntry(mostProbableTrack);
		Int_t nMCHits = GetSumOfAllValidMCHits(&myEntry);

		if (nMCHits == trackInfo["AllHits"].GetLink(0).GetWeight()){
			fMapTrackQualification[trackInfo["AllHits"].GetLink(0).GetIndex()] = qualityNumbers::kFullyFound;
			fMCIdTrackId[mostProbableTrack] = trackId;
		} else {
			if (fMapTrackQualification[trackInfo["AllHits"].GetLink(0).GetIndex()] != qualityNumbers::kFullyFound){
				fMapTrackQualification[trackInfo["AllHits"].GetLink(0).GetIndex()] = qualityNumbers::kPartiallyFound;
				fMCIdTrackId[mostProbableTrack] = trackId;
			}
		}
	} else {
		Int_t highestCount = 0;
		Int_t allCounts = 0;
		for (int i = 0; i < trackInfo["AllHits"].GetNLinks(); i++){
			allCounts += trackInfo["AllHits"].GetLink(i).GetWeight();
			if (trackInfo["AllHits"].GetLink(i).GetWeight() > highestCount){
				highestCount = trackInfo["AllHits"].GetLink(i).GetWeight();
				mostProbableTrack = trackInfo["AllHits"].GetLink(i).GetIndex();
			}
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
	if (fVerbose > 0){
		for (int j = 0; j < trackInfo["AllHits"].GetNLinks(); j++){
			FairLink myLink = trackInfo["AllHits"].GetLink(j);
			PndMCEntry myEntry = fIdealTracksData.GetEntry(myLink.GetIndex());

			std::cout << "Ideal Tracking: Track " << myLink.GetIndex() << ": ";
			PrintTrackDataSummary(myEntry);

		}
		std::cout << "MostProbableTrack: " << mostProbableTrack << " Quality: " << fMapTrackQualification[mostProbableTrack] << std::endl;
		std::cout << std::endl;
	}

	return mostProbableTrack;
}

void PndTrackingQualityAnalysis::FillMapTrackQualifikation()
{
	fMapTrackQualification.clear();
	fMapTrackMCStatus.clear();
	for (int i = 0; i < fIdealTracksData.GetNEntries(); i++){
		if (fIdealTracksData.GetEntry(i).GetNLinks() > 0){
			PndMCTrack* mcTrack = (PndMCTrack*)fMCTrack->At(i);
			Bool_t primaryTrack = (mcTrack->GetMotherID() < 0);
			Bool_t atLeastThreeHits = kFALSE;
			Int_t nHits = 0;
//			for (int branchIndex = 0; branchIndex < fBranchNames.size(); branchIndex++){
//				TString branchName = fBranchNames[branchIndex];
//				nHits += GetNIdealHits(i, branchName);
//			}
			nHits += GetNIdealHits(i, "MVDHitsPixel");
			nHits += GetNIdealHits(i, "MVDHitsStrip");
			nHits += GetNIdealHits(i, "STTHit");
			nHits += GetNIdealHits(i, "GEMHit");
			//std::cout << "FillMapTrackQualifikation: NHits: " << nHits << std::endl;
			
			if (nHits > 2) atLeastThreeHits = kTRUE;

			if (atLeastThreeHits) {
				if (primaryTrack) {
					fMapTrackQualification[i] = qualityNumbers::kAtLeastThreePrim;
				} else {
					fMapTrackQualification[i] = qualityNumbers::kAtLeastThreeSec;
				}
			//	std::cout << "AtLeastThreeHits: Track "<< i << " status: " << fMapTrackQualification[i] << std::endl;
			}

			else if (primaryTrack){  //No hits for primary track in central tracking detectors
				fMapTrackQualification[i] = qualityNumbers::kLessThanThreePrim;
			}
			
			PndMCEntry entry = fIdealTracksData.GetEntry(i);
			if ((*fPossibleTrack)((FairMultiLinkedData*)&entry, primaryTrack))
			{
				if (primaryTrack) {
					fMapTrackQualification[i] = qualityNumbers::kPossiblePrim;
				} else {
					fMapTrackQualification[i] = qualityNumbers::kPossibleSec;
				}
			}
		}
	}
	if (fVerbose > 0){
		std::cout << "-I- PndMCTestPatternRecoQuality::FillMapTrackQualifikation:" << std::endl;
//		PrintTrackQualityMap();
	}
	fMapTrackMCStatus = fMapTrackQualification;
}

//Bool_t PndTrackingQualityAnalysis::PossibleTrack(FairMultiLinkedData& mcForward)
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

Int_t PndTrackingQualityAnalysis::GetSumOfAllValidMCHits(FairMultiLinkedData* trackData)
{
	Int_t result = 0;
	for (size_t branchIndex = 0; branchIndex < fBranchNames.size(); branchIndex++){
		if (fBranchNames[branchIndex] == "GEMHit"){
			FairMultiLinkedData gemHits = trackData->GetLinksWithType(ioman->GetBranchId("GEMHit"));
			for (int i = 0; i < gemHits.GetNLinks(); i++){
				FairLink gemHit = gemHits.GetLink(i);
				result += IsCorrectGemHit(gemHit);
			}
		} else {
			result += trackData->GetLinksWithType(ioman->GetBranchId(fBranchNames[branchIndex])).GetNLinks();
		}
	}
	return result;
}


Bool_t PndTrackingQualityAnalysis::IsCorrectGemHit(FairLink& gemLink)
{
	PndMCEntry linkToGEMPoint = fMCMatch->GetMCInfoSingle(gemLink, "GEMPoint");
	return (linkToGEMPoint.GetNLinks() == 1);
}

void PndTrackingQualityAnalysis::CalcEfficiencies(Int_t mostProbableTrack, std::map<TString, FairMultiLinkedData>& trackInfo)
{
	if (mostProbableTrack < 0) return;
	for (size_t branchIndex = 0; branchIndex < fBranchNames.size(); branchIndex++){
		Int_t nMcHits = GetNIdealHits(mostProbableTrack, fBranchNames[branchIndex]);
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

Int_t PndTrackingQualityAnalysis::GetNIdealHits(Int_t trackId, TString branchName)
{
	PndMCEntry idealTrack = fIdealTracksData.GetEntry(trackId);
	return GetNIdealHits(idealTrack, branchName);

}


Int_t PndTrackingQualityAnalysis::GetNIdealHits(FairMultiLinkedData& track, TString branchName)
{
	Int_t numberGemHits = 0;
	if (branchName == "GEMHit"){
		FairMultiLinkedData gemHits = track.GetLinksWithType(ioman->GetBranchId("GEMHit"));
		for (int i = 0; i < gemHits.GetNLinks(); i++){
			FairLink gemHit = gemHits.GetLink(i);
			numberGemHits += IsCorrectGemHit(gemHit);
		}
		return numberGemHits;
	}
	return track.GetLinksWithType(ioman->GetBranchId(branchName)).GetNLinks();
}

void PndTrackingQualityAnalysis::PrintTrackDataSummary(FairMultiLinkedData& trackData, Bool_t detailedInfo)
{
	if (detailedInfo == kTRUE) std::cout << std::endl;
	for (size_t branchIndex = 0; branchIndex < fBranchNames.size(); branchIndex++){
		TString branchName = fBranchNames[branchIndex];
		std::cout << branchName << " " << GetNIdealHits(trackData, branchName);
		if (detailedInfo == kTRUE){
			std::cout << " : " << trackData.GetLinksWithType(ioman->GetBranchId(branchName)) << " | " << std::endl;
		} else {
			std::cout << " | ";
		}
	}
	std::cout << std::endl;
}


void PndTrackingQualityAnalysis::PrintTrackQualityMap(Bool_t detailedInfo)
{
	for (std::map<Int_t, Int_t>::iterator iter = fMapTrackQualification.begin(); iter != fMapTrackQualification.end(); iter++){
		std::cout << "TrackID: " << iter->first << " MCQuality: "  << fMapTrackMCStatus[iter->first] << " Found: " << fMCTrackFound[iter->first] << " MCData: ";
		PndMCEntry entry = fIdealTracksData.GetEntry(iter->first);
		PrintTrackDataSummary(entry, detailedInfo);
	}
	std::cout << std::endl;
}

void PndTrackingQualityAnalysis::PrintTrackMCStatusMap()
{
	std::cout << "PrintTrackMCStatusMap: " << std::endl;
	for (std::map<Int_t, Int_t>::iterator iter = fMapTrackMCStatus.begin(); iter != fMapTrackMCStatus.end(); iter++){
		std::cout << "TrackID: " << iter->first << " Quality: "  << iter->second << " Found: " << fMCTrackFound[iter->first];
		std::cout << std::endl;
	}
	std::cout << std::endl;
}


Bool_t PndTrackingQualityAnalysis::IsBetterTrackExisting(Int_t& mcIndex,  int quality)
{
	if (fMapTrackQualification.count(mcIndex) == 1){
		if (fMapTrackQualification[mcIndex] > 0 && fMapTrackQualification[mcIndex] > quality) return true;
	}
	return false;
}
