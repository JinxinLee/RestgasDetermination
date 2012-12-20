/*
 * PndMCMatch.cxx
 *
 *  Created on: Nov 23, 2009
 *      Author: stockman
 */

#include "PndMCMatch.h"
#include "FairRootManager.h"

#include "TClonesArray.h"
#include <iostream>

ClassImp(PndMCMatch);

PndMCMatch::PndMCMatch():fUltimateStage(0), fVerbose(0) {
	fFinalStageML.SetPersistanceCheck(kFALSE);
}

PndMCMatch::~PndMCMatch() {
	for (TListIterator iter = fList.begin(); iter != fList.end(); iter++){
		delete(iter->second);
	}
	fList.clear();
}

void PndMCMatch::AddElement(Int_t sourceType, int index, Int_t targetType, int link)
{
	FairLink myPair(targetType, link);
	AddElement(sourceType, index, myPair);
}

void PndMCMatch::AddElement(Int_t type, int index, FairLink link){
	fList[type]->AddLink(link, index);
}



void PndMCMatch::SetElements(Int_t sourceType, int index, FairMultiLinkedData* links){
	fList[sourceType]->SetEntry(links, index);
}

void PndMCMatch::InitStage(Int_t type, std::string fileName, std::string branchName)
{
	if (fList[type] == 0){
		PndMCStage* newStage = new PndMCStage(type, fileName, branchName);
		fList[type] = newStage;
		if (fVerbose > 1)
			std::cout << "InitStages: " << *newStage;
	}
	else{
		std::cout << "-W- PndMCMatch::InitStage: Stage " << type << " exists already!" << std::endl;
	}
}

void PndMCMatch::InitStage(std::string branchName, std::string fileName){
	FairRootManager* ioman = FairRootManager::Instance();
	if (ioman->CheckBranch(branchName.c_str()) == 1){
		Int_t type = ioman->GetBranchId(branchName.c_str());
		if (type > -1){
			InitStage(type, fileName, branchName);
		}
		else{
			std::cout << "-W- PndMCMatch::InitStage: Branch name " << branchName << " not registered!" << std::endl;
		}
	}
	else {
		std::cout << "-W- PndMCMatch::InitStage: Branch name " << branchName << " not persistant!" << std::endl;
	}
}

void PndMCMatch::RemoveStage(Int_t type){
	fList.erase(type);
}


void PndMCMatch::SetCommonWeightStages(Float_t weight)
{
	for (int i = 0; i < GetNMCStages(); i++){
		if (GetMCStage(i))
			GetMCStage(i)->SetWeight(weight);
	}
}

PndMCResult PndMCMatch::GetMCInfo(TString start, TString stop){
	FairRootManager* ioman = FairRootManager::Instance();
	return GetMCInfo(ioman->GetBranchId(start), ioman->GetBranchId(stop));
}

PndMCResult PndMCMatch::GetMCInfo(Int_t start, Int_t stop){

	PndMCResult result(start, stop);
	if(!IsTypeInList(start))
		return result;
	if (start < stop)
		return GetMCInfoForward(start, stop);
	else
		return GetMCInfoBackward(start, stop);
}

PndMCEntry PndMCMatch::GetMCInfoSingle(FairLink aLink, TString stop)
{
	FairRootManager* ioman = FairRootManager::Instance();
	return GetMCInfoSingle(aLink, ioman->GetBranchId(stop));
}

PndMCEntry PndMCMatch::GetMCInfoSingle(FairLink aLink, Int_t stop)
{
	PndMCEntry result;
	if(!IsTypeInList((Int_t)aLink.GetType()))
		return result;
	if(!(fList[(Int_t)aLink.GetType()]->GetNEntries() > aLink.GetIndex()))
		return result;

	if (aLink.GetType() < stop)
		return GetMCInfoForwardSingle(aLink, stop);
	else
		return GetMCInfoBackwardSingle(aLink, stop);
}

