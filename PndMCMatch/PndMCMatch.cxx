/*
 * PndMCMatch.cxx
 *
 *  Created on: Nov 23, 2009
 *      Author: stockman
 */

#include "PndMCMatch.h"

#include "TClonesArray.h"
#include <iostream>

ClassImp(PndMCMatch);

PndMCMatch::PndMCMatch():fUltimateStage(kMCTrack) {
	// TODO Auto-generated constructor stub

}

PndMCMatch::~PndMCMatch() {
	for (TListIterator iter = fList.begin(); iter != fList.end(); iter++){
		delete(iter->second);
	}
	fList.clear();
}

void PndMCMatch::AddElement(fDetectorType sourceType, int index, fDetectorType targetType, int link)
{
	FairLink myPair(targetType, link);
	AddElement(sourceType, index, myPair);
}

void PndMCMatch::AddElement(fDetectorType type, int index, FairLink link){
	fList[type]->AddLink(link, index);
}



void PndMCMatch::SetElements(fDetectorType sourceType, int index, FairLinkedData* links){
	fList[sourceType]->SetEntry(links, index);
}

void PndMCMatch::InitStage(fDetectorType type, std::string fileName, std::string branchName)
{
	if (fList[type] == 0){
		PndMCStage* newStage = new PndMCStage(type, fileName, branchName);
		fList[type] = newStage;
	}
	else{
		std::cout << "-W- PndMCMatch::InitStage: Stage " << type << " exists already!" << std::endl;
	}
}

void PndMCMatch::RemoveStage(fDetectorType type){
	fList.erase(type);
}


void PndMCMatch::SetCommonWeightStages(Float_t weight)
{
	for (int i = 0; i < GetNMCStages(); i++){
		GetMCStage(i)->SetWeight(weight);
	}
}

PndMCResult PndMCMatch::GetMCInfo(fDetectorType start, fDetectorType stop){

	PndMCResult result(start, stop);
	if(!IsTypeInList(start))
		return result;
	if (start < stop)
		return GetMCInfoForward(start, stop);
	else
		return GetMCInfoBackward(start, stop);
}

PndMCResult PndMCMatch::GetMCInfoForward(fDetectorType start, fDetectorType stop)
{
	PndMCResult result(start, stop);
	PndMCStage startVec = *(fList[start]);
	for (int i = 0; i < startVec.GetNEntries(); i++){
		FairLink tempLink(startVec.GetStageId(), i);

		FairMultiLinkedData tempStage;
		tempStage.AddLink(tempLink, true);
		FindStagesPointingToLinks(tempStage, stop);
		//FindStagesPointingToLink(tempLink);
		result.SetEntry(&fFinalStageML, i);
		fFinalStageML.Reset();
	}
	return result;

}

PndMCResult PndMCMatch::GetMCInfoBackward(fDetectorType start, fDetectorType stop)
{
	PndMCResult result(start, stop);
	PndMCStage startVec = *(fList[start]);
	for (int i = 0; i < startVec.GetNEntries(); i++){
//		std::cout << "Stage: " << i << std::endl;
		ClearFinalStage();
		PndMCEntry temp = startVec.GetEntry(i);
		temp.MultiplyAllWeights(startVec.GetWeight());
		GetNextStage(temp, stop);
		//if (result.GetNEntries() > 0)
		result.SetEntry(&fFinalStageML, result.GetNEntries());
	}
	return result;
}


void PndMCMatch::FindStagesPointingToLinks(FairMultiLinkedData links, fDetectorType stop)
{
	FairMultiLinkedData tempLinks;
	for (int i = 0; i < links.GetNLinks(); i++){
		FairLink myLink = links.GetLink(i);
		FairMultiLinkedData myNewLinks = FindStagesPointingToLink(myLink);
		if (myNewLinks.GetNLinks() == 0)
			fFinalStageML.AddLink(myLink, true);
		else{
			for (int j = 0; j < myNewLinks.GetNLinks(); j++){
				if (myNewLinks.GetLink(j).GetType() == (Int_t)stop)
					fFinalStageML.AddLink(myNewLinks.GetLink(j), true);
				else if (myNewLinks.GetLink(j).GetType() > (Int_t)stop)
					fFinalStageML.AddLink(myLink, true);
				else
					tempLinks.AddLink(myNewLinks.GetLink(j), true);
			}
		}
	}
	if (tempLinks.GetNLinks() != 0)
		FindStagesPointingToLinks(tempLinks, stop);
}


FairMultiLinkedData PndMCMatch::FindStagesPointingToLink(FairLink link)
{
	FairMultiLinkedData result;
	TListIteratorConst iter = fList.find((fDetectorType)link.GetType());
	for(;iter!= fList.end(); iter++){
		if (iter->second->PosInList(link).GetNLinks() > 0){
			result.AddLinks(iter->second->PosInList(link), true);
		}
	}
	return result;
}


