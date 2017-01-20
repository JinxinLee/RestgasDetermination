// -------------------------------------------------------------------------
// -----                PndTrackingQualityTask source file             -----
// -----                  Created 18/07/08  by T.Stockmanns        -----
// -------------------------------------------------------------------------

/**
 * The quality numbers, qualityNumbers, as used here are defined in PndTrackingQualityAnalysis.h
 */

// C++ includes
#include <iostream>
#include <functional>

// ROOT includes
#include "TROOT.h"
#include "TClonesArray.h"
#include "THStack.h"

// FairRoot includes
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairHit.h"
#include "FairMultiLinkedData.h"

// PandaRoot includes
#include "PndMCTrack.h"
#include "PndTrack.h"
#include "PndMCEntry.h"
#include "RhoHistogram/RhoTuple.h"

// Class includes
#include "PndTrackingQualityTask.h"
#include "PndTrackingQualityAnalysis.h"

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
	InitializeHistograms();

	ioman = FairRootManager::Instance();
	if (!ioman) {
		std::cout << "-E- PndTrackingQualityTask::Init: "
				<< "RootManager not instantiated!" << std::endl;
		return kFATAL;
	}

	fMCMatch = (PndMCMatch*) ioman->GetObject("MCMatch");

	fTrack = (TClonesArray*) ioman->GetObject(fTrackBranchName);
	fMCTrack = (TClonesArray*) ioman->GetObject("MCTrack");

	fTuple = new RhoTuple("qaTuple", "QA Rho");

	if (fBranchNames.size() == 0){
		AddHitsBranchName("MVDHitsPixel");
		AddHitsBranchName("MVDHitsStrip");
		AddHitsBranchName("STTHit");
//		AddHitsBranchName("GEMHit");
		std::cout << "PndTrackingQualityAnalysis::Init() CorrectedSkewedHits present: " << FairRootManager::Instance()->GetBranchId("CorrectedSkewedHits") << " ";
		if (FairRootManager::Instance()->GetBranchId("CorrectedSkewedHits")  > 0){
			std::cout << "kTRUE";
			AddHitsBranchName("CorrectedSkewedHits");
		}
		std::cout << std::endl;
	}


	for (size_t i = 0; i < fBranchNames.size(); i++){
		fMapEfficiencies[fBranchNames[i]] = new TH2D(fBranchNames[i], fBranchNames[i], 100, 0., 100., 50, 0, 1.1);
		fMapEfficiencies[fBranchNames[i]]->SetDrawOption("COLz");
	}

	std::cout
			<< "-I- PndTrackingQualityTask::Init: Initialization successfull"
			<< std::endl;

	return kSUCCESS;
}

void PndTrackingQualityTask::InitializeHistograms() {
	fPHisto = new TH1D("fPHisto", "Momentum Resolution", 1000, -1, 1);
	fPHisto->GetXaxis()->SetTitle("p^{RECO} - p^{MC} / GeV");
	fPHisto->GetYaxis()->SetTitle("counts");
	fPRelHisto = new TH1D("fPRelHisto", "Relative Momentum Resolution", 1000, -1, 1);
	fPRelHisto->GetXaxis()->SetTitle("(p^{RECO} - p^{MC}) / p^{MC}");
	fPRelHisto->GetYaxis()->SetTitle("counts");
	fPtHisto = new TH1D("fPtHisto", "Transverse Momentum Resolution", 1000, -1, 1);
	fPtHisto->GetXaxis()->SetTitle("p_{t}^{RECO} - p_{t}^{MC} / GeV");
	fPtHisto->GetYaxis()->SetTitle("counts");
	fPtRelHisto = new TH1D("fPtRelHisto", "Relative Transverse Momentum Resolution", 1000, -1, 1);
	fPtRelHisto->GetXaxis()->SetTitle("(p_{t}^{RECO} - p_{t}^{MC}) / p_{t}^{MC}");
	fPtRelHisto->GetYaxis()->SetTitle("counts");

	fQualyHisto = new TH1I("fQualyHisto", "Quality of Trackfinding;;Counts", 26, -15.5, 10.5);
	fQualyHisto->SetDrawOption("TEXT HIST");
	LabelQualyHistogram(fQualyHisto);

	fQualyHisto_all = new TH1I("fQualyHisto_all", "Quality of Trackfinding;;Counts", 26, -15.5, 10.5);
	fQualyHisto_pos = new TH1I("fQualyHisto_pos", "Quality of Trackfinding;;Counts", 26, -15.5, 10.5);
	fQualyHisto_neg = new TH1I("fQualyHisto_neg", "Quality of Trackfinding;;Counts", 26, -15.5, 10.5);
	fQualyHisto_mc = new TH1I("fQualyHisto_mc", "Quality of Trackfinding;;Counts", 26, -15.5, 10.5);
	fQualyStack = new THStack();
}

