/*
 * PndMCDataCrawler.cpp
 *
 *  Created on: Jul 5, 2011
 *      Author: stockman
 */

#include <PndMCDataCrawler.h>

#include "TTree.h"
#include "TBranch.h"
#include "TClonesArray.h"

ClassImp(PndMCDataCrawler);

PndMCDataCrawler::PndMCDataCrawler(): fUltimateStage(0), fVerbose(2) {
	fIoman = FairRootManager::Instance();
}

PndMCDataCrawler::~PndMCDataCrawler() {
}

void PndMCDataCrawler::Init() {
	fIoman->GetObject("MVDPoint");
	fIoman->GetObject("MVDStripDigis");
	fIoman->GetObject("MVDPixelDigis");
	fIoman->GetObject("MVDHitsPixel");
	fIoman->GetObject("MVDHitsStrip");
	fIoman->GetObject("MVDPixelClusterCand");
	fIoman->GetObject("MVDStripClusterCand");
	fIoman->GetObject("MVDSortedStripDigis");
	fIoman->GetObject("MVDSortedPixelDigis");


	fIoman->GetObject("EmcCluster");
	fIoman->GetObject("EmcBump");
	fIoman->GetObject("EmcDigi");
	fIoman->GetObject("EmcHit");
	fIoman->GetObject("EmcPoint");
	fIoman->GetObject("EmcRecoHit");
	fIoman->GetObject("EmcSharedDigi");
	fIoman->GetObject("EmcWaveform");



	fIoman->GetObject("STTPoint");
	fIoman->GetObject("STTHit");
	fIoman->GetObject("SttHelixHit");
	fIoman->GetObject("STTTrack");
	fIoman->GetObject("STTTrackCand");

	fIoman->GetObject("PndTpcPoint");
	fIoman->GetObject("PndTpcPrimaryCluster");
	fIoman->GetObject("PndTpcDriftedElectron");
	fIoman->GetObject("PndTpcAvalanche");
	fIoman->GetObject("PndTpcSignal");
	fIoman->GetObject("PndTpcDigi");
	fIoman->GetObject("PndTpcCluster");


	fIoman->GetObject("GEMPoint");
	fIoman->GetObject("GEMDigi");
	fIoman->GetObject("GEMHit");

	fIoman->GetObject("PndDchPoint");
	fIoman->GetObject("PndDchDigi");
	fIoman->GetObject("PndDchCylinderHit");

	fIoman->GetObject("MdtPoint");
	fIoman->GetObject("MdtHit");
	fIoman->GetObject("MdtTrk");

	fIoman->GetObject("LheHit");
	fIoman->GetObject("LheCandidate");
	fIoman->GetObject("LheTrack");
	fIoman->GetObject("MVDIdealTrackCand");
	fIoman->GetObject("MVDRiemannTrackCand");
	fIoman->GetObject("PndTrack");
	fIoman->GetObject("LheGenTrack");
	fIoman->GetObject("PidChargedCand");
	fIoman->GetObject("PidNeutralCand");
}

FairMultiLinkedData PndMCDataCrawler::GetInfo(FairMultiLinkedData startLink, TString stopStage)
{
	return GetInfo(startLink, fIoman->GetBranchId(stopStage));
}

FairMultiLinkedData PndMCDataCrawler::GetInfo(FairMultiLinkedData startLink, Int_t stopStageId)
{
	fFinalStage.Reset();
	if (fVerbose > 1)std::cout << "StartLink: " << startLink << std::endl;
	if (fVerbose > 1)std::cout << "StopStageLink: " << fIoman->GetBranchName(stopStageId) << std::endl;
	GetNextStage(startLink, stopStageId);
	return fFinalStage;
}

FairMultiLinkedData* PndMCDataCrawler::GetEntry(FairLink link){
	return GetEntry(link.GetFile(), link.GetEntry(), link.GetType(), link.GetIndex());
}