FairMultiLinkedData PndMCMatch::FindLinksToStage(fDetectorType stage)
{
	FairMultiLinkedData result;
	for (int i = 0; i < GetNMCStages(); i++){
		result.AddLinks(GetMCStage(i)->GetLinksWithType(stage), true);
	}
	return result;
}


void PndMCMatch::CreateArtificialStage(fDetectorType stage, std::string fileName, std::string branchName)
{
	FairMultiLinkedData stageLinks = FindLinksToStage(stage);
	if (stageLinks.GetNLinks() > 0){
		InitStage(stage, fileName, branchName);
		FairMultiLinkedData artData;
		artData.SetLink(FairLink((Int_t)kUnknown, -1));
		for (int i = 0; i < stageLinks.GetNLinks(); i++){
			fList[stage]->SetEntry(&artData, stageLinks.GetLink(i).GetIndex());
		}
	}
}



void PndMCMatch::GetNextStage(PndMCEntry& startStage, fDetectorType stopStage){

	PndMCEntry tempStage;

	for (int i = 0; i < startStage.GetNLinks(); i++){
		if (startStage.GetLink(i).GetType() == stopStage){
			AddToFinalStage(startStage.GetLink(i),1);
		}
		else if (startStage.GetLink(i).GetType() == fUltimateStage){
		}
		else{
			tempStage = GetEntry(startStage.GetLink(i));
//			std::cout << "Link ";
			//startStage.GetLink(i).Print();

//			for (int j = 0; j < tempStage.size(); j++){
//				if (tempStage[j].first > stopStage)
//					AddToFinalStage(startStage[i]);
//			}
			if (tempStage.GetNLinks() == 0){
				AddToFinalStage(startStage.GetLink(i),1);
			}
			else{
				double tempStageWeight = GetMCStageType(static_cast<fDetectorType>(tempStage.GetSource()))->GetWeight();
				double startLinkWeight = startStage.GetLink(i).GetWeight()/startStage.GetNLinks();
				//std::cout << " StageWeight: " << tempStageWeight << " startLinkWeight: " << startLinkWeight;
				tempStage.MultiplyAllWeights(tempStageWeight);

				if ((tempStageWeight * startLinkWeight) == 0.){
//					std::cout << " NLinks: " << tempStage.GetNLinks();
					tempStage.AddAllWeights(startLinkWeight/tempStage.GetNLinks());
				}
				else
					tempStage.MultiplyAllWeights(startLinkWeight*startStage.GetNLinks());


			}
//			std::cout << " TempStage: " << tempStage;

			GetNextStage(tempStage, stopStage);
		}
	}
}

PndMCEntry PndMCMatch::GetEntry(fDetectorType type, int index){

	PndMCEntry empty;
	if (index < 0) return empty;
	if (fList[type] == 0)
		return empty;
	if (fList[type]->GetNEntries() > index)
		return fList[type]->GetEntry(index);
	return empty;
}

PndMCEntry PndMCMatch::GetEntry(FairLink link){
	return GetEntry(static_cast<fDetectorType>(link.GetType()), link.GetIndex());
}

void PndMCMatch::AddToFinalStage(FairLink hitPair, Float_t mult){
	fFinalStageML.AddLink(hitPair, true, mult);
}

void PndMCMatch::ClearFinalStage(){
	fFinalStageML.Reset();
}

void PndMCMatch::ClearMCList()
{
	for (TListIterator iter = fList.begin(); iter != fList.end(); iter++){
		if (iter->second != 0)
			iter->second->ClearEntries();
		else fList.erase(iter);
	}
	//fList.clear();
}

bool PndMCMatch::IsTypeInList(fDetectorType type){
	for(TListIterator iter = fList.begin(); iter != fList.end(); iter++){
		if (iter->first == type)
			return true;
	}
	return false;
}

void PndMCMatch::LoadInMCLists(TClonesArray* myLinkArray){
	for (int i = 0; i < myLinkArray->GetEntriesFast(); i++){
		PndMCEntry* myLink = (PndMCEntry*)myLinkArray->At(i);
		//std::cout << "myLink.size(): " << myLink->GetNLinks() << ":";
		if (IsTypeInList((fDetectorType)myLink->GetSource())){
			//fList[(fDetectorType)myLink->GetSource()]->ClearEntries();
			fList[(fDetectorType)myLink->GetSource()]->SetEntry(*myLink);
			fList[(fDetectorType)myLink->GetSource()]->SetLoaded(kTRUE);
		}
	}
}
