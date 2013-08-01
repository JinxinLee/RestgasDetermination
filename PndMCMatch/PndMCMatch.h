/*
 * PndMCMatch.h
 *
 *  Created on: Nov 23, 2009
 *      Author: stockman
 */

#ifndef PNDMCMATCH_H_
#define PNDMCMATCH_H_


#include "PndDetectorList.h"
#include "PndMCStage.h"
#include "PndMCResult.h"

#include <TNamed.h>

#include <map>
#include <vector>
#include <string>

typedef std::map<Int_t, PndMCStage*>::iterator TListIterator;
typedef std::map<Int_t, PndMCStage*>::const_iterator TListIteratorConst;

class PndMCMatch: public TNamed {
public:
	PndMCMatch();
	PndMCMatch(const char* name, const char* title)
		:TNamed(name, title), fUltimateStage(0), fVerbose(0){
		fFinalStageML.SetPersistanceCheck(kFALSE);
	};
	virtual ~PndMCMatch();

	void AddElement(Int_t type, int index, FairLink link);
	void AddElement(Int_t sourceType, int index, Int_t targetType, int link);
	void SetElements(Int_t sourceType, int index, FairMultiLinkedData* links);
	void InitStage(Int_t type, std::string fileName, std::string branchName);
	void InitStage(std::string fileName, std::string branchName = "");
	void RemoveStage(Int_t type);
	void RemoveStage(TString stage){RemoveStage(FairRootManager::Instance()->GetBranchId(stage));}
	void LoadInMCLists(TClonesArray* myLinkArray);
	void ClearMCList();

	void SetCommonWeightStages(Float_t weight);

	PndMCEntry GetEntry(Int_t type, int index);
	PndMCEntry GetEntry(FairLink link);

	PndMCResult GetMCInfo(Int_t start, Int_t stop);
	PndMCResult GetMCInfo(TString start, TString stop);
	PndMCEntry  GetMCInfoSingle(FairLink aLink, Int_t stop);
	PndMCEntry  GetMCInfoSingle(FairLink aLink, TString stop);

	int GetNMCStages() const {return fList.size();}

	PndMCStage* GetMCStage(int index) const{
		TListIteratorConst iter = fList.begin();
		for (int i = 0; i < index; i++)
			iter++;
		return (iter->second);
	}

	PndMCStage* GetMCStageType(TString branch){
		FairRootManager* ioman = FairRootManager::Instance();
		if (ioman->GetBranchId(branch) > 0)
			return fList[ioman->GetBranchId(branch)];
		else
			return 0;
	}

	PndMCStage* GetMCStageType(Int_t type){
		return fList[type];
	}

	void CreateArtificialStage(TString branchName, std::string fileName = "");
	void CreateArtificialStage(Int_t stage, std::string fileName = "", std::string branchName = "");

	FairMultiLinkedData FindLinksToStage(Int_t stage);

	bool IsTypeInList(Int_t type);

	void Print(std::ostream& out = std::cout){out << *this;}

	friend std::ostream& operator<< (std::ostream& out, const PndMCMatch& match){
		for (int i = 0; i < match.GetNMCStages(); i++){
			if (match.GetMCStage(i)->GetLoaded() == kTRUE){
				match.GetMCStage(i)->Print(out);
				out << std::endl;
			}
		}
		return out;
	}

private:
	Int_t fUltimateStage; ///< last stage in link chain. Here all recursive operations must stop.
	std::map<Int_t, PndMCStage*> fList;
	FairMultiLinkedData fFinalStageML;
	Int_t fVerbose;

	void FindStagesPointingToLinks(FairMultiLinkedData links, Int_t stop);
	FairMultiLinkedData FindStagesPointingToLink(FairLink link);

	PndMCResult GetMCInfoForward(Int_t start, Int_t stop);
	PndMCResult GetMCInfoBackward(Int_t start, Int_t stop);
	PndMCEntry GetMCInfoForwardSingle(FairLink link, Int_t stop);
	PndMCEntry GetMCInfoBackwardSingle(FairLink link, Int_t stop, Double_t weight = 1.);

	void GetNextStage(FairMultiLinkedData& startEntry, Int_t stopStage);
	void AddToFinalStage(FairLink link, Float_t mult);
	void ClearFinalStage();
	ClassDef(PndMCMatch, 1);
};

#endif /* PNDMCMATCH_H_ */