PndMCResult PndMCMatch::GetMCInfoForward(Int_t start, Int_t stop)
{
	PndMCResult result(start, stop);
	PndMCStage startVec = *(fList[start]);
	for (int i = 0; i < startVec.GetNEntries(); i++){
		FairLink tempLink(startVec.GetStageId(), i);

		//std::cout << "FairLink: " << tempLink << std::endl;
		PndMCEntry tempEntry(GetMCInfoForwardSingle(tempLink, stop));
		if (tempEntry.GetNLinks() > 0)
			if (tempEntry.GetLink(0).GetType() == start)
				tempEntry.DeleteLink(tempEntry.GetLink(0));
		tempEntry.SetSource(start);
		tempEntry.SetPos(i);
		//std::cout << "PndMCEntry: " << tempEntry << std::endl;
		result.SetEntry(tempEntry);
	}
	//result.RemoveType(start);
	return result;

}

PndMCEntry PndMCMatch::GetMCInfoForwardSingle(FairLink link, Int_t stop)
{
	PndMCEntry result;
	ClearFinalStage();

	FairMultiLinkedData tempStage;
	tempStage.SetPersistanceCheck(kFALSE);
	tempStage.AddLink(link, true);
	FindStagesPointingToLinks(tempStage, stop);
	result.SetLinks(fFinalStageML);
	return result;

}

PndMCResult PndMCMatch::GetMCInfoBackward(Int_t start, Int_t stop)
{
	PndMCResult result(start, stop);
	PndMCStage startVec = *(fList[start]);
	for (int i = 0; i < startVec.GetNEntries(); i++){
//		std::cout << "Stage: " << i << std::endl;
/*		ClearFinalStage();
		FairMultiLinkedData temp = startVec.GetEntry(i);
		temp.MultiplyAllWeights(startVec.GetWeight());
		GetNextStage(temp, stop);
		//if (result.GetNEntries() > 0)
*/
		FairLink tempLink(start, i);
		GetMCInfoBackwardSingle(tempLink, stop, startVec.GetWeight());
		result.SetEntry(&fFinalStageML, result.GetNEntries());
	}
	return result;
}

PndMCEntry PndMCMatch::GetMCInfoBackwardSingle(FairLink aLink, Int_t stop, Double_t weight)
{
	PndMCEntry result;
	FairMultiLinkedData multiLink = fList[(Int_t)aLink.GetType()]->GetEntry(aLink.GetIndex());

	ClearFinalStage();
	multiLink.MultiplyAllWeights(weight);
	GetNextStage(multiLink, stop);
	//if (result.GetNEntries() > 0)
	result.SetLinks(fFinalStageML);

	return result;
}


