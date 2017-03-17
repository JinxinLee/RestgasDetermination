// -------------------------------------------------------------------------
// -----                OnlineDBuilderTask source file             -----
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
#include "TVector2.h"
#include "TVector3.h"

// FairRoot includes
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairHit.h"
#include "FairMultiLinkedData.h"

// PandaRoot includes
#include "PndMCTrack.h"
#include "PndTrack.h"

// Class includes
#include "OnlineDBuilderTask.h"


// -----   Default constructor   -------------------------------------------
OnlineDBuilderTask::OnlineDBuilderTask(TString trackBranchName) :
	FairTask("Creates PndMC test")
{
	AddTrackBranch(trackBranchName);
}
// -------------------------------------------------------------------------
void OnlineDBuilderTask::SetParContainers() {

}

// -----   Destructor   ----------------------------------------------------
OnlineDBuilderTask::~OnlineDBuilderTask() {
}

// -----   Public method Init   --------------------------------------------
InitStatus OnlineDBuilderTask::Init() {

	ioman = FairRootManager::Instance();
	if (!ioman) {
		std::cout << "-E- OnlineDBuilderTask::Init: "
				<< "RootManager not instantiated!" << std::endl;
		return kFATAL;
	}

	for (std::map<TString, TClonesArray*>::iterator iter = fTrack.begin(); iter != fTrack.end(); iter++){
		iter->second = (TClonesArray*) ioman->GetObject(iter->first);
	}
	fMCTrack = (TClonesArray*) ioman->GetObject("MCTrack");

	fDPlusArray = ioman->Register("DPlus", "TLorentzVector", "Analysis", kTRUE);
	fDMinusArray = ioman->Register("DMinus", "TLorentzVector", "Analysis", kTRUE);
	fDPlusArrayTrue = ioman->Register("DPlusTrue", "TLorentzVector", "Analysis", kTRUE);
	fDMinusArrayTrue = ioman->Register("DMinusTrue", "TLorentzVector", "Analysis", kTRUE);

	std::cout
			<< "-I- OnlineDBuilderTask::Init: Initialization successfull"
			<< std::endl;

	return kSUCCESS;
}

void OnlineDBuilderTask::Exec(Option_t* opt) {
	fDMinusArray->Delete();
	fDPlusArray->Delete();
	fDMinusArrayTrue->Delete();
	fDPlusArrayTrue->Delete();
	std::cout << "-I- OnlineDBuilderTask::Exec: Event " << FairRootManager::Instance()->GetEntryNr() << std::endl;

	std::vector<PndTrack*> plusTracks;
	std::vector<PndTrack*> minusTracks;

//	std::cout << "-I- OnlineDBuilderTask::Exec: " << " fTrack->GetEntriesFast(): " << fTrack->GetEntriesFast() << std::endl;
	for (std::map<TString, TClonesArray*>::iterator iter = fTrack.begin(); iter != fTrack.end(); iter++){
		TClonesArray* trackArray = iter->second;
		for (Int_t trackIndex = 0; trackIndex < trackArray->GetEntriesFast(); trackIndex++){
			PndTrack* myTrack = (PndTrack*)trackArray->At(trackIndex);
			if (myTrack->GetParamFirst().GetQ() > 0) plusTracks.push_back(myTrack);
			if (myTrack->GetParamFirst().GetQ() < 0) minusTracks.push_back(myTrack);
		}
	}
//	std::cout << "-I- OnlineDBuilderTask::Exec: " << " After first Loop -- " << " plusTracks.size(): " << plusTracks.size() << ", minusTracks.size(): " << minusTracks.size() << std::endl;

	std::cout << "CombinePlusTracks: " << plusTracks.size() << std::endl;
	for (size_t i = 0; i < plusTracks.size(); i++){
		std::cout << plusTracks[i]->GetLinksWithType(FairRootManager::Instance()->GetBranchId("MCTrack")) << std::endl;
	}
	std::cout << " MinusTracks: " << minusTracks.size() << std::endl;
	for (size_t i = 0; i < minusTracks.size(); i++){
		std::cout << minusTracks[i]->GetLinksWithType(FairRootManager::Instance()->GetBranchId("MCTrack")) << std::endl;
	}
	std::vector<std::pair<TLorentzVector, int> > Dminus = CombineFirstWithTwoSecond(plusTracks, minusTracks);
	std::cout << std::endl;
	std::vector<std::pair<TLorentzVector, int > > Dplus = CombineFirstWithTwoSecond(minusTracks, plusTracks);
	std::cout << std::endl;
//	std::cout << "-I- OnlineDBuilderTask::Exec: Dminus.size: " << Dminus.size() << ", Dplus.size: " << Dplus.size() << std::endl;

	for (size_t i = 0; i < Dminus.size(); i++){
		TLorentzVector* vec = new ((*fDMinusArray)[fDMinusArray->GetEntriesFast()]) TLorentzVector(Dminus[i].first);
		std::cout << "DMinus " << Dminus[i].second << " " << vec->Px() << "/" << vec->Py() << "/" << vec->Pz() << " " << vec->M() << std::endl;
		if (Dminus[i].second == -1){
			new ((*fDMinusArrayTrue)[fDMinusArrayTrue->GetEntriesFast()]) TLorentzVector(Dminus[i].first); //TLorentzVector* vec1 =  //[R.K.03/2017] unused variable
		}
	}
	for (size_t i = 0; i < Dplus.size(); i++){
//		std::cout << "-I- OnlineDBuilderTask::Exec:" << "TCA n entries: " << fDPlusArray->GetEntriesFast() << std::endl;
		TLorentzVector* vec = new ((*fDPlusArray)[fDPlusArray->GetEntriesFast()]) TLorentzVector(Dplus[i].first);
		std::cout << "DPlus " << Dplus[i].second << " " << vec->Px() << "/" << vec->Py() << "/" << vec->Pz() << " " << vec->M() << std::endl;

		if (Dplus[i].second == 1){
			new ((*fDPlusArrayTrue)[fDPlusArrayTrue->GetEntriesFast()]) TLorentzVector(Dplus[i].first); //TLorentzVector* vec2 =  //[R.K.03/2017] unused variable
		}
	}
	std::cout << "-I- OnlineDBuilderTask::Exec: Finish " << std::endl << std::endl;
}


