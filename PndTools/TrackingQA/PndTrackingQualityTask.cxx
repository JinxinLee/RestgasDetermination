// -------------------------------------------------------------------------
// -----                PndTrackingQualityTask source file             -----
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
#include "PndTrackingQualityTask.h"
#include "PndTrackingQualityAnalysis.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairHit.h"
#include "FairMultiLinkedData.h"

#include "PndMCTrack.h"
#include "PndTrack.h"
#include "PndMCEntry.h"

// -----   Default constructor   -------------------------------------------
PndTrackingQualityTask::PndTrackingQualityTask(TString trackBranchName, Bool_t pndTrackData) :
	FairTask("Creates PndMC test"), fEventNr(0), fTrackBranchName(trackBranchName), fPndTrackOrTrackCand(pndTrackData)
{
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndTrackingQualityTask::~PndTrackingQualityTask() {
}

// -----   Public method Init   --------------------------------------------
InitStatus PndTrackingQualityTask::Init() {


	fPHisto = new TH1D("fPHisto", "Momentum Resolution", 1000, -10, 10);
	fPtHisto = new TH1D("fPtHisto", "Transverse Momentum Resolution", 1000, -10, 10);
	fQualyHisto = new TH1I("fQualyHisto", "Quality of Trackfinding", 26, -15.5, 10.5);

	ioman = FairRootManager::Instance();
	if (!ioman) {
		std::cout << "-E- PndTrackingQualityTask::Init: "
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
			<< "-I- PndTrackingQualityTask::Init: Initialization successfull"
			<< std::endl;

	return kSUCCESS;
}

// -------------------------------------------------------------------------
void PndTrackingQualityTask::SetParContainers() {
}

// -----   Public method Exec   --------------------------------------------
void PndTrackingQualityTask::Exec(Option_t* opt) {

	std::cout << "----- Event " << fEventNr << " ------" << std::endl;
	fEventNr++;

	PndTrackingQualityAnalysis qaAna(fTrackBranchName, fPndTrackOrTrackCand);
	qaAna.SetVerbose(fVerbose);
	qaAna.SetHitsBranchNames(fBranchNames);
	qaAna.Init();
	qaAna.AnalyseEvent();

	FillQualyHisto(qaAna.GetTrackQualifikation(), qaAna.GetNGhosts());
	FillMCStatus(qaAna.GetTrackMCStatus());
	FillEfficiencies(qaAna.GetEfficiencies());
	FillPResolution(qaAna.GetPResolution());
	FillPtResolution(qaAna.GetPtResolution());
//	if (fVerbose > 1)
		qaAna.PrintTrackQualityMap();

}

Int_t PndTrackingQualityTask::GetSumOfAllValidMCHits(FairMultiLinkedData* trackData)
{
	Int_t result = 0;
	for (int branchIndex = 0; branchIndex < fBranchNames.size(); branchIndex++){
		result += trackData->GetLinksWithType(ioman->GetBranchId(fBranchNames[branchIndex])).GetNLinks();
	}
	return result;
}


void PndTrackingQualityTask::FillQualyHisto(std::map<Int_t, Int_t> trackQualifikation, Int_t nGhosts)
{

	fQualyHisto->Fill(5, nGhosts);
	for(std::map<Int_t, Int_t>::iterator iter = trackQualifikation.begin(); iter != trackQualifikation.end(); iter++){
		fQualyHisto->Fill(iter->second);
		if (iter->second > 0){
			fQualyHisto->Fill(8);
		}
		else {
			fQualyHisto->Fill(7);
		}
	}
}

void PndTrackingQualityTask::FillMCStatus(std::map<Int_t, Int_t> trackMCStatus)
{
	for(std::map<Int_t, Int_t>::iterator iter = trackMCStatus.begin(); iter != trackMCStatus.end(); iter++){
		fQualyHisto->Fill(iter->second - 6);
	}
}


void PndTrackingQualityTask::FillEfficiencies(std::map<Int_t, std::map<TString, std::pair<Double_t, Int_t > > > efficiencies)
{
	for (std::map<Int_t, std::map<TString, std::pair<Double_t, Int_t> > >::iterator iterTracks = efficiencies.begin(); iterTracks != efficiencies.end(); iterTracks++ ){
		std::map<TString, std::pair<Double_t, Int_t> > branchEfficiency = iterTracks->second;
		for (std::map<TString, std::pair<Double_t, Int_t> >::iterator iterBranch = branchEfficiency.begin(); iterBranch != branchEfficiency.end(); iterBranch++ ){
			fMapEfficiencies[iterBranch->first]->Fill(iterBranch->second.second, iterBranch->second.first);
		}
	}
}

void PndTrackingQualityTask::FillPResolution (std::map<Int_t, Double_t> pResolution)
{
	for (std::map<Int_t, Double_t>::iterator iter = pResolution.begin(); iter != pResolution.end(); iter++ ){
		fPHisto->Fill(iter->second);
	}
}
void PndTrackingQualityTask::FillPtResolution(std::map<Int_t, Double_t> ptResolution)
{
	for (std::map<Int_t, Double_t>::iterator iter = ptResolution.begin(); iter != ptResolution.end(); iter++ ){
		fPtHisto->Fill(iter->second);
	}
}


void PndTrackingQualityTask::Finish() {
	for (int i = 0; i < fBranchNames.size(); i++){
		fMapEfficiencies[fBranchNames[i]]->Write();
	}
	fPHisto->Write();
	fPtHisto->Write();
	fQualyHisto->Write();
	Int_t allTracksWithHits = 0;
	Int_t allPossibleTracksWithHits = 0;
	Int_t allTracksWithHitsNotFound = 0;

	allTracksWithHits += fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(-10));
	allTracksWithHits += fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(-9));
	allTracksWithHits += fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(-8));
	allTracksWithHits += fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(-7));

	allPossibleTracksWithHits += fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(-8));
	allPossibleTracksWithHits += fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(-7));



	allTracksWithHitsNotFound += fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(-4));
	allTracksWithHitsNotFound += fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(-3));
	allTracksWithHitsNotFound += fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(-2));
	allTracksWithHitsNotFound += fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(-1));

	std::cout << "fQualyHisto: All Tracks: " << fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(-11)) + allTracksWithHits << std::endl
			  << " Primary Tracks wo hits: " << fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(-11)) << std::endl
			  << " All Tracks with hits: " << allTracksWithHits << " not Found: " << allTracksWithHitsNotFound << std::endl
			  << " Primary Tracks with 3 hits: " << fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(-10)) << " not Found: " << fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(-4)) << std::endl
			  << " Secondary Tracks with 3 hits: " << fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(-9)) << " not Found: " << fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(-3)) << std::endl
			  << " Primary Tracks possible: " << fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(-8)) << " not Found: " << fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(-2)) << std::endl
			  << " Secondary Tracks possible: " << fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(-7)) << " not Found: " << fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(-1)) << std::endl

			  << " FullyFound: "    << fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(1)) 	<< " "
			  << (Double_t)fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(1)) / allTracksWithHits * 100 << "% "
			  << (Double_t)fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(1)) / allPossibleTracksWithHits * 100 << "% "

			  << " PartlyFound: "  << fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(2)) 	<< " "
			  << (Double_t)fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(2)) / allTracksWithHits * 100 << "% "
			  << (Double_t)fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(2)) / allPossibleTracksWithHits * 100 << "% "

			  << " Spurious: " 	<< fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(3)) 		<< " "
			  << (Double_t)fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(3)) / allTracksWithHits * 100 << "% "
			  << (Double_t)fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(3)) / allPossibleTracksWithHits * 100 << "% "

			  << " Ghosts: "	<< fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(5))		<< " "
			  << (Double_t)fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(5)) / allTracksWithHits * 100 << "% "
			  << (Double_t)fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(5)) / allPossibleTracksWithHits * 100 << "% " << std::endl;
}

ClassImp( PndTrackingQualityTask);
