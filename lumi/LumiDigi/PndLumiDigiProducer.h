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

//#include "PndStringVector.h"
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
#include <TVector2.h>

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

	/**Set Pointer to the Digitization Parameters*/
	PndLumiDigiPara* fDigiPar;


    /** Pointer to input array of PndLumiPoints */
	TClonesArray* fLumiPoint;


	/** Pointer to output array of PndLumiDigi */
	TClonesArray* fLumiDigi;

	PndLumiTransposition *fGeoH;

	Bool_t IsActive(TString detname, SensorSide side);



    ClassDef(PndLumiDigiProducer,1);
};
#endif /*PNDLUMIDIGIPRODUCER_H_*/
