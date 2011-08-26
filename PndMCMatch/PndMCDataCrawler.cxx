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

PndMCDataCrawler::PndMCDataCrawler(): fUltimateStage(0), fVerbose(0) {
	fIoman = FairRootManager::Instance();
}

PndMCDataCrawler::~PndMCDataCrawler() {
}

FairMultiLinkedData PndMCDataCrawler::GetInfo(FairMultiLinkedData startLink, TString stopStage)
{
	return GetInfo(startLink, fIoman->GetBranchId(stopStage));
}

FairMultiLinkedData PndMCDataCrawler::GetInfo(FairMultiLinkedData startLink, Int_t stopStageId)
{
	fFinalStage.Reset();
	if (fVerbose > 1)std::cout << "StartLink: " << startLink << std::endl;
	GetNextStage(startLink, stopStageId);
	return fFinalStage;
}

FairMultiLinkedData* PndMCDataCrawler::GetEntry(FairLink link){
	return GetEntry(link.GetFile(), link.GetEntry(), link.GetType(), link.GetIndex());
}

FairMultiLinkedData* PndMCDataCrawler::GetEntry(Int_t fileId, Int_t eventNr, Int_t type, Int_t index){
	if (fVerbose > 0)std::cout << "Requested Entry: " << fileId << " " << eventNr << " " << type << " " << index << std::endl;
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
