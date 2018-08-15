#ifndef PNDMVDEVENTMERGER_H
#define PNDMVDEVENTMERGER_H

#include "TString.h"
#include "TObject.h"
#include "TObjArray.h"
#include "TClonesArray.h"

#include <vector>

class PndMvdEventMerger
{
public:
	PndMvdEventMerger();
	PndMvdEventMerger(TString fileName, TString branchName, Int_t nEvents, Int_t nMerged);
	virtual ~PndMvdEventMerger();
	
	TClonesArray* GetEvent(Int_t iEvent);
	Int_t AddTClonesArray(TClonesArray* target, TClonesArray* source);
		
	
private:
	std::vector<TClonesArray*> fBuffer;
	Int_t FillBuffer(TString fileName, TString branchName, Int_t nEvents, Int_t nMerged);
	
	ClassDef(PndMvdEventMerger,1);
	
};

#endif /*PNDMVDEVENTMERGER_H*/
