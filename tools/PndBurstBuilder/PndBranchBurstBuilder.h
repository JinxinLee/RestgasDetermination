/*
 * PndBranchBurstBuilder.h
 *
 *  Created on: July 12, 2017
 *      Author: Steinschaden
 *
 */

 // Basic Class for building Bursts of events in the simulation.
//For the different simulation types (event based, semi time based, time based) the appropriate classes must be derived
// the pure virtual function "GetBurstData" must be implemented

#ifndef PndBranchBurstBuilder_H_
#define PndBranchBurstBuilder_H_



#include "TClonesArray.h"
#include "FairTask.h"
#include <vector>


class PndBranchBurstBuilder : public FairTask
{
public:

  /** Default constructor **/
	PndBranchBurstBuilder();
/** Destructor **/
	virtual ~PndBranchBurstBuilder();


	 /** Virtual method Init **/
    virtual InitStatus Init();

    //virtual void SetParContainers();
   // virtual InitStatus ReInit();




    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);

    virtual TClonesArray* GetBurstData(size_t inBranchNum)=0;

//  virtual void  ExecuteTask(Option_t *option="0");
//  virtual void  ExecuteTasks(Option_t *option);

//    virtual void FinishEvent();
//    virtual void FinishTask();
//    virtual void Finish() { };


// Set OutputBrnachName prefix. Default one is "Burst"
    void SetOutputPrefix(TString prefix){fOutputPrefix=prefix;};
// Set and add the branches for which you want to combine events
    void AddInputBranch(TString branchName){ fInBranchNames.push_back(branchName);};

 // Set Persistance to kTRUE if you want to store the combined Events. otherwise they are just buffered for the direct use of other tasks in the same run
    void SetPersistence(Bool_t val){ fPersistence = val;};



protected:

    std::vector<TString> fInBranchNames;
    std::vector<TClonesArray*> fInArrays;
    std::vector<TClonesArray*> fOutArrays;

    TString fOutputPrefix;
    Bool_t fPersistence;



  ClassDef(PndBranchBurstBuilder,0);


};

#endif /*PndBranchBurstBuilder_H_*/