void PndMCMatch::FindStagesPointingToLinks(FairMultiLinkedData links, Int_t stop)
{
	FairMultiLinkedData tempLinks;
	tempLinks.SetPersistanceCheck(kFALSE);
	for (int i = 0; i < links.GetNLinks(); i++){
		FairLink myLink = links.GetLink(i);
		FairMultiLinkedData myNewLinks = FindStagesPointingToLink(myLink);
		myNewLinks.SetPersistanceCheck(kFALSE);
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
	result.SetPersistanceCheck(kFALSE);
	TListIteratorConst iter = fList.find((Int_t)link.GetType());
	for(;iter!= fList.end(); iter++){
		if (iter->second->PosInList(link).GetNLinks() > 0){
			result.AddLinks(iter->second->PosInList(link), true);
		}
	}
	return result;
}


FairMultiLinkedData PndMCMatch::FindLinksToStage(Int_t stage)
{
	FairMultiLinkedData result;
	result.SetPersistanceCheck(kFALSE);
	for (int i = 0; i < GetNMCStages(); i++){
		result.AddLinks(GetMCStage(i)->GetLinksWithType(stage), true);
	}
	return result;
}

void PndMCMatch::CreateArtificialStage(TString branchName, std::string fileName)
{
	FairRootManager* ioman = FairRootManager::Instance();
	std::cout << "Create Artificial Stage for " << branchName;
	std::cout << " with ID " << ioman->GetBranchId(branchName) << std::endl;
	if (ioman->GetBranchId(branchName) > -1){
		CreateArtificialStage(ioman->GetBranchId(branchName), fileName, branchName.Data());
	}
	else {
		std::cout << "-E- PndMCMatch::CreateArtificialStage: Branch does not exist: " << branchName << std::endl;
	}
}

void PndMCMatch::CreateArtificialStage(Int_t stage, std::string fileName, std::string branchName)
{
	FairMultiLinkedData stageLinks = FindLinksToStage(stage);
	stageLinks.SetPersistanceCheck(kFALSE);
	if (stageLinks.GetNLinks() > 0){
		InitStage(stage, fileName, branchName);
		FairMultiLinkedData artData;
		artData.SetLink(FairLink((Int_t)-1, -1));
		for (int i = 0; i < stageLinks.GetNLinks(); i++){
			if (stageLinks.GetLink(i).GetIndex() > -1)
				fList[stage]->SetEntry(&artData, stageLinks.GetLink(i).GetIndex());
		}
		fList[stage]->SetLoaded(kTRUE);

	}
}



void PndMCMatch::GetNextStage(FairMultiLinkedData& startStage, Int_t stopStage){

	PndMCEntry tempStage;

	for (int i = 0; i < startStage.GetNLinks(); i++){
		if (startStage.GetLink(i).GetType() == stopStage){
			AddToFinalStage(startStage.GetLink(i),1);
			if (fVerbose > 0){
				std::cout << "FinalStage: " << fFinalStageML << std::endl;
				std::cout << "---------------------" << std::endl;
			}
		}
		else if (startStage.GetLink(i).GetType() == fUltimateStage){
		}
		else{
			tempStage = GetEntry(startStage.GetLink(i));
			if (fVerbose > 0){
				std::cout << "TempStage Start";
				startStage.GetLink(i).Print();
				std::cout << " --> " << tempStage << std::endl;
			}
//			std::cout << "Link ";
			//startStage.GetLink(i).Print();

//			for (int j = 0; j < tempStage.size(); j++){
//				if (tempStage[j].first > stopStage)
//					AddToFinalStage(startStage[i]);
//			}
			if (tempStage.GetNLinks() == 0){
				AddToFinalStage(startStage.GetLink(i),1);
				if (fVerbose > 0){
					std::cout << "FinalStage: " << fFinalStageML << std::endl;
					std::cout << "---------------------" << std::endl;
				}
			}
			else{
				double tempStageWeight = GetMCStageType(static_cast<Int_t>(tempStage.GetSource()))->GetWeight();
				double startLinkWeight = startStage.GetLink(i).GetWeight();

				if (fVerbose > 0){
					std::cout << "Tempstage " << tempStage.GetSource() << ": weight " << tempStageWeight << std::endl;
					std::cout << "StartLinkWeight " << startLinkWeight << std::endl;
				}
					//std::cout << " StageWeight: " << tempStageWeight << " startLinkWeight: " << startLinkWeight;
				tempStage.MultiplyAllWeights(tempStageWeight);

				if ((tempStageWeight * startLinkWeight) == 0){
					tempStage.MultiplyAllWeights(tempStageWeight);
					tempStage.AddAllWeights(startLinkWeight/startStage.GetNLinks());
					if (fVerbose > 0){
						std::cout << " NLinks: " << tempStage.GetNLinks() << " ";
						std::cout << "AddAllWeights: " << startLinkWeight/startStage.GetNLinks() << std::endl;
					}
				}
				else{
					tempStage.MultiplyAllWeights(startLinkWeight);
					if (fVerbose > 0)
						std::cout << "MultiplyAllWeights: " << startLinkWeight << std::endl;
				}

			}
			if (fVerbose > 0)
				std::cout<< "TempStage Stop: " << tempStage << std::endl;

			GetNextStage(tempStage, stopStage);
		}
	}
}

PndMCEntry PndMCMatch::GetEntry(Int_t type, int index){

	PndMCEntry empty;
	if (index < 0) return empty;
	if (fList[type] == 0)
		return empty;
	if (fList[type]->GetNEntries() > index)
		return fList[type]->GetEntry(index);
	return empty;
}

PndMCEntry PndMCMatch::GetEntry(FairLink link){
	return GetEntry(static_cast<Int_t>(link.GetType()), link.GetIndex());
}

void PndMCMatch::AddToFinalStage(FairLink hitPair, Float_t mult){
	fFinalStageML.AddLink(hitPair, true, mult);
}

void PndMCMatch::ClearFinalStage(){
	fFinalStageML.ResetLinks();
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

bool PndMCMatch::IsTypeInList(Int_t type){
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
		if (IsTypeInList((Int_t)myLink->GetSource())){
			//fList[(Int_t)myLink->GetSource()]->ClearEntries();
			fList[(Int_t)myLink->GetSource()]->SetEntry(*myLink);
			fList[(Int_t)myLink->GetSource()]->SetLoaded(kTRUE);
		}
	}
}
