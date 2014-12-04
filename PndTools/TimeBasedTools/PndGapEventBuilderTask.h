#ifndef PndGapEventBuilderTask_H_
#define PndGapEventBuilderTask_H_

#include "FairTask.h"
//#include "PndSdsHit.h"

#include "FairTSBufferFunctional.h"

#include <vector>

class PndGapEventBuilderTask : public FairTask
{
public:
	PndGapEventBuilderTask();
	virtual ~PndGapEventBuilderTask();

	 /** Virtual method Init **/
    virtual void SetParContainers();
    virtual InitStatus Init();
    virtual InitStatus ReInit();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);
    virtual void FinishEvent();

    void SetTimeGap(double val)	{fTimeGap = val;}
    void SetBranchName(TString name) {fMainBranchName = name;};

    void AddAdditionalBranches(TString branchName, Double_t timeWindow){
    	std::pair<TString, Double_t> val(branchName, timeWindow);
    	fAddBranches.push_back(val);
    }

    void SetPersistence(Bool_t val){ fPersistence = val;};


private:

  int fEventNr;

  double fTimeGap;
  TString fMainBranchName;
  TClonesArray* fMainHitArray;
  TClonesArray* fMainEventHitArray;

  std::vector<std::pair<TString, Double_t> > fAddBranches;
  std::vector<TClonesArray*> fAddHitArray;
  std::vector<TClonesArray*> fAddEventHitArray;

	Bool_t fInitDone;
	Bool_t fPersistence;

	BinaryFunctor* fStartFunctor;
	BinaryFunctor* fStopFunctor;
	BinaryFunctor* fTimeGapFunctor;


  void Register();
  void Reset();
  void ProduceHits();

  ClassDef(PndGapEventBuilderTask,1);


};

#endif /*PndGapEventBuilderTask_H_*/
