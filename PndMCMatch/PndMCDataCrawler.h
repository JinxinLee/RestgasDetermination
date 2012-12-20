/*
 * PndMCDataCrawler.h
 *
 *  Created on: Jul 5, 2011
 *      Author: stockman
 */

#ifndef PNDMCDATACRAWLER_H_
#define PNDMCDATACRAWLER_H_

#include "FairMultiLinkedData.h"
#include "FairRootManager.h"

#include "TObject.h"

class PndMCDataCrawler : public TObject{
public:
	PndMCDataCrawler();
	virtual ~PndMCDataCrawler();

	FairMultiLinkedData GetInfo(FairMultiLinkedData startLink, TString stopStage);
	FairMultiLinkedData GetInfo(FairMultiLinkedData startLink, Int_t stopStageId);

	FairMultiLinkedData* GetEntry(Int_t fileId, Int_t eventNr, Int_t type, Int_t index);
	FairMultiLinkedData* GetEntry(FairLink link);

	void SetStoreIntermediate(Bool_t val = kTRUE){fStoreIntermediate = val;}
	void SetStoreAllEndpoints(Bool_t val = kTRUE){fStoreAllEndpoints = val;}

	void Init();

	void SetVerbose(Int_t val){fVerbose = val;};
private:
	FairRootManager* fIoman;
	FairMultiLinkedData fFinalStage;
	Int_t fUltimateStage; ///< last stage in link chain. Here all recursive operations must stop.

	Int_t fVerbose;
	Bool_t fStoreAllEndpoints; ///< true if non-stop-stage data is stored in results
	Bool_t fStoreIntermediate; ///< true if all intermediate steps should be stored

	void GetNextStage(FairMultiLinkedData& startEntry, Int_t stopStage);
	void AddToFinalStage(FairLink link, Float_t mult);
	void ClearFinalStage();

	ClassDef(PndMCDataCrawler, 1);
};

#endif /* PNDMCDATACRAWLER_H_ */