void PndTrackingQualityTask::LabelQualyHistogram(TH1 * hist) {
	hist->GetXaxis()->SetBinLabel(hist->FindFixBin(qualityNumbers::kFullyFound), "Fully found");
	hist->GetXaxis()->SetBinLabel(hist->FindFixBin(qualityNumbers::kPartiallyFound), "Partially found");
	hist->GetXaxis()->SetBinLabel(hist->FindFixBin(qualityNumbers::kSpuriousFound), "Spurious found");
	hist->GetXaxis()->SetBinLabel(hist->FindFixBin(qualityNumbers::kGhost), "Ghosts");
	hist->GetXaxis()->SetBinLabel(hist->FindFixBin(qualityNumbers::kNotFound), "Total not found");
	hist->GetXaxis()->SetBinLabel(hist->FindFixBin(qualityNumbers::kFound), "Total found");
	hist->GetXaxis()->SetBinLabel(hist->FindFixBin(qualityNumbers::kPossibleSec), "Possible, Sec.");
	hist->GetXaxis()->SetBinLabel(hist->FindFixBin(qualityNumbers::kPossiblePrim), "Possible, Prim.");
	hist->GetXaxis()->SetBinLabel(hist->FindFixBin(qualityNumbers::kAtLeastThreeSec), ">= 3 Hits, Sec.");
	hist->GetXaxis()->SetBinLabel(hist->FindFixBin(qualityNumbers::kAtLeastThreePrim), ">= 3 Hits, Prim.");
	hist->GetXaxis()->SetBinLabel(hist->FindFixBin(qualityNumbers::kLessThanThreePrim), "< 3 Hits, Prim.");
	hist->GetXaxis()->SetBinLabel(hist->FindFixBin(qualityNumbers::kMcPossibleSec), "MC: Possible, Sec.");
	hist->GetXaxis()->SetBinLabel(hist->FindFixBin(qualityNumbers::kMcPossiblePrim), "MC: Possible, Prim.");
	hist->GetXaxis()->SetBinLabel(hist->FindFixBin(qualityNumbers::kMcAtLeastThreeSec), "MC: >= 3 Hits, Sec.");
	hist->GetXaxis()->SetBinLabel(hist->FindFixBin(qualityNumbers::kMcAtLeastThreePrim), "MC: >= 3 Hits, Prim.");
	hist->GetXaxis()->SetBinLabel(hist->FindFixBin(qualityNumbers::kMcLessThanThreePrim), "MC: < 3 Hits, Prim.");
}


// -------------------------------------------------------------------------
void PndTrackingQualityTask::SetParContainers() {
}

// -----   Public method Exec   --------------------------------------------
void PndTrackingQualityTask::Exec(Option_t* opt) {

	std::cout << "----- Event " << fEventNr << " ------" << std::endl;


	PndTrackingQualityAnalysis qaAna(fTrackBranchName, new CircleHoughTrackFunctor(), fPndTrackOrTrackCand);
	qaAna.SetVerbose(fVerbose);
	qaAna.SetHitsBranchNames(fBranchNames);
	qaAna.Init();
	qaAna.AnalyseEvent();

	std::map<Int_t, Int_t> qualiMap = qaAna.GetTrackQualification();
	std::map<Int_t, Int_t> mcStatusMap = qaAna.GetTrackMCStatus();
	std::map<Int_t, Int_t> mcFoundMap = qaAna.GetMCTrackFound();
	std::map<Int_t, TVector3> recoPMap = qaAna.GetP();
//	std::map<Int_t, Double_t> recoPtMap = qaAna.GetPt();

	FillQualyHisto(qualiMap, qaAna.GetNGhosts());
	FillMCStatus(mcStatusMap);
	FillEfficiencies(qaAna.GetEfficiencies());
	MapToHist(qaAna.GetPResolution(), fPHisto);
	MapToHist(qaAna.GetPResolutionRel(), fPRelHisto);
	MapToHist(qaAna.GetPtResolution(), fPtHisto);
	MapToHist(qaAna.GetPtResolutionRel(), fPtRelHisto);

	// Save the Tree (/RhoTuple) for some possible additional analysis
	for (std::map<Int_t, Int_t>::iterator iter = qualiMap.begin(); iter != qualiMap.end(); iter++) {
		Int_t mcTrackId = iter->first;
		Int_t trackQuality = iter->second;

		TVector3 recoMomentum = recoPMap[mcTrackId];

		PndMCTrack * myMcTrack = (PndMCTrack *) fMCTrack->At(mcTrackId);
		TVector3 mcMomentum = myMcTrack->GetMomentum();
		Int_t pdgId = myMcTrack->GetPdgCode();

		fTuple->Column("EvtNr", (Int_t) fEventNr);  // number of the currently processed event

		fTuple->Column("McTrackId", (Int_t) mcTrackId);  // id of the currently processed track
		fTuple->Column("McTrackFoundNTimes", (Int_t) mcFoundMap[mcTrackId]);  // A given MC track was found N times
		fTuple->Column("TrackQuality", (Int_t) trackQuality);  // the quality of the current track
		fTuple->Column("Mc_TrackQuality", (Int_t) mcStatusMap[mcTrackId] - 6);  // the mc quality of the current track, -6 to match the global qualityNumbers

		fTuple->Column("PdgId", (Int_t) pdgId);
		fTuple->Column("Mc_px", (Double_t) mcMomentum.Px());  // To compare to the additionally saved pt histogram, call Draw("Reco_pt:Mc_pt","TrackQuality > 0") on the Tuple
		fTuple->Column("Mc_py", (Double_t) mcMomentum.Py());
		fTuple->Column("Mc_pz", (Double_t) mcMomentum.Pz());
		fTuple->Column("Mc_pt", (Double_t) mcMomentum.Pt());
		fTuple->Column("Reco_px", (Double_t) recoMomentum.Px());
		fTuple->Column("Reco_py", (Double_t) recoMomentum.Py());
		fTuple->Column("Reco_pz", (Double_t) recoMomentum.Pz());
		fTuple->Column("Reco_pt", (Double_t) recoMomentum.Pt());
//		fTuple->Column("Reco_pt2", (Double_t) recoPtMap[mcTrackId]);  // cross check

		fTuple->DumpData();
	}

//	if (fVerbose > 1)
		qaAna.PrintTrackQualityMap();
	fEventNr++;
}

