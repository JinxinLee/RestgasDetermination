// -------------------------------------------------------------------------
// -----                PndMCTestMomentumCompare source file             -----
// -----                  Created 18/07/08  by T.Stockmanns        -----
// -------------------------------------------------------------------------
// libc includes
#include <iostream>

// Root includes
#include "TROOT.h"
#include "TClonesArray.h"

// framework includes
#include "FairRootManager.h"
#include "PndMCTestMomentumCompare.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairHit.h"
#include "FairLinkedData.h"

#include "PndMCTrack.h"
#include "PndTrack.h"
#include "PndMCEntry.h"

// -----   Default constructor   -------------------------------------------
PndMCTestMomentumCompare::PndMCTestMomentumCompare() :
	FairTask("Creates PndMC test"), fEventNr(0), fNPerfectTracks(0) {
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndMCTestMomentumCompare::~PndMCTestMomentumCompare() {
}

// -----   Public method Init   --------------------------------------------
InitStatus PndMCTestMomentumCompare::Init() {

	//  fMCMatch->InitStage(kMCTrack, "", "MCTrack");

	fPHisto = new TH1D("fPHisto", "Momentum Resolution", 1000, -10, 10);
	fPtHisto = new TH1D("fPtHisto", "Transverse Momentum Resolution", 1000,
			-10, 10);
	fQualyHisto = new TH1I("fQualyHisto", "Quality of Track", 11, -0.5, 10.5);

	FairRootManager* ioman = FairRootManager::Instance();
	if (!ioman) {
		std::cout << "-E- PndMCTestMomentumCompare::Init: "
				<< "RootManager not instantiated!" << std::endl;
		return kFATAL;
	}

	fMCMatch = (PndMCMatch*) ioman->GetObject("MCMatch");

	fTrack = (TClonesArray*) ioman->GetObject("MVDTrack");
	fMCTrack = (TClonesArray*) ioman->GetObject("MCTrack");
	fTrackCand = (TClonesArray*) ioman->GetObject("MVDRiemannTrackCand");

	std::cout
			<< "-I- PndMCTestMomentumCompare::Init: Initialization successfull"
			<< std::endl;

	return kSUCCESS;
}

// -------------------------------------------------------------------------
void PndMCTestMomentumCompare::SetParContainers() {
	// Get Base Container
	//  FairRun* ana = FairRun::Instance();
	//  FairRuntimeDb* rtdb=ana->GetRuntimeDb();

}

// -----   Public method Exec   --------------------------------------------
void PndMCTestMomentumCompare::Exec(Option_t* opt) {
	//fMCMatch->CreateArtificialStage(kMCTrack, "", "");
	FairRootManager* ioman = FairRootManager::Instance();
	std::cout << "----- Event " << fEventNr << " ------" << std::endl;
	fEventNr++;

	fMCMatch->CreateArtificialStage("MCTrack");
	fMCMatch->SetCommonWeightStages(0);
	fMCMatch->GetMCStageType("MVDHitsPixel")->SetWeight(1);
	fMCMatch->GetMCStageType("MVDHitsStrip")->SetWeight(1);
	//fMCMatch->Print();
	PndMCResult myResult = fMCMatch->GetMCInfo("MVDTrack", "MCTrack");
	PndMCResult forward1 = fMCMatch->GetMCInfo("MCTrack", "MVDHitsStrip");
	std::cout << "MVDTrack: " << *fMCMatch->GetMCStageType("MVDRiemannTrackCand") << std::endl;
	forward1.RemoveType(ioman->GetBranchId("MVDStripClusterCand"));
	std::cout << myResult;
	std::cout << forward1;

	double dP = 0, P = 0;
	double dPt = 0, Pt = 0;
	myResult.RemoveType(-1);

	for (int i = 0; i < myResult.GetNEntries(); i++){
		if (myResult.GetEntry(i).GetNLinks() > 1){
			std::cout << myResult.GetEntry(i) << " No Match! Fill 5!" << std::endl;
			fQualyHisto->Fill(5);
		}
	}
	for (int i = 0; i < fMCTrack->GetEntries() && i < forward1.GetNEntries(); i++) {
		PndMCTrack* myTrack = (PndMCTrack*)fMCTrack->At(i);
		if (myTrack->GetMotherID() < 0 && forward1.GetEntry(i).GetNLinks() > 2) {
			std::cout << "Forward: " << i << ": " << forward1.GetEntry(i) << std::endl;
			fQualyHisto->Fill(0);
			for (int j = 0; j < myResult.GetNEntries(); j++) {

				if (myResult.GetEntry(j).GetNLinks() == 1) {
					std::cout << "Result " << j << ": " << myResult.GetEntry(j) << std::endl;
					if (myResult.GetEntry(j).GetLink(0).GetIndex() == i){
						std::cout << "NLinks " << forward1.GetEntry(i).GetNLinks() << " Weight: " << myResult.GetEntry(j).GetLink(0).GetWeight() << std::endl;
						if (forward1.GetEntry(i).GetNLinks() == myResult.GetEntry(j).GetLink(0).GetWeight()) {
							fQualyHisto->Fill(9);
							std::cout << "Fill 9" << std::endl;
							break;
						} else {
							fQualyHisto->Fill(8);
							std::cout << "Fill 8" << std::endl;
							break;
						}
					}
				}
				else if (myResult.GetEntry(j).GetNLinks() == 2){
					Float_t weight1 = myResult.GetEntry(j).GetLink(0).GetWeight();
					Float_t weight2 = myResult.GetEntry(j).GetLink(1).GetWeight();
					if ((weight1 > 2 && weight2 == 1) || (weight1 == 1 && weight2 > 2)){
						int mcIndex = -1;
						if (weight1 > 2){
							mcIndex = myResult.GetEntry(j).GetLink(0).GetIndex();
						}
						else{
							mcIndex = myResult.GetEntry(j).GetLink(1).GetIndex();
						}
						if (mcIndex == i){
							fQualyHisto->Fill(7);
							std::cout << "Fill 7" << std::endl;
							break;
						}
					}
				}
			}
		}
	}
	std::cout << "NEntries: " << myResult.GetNEntries() << std::endl;
	Int_t nOneLink = 0;

	for (int i = 0; i < myResult.GetNEntries(); i++) {
		PndMCEntry myLinks = myResult.GetMCLink(i);
		myLinks.RemoveType(-1);
		PndTrack* myTrack = (PndTrack*) fTrack->At(i);
		PndTrackCand* myTrackCand = (PndTrackCand*) fTrackCand->At(
				myTrack->GetLink(0).GetIndex());
		if (fVerbose > 2)std::cout << "TrackMatch for Track " << i << std::endl;
		//myTrackCand->Print();
		if (fVerbose > 2)myTrack->GetParamFirst().Print();
		P = dP = myTrack->GetParamFirst().GetMomentum().Mag();
		Pt = dPt = myTrack->GetParamFirst().GetMomentum().Pt();
		//myTrackCand->Print();

		if (fVerbose > 2)std::cout << "Belongs to: " << std::endl;

		for (int j = 0; j < myLinks.GetNLinks(); j++) {
			std::cout << "NLinks: " << myLinks.GetNLinks() << std::endl;
			if (myLinks.GetNLinks() == 1){
				if (myLinks.GetLink(j).GetType() == ioman->GetBranchId("MCTrack")) {
					nOneLink++;
					fNPerfectTracks++;
					if (fVerbose > 2)std::cout << "MCTrack " << myLinks.GetLink(j).GetIndex()
							<< std::endl;
					if (myLinks.GetLink(j).GetIndex() < fMCTrack->GetEntries()) {
						PndMCTrack* myMCTrack = (PndMCTrack*) fMCTrack->At(
								myLinks.GetLink(j).GetIndex());
						if (fVerbose > 2)std::cout << "P: " << myMCTrack->Get4Momentum().Px() << " "
								<< myMCTrack->Get4Momentum().Py() << " "
								<< myMCTrack->Get4Momentum().Pz() << " Pabs "
								<< myMCTrack->Get4Momentum().P() << " PID: "
								<< myMCTrack->GetPdgCode() << std::endl;
						if (fVerbose > 2)std::cout << "--------------------------------"
								<< std::endl;
						dP -= myMCTrack->Get4Momentum().P();
						dPt -= myMCTrack->Get4Momentum().Pt();
						fPHisto->Fill((dP / P));
						fPtHisto->Fill((dPt / Pt));
					} else
						std::cout << "Index out of bounds: Index : "
								<< myLinks.GetLink(j).GetIndex() << " Bounds: "
								<< fMCTrack->GetEntries() << std::endl;
				}
			}
		}
		if (fVerbose > 2)std::cout << std::endl;
	}
	std::cout << "NOneLink: " << nOneLink << std::endl;
}

void PndMCTestMomentumCompare::Finish() {
	fPHisto->Write();
	fPtHisto->Write();
	fQualyHisto->Write();
	std::cout << "fNPerfectTracks: " << fNPerfectTracks << std::endl;
	std::cout << "fQualyHisto: NPossible Tracks " << fQualyHisto->GetBinContent(1)
			  << " Ghosts: "    << fQualyHisto->GetBinContent(6)
			  << " Spurious: "  << fQualyHisto->GetBinContent(8)
			  << " Found: " 	<< fQualyHisto->GetBinContent(9)
			  << " FullyReco: "	<< fQualyHisto->GetBinContent(10) << std::endl;
}

ClassImp( PndMCTestMomentumCompare);
