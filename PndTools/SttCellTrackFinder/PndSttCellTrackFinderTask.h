#ifndef PndSttCellTrackFinderTask_H_
#define PndSttCellTrackFinderTask_H_

#include "FairTask.h"
#include "PndGeoSttPar.h"
#include "PndSttGeometryMap.h"
#include "PndSttStrawMap.h"
#include "PndSttCellTrackFinder.h"

#include <vector>

class PndSttCellTrackFinderTask: public FairTask {
public:

	PndSttCellTrackFinderTask() :
			FairTask("Stt Cell Track Finder"), fVerbose(0), fPersistence(kTRUE), fAnalyseSteps(
					kFALSE) {
	}
	;

	virtual ~PndSttCellTrackFinderTask() {
	}
	;

	/** Virtual method Init **/
	virtual void SetParContainers();
	virtual InitStatus Init();
	//virtual InitStatus ReInit();

	/** Virtual method Exec **/
	virtual void Exec(Option_t* opt);

	virtual void FinishEvent();

	virtual void FinishTask();

	//void AddHitBranch(TString branchName);//Has to be called before Init() is used otherwise the default names are taken!

	void SetVerbose(Int_t verbose) {
		fVerbose = verbose;
	}
	;
	void SetPersistence(Bool_t val) {
		fPersistence = val;
	}
	;
	void SetAnalyseSteps(Bool_t val) {
		fAnalyseSteps = val;
	}
	;
	//  void SetGeoH(PndGeoHandling geoH){ fGeoH=geoH;};

	//void FillHitArray();
	void InitHitArray(TString branchName);

private:
	std::vector<TString> fHitBranch;
	//TString fTrackBranch;

	std::vector<TClonesArray*> fHitArray;

	// first step of trackfinding
	TClonesArray* fFirstTrackCandArray;
	TClonesArray* fFirstRiemannTrackArray;

	// second step of trackfinding
	TClonesArray* fCombiTrackCandArray;
	TClonesArray* fCombiRiemannTrackArray;
	TClonesArray* fCombiTrackArray;
	TClonesArray* fCorrectedIsochronesArray;
	TClonesArray* fCorrectedCombiTrackCandArray;
	TClonesArray* fCorrectedCombiRiemannTrackArray;
	TClonesArray* fCorrectedCombiTrackArray;

	Int_t fVerbose;
	Bool_t fPersistence; // safe data?
	Bool_t fAnalyseSteps; // if true, results of first step (generating tracklets) will be stored for AnalysisTask

	PndGeoSttPar *fSttParameters; 	// needed for SttStrawMap
	TClonesArray *fTubeArray;		// needed for SttStrawMap

	PndSttGeometryMap *fSttGeometryMap;
	PndSttStrawMap *fSttStrawMap;
	PndSttCellTrackFinder fTrackFinder;

	TClonesArray* fEventHeader;

	//for runtime analysis: numHits, numHitsWithoutDouble, numUnambiguousHits
	std::vector<std::vector<int> >  fNumHitsPerEvent;

ClassDef(PndSttCellTrackFinderTask,1)
	;

};

#endif /*PndSttCellTrackFinderTask_H_*/
