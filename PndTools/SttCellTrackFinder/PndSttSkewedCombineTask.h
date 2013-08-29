#ifndef PndSttSkewedCombineTask_H_
#define PndSttSkewedCombineTask_H_

#include "FairTask.h"
#include "PndGeoSttPar.h"
#include "PndSttStrawMap.h"

#include "PndSttGeometryMap.h"

#include <vector>

class PndSttSkewedCombineTask: public FairTask {
public:

	PndSttSkewedCombineTask() :FairTask("Stt Skewed Combine"), fVerbose(0), fPersistence(
					kTRUE) {};

	virtual ~PndSttSkewedCombineTask() {};

	/** Virtual method Init **/
	virtual void SetParContainers();
	virtual InitStatus Init();
	//virtual InitStatus ReInit();

	/** Virtual method Exec **/
	virtual void Exec(Option_t* opt);
	virtual void FinishEvent();

	//void AddHitBranch(TString branchName);//Has to be called before Init() is used otherwise the default names are taken!

	void SetVerbose(Int_t verbose) {
		fVerbose = verbose;
	}
	;
	void SetPersistence(Bool_t val) {
		fPersistence = val;
	}
	;

	//  void SetGeoH(PndGeoHandling geoH){ fGeoH=geoH;};


private:

	TClonesArray* fSttHits;

	TClonesArray* fCombinedSkewedHits;

	PndSttStrawMap fStrawMap;	// for getting more information about the tubes
	PndSttGeometryMap* fGeometryMap;// for initializing the neighbors of each tube

	Int_t fVerbose;
	Bool_t fPersistence; // safe data?

	PndGeoSttPar *fSttParameters; 	// needed for SttStrawMap
	TClonesArray *fTubeArray;		// needed for SttStrawMap

	TClonesArray* fEventHeader;
	//PndGeoHandling* fGeoH;

//	void Register();
//	void Reset();
//	void ProduceHits();

ClassDef(PndSttSkewedCombineTask,1)
	;

};

#endif /*PndSttSkewedCombineTask_H_*/
