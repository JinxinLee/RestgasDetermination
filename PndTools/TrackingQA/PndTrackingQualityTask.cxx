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
	fQualyHisto = new TH1I("fQualyHisto", "Quality of Trackfinding", 12, -1.5, 10.5);

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

	PndTrackingQualityAnalysis qaAna(fTrackBranchName, new RiemannMvdSttGemFunctor(), fPndTrackOrTrackCand);
	qaAna.SetVerbose(fVerbose);
	qaAna.SetHitsBranchNames(fBranchNames);
	qaAna.Init();
	qaAna.AnalyseEvent();

	FillQualyHisto(qaAna.GetTrackQualifikation(), qaAna.GetNGhosts());
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

	fQualyHisto->Fill(10, nGhosts);
	for(std::map<Int_t, Int_t>::iterator iter = trackQualifikation.begin(); iter != trackQualifikation.end(); iter++){
		fQualyHisto->Fill(iter->second);
		if (iter->second > -1){
			fQualyHisto->Fill(9);
		}
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
	std::cout << "fQualyHisto: NPossible Tracks " << fQualyHisto->GetBinContent(11)
			  << " FullyFound: "    << fQualyHisto->GetBinContent(4) 	<< " " << (Double_t)fQualyHisto->GetBinContent(4) / (Double_t)fQualyHisto->GetBinContent(11) * 100 << "% "
			  << " PartlyFound: "  << fQualyHisto->GetBinContent(5) 	<< " " << (Double_t)fQualyHisto->GetBinContent(5) / (Double_t)fQualyHisto->GetBinContent(11) * 100 << "% "
			  << " Spurious: " 	<< fQualyHisto->GetBinContent(6) 		<< " " << (Double_t)fQualyHisto->GetBinContent(6) / (Double_t)fQualyHisto->GetBinContent(11) * 100 << "% "
			  << " NotFound: " << fQualyHisto->GetBinContent(2) 		<< " " << (Double_t)fQualyHisto->GetBinContent(2) / (Double_t)fQualyHisto->GetBinContent(11) * 100 << "% "
			  << " Ghosts: "	<< fQualyHisto->GetBinContent(12)		<< " " << (Double_t)fQualyHisto->GetBinContent(12) / (Double_t)fQualyHisto->GetBinContent(11) * 100 << "% " << std::endl;
}

ClassImp( PndTrackingQualityTask);