FairMultiLinkedData* PndMCDataCrawler::GetEntry(Int_t fileId, Int_t eventNr, Int_t type, Int_t index){
	if (fVerbose > 0) std::cout << "Requested Entry: " << fileId << " " << eventNr << " " << type << " " << index << std::endl;
	if (index < 0) return 0;

	TTree* dataTree;
	if (fileId < 0)
		dataTree = fIoman->GetInTree();
	else if (fileId == 0)
		dataTree = fIoman->GetBGChain();
	else
		dataTree = fIoman->GetSignalChainNo(fileId);

	if (dataTree == 0)
		dataTree = fIoman->GetInTree();

	TBranch* dataBranch = dataTree->GetBranch(fIoman->GetBranchName(type));
	if (fVerbose > 0) std::cout << "DataBranch: " << fIoman->GetBranchName(type) << " " << dataBranch << std::endl;
	if (dataBranch == 0)
		return 0;

	TClonesArray* dataArray = (TClonesArray*)fIoman->GetObject(fIoman->GetBranchName(type));

	if (eventNr < 0){

	}
	else if (eventNr < dataBranch->GetEntries()){
		dataBranch->GetEntry(eventNr);
	}
	else
		return 0;

	if (fVerbose > 0) std::cout << "DataArray size: " << dataArray->GetEntriesFast() << std::endl;
	if (index < dataArray->GetEntriesFast()){
		FairMultiLinkedData* data = (FairMultiLinkedData*)dataArray->At(index);
		if (fVerbose > 0) std::cout << "Data: " << *data << std::endl;
		return data;
	}
	return 0;
}

void PndMCDataCrawler::GetNextStage(FairMultiLinkedData& startStage, Int_t stopStage)
{
	FairMultiLinkedData* tempStage;
	for (int i = 0; i < startStage.GetNLinks(); i++){
		FairLink actualLink = startStage.GetLink(i);
		if (fVerbose > 0) std::cout << "Actual Link: " << actualLink << std::endl;
		if (actualLink.GetType() == stopStage){
			AddToFinalStage(actualLink, 1);
			if (fVerbose > 0){
				std::cout << "ActualLink.Type == stopStage" << std::endl;
				std::cout << "FinalStage: " << fFinalStage << std::endl;
				std::cout << "---------------------" << std::endl;
			}
		}
		else if (actualLink.GetType() == fUltimateStage){
			AddToFinalStage(actualLink, 1);
			if (fVerbose > 0){
				std::cout << "ActualLink.Type == UltimateStage" << std::endl;
				std::cout << "FinalStage: " << fFinalStage << std::endl;
				std::cout << "---------------------" << std::endl;
			}
		}
		else{
			tempStage = GetEntry(actualLink);
			if (tempStage == 0){
				AddToFinalStage(actualLink, 1);
				if (fVerbose > 0){
					std::cout << "TempStage == 0" << std::endl;
					std::cout << "FinalStage: " << fFinalStage << std::endl;
					std::cout << "---------------------" << std::endl;
				}
				continue;
			}
			if (fVerbose > 0){
				std::cout << "TempStage Start";
				actualLink.Print();
				std::cout << " --> " << *tempStage << std::endl;
			}
			if (tempStage->GetNLinks() == 0){
				AddToFinalStage(actualLink,1);
				if (fVerbose > 0){
					std::cout << "tempStage->GetNLinks == 0" << std::endl;
					std::cout << "FinalStage: " << fFinalStage << std::endl;
					std::cout << "---------------------" << std::endl;
				}
			}
			else{
				double tempStageWeight = 1;
				double startLinkWeight = startStage.GetLink(i).GetWeight();

				if (fVerbose > 0){
					//std::cout << "Tempstage " << tempStage.GetSource() << ": weight " << tempStageWeight << std::endl;
					std::cout << "StartLinkWeight " << startLinkWeight << std::endl;
				}
					//std::cout << " StageWeight: " << tempStageWeight << " startLinkWeight: " << startLinkWeight;
				tempStage->MultiplyAllWeights(tempStageWeight);

				if ((tempStageWeight * startLinkWeight) == 0){
					tempStage->MultiplyAllWeights(tempStageWeight);
					tempStage->AddAllWeights(startLinkWeight/startStage.GetNLinks());
					if (fVerbose > 0){
						std::cout << " NLinks: " << tempStage->GetNLinks() << " ";
						std::cout << "AddAllWeights: " << startLinkWeight/startStage.GetNLinks() << std::endl;
					}
				}
				else{
					tempStage->MultiplyAllWeights(startLinkWeight);
					if (fVerbose > 0)
						std::cout << "MultiplyAllWeights: " << startLinkWeight << std::endl;
				}

			}
//			if (fVerbose > 0)
//				std::cout<< "TempStage Stop: " << *tempStage << std::endl;

			GetNextStage(*tempStage, stopStage);
		}
	}
}

void PndMCDataCrawler::AddToFinalStage(FairLink hitPair, Float_t mult){
	fFinalStage.AddLink(hitPair, false, mult);
}