std::vector<std::pair<TLorentzVector, int> > OnlineDBuilderTask::CombineFirstWithTwoSecond(std::vector<PndTrack*> first, std::vector<PndTrack*> second)
{
	std::vector<std::pair<TLorentzVector, int> > result;
//	std::cout << "OnlineDBuilderTask::CombineFirstWithTwoSecond: " << "first.size() = " << first.size() << std::endl;
	if (second.size() == 0) return result;
	for (size_t outer = 0; outer < first.size(); outer++){
//		std::cout << "OnlineDBuilderTask::CombineFirstWithTwoSecond: " << "second.size() = " << second.size() << std::endl;
		for (size_t inner1 = 0; inner1 < (second.size() - 1); inner1++){
			//bool temp = inner1 < second.size() - 1; //[R.K. 01/2017] unused variable
//			std::cout << "OnlineDBuilderTask::CombineFirstWithTwoSecond: " << "inner: " << 0 << ", second.size() - 1: " << second.size() - 1 << ", inner < second.size() - 1: " << temp  << std::endl;
			for (size_t inner2 = inner1 + 1; inner2 < second.size(); inner2++){
				PndTrack* kTrack = first[outer];
				PndTrack* pi1Track = second[inner1];
				PndTrack* pi2Track = second[inner2];

				std::cout << "KTrack: " << kTrack->GetLinksWithType(FairRootManager::Instance()->GetBranchId("MCTrack")) << " " << std::endl;
				std::cout << "pi1Track: " << pi1Track->GetLinksWithType(FairRootManager::Instance()->GetBranchId("MCTrack")) << " " << std::endl;
				std::cout << "pi2Track: " << pi2Track->GetLinksWithType(FairRootManager::Instance()->GetBranchId("MCTrack")) << " " << std::endl;

				FairMultiLinkedData kTrackLinks = kTrack->GetLinksWithType(FairRootManager::Instance()->GetBranchId("MCTrack"));
				FairMultiLinkedData pi1Links =  pi1Track->GetLinksWithType(FairRootManager::Instance()->GetBranchId("MCTrack"));
				FairMultiLinkedData pi2Links = pi2Track->GetLinksWithType(FairRootManager::Instance()->GetBranchId("MCTrack"));

				//int mostProbMCK = -1; //[R.K.03/2017] unused variable
				int countK = -1;
				int kMcId = -1;
				for (int i = 0; i < kTrackLinks.GetNLinks(); i++){
					if (kTrackLinks.GetLink(i).GetWeight() > countK){
						countK = kTrackLinks.GetLink(i).GetWeight();
						//mostProbMCK = i; //[R.K.03/2017] unused variable
						kMcId = kTrackLinks.GetLink(i).GetIndex();
					}
				}

				//int mostProbMCPi = -1; //[R.K.03/2017] unused variable
				int countPi = -1;
				int pi1McId = -1;
				for (int i = 0; i < pi1Links.GetNLinks(); i++){
					if (pi1Links.GetLink(i).GetWeight() > countPi){
						countPi = pi1Links.GetLink(i).GetWeight();
						//mostProbMCPi = i; //[R.K.03/2017] unused variable
						pi1McId = pi1Links.GetLink(i).GetIndex();
					}
				}

				//int mostProbMCPi2 = -1; //[R.K.03/2017] unused variable
				//int countPi2 = -1; //[R.K. 01/2017] unused variable
				int pi2McId = -1;
				for (int i = 0; i < pi2Links.GetNLinks(); i++){
					if (pi2Links.GetLink(i).GetWeight() > countPi){
						countPi = pi2Links.GetLink(i).GetWeight();
						//mostProbMCPi2 = i; //[R.K.03/2017] unused variable
						pi2McId = pi2Links.GetLink(i).GetIndex();
					}
				}

				TVector3 kMom = CorrectTrackParForPrimaryVertex(kTrack->GetParamFirst());
				TVector3 piMom = CorrectTrackParForPrimaryVertex(pi1Track->GetParamFirst());
				TVector3 pi2Mom = CorrectTrackParForPrimaryVertex(pi2Track->GetParamFirst());

				TVector3 kMomOrig = kTrack->GetParamFirst().GetMomentum();
				TVector3 piMomOrig = pi1Track->GetParamFirst().GetMomentum();
				TVector3 pi2MomOrig = pi2Track->GetParamFirst().GetMomentum();

//				kMom = kMomOrig;
//				piMom = piMomOrig;
//				pi2Mom = pi2MomOrig;

				TLorentzVector K, pi1, pi2, D;

				K.SetXYZM(kMom.X(), kMom.Y(), kMom.Z(), TDatabasePDG::Instance()->GetParticle(321)->Mass());
				pi1.SetXYZM(piMom.X(), piMom.Y(), piMom.Z(), TDatabasePDG::Instance()->GetParticle(211)->Mass());
				pi2.SetXYZM(pi2Mom.X(), pi2Mom.Y(), pi2Mom.Z(), TDatabasePDG::Instance()->GetParticle(211)->Mass());

				D = K + pi1 + pi2;
				std::cout << "D: " << D.Px() << "/" << D.Py() << "/" << D.Pz() << " " << D.M() << std::endl;
				int matchIndex = 0;
				if (kMcId == 3){
					if(pi1McId == 4){
						if (pi2McId == 5){
							std::cout << "D+ Match!" << std::endl;
							matchIndex = 1;
						}
					}
					else if (pi1McId == 5){
						if (pi2McId == 4){
							std::cout << "D+ Match!" << std::endl;
							matchIndex = 1;
						}
					}
				}
				if (kMcId == 6){
					if(pi1McId == 7){
						if (pi2McId == 8){
							std::cout << "D- Match!" << std::endl;
							matchIndex = -1;
						}
					}
					else if (pi1McId == 8){
						if (pi2McId == 7){
							std::cout << "D- Match!" << std::endl;
							matchIndex = -1;
						}
					}
				}

				std::cout << "Created D Meson: " << matchIndex << std::endl;
				result.push_back(std::make_pair(D, matchIndex));
			}
		}
	}
//	std::cout << "OnlineDBuilderTask::CombineFirstWithTwoSecond: " << " End of Method" << std::endl;
	return result;
}

