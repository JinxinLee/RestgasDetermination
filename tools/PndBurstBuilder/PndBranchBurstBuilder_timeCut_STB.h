/*
 * PndBranchBurstBuilder_timeCut_STB.h
 *
 *  Created on: July 12, 2017
 *      Author: Steinschaden
 *
 */

 //*  Task Class for building Bursts of events in the simulation.
//    This Version contains a workaround for "semi-timebased branches" of the PndBranchBurstBuilder_timeCut
// 	  It checks if the fastest hit of an event occures within a defined time window and combines these events into Bursts
// 	  As the Event based output, the data in the Bursts are not sorted in time!

//    Attention: to Keep this workaround simple not every potential case is taken into account.
//	  If the used time windows are to small so that maybe no event occurse, unwanted splitting of events within the next Burst will happen!

#ifndef PndBranchBurstBuilder_timeCut_STB_H_
#define PndBranchBurstBuilder_timeCut_STB_H_


#include "PndBranchBurstBuilder.h"
#include "TClonesArray.h"
//#include "FairTask.h"
//#include <vector>



class PndBranchBurstBuilder_timeCut_STB : public PndBranchBurstBuilder
{
public:

  /** Default constructor **/
	PndBranchBurstBuilder_timeCut_STB();
/** Destructor **/
	virtual ~PndBranchBurstBuilder_timeCut_STB();


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
    virtual void FinishTask();
//    virtual void Finish() { };

protected:

  double fTimePeriod;
  double fBurstNum;
  bool fWriteOut;
  std::vector<TClonesArray*> fTempArrays;


  ClassDef(PndBranchBurstBuilder_timeCut_STB,0);


};

#endif /*PndBranchBurstBuilder_timeCut_STB_H_*/
