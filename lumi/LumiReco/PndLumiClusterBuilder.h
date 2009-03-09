/*
 * PndLumiClusterBuilder.h
 *
 *  Created on: Nov 18, 2008
 *      Author: tsito
 */

#ifndef PNDLUMICLUSTERBUILDER_H_
#define PNDLUMICLUSTERBUILDER_H_

#include "FairTask.h"
#include "FairRootManager.h"

#include "TClonesArray.h"


#include "PndLumiPoint.h"
#include "PndLumiCalcStripDigi.h"
#include "PndLumiTransposition.h"
#include "PndLumiCluster.h"

class PndLumiClusterBuilder : public FairTask
{
public:
	PndLumiClusterBuilder();
	PndLumiClusterBuilder(Double_t pitch,
			Double_t orient, Double_t W, Double_t L,
			Double_t thr, Double_t noise, Double_t sigma);//		Double_t side);
	~PndLumiClusterBuilder();
	virtual InitStatus Init();
	virtual void Exec(Option_t* opt);

private:
	//Input array
	TClonesArray* fLumiPoint;
	//Ouput array
	TClonesArray* fLumiClusterFront;
	TClonesArray* fLumiClusterBack;

	Double_t fPitch, fOrient, fWidth,
	fLength, fThreshold, fNoise, fSigma, fSide;

	Int_t fVerboseLevel;

	ClassDef(PndLumiClusterBuilder,1);


};

#endif /* PNDLUMICLUSTERBUILDER_H_ */
