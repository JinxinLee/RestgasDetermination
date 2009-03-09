/*
 * PndLumiRecoTask.h
 *
 *  Created on: Nov 10, 2008
 *      Author: tsito
 */

#ifndef PNDLUMIRECOTASK_H_
#define PNDLUMIRECOTASK_H_

#include "FairTask.h"
#include "FairRootManager.h"
#include "FairGeoVector.h"
#include "PndLumiCluster.h"
#include "PndLumiRecoHit.h"
#include "PndLumiTransposition.h"

#include "TClonesArray.h"
#include "TString.h"
#include "TMath.h"

#include <string>

class PndLumiRecoTask : public FairTask
{
public:
	/** Constructors */
	PndLumiRecoTask(){;}
	PndLumiRecoTask(Double_t pitch, Double_t sigma,
			Double_t orient);

	virtual ~PndLumiRecoTask(){;}

	/** Virtual method Init */
	virtual InitStatus Init();

	/** Virtual method Exec */
	virtual void Exec(Option_t* opt);

private:
	/** Input array */
	TClonesArray *fLumiClusterArrayFront;
	TClonesArray *fLumiClusterArrayBack;
	/** Output array */
	TClonesArray *fLumiRecoArray;

	Double_t fSigma, fPitch, fOrient;

	Double_t SmearPosition(Double_t p);
	TVector3 StripDirtoLoc(Double_t angle,
				FairGeoVector loc);


	ClassDef(PndLumiRecoTask, 1);


};

#endif /* PNDLUMIRECOTASK_H_ */
