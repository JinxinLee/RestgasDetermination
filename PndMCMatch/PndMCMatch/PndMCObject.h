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
	PndMCObject(fDetectorType type):fStageId(type){};
	PndMCObject(const PndMCObject& obj):
		fStageId(obj.GetStageId()),fStage(obj.GetEntryVector())
	{}
	PndMCObject(fDetectorType type, std::vector<PndMCEntry> stage):
		fStageId(type)
	{
		SetStage(stage);
	};
	virtual ~PndMCObject();

	void SetStageId(fDetectorType type){fStageId = type;}

	void SetEntry(std::vector<Int_t> type, std::vector<Int_t> link, int index);
	void SetEntry(FairLinkedData* data, int index);
	void SetEntry(PndMCEntry link);

	void SetStage(std::vector<PndMCEntry> stage);
	void SetLink(FairLink link, int index);
	void AddLink(FairLink link, int index);

	PndMCEntry GetEntry(int index) const {return fStage[index];}
	FairLink	GetSingleLink(int entryIndex, int linkIndex) const {
		return fStage.at(entryIndex).GetLink(linkIndex);
	}

	fDetectorType GetStageId(void) const {return fStageId;}

	PndMCEntry GetMCLink(Int_t index){return fStage.at(index);}

	int GetNEntries() const {return fStage.size();}
	int GetNLinks(int entryIndex)const {return fStage.at(entryIndex).GetNLinks();}
	std::vector<PndMCEntry> GetEntryVector() const{return fStage;}

	FairMultiLinkedData PosInList(std::pair<int, int> link);
	FairMultiLinkedData PosInList(FairLink link);

	FairMultiLinkedData GetLinksWithType(fDetectorType type){
		FairMultiLinkedData result;
		for (int i = 0; i < GetNEntries(); i++){
			result.AddLinks(GetMCLink(i).GetLinksWithType(type), false);
		}
		return result;
	}

	void ClearEntries(){fStage.clear();}

	virtual void Print(std::ostream& out = std::cout){out << *this;}

	void operator=(const PndMCObject& obj){
		fStageId = obj.GetStageId();
		fStage = obj.GetEntryVector();
	}

	friend std::ostream& operator<< (std::ostream& out, const PndMCObject& obj){
		std::vector<PndMCEntry> stages = obj.GetEntryVector();
		for (int i = 0; i < stages.size(); i++){
			out << i << ": ";
			stages[i].Print(out);
		}
		return out;
	}

private:
	void AdoptSize(int index);
	std::vector<PndMCEntry> fStage;
	fDetectorType fStageId;



	ClassDef(PndMCObject, 0);
};

#endif /* PNDMCOBJECT_H_ */