TVector3 OnlineDBuilderTask::CorrectTrackParForPrimaryVertex(FairTrackParP input) {
	TVector2 tMomVector(input.GetMomentum().X(), input.GetMomentum().Y());
	TVector2 perpMomVector(- tMomVector.Y(), tMomVector.X());
	TVector2 posVector(input.GetPosition().X(), input.GetPosition().Y());

//	TVector2 circleCenter(posVector);
	TVector2 circleCenter = posVector - input.GetQ() * perpMomVector * 100/(0.3 * 2);  //todo this has to be checked if sign is correct!
//	circleCenter -= (tMomVector.Mod() * 100/(0.3 * 2)) * tMomVector.Rotate(TMath::Pi()/2.).Unit() * input.GetQ();

	std::cout << "MomentumVector: " << tMomVector.X() << "/" << tMomVector.Y()
			<< " PerMomVector: " << perpMomVector.X() << "/" << perpMomVector.Y()
			<< " posVector: " << posVector.X() << "/" << posVector.Y() << " Q: " << input.GetQ() << std::endl;

	TVector2 tempVec = posVector - circleCenter;
	TVector2 tempVec2 = -1 * circleCenter;

	std::cout << "CircleCenter: " << circleCenter.X() << "/" << circleCenter.Y()
			<< " tempVec: " << tempVec.X() << "/" << tempVec.Y()
			<< " tempVec2: " << tempVec2.X() << "/" << tempVec2.Y() << std::endl;
 	Double_t deltaPhi = TVector2::Phi_mpi_pi(tempVec.Phi() - tempVec2.Phi());			//todo check if tempVec - tempVec2 or vice versa
	TVector2 corrMom = tMomVector.Rotate(-deltaPhi);										//todo check if deltaPhi or -deltaPhi
	TVector2 corrPos = tempVec.Rotate(-deltaPhi);
	corrPos += circleCenter;
	std::cout << "Phi: " << deltaPhi << " correctedMomentum: " << corrMom.X() << "/" << corrMom.Y() << std::endl;
	std::cout << "CorrectedPosition: " << corrPos.X() << "/" << corrPos.Y() << std::endl;
	std::cout << "Corrected vs. Original -- x: " << corrMom.X() << " vs. " << input.GetMomentum().X() << "; y: " <<  corrMom.Y() << " vs. " << input.GetMomentum().Y() << std::endl;
	return TVector3(corrMom.X(), corrMom.Y(), input.GetMomentum().Z());
}

void OnlineDBuilderTask::Finish(){
	fDMinusArray->Delete();
	fDPlusArray->Delete();
}

ClassImp( OnlineDBuilderTask);