Int_t PndTrackingQualityTask::GetSumOfAllValidMCHits(FairMultiLinkedData* trackData)
{
	Int_t result = 0;
	for (size_t branchIndex = 0; branchIndex < fBranchNames.size(); branchIndex++){
		result += trackData->GetLinksWithType(ioman->GetBranchId(fBranchNames[branchIndex])).GetNLinks();
	}
	return result;
}


void PndTrackingQualityTask::FillQualyHisto(std::map<Int_t, Int_t> trackQualifikation, Int_t nGhosts)
{

	fQualyHisto->Fill(qualityNumbers::kGhost, nGhosts);
	for(std::map<Int_t, Int_t>::iterator iter = trackQualifikation.begin(); iter != trackQualifikation.end(); iter++){
		fQualyHisto->Fill(iter->second);
		if (iter->second > 0){
			fQualyHisto->Fill(qualityNumbers::kFound);
		}
		else {
			fQualyHisto->Fill(qualityNumbers::kNotFound);
		}
	}
}

void PndTrackingQualityTask::FillMCStatus(std::map<Int_t, Int_t> trackMCStatus)
{
	int mcOffset = qualityNumbers::kPossibleSec - qualityNumbers::kMcPossibleSec;
	for(std::map<Int_t, Int_t>::iterator iter = trackMCStatus.begin(); iter != trackMCStatus.end(); iter++){
		fQualyHisto->Fill(iter->second - mcOffset);  // mcOffset = 6
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
void PndTrackingQualityTask::MapToHist (std::map<Int_t, Double_t> map, TH1 * histo) {
	for (std::map<Int_t, Double_t>::iterator iter = map.begin(); iter != map.end(); iter++ ){
		histo->Fill(iter->second);
	}
}

void PndTrackingQualityTask::Finish() {
	ColorHistogram();
	fQualyStack->Add(fQualyHisto_mc);
	fQualyStack->Add(fQualyHisto_neg);
	fQualyStack->Add(fQualyHisto_pos);
	fQualyStack->Add(fQualyHisto_all);
	fQualyStack->SetName("fQualyHistoColor");
	fQualyStack->SetTitle(fQualyHisto->GetTitle());
	// gROOT->SetBatch(kTRUE);
	// fQualyHisto->Draw();
	// LabelQualyHistogram((TH1*) fQualyStack);

	for (size_t i = 0; i < fBranchNames.size(); i++){
		fMapEfficiencies[fBranchNames[i]]->Write();
	}
	fPHisto->Write();
	fPRelHisto->Write();
	fPtHisto->Write();
	fPtRelHisto->Write();
	fQualyHisto->Write();
	fQualyStack->Write();
	fTuple->GetInternalTree()->Write();
	
	Int_t allTracksWithHits = 0;
	Int_t allPossibleTracksWithHits = 0;
	Int_t allTracksWithHitsNotFound = 0;

	allTracksWithHits += fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kMcAtLeastThreePrim));  // Todo
	allTracksWithHits += fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kMcAtLeastThreeSec));
	allTracksWithHits += fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kMcPossiblePrim));
	allTracksWithHits += fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kMcPossibleSec));

	allPossibleTracksWithHits += fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kMcPossiblePrim));
	allPossibleTracksWithHits += fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kMcPossibleSec));



	allTracksWithHitsNotFound += fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kAtLeastThreePrim));
	allTracksWithHitsNotFound += fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kAtLeastThreeSec));
	allTracksWithHitsNotFound += fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kPossiblePrim));
	allTracksWithHitsNotFound += fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kPossibleSec));

	std::cout << "fQualyHisto: All Tracks: " << fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(-11)) + allTracksWithHits << std::endl
			  << " Primary Tracks < 3 hits: " << fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(-11)) << std::endl
			  << " All Tracks with hits: " << allTracksWithHits << ". Not Found: " << allTracksWithHitsNotFound << std::endl
			  << " Primary Tracks with >= 3 hits, but not a possible track: " << fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kMcAtLeastThreePrim)) << ". Not Found: " << fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kAtLeastThreePrim)) << std::endl
			  << " Secondary Tracks with >= 3 hits, but not a possible track: " << fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kMcAtLeastThreeSec)) << ". Not Found: " << fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kAtLeastThreeSec)) << std::endl
			  << " Primary Tracks possible: " << fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kMcPossiblePrim)) << ". Not Found: " << fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kPossiblePrim)) << std::endl
			  << " Secondary Tracks possible: " << fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kMcPossibleSec)) << ". Not Found: " << fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kPossibleSec)) << std::endl

			  << " FullyFound: "    << fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kFullyFound)) 	<< " "
			  << (Double_t)fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kFullyFound)) / allTracksWithHits * 100 << "% (of all tracks), "
			  << (Double_t)fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kFullyFound)) / allPossibleTracksWithHits * 100 << "% (of all tracks possible)"

			  << " PartlyFound: "  << fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kPartiallyFound)) 	<< " "
			  << (Double_t)fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kPartiallyFound)) / allTracksWithHits * 100 << "% "
			  << (Double_t)fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kPartiallyFound)) / allPossibleTracksWithHits * 100 << "% "

			  << " Spurious: " 	<< fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kSpuriousFound)) 		<< " "
			  << (Double_t)fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kSpuriousFound)) / allTracksWithHits * 100 << "% "
			  << (Double_t)fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kSpuriousFound)) / allPossibleTracksWithHits * 100 << "% "

			  << " Ghosts: "	<< fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kGhost))		<< " "
			  << (Double_t)fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kGhost)) / allTracksWithHits * 100 << "% "
			  << (Double_t)fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kGhost)) / allPossibleTracksWithHits * 100 << "% " << std::endl;
}

