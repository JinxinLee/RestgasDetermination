// -------------------------------------------------------------------------
// -----                PndMCTestPatternRecoQuality source file             -----
// -----                  Created 18/07/08  by T.Stockmanns        -----
// -------------------------------------------------------------------------
// libc includes
#include <iostream>
#include <functional>

// Root includes
#include "TROOT.h"
#include "TClonesArray.h"

// framework includes
#include "FairRootManager.h"
#include "PndMCTestPatternRecoQuality.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairHit.h"
#include "FairMultiLinkedData.h"

#include "PndMCTrack.h"
#include "PndTrack.h"
#include "PndMCEntry.h"

// -----   Default constructor   -------------------------------------------
PndMCTestPatternRecoQuality::PndMCTestPatternRecoQuality(TString trackBranchName) :
	FairTask("Creates PndMC test"), fEventNr(0), fTrackBranchName(trackBranchName)
{
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndMCTestPatternRecoQuality::~PndMCTestPatternRecoQuality() {
}

// -----   Public method Init   --------------------------------------------
InitStatus PndMCTestPatternRecoQuality::Init() {

	//  fMCMatch->InitStage(kMCTrack, "", "MCTrack");


	fPHisto = new TH1D("fPHisto", "Momentum Resolution", 1000, -10, 10);
	fPtHisto = new TH1D("fPtHisto", "Transverse Momentum Resolution", 1000,
			-10, 10);
	fQualyHisto = new TH1I("fQualyHisto", "Quality of Trackfinding", 12, -1.5, 10.5);

	ioman = FairRootManager::Instance();
	if (!ioman) {
		std::cout << "-E- PndMCTestPatternRecoQuality::Init: "
				<< "RootManager not instantiated!" << std::endl;
		return kFATAL;
	}

	fMCMatch = (PndMCMatch*) ioman->GetObject("MCMatch");

	fTrack = (TClonesArray*) ioman->GetObject(fTrackBranchName);
	fMCTrack = (TClonesArray*) ioman->GetObject("MCTrack");

	if (fBranchNames.size() == 0){
		AddHitsBranchName("MVDHitsPixel");
		AddHitsBranchName("MVDHitsStrip");
		AddHitsBranchName("STTHit");
		AddHitsBranchName("GEMHit");
	}


	for (int i = 0; i < fBranchNames.size(); i++){
		fMapEfficiencies[fBranchNames[i]] = new TH2D(fBranchNames[i], fBranchNames[i], 100, 0., 100., 50, 0, 1.1);
	}

	std::cout
			<< "-I- PndMCTestPatternRecoQuality::Init: Initialization successfull"
			<< std::endl;

	return kSUCCESS;
}

// -------------------------------------------------------------------------
void PndMCTestPatternRecoQuality::SetParContainers() {
	// Get Base Container
	//  FairRun* ana = FairRun::Instance();
	//  FairRuntimeDb* rtdb=ana->GetRuntimeDb();

}

// -----   Public method Exec   --------------------------------------------
void PndMCTestPatternRecoQuality::Exec(Option_t* opt) {

	std::cout << "----- Event " << fEventNr << " ------" << std::endl;
	fEventNr++;
	fNGhosts = 0;
	fMCMatch->CreateArtificialStage("MCTrack");


	fIdealTrackData = fMCMatch->GetMCInfo("MCTrack", "FTSHit");							//ideal pattern reconstruction for mvd. Replace MVDHitsStrip with the branch name of your detector
	fIdealTrackData.RemoveType(ioman->GetBranchId("MVDStripClusterCand"));

	FillMapTrackQualifikation();

	for (int i = 0; i < fTrack->GetEntriesFast(); i++){
		PndTrack* myTrack = (PndTrack*) fTrack->At(i);
		PndTrackCand myTrackCand = myTrack->GetTrackCand();
		fMapLinkData["AllHits"].ResetLinks();


		if (fVerbose > 0)
			std::cout << "-I- PndMCTestPatternRecoQuality::Exec Hits for Track:" << i << std::endl;

		for (int branchIndex = 0; branchIndex < fBranchNames.size(); branchIndex++){
			fMapLinkData[fBranchNames[branchIndex]] = GetMCInfoForBranch(fBranchNames[branchIndex], myTrackCand);
			fMapLinkData["AllHits"].AddLinks(fMapLinkData[fBranchNames[branchIndex]]);

			if (fVerbose > 0) std::cout << fBranchNames[branchIndex] << ": " << fMapLinkData[fBranchNames[branchIndex]] << std::endl;
		}

		Int_t mostProbableTrack = AnalyseData();
		if (mostProbableTrack == -1) continue;

		PndMCTrack* mcTrack = (PndMCTrack*)fMCTrack->At(mostProbableTrack);

		TVector3 mom(myTrack->GetParamFirst().GetPx(), myTrack->GetParamFirst().GetPy(), myTrack->GetParamFirst().GetPz());
		TVector3 McMom(mcTrack->GetMomentum());

		fPHisto->Fill(mom.Mag() - McMom.Mag());
		fPtHisto->Fill(mom.Pt() - McMom.Pt());


		if (fVerbose > 0) {
			for (int j = 0; j < fMapLinkData["AllHits"].GetNLinks(); j++){
				FairLink myLink = fMapLinkData["AllHits"].GetLink(j);
				PndMCEntry myEntry = fIdealTrackData.GetEntry(myLink.GetIndex());

				std::cout << "MC Data " << myLink.GetIndex() << ": ";
				PrintTrackDataSummary(myEntry);

			}
			std::cout << std::endl;
		}
	}
	FillQualyHisto();
	PrintTrackQualityMap();

}

Int_t PndMCTestPatternRecoQuality::AnalyseData()
{

	Int_t mostProbableTrack = -1;
	if (fMapLinkData["AllHits"].GetNLinks() == 1){
		mostProbableTrack = fMapLinkData["AllHits"].GetLink(0).GetIndex();
		PndMCEntry myEntry = fIdealTrackData.GetEntry(mostProbableTrack);
		Int_t nMCHits = GetSumOfAllValidMCHits(&myEntry);

		if (nMCHits == fMapLinkData["AllHits"].GetLink(0).GetWeight()){
			fMapTrackQualifikation[fMapLinkData["AllHits"].GetLink(0).GetIndex()] = 2;
		} else {
			fMapTrackQualifikation[fMapLinkData["AllHits"].GetLink(0).GetIndex()] = 3;
		}
	} else {
		Int_t highestCount = 0;
		Int_t allCounts = 0;
		for (int i = 0; i < fMapLinkData["AllHits"].GetNLinks(); i++){
			allCounts += fMapLinkData["AllHits"].GetLink(i).GetWeight();
			if (fMapLinkData["AllHits"].GetLink(i).GetWeight() > highestCount){
				highestCount = fMapLinkData["AllHits"].GetLink(i).GetWeight();
				mostProbableTrack = fMapLinkData["AllHits"].GetLink(i).GetIndex();
			}
		}
		std::cout << "AnalyseData: " << highestCount << " / " << allCounts << " in " << mostProbableTrack << std::endl;
		if ((Double_t)highestCount/(Double_t)allCounts > 0.7){
			fMapTrackQualifikation[mostProbableTrack] = 4;
		}
		else {
			fNGhosts++;
			return -1;;
		}
	}

	CalcEfficiencies(mostProbableTrack);
	return mostProbableTrack;
}

Int_t PndMCTestPatternRecoQuality::GetSumOfAllValidMCHits(FairMultiLinkedData* trackData)
{
	Int_t result = 0;
	for (int branchIndex = 0; branchIndex < fBranchNames.size(); branchIndex++){
		result += trackData->GetLinksWithType(ioman->GetBranchId(fBranchNames[branchIndex])).GetNLinks();
	}
	return result;
}

void PndMCTestPatternRecoQuality::CalcEfficiencies(Int_t mostProbableTrack)
{
	if (mostProbableTrack < 0) return;

	PndMCEntry myEntry = fIdealTrackData.GetEntry(mostProbableTrack);

	for (int branchIndex = 0; branchIndex < fBranchNames.size(); branchIndex++){
		Double_t nMcHits = myEntry.GetLinksWithType(ioman->GetBranchId(fBranchNames[branchIndex])).GetNLinks();
		FairMultiLinkedData foundHits = fMapLinkData[fBranchNames[branchIndex]];
		for (int i = 0; i < foundHits.GetNLinks(); i++){
			if (foundHits.GetLink(i).GetIndex() == mostProbableTrack){
				Double_t nFoundHits = foundHits.GetLink(i).GetWeight();
				fMapEfficiencies[fBranchNames[branchIndex]]->Fill(nMcHits, nFoundHits/nMcHits);
			}
		}
	}
}

void PndMCTestPatternRecoQuality::FillQualyHisto()
{
//	fQualyHisto->Fill(0., fMapTrackQualifikation.size());
	fQualyHisto->Fill(10, fNGhosts);
	for(std::map<Int_t, Int_t>::iterator iter = fMapTrackQualifikation.begin(); iter != fMapTrackQualifikation.end(); iter++){
		std::cout << "Map: " << iter->first << " " << iter->second << std::endl;
		fQualyHisto->Fill(iter->second);
		if (iter->second > -1){
			fQualyHisto->Fill(9);
		}
	}
}


void PndMCTestPatternRecoQuality::FillMapTrackQualifikation()
{
	fMapTrackQualifikation.clear();

	for (int i = 0; i < fIdealTrackData.GetNEntries(); i++){
		if (fIdealTrackData.GetEntry(i).GetNLinks() > 0){
			Bool_t atLeastOneHit = kFALSE;
			for (int branchIndex = 0; branchIndex < fBranchNames.size(); branchIndex++){
				atLeastOneHit = atLeastOneHit | fIdealTrackData.GetEntry(i).GetLinksWithType(ioman->GetBranchId(fBranchNames[branchIndex])).GetNLinks() > 0;
			}
			if (atLeastOneHit)
			{
				fMapTrackQualifikation[i] = -1;
			} else {

//				fMapTrackQualifikation[i] = -1;						//No hits in central tracking detectors
			}
			PndMCEntry entry = fIdealTrackData.GetEntry(i);
			if (PossibleTrack(entry)){
				fMapTrackQualifikation[i] = 0;
			}
		}
	}
	if (fVerbose > 0){
		std::cout << "-I- PndMCTestPatternRecoQuality::FillMapTrackQualifikation:" << std::endl;
		PrintTrackQualityMap();
	}

}

Bool_t PndMCTestPatternRecoQuality::PossibleTrack(FairMultiLinkedData& mcForward)
{
	Bool_t possibleTrack = kFALSE;

	if (mcForward.GetLinksWithType(ioman->GetBranchId("MVDHitsPixel")).GetNLinks() + mcForward.GetLinksWithType(ioman->GetBranchId("MVDHitsStrip")).GetNLinks() > 3 ||
			mcForward.GetLinksWithType(ioman->GetBranchId("STTHit")).GetNLinks() > 5)
	{
		possibleTrack = kTRUE;
	}

	return possibleTrack;
}


FairMultiLinkedData PndMCTestPatternRecoQuality::GetMCInfoForBranch(TString branchName, PndTrackCand& trackCand)
{
	FairMultiLinkedData result;
	FairMultiLinkedData linksOfType = trackCand.GetLinksWithType(ioman->GetBranchId(branchName));
	for (int j = 0; j < linksOfType.GetNLinks(); j++){
		PndMCEntry mcTracks = fMCMatch->GetMCInfoSingle(linksOfType.GetLink(j), "MCTrack");
		mcTracks.SetAllWeights(1.);
		result.AddLinks(mcTracks);
	}
	return result;
}


void PndMCTestPatternRecoQuality::PrintTrackQualityMap()
{
	for (std::map<Int_t, Int_t>::iterator iter = fMapTrackQualifikation.begin(); iter != fMapTrackQualifikation.end(); iter++){
		std::cout << iter->first << " : "  << iter->second << " Data: ";
		PndMCEntry entry = fIdealTrackData.GetEntry(iter->first);
		PrintTrackDataSummary(entry);
	}
	std::cout << std::endl;
}

void PndMCTestPatternRecoQuality::PrintTrackDataSummary(FairMultiLinkedData& trackData)
{
	for (int branchIndex = 0; branchIndex < fBranchNames.size(); branchIndex++){
		std:: cout << fBranchNames[branchIndex] << " " << trackData.GetLinksWithType(ioman->GetBranchId(fBranchNames[branchIndex])).GetNLinks() << " | ";
	}
	std::cout << std::endl;
}


void PndMCTestPatternRecoQuality::Finish() {
	for (int i = 0; i < fBranchNames.size(); i++){
		fMapEfficiencies[fBranchNames[i]]->Write();
	}
	fPHisto->Write();
	fPtHisto->Write();
	fQualyHisto->Write();
	std::cout << "fQualyHisto: NPossible Tracks " << fQualyHisto->GetBinContent(11)
			  << " FullyFound: "    << fQualyHisto->GetBinContent(4)
			  << " PartlyFound: "  << fQualyHisto->GetBinContent(5)
			  << " Spurious: " 	<< fQualyHisto->GetBinContent(6)
			  << " NotFound: " << fQualyHisto->GetBinContent(2)
			  << " Ghosts: "	<< fQualyHisto->GetBinContent(12) << std::endl;
}

ClassImp( PndMCTestPatternRecoQuality);
