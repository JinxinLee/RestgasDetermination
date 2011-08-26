/*
 * PndSdsPixelWimeWalkCorrTask.h
 *
 *  Created on: Aug 27, 2010
 *      Author: Simone Esch
 */

#ifndef PNDSDSTIMEWALKCORRTASK_H_
#define PNDSDSTIMEWALKCORRTASK_H_

#include "PndSdsTask.h"
#include "TClonesArray.h"
#include "PndSdsPixelDigiPar.h"
#include <vector>
#include "PndSdsDigiPixel.h"
#include "PndSdsDigiPixelMCInfo.h"
#include "PndSdsTotDigiPar.h"
#include "PndSdsTimeWalkCorrSimple.h"
#include "PndSdsFE.h"


class PndSdsTimeWalkCorrTask : public PndSdsTask {
public:
	PndSdsTimeWalkCorrTask();
	PndSdsTimeWalkCorrTask(const char* name);
	virtual ~PndSdsTimeWalkCorrTask();

	InitStatus Init();
	void SetParContainers();
	void Exec(Option_t* opt);
	  virtual void FinishTask();
protected:
	Bool_t fPersistance;
	TClonesArray* fDigiArray;
	TClonesArray* fDigiCorrArray;
    PndSdsPixelDigiPar* fDigiPar;
    PndSdsTotDigiPar* fDigiTotPar;
    PndSdsFE* fFEModel;
    Int_t fVerbose;

    ClassDef(PndSdsTimeWalkCorrTask, 1);
};

#endif /* PNDSDSTIMEWALKCORRTASK_H_ */
