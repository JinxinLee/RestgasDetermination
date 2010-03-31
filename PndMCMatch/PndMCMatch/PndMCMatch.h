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

typedef std::map<fDetectorType, PndMCStage*>::iterator TListIterator;
typedef std::map<fDetectorType, PndMCStage*>::const_iterator TListIteratorConst;

class PndMCMatch: public TNamed {
public:
	PndMCMatch();
	PndMCMatch(const char* name, const char* title)
		:TNamed(name, title), fUltimateStage(kMCTrack){};
	virtual ~PndMCMatch();

	void AddElement(fDetectorType type, int index, FairLink link);
	void AddElement(fDetectorType sourceType, int index, fDetectorType targetType, int link);
	void SetElements(fDetectorType sourceType, int index, FairLinkedData* links);
	void InitStage(fDetectorType type, std::string fileName, std::string branchName);
	void RemoveStage(fDetectorType type);
	void LoadInMCLists(TClonesArray* myLinkArray);
	void ClearMCList();

	void SetCommonWeightStages(Float_t weight);

	PndMCEntry GetEntry(fDetectorType type, int index);
	PndMCEntry GetEntry(FairLink link);

	PndMCResult GetMCInfo(fDetectorType start, fDetectorType stop);
	int GetNMCStages() const {return fList.size();}

	PndMCStage* GetMCStage(int index) const{
		TListIteratorConst iter = fList.begin();
		for (int i = 0; i < index; i++)
			iter++;
		return (iter->second);
	}

	PndMCStage* GetMCStageType(fDetectorType type){
		return fList[type];
	}

	void CreateArtificialStage(fDetectorType stage, std::string fileName = "", std::string branchName = "");

	FairMultiLinkedData FindLinksToStage(fDetectorType stage);

	bool IsTypeInList(fDetectorType type);

	void Print(std::ostream& out = std::cout){out << *this;}

	friend std::ostream& operator<< (std::ostream& out, const PndMCMatch& match){
		for (int i = 0; i < match.GetNMCStages(); i++){
			match.GetMCStage(i)->Print(out);
			out << std::endl;
		}
		return out;
	}

private:
	fDetectorType fUltimateStage; ///< last stage in link chain. Here all recursive operations must stop.
	std::map<fDetectorType, PndMCStage*> fList;
	FairMultiLinkedData fFinalStageML;

	void FindStagesPointingToLinks(FairMultiLinkedData links, fDetectorType stop);
	FairMultiLinkedData FindStagesPointingToLink(FairLink link);

	PndMCResult GetMCInfoForward(fDetectorType start, fDetectorType stop);
	PndMCResult GetMCInfoBackward(fDetectorType start, fDetectorType stop);
	void GetNextStage(PndMCEntry& startEntry, fDetectorType stopStage);
	void AddToFinalStage(FairLink link, Float_t mult);
	void ClearFinalStage();
	ClassDef(PndMCMatch, 1);
};

#endif /* PNDMCMATCH_H_ */
