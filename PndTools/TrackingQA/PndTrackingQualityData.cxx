/*
 * PndTrackingQualityData.cxx
 *
 *  Created on: Aug 23, 2013
 *      Author: stockman
 */

#include "PndTrackingQualityData.h"

#include "PndTrack.h"

ClassImp(PndTrackingQualityData);

PndTrackingQualityData::PndTrackingQualityData ():
		ioman(0), fMCMatch(0), fVerbose(0)
{
}

PndTrackingQualityData::PndTrackingQualityData(std::vector<TString>& branchNames):
		fBranchNames(branchNames), ioman(0), fMCMatch(0), fVerbose(0)
{
}

PndTrackingQualityData::~PndTrackingQualityData()
{
}

void PndTrackingQualityData::Init()
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

	if (fBranchNames.size() == 0){
		AddHitsBranchName("MVDHitsPixel");
		AddHitsBranchName("MVDHitsStrip");
		AddHitsBranchName("STTHit");
		AddHitsBranchName("GEMHit");
	}

}


std::map<TString, FairMultiLinkedData> PndTrackingQualityData::AnalyseTrack(PndTrack* track)
{
	PndTrackCand trackCand = track->GetTrackCand();
	return AnalyseTrackCand(&trackCand);
}

std::map<TString, FairMultiLinkedData> PndTrackingQualityData::AnalyseTrackCand(PndTrackCand* trackCand)
{
	std::map<TString, FairMultiLinkedData> trackInfo;

	if (fVerbose > 0) {
		std::cout << "PndTrackingQualityData::AnalyseTrackCand: TrackInfo" << std::endl;
	}
	for (int branchIndex = 0; branchIndex < fBranchNames.size(); branchIndex++){
		trackInfo[fBranchNames[branchIndex]] = GetMCInfoForBranch(fBranchNames[branchIndex], trackCand);
		trackInfo["AllHits"].AddLinks(trackInfo[fBranchNames[branchIndex]]);

		if (fVerbose > 0) {
			std::cout << fBranchNames[branchIndex] << ": " << trackInfo[fBranchNames[branchIndex]] << std::endl;
		}
	}
	if (fVerbose > 0)
		std::cout << "AllHits: " << trackInfo["AllHits"] << std::endl;
	return trackInfo;
}

FairMultiLinkedData PndTrackingQualityData::GetMCInfoForBranch(TString branchName, PndTrackCand* trackCand)
{
	FairMultiLinkedData result;
	FairMultiLinkedData linksOfType = trackCand->GetLinksWithType(ioman->GetBranchId(branchName));
	for (int j = 0; j < linksOfType.GetNLinks(); j++){
		PndMCEntry mcTracks = fMCMatch->GetMCInfoSingle(linksOfType.GetLink(j), "MCTrack");
		mcTracks.RemoveType(-2);
		mcTracks.SetAllWeights(1.);
		result.AddLinks(mcTracks);
	}
	return result;
}

