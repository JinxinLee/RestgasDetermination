/*
 * PndLumiDigiProducer.h
 *
 *      Author: tsito
 *
 */

#ifndef PNDLUMIDIGIPRODUCER_H_
#define PNDLUMIDIGIPRODUCER_H_

#include "FairTask.h"
#include "FairRootManager.h"
#include "FairParamList.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairGeoNode.h"
#include "FairGeoVector.h"

#include "TGeoManager.h"

#include "PndLumiPoint.h"
#include "PndLumiDigi.h"
#include "PndLumiStrip.h"
#include "PndLumiCalcStripDigi.h"
#include "PndLumiTransposition.h"
#include "PndLumiDigiPara.h"

#include "TMath.h"
#include "TClonesArray.h"
#include "TVector3.h"
#include <cmath>
#include <vector>

using std::cout;
using std::endl;

class PndLumiDigiProducer : public FairTask
{
public:

	/** Constructors */
	PndLumiDigiProducer();

	PndLumiDigiProducer(Int_t);

	/** Destructor */
	~PndLumiDigiProducer();

	/** Load paramaters for digitization */
	virtual void SetParContainers();


	virtual InitStatus Init();

	virtual InitStatus ReInit();

	virtual void Exec(Option_t* opt);

	void Print() const;

private:

	Int_t fVerbose;
	Bool_t fDiffusion;

	/**Set Pointer to the Digitization Parameters*/
	PndLumiDigiPara* fDigiPar;


    /** Pointer to input array of PndLumiPoints */
	TClonesArray* fLumiPoint;


	/** Pointer to output array of PndLumiDigi */
	TClonesArray* fLumiDigi;

	PndLumiTransposition *fGeoH;
	Int_t getDetId(TString detname);
	ClassDef(PndLumiDigiProducer,2);
};
#endif /*PNDLUMIDIGIPRODUCER_H_*/
