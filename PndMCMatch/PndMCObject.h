/*
 * PndMCStage.h
 *
 *  Created on: Dec 1, 2009
 *      Author: stockman
 */

#ifndef PNDMCOBJECT_H_
#define PNDMCOBJECT_H_

#include "PndDetectorList.h"
#include "PndMCEntry.h"

#include <TObject.h>
#include <vector>
#include <string>

#include<iostream>

class PndMCObject: public TObject {
public:
	PndMCObject();
	PndMCObject(Int_t type):fStageId(type){};
	PndMCObject(const PndMCObject& obj):
		fStageId(obj.GetStageId()),fStage(obj.GetEntryVector())
	{}
	PndMCObject(Int_t type, std::vector<PndMCEntry> stage):
		fStageId(type)
	{
		SetStage(stage);
	};
	virtual ~PndMCObject();

	void SetStageId(Int_t type){fStageId = type;}

	void SetEntry(std::vector<Int_t> type, std::vector<Int_t> link, int index);
	void SetEntry(FairMultiLinkedData* data, int index);
	void SetEntry(PndMCEntry link);

	void SetStage(std::vector<PndMCEntry> stage){fStage = stage;}
	void SetLink(FairLink link, int index);
	void AddLink(FairLink link, int index);

	PndMCEntry GetEntry(int index) const;
	FairLink	GetSingleLink(int entryIndex, int linkIndex) const {
		return fStage.at(entryIndex).GetLink(linkIndex);
	}

	Int_t GetStageId(void) const {return fStageId;}

	PndMCEntry GetMCLink(Int_t index){return fStage.at(index);}

	int GetNEntries() const {return fStage.size();}
	int GetNLinks(int entryIndex)const {return fStage.at(entryIndex).GetNLinks();}
	std::vector<PndMCEntry> GetEntryVector() const{return fStage;}

	FairMultiLinkedData PosInList(std::pair<int, int> link);
	FairMultiLinkedData PosInList(FairLink link);

	FairMultiLinkedData GetLinksWithType(Int_t type){
		FairMultiLinkedData result;
		for (int i = 0; i < GetNEntries(); i++){
			result.AddLinks(GetMCLink(i).GetLinksWithType(type));
		}
		return result;
	}

	void RemoveType(Int_t type);

	virtual void ClearEntries(){fStage.clear();}

	virtual void Print(std::ostream& out = std::cout){out << *this;}

	void operator=(const PndMCObject& obj){
		fStageId = obj.GetStageId();
		fStage = obj.GetEntryVector();
	}

	friend std::ostream& operator<< (std::ostream& out, const PndMCObject& obj){
		std::vector<PndMCEntry> stages = obj.GetEntryVector();
		for (int i = 0; i < stages.size(); i++){
			if (stages[i].GetNLinks() > 0){
				out << i << ": ";
				stages[i].Print(out);
				out << std::endl;
			}
		}
		return out;
	}

private:
	void AdoptSize(int index);
	std::vector<PndMCEntry> fStage;
	Int_t fStageId;



	ClassDef(PndMCObject, 0);
};

#endif /* PNDMCOBJECT_H_ */