void PndTrackingQualityTask::ColorHistogram() {
	fQualyHisto_pos->SetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kGhost), fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kGhost)));
	fQualyHisto_pos->SetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kSpuriousFound), fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kSpuriousFound)));
	fQualyHisto_pos->SetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kPartiallyFound), fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kPartiallyFound)));
	fQualyHisto_pos->SetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kFullyFound), fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kFullyFound)));
	fQualyHisto_pos->SetLineColor(kGreen + 2);

	fQualyHisto_all->SetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kFound), fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kFound)));
	fQualyHisto_all->SetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kNotFound), fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kNotFound)));
	fQualyHisto_all->SetLineColor(kBlack);


	fQualyHisto_neg->SetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kPossibleSec), fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kPossibleSec)));
	fQualyHisto_neg->SetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kPossiblePrim), fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kPossiblePrim)));
	fQualyHisto_neg->SetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kAtLeastThreeSec), fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kAtLeastThreeSec)));
	fQualyHisto_neg->SetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kAtLeastThreePrim), fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kAtLeastThreePrim)));
	fQualyHisto_neg->SetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kLessThanThreePrim), fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kLessThanThreePrim)));
	fQualyHisto_neg->SetLineColor(kRed + 2);

	fQualyHisto_mc->SetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kMcPossibleSec), fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kMcPossibleSec)));
	fQualyHisto_mc->SetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kMcPossiblePrim), fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kMcPossiblePrim)));
	fQualyHisto_mc->SetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kMcAtLeastThreeSec), fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kMcAtLeastThreeSec)));
	fQualyHisto_mc->SetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kMcAtLeastThreePrim), fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kMcAtLeastThreePrim)));
	fQualyHisto_mc->SetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kMcLessThanThreePrim), fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kMcLessThanThreePrim)));
	fQualyHisto_mc->SetLineColor(kBlue);

}

ClassImp( PndTrackingQualityTask);
