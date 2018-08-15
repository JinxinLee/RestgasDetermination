/*
 * PndBranchBurstBuilder_eb.h
 *
 *  Created on: July 12, 2017
 *      Author: Steinschaden
 *
 */

 //*  Task Class for building Bursts of events in the simulation.
//    This Version combines a certain number of TTree entries ("Events") coming from an event based simulation/digitization to Burst of Events.
// 	  As the Event based output, the data in the Bursts are not sorted in time!
//    Between the output of the Bursts empty events are written to the output array.

#ifndef PndBranchBurstBuilder_eventBased_H_
#define PndBranchBurstBuilder_eventBased_H_


#include "PndBranchBurstBuilder.h"


class PndBranchBurstBuilder_eventBased : public PndBranchBurstBuilder
{
public:

  /** Default constructor **/
	PndBranchBurstBuilder_eventBased();
/** Destructor **/
	virtual ~PndBranchBurstBuilder_eventBased();


	 /** Virtual method Init **/
    virtual InitStatus Init();

    //virtual void SetParContainers();
   // virtual InitStatus ReInit();

// Method to collect the data of full Burst for an Single Branch. inBranchNum state the position in the InputBranchName vector
    virtual TClonesArray* GetBurstData(size_t branchNum);


    /** Virtual method Exec **/
   // virtual void Exec(Option_t* opt);

//  virtual void  ExecuteTask(Option_t *option="0");
//  virtual void  ExecuteTasks(Option_t *option);

//    virtual void FinishEvent();
    virtual void FinishTask();
//    virtual void Finish() { };

    void SetNEvents(int num){ fNEvents = num;};

protected:

  int fNEvents;
  std::vector<TClonesArray*> fTempArrays;

  ClassDef(PndBranchBurstBuilder_eventBased,0);


};

#endif /*PndBranchBurstBuilder_eventBased_H_*/
