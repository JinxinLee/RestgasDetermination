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
	PndLumiRecoTask();

	virtual ~PndLumiRecoTask(){;}

	/** Virtual method Init */
	virtual InitStatus Init();

	/** Virtual method Exec */
	virtual void Exec(Option_t* opt);

	void Print() const;

private:
	/** Input array */
	TClonesArray *fLumiClusterArray;

	/** Output array */
	TClonesArray *fLumiRecoArray;

	TVector2 CalcLineCross(Double_t yf, Double_t yb,
            Double_t of, Double_t ob) const;

	ClassDef(PndLumiRecoTask, 1);
};

#endif /* PNDLUMIRECOTASK_H_ */
