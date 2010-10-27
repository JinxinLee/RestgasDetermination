/*
 * PndMvdTimeWalkCorrctionTask.h
 *
 *  Created on: Aug 27, 2010
 *      Author: Simone Esch
 */

#ifndef PNDMVDTIMEWALKCORRTASK_H_
#define PNDMVDTIMEWALKCORRTASK_H_

#include "PndSdsTimeWalkCorrTask.h"
#include "TString.h"
#include "FairRuntimeDb.h"
#include "FairRun.h"

class PndMvdTimeWalkCorrTask: public PndSdsTimeWalkCorrTask {
public:
	PndMvdTimeWalkCorrTask();
	virtual ~PndMvdTimeWalkCorrTask();

	void SetBranchNames();

	void SetParContainers();

        ClassDef(PndMvdTimeWalkCorrTask, 1);
};

#endif /* PNDMVDTIMEWALKCORRTASK_H_ */
