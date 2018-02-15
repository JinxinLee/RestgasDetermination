/*
 * PndBranchBurstBuilder_timeCut.h
 *
 *  Created on: July 12, 2017
 *      Author: Steinschaden
 *
 */

 //*  Task Class for building Bursts of events in the time based simulation.
//  This simple Burst builder combines the data of the continuos data stream within a certain time window into on Burst.
//    Therefore it can only work on data sorted in time and is ignoring any kind of eventstructure from the simulation stage.
//    and the Input braches containing the Hits from the digitization are not useable after the burst builder Task anymore!
//    The Output Bursts are also sorted in time.

#ifndef PndBranchBurstBuilder_timeCut_H_
#define PndBranchBurstBuilder_timeCut_H_


#include "PndBranchBurstBuilder.h"
#include "FairTSBufferFunctional.h"




class PndBranchBurstBuilder_timeCut : public PndBranchBurstBuilder
{
public:

  /** Default constructor **/
	PndBranchBurstBuilder_timeCut();
/** Destructor **/
	virtual ~PndBranchBurstBuilder_timeCut();


	 /** Virtual method Init **/
    virtual InitStatus Init();

    //virtual void SetParContainers();
   // virtual InitStatus ReInit();

// Method to collect the data of full Burst for an Single Branch. inBranchNum state the position in the InputBranchName vector
    virtual TClonesArray* GetBurstData(size_t branchNum);

void SetTimePeriod (double val){fTimePeriod = val;};
    /** Virtual method Exec **/
   // virtual void Exec(Option_t* opt);

//  virtual void  ExecuteTask(Option_t *option="0");
//  virtual void  ExecuteTasks(Option_t *option);

//    virtual void FinishEvent();
//    virtual void FinishTask();
//    virtual void Finish() { };

protected:

    double fTimePeriod;
	BinaryFunctor* fStopFunctor;

  ClassDef(PndBranchBurstBuilder_timeCut,0);


};

#endif /*PndBranchBurstBuilder_timeCut_H_*/
