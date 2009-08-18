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
			Double_t orient_f, Double_t orient_b, Double_t W,
			Double_t L,	Double_t thr, Double_t noise, Double_t sigma);//		Double_t side);

	~PndLumiClusterBuilder();

	virtual InitStatus Init();

	virtual void Exec(Option_t* opt);

	std::map<Int_t,PndLumiStrip> GetClusters(std::vector<PndLumiStrip> strip);

	std::map<Int_t,Double_t> GetLeftAndRight(std::map<Int_t,PndLumiStrip> clust);

	Double_t GetClusterPosition(Double_t dir, TVector2 zero, Double_t eLoss,
			FairGeoVector posInL, FairGeoVector posOutL);
	Double_t GetStripZeroId(Double_t dir, Double_t x0, Double_t y0);

private:
	//Input array
	TClonesArray* fLumiPoint;
	//Ouput array
	TClonesArray* fLumiCluster;

	Double_t fPitch, fOrient_front, fOrient_back, fWidth,
	fLength, fThreshold, fNoise, fSigma;

	Int_t fVerboseLevel;

	ClassDef(PndLumiClusterBuilder,1);


};

#endif /* PNDLUMICLUSTERBUILDER_H_ */
