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
#include "PndSdsTotDigiPar.h"
#include "PndSdsTimeWalkCorrSimple.h"

class PndSdsTimeWalkCorrTask : public PndSdsTask {
public:
	PndSdsTimeWalkCorrTask();
	PndSdsTimeWalkCorrTask(const char* name);
	virtual ~PndSdsTimeWalkCorrTask();

	InitStatus Init();
	void SetParContainers();
	void Exec(Option_t* opt);
protected:
	Bool_t fPersistance;
	TClonesArray* fDigiArray;
	TClonesArray* fDigiCorrArray;
    PndSdsPixelDigiPar* fDigiPar;
    PndSdsTotDigiPar* fDigiTotPar;
    PndSdsTimeWalkCorr* fTimeWalkCorr;
    Double_t fTimeCorrection;
    Int_t fVerbose;


    ClassDef(PndSdsTimeWalkCorrTask, 1);
};

#endif /* PNDSDSTIMEWALKCORRTASK_H_ */
