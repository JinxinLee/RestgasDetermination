#ifndef PndSolCorrTask_H_
#define PndSolCorrTask_H_




// Class to correct TimeStamps according to the flight path of the Particles, assuming speed of light and a straigt track path from the origin. Afterwards they get sorted timewise and written to the Outputfile
//With AddInputBranch(TString branchName) Branches are added to the task, with SetOutBranchName(TString name) the Output Branch name can be defined.

#include "TClonesArray.h"
#include "FairTask.h"
#include "FairTimeStamp.h"
#include "FairHit.h"
#include "FairTSBufferFunctional.h"

#include <vector>
#include <map>

class PndSolCorrTask : public FairTask
{
public:

  /** Default constructor **/  
	PndSolCorrTask(); 
/** Destructor **/
	virtual ~PndSolCorrTask();


	 /** Virtual method Init **/
    //virtual void SetParContainers();
    virtual InitStatus Init();
   // virtual InitStatus ReInit();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);
//    virtual void FinishEvent();
    virtual void FinishTask();
    

    void SetOutBranchName(TString name) {fOutBranchName = name;};
    void AddInputBranch(TString branchName){ fInBranches.push_back(branchName);};
    
    void RunContinuous(Bool_t val = kTRUE){fRunContinuous = val;};
    void SetPersistence(Bool_t val){ fPersistence = val;};
    
    FairTimeStamp* SolCorr(FairHit* inHit);


private:

    TString fOutBranchName;
    std::vector<TString> fInBranches;
    std::vector<TClonesArray*> fInArrays;
    TClonesArray* fOutArray;
 
    std::multimap<double, FairTimeStamp*> fDataCorr_map;
    
    Bool_t fRunContinuous;
    
    Bool_t fPersistence;    
    
    BinaryFunctor* fStopFunctor; 
    double fReadOutStop;    

  
  ClassDef(PndSolCorrTask,0);


};

#endif /*PndSolCorrTask_H_*/
