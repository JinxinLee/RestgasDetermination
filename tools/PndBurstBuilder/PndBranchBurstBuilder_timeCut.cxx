/*
 * PndEventCombinerTask_tb.cxx
 *
 *  Created on: July 13, 2017
 *      Author: Steinschaden
 *
 */

#include "PndBranchBurstBuilder_timeCut.h"
#include "TClonesArray.h"

// framework includes
#include "FairRootManager.h"
#include "FairTimeStamp.h"



PndBranchBurstBuilder_timeCut::PndBranchBurstBuilder_timeCut() :
PndBranchBurstBuilder()
{
	fOutputPrefix= "Burst_tb";
	fTimePeriod = 2000.0; // in nano seconds

}
PndBranchBurstBuilder_timeCut::~PndBranchBurstBuilder_timeCut()
{
}

// -----   Public method Init   --------------------------------------------
InitStatus PndBranchBurstBuilder_timeCut::Init()
{

	  InitStatus status =  PndBranchBurstBuilder::Init();

  fStopFunctor = new StopTime();

  return status;
}

TClonesArray* PndBranchBurstBuilder_timeCut::GetBurstData(size_t branchNum)
{
	  FairRootManager* ioman = FairRootManager::Instance();

	  // the absorbtion of the Array to the Writeout Array will "clear" the memmory of this Array so that in the next loop only new data are present
	  return ioman->GetData(fInBranchNames[branchNum], fStopFunctor, ioman->GetEntryNr()*fTimePeriod+fTimePeriod);
}



ClassImp(PndBranchBurstBuilder_timeCut);

