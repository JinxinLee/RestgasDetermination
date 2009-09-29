#ifndef PNDANALYSIS_H
#define PNDANALYSIS_H 1

#include <string>
#include <vector>

#include "TString.h"

#include "TCandList.h"
#include "FairRootManager.h"

class TChain;
class TClonesArray;
class TBranch;

class PndPidListMaker;
class PndEventInfo;


class PndAnalysis
{
public:
	PndAnalysis();
	~PndAnalysis();

	void Rewind();
	void Reset() { Rewind(); }
	int  GetEvent(int n=-1);
	int  GetEntries(); 
	bool FillList(TCandList &l, std::string listkey="All");
  //	const PndEventInfo* GetEventInfo();
//	Float_t GetTag(const char* bname);
//	Int_t   GetTagI(const char* bname);
  
private:
  
	void Init();
	//void SetupBranchNames();
	
	// Private Member Variables
	
  FairRootManager *fRootManager;
	PndPidListMaker	*fPidListMaker;
	int             fEvtCount;
	int             fChainEntries;
	bool            fEventRead;
	
	TClonesArray *fChargedCands;
	TClonesArray *fNeutralCands;
  TClonesArray *fChargedProbability;
  TClonesArray *fNeutralProbability;
	TClonesArray *fMcCands;
//	TClonesArray *fMicroCands;
	
	//TClonesArray *fEventInfo;
	//PndEventInfo *fCurrentEventInfo;
	
	TCandList allCands;
	TCandList chargedCands;
	TCandList neutralCands;  
	TCandList mcCands;
	
  	ClassDef(PndAnalysis,0);
};


#endif

