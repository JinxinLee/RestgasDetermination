#ifndef PndSttCellTrackFinderTask_H_
#define PndSttCellTrackFinderTask_H_

#include "FairTask.h"
#include "PndGeoSttPar.h"
#include "PndSttGeometryMap.h"
#include "PndSttStrawMap.h"
#include "PndSttCellTrackFinder.h"

#include <vector>

extern "C" int* AllocateStaticData(int*, int);
extern "C" void FreeStaticData(int*);

class PndSttCellTrackFinderTask: public FairTask {
public:

	PndSttCellTrackFinderTask() :
			FairTask("Stt Cell Track Finder"), fUseGPU(kFALSE), fDev_tubeNeighborings(0), fCalcWithCorrectedIsochrones(kFALSE), fVerbose(0), fPersistence(kTRUE), fAnalyseSteps(
					kFALSE) , fTrackFinder(0){
		fOutBranchNamePrefix = "";
		fInBranchNamePrefix = "";
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
	void SetUseGPU(Bool_t val){
		fUseGPU=val;
	};

	void SetCalcWithCorrectedIsochrones(Bool_t val){
		fCalcWithCorrectedIsochrones=val;
	}
	// define a Prefix for the Output in case you run more Instances of the CellTrackFinderTask
	void SetOutBranchNamePrefix(TString prefix){fOutBranchNamePrefix = prefix+"_";};
	// set a prefix for the defoult inbut Branch names
	void SetInBranchNamePrefix(TString prefix){fInBranchNamePrefix = prefix+"_";};
	//if nothing is defined, the standard STTHit branches are used.
	//If InputBranches are defined, so far only Branches holding data of type STTHit are further processed (See "initHitArray" method)
	void AddHitBranch(TString branchName){fHitBranch.push_back(branchName);};
	// initialise all STTHit type data,  If someone wants to change this also the "Addhit" method of the Trackfinder must be adapted!
	void InitHitArray(TString branchName);

private:

	//for GPU computing
	Bool_t fUseGPU;
	int* fDev_tubeNeighborings;

	Bool_t fCalcWithCorrectedIsochrones;

	TString fOutBranchNamePrefix;
	TString fInBranchNamePrefix;

	std::vector<TString> fHitBranch;
	std::vector<TString> fSTTHitBranch;
	//TString fTrackBranch;

	std::vector<TClonesArray*> fSTTHitArray;

	// first step of trackfinding
	TClonesArray* fFirstTrackCandArray;
	TClonesArray* fFirstTrackArray;
	TClonesArray* fFirstRiemannTrackArray;

	// second step of trackfinding
	TClonesArray* fCombiTrackCandArray;
	TClonesArray* fCombiRiemannTrackArray;
	TClonesArray* fCombiTrackArray;
	TClonesArray* fCorrectedIsochronesArray;

	Int_t fVerbose;
	Bool_t fPersistence; // safe data?
	Bool_t fAnalyseSteps; // if true, results of first step (generating tracklets) will be stored for AnalysisTask

	PndGeoSttPar *fSttParameters; 	// needed for SttStrawMap
	TClonesArray *fTubeArray;		// needed for SttStrawMap

	PndSttGeometryMap *fSttGeometryMap;
	PndSttStrawMap *fSttStrawMap;
	PndSttCellTrackFinder* fTrackFinder;

	TClonesArray* fEventHeader;

	//for runtime analysis: numHits, numHitsWithoutDouble, numUnambiguousHits
	std::vector<std::vector<int> >  fNumHitsPerEvent;

ClassDef(PndSttCellTrackFinderTask,1)
	;

};

#endif /*PndSttCellTrackFinderTask_H_*/
