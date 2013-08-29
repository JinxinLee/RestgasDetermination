/*
 * PndMCObject.cpp
 *
 *  Created on: Dec 1, 2009
 *      Author: stockman
 */

#include "PndMCObject.h"

ClassImp(PndMCObject);

PndMCObject::PndMCObject() {
	//std::cout << "-I- PndMCObject::PndMCObject : Use of default constructor" << std::endl;
}

PndMCObject::~PndMCObject() {
	// TODO Auto-generated destructor stub
}


void PndMCObject::SetEntry(PndMCEntry entry){
	if (entry.GetPos() < 0){
		std::cout << "-E- PndMCObject::SetEntry(PndMCEntry): pos " << entry.GetPos() << std::endl;
		return;
	}
	AdoptSize(entry.GetPos());
	fStage[entry.GetPos()] = entry;
}

void PndMCObject::SetEntry(std::vector<Int_t> type, std::vector<Int_t> link, int index){
	PndMCEntry myEntry;
	myEntry.SetPos(index);
	myEntry.SetPersistanceCheck(kFALSE);
	for (int i = 0; i < type.size(); i++){
		myEntry.AddLink(FairLink(type[i],link[i]));
	}
	SetEntry(myEntry);
}

void PndMCObject::SetEntry(FairMultiLinkedData* data, int index)
{
	AdoptSize(index);
	fStage[index].SetLinks(data->GetLinks());
}

void PndMCObject::SetLink(FairLink link, int index)
{
	if (index < fStage.size()){
		fStage[index].ResetLinks();
	}
	AddLink(link, index);
}


void PndMCObject::AddLink(FairLink link, int index)
{
	AdoptSize(index);
	fStage[index].AddLink(link);
	//std::cout << "AddLink " << index << ": "<< fStageDet[index][fStageDet[index].size()-1] << " " << fStageHit[index][fStageHit[index].size()-1] << std::endl;
}

void PndMCObject::AdoptSize(int index){
	int start = fStage.size();
	while (fStage.size() < index+1){
		PndMCEntry myVec;
		myVec.SetPos(fStage.size());
		myVec.SetSource(GetStageId());
		myVec.SetPersistanceCheck(kFALSE);
		//std::pair<int,int> myPair(fStageId, start);
		//myVec.AddLink(myPair);
		fStage.push_back(myVec);
		start++;
	}
}


PndMCEntry PndMCObject::GetEntry(int index) const
{
	if (index > -1)
		return fStage.at(index);
	else {
		std::cout << "-W- PndMCObject::GetEntry: Entry "  << index << " not a valid Stage" << std::endl;
		return PndMCEntry();
	}
}

FairMultiLinkedData PndMCObject::PosInList(FairLink link){
	FairMultiLinkedData result;
	result.SetPersistanceCheck(kFALSE);
	for (int i = 0; i < fStage.size(); i++){
		if (fStage[i].IsLinkInList(link.GetType(), link.GetIndex()))
			result.AddLink(FairLink(GetStageId(), i));
	}
	return result;
}

void PndMCObject::RemoveType(Int_t type)
{
	for (int i = 0; i < fStage.size(); i++){
		fStage[i].RemoveType(type);
	}
}
