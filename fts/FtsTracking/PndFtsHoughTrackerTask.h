// *************************************************************************
// Author: Martin Galuska
// martin (dot) j [dot] galuska <at> physik {dot} uni (minus) giessen <dot> de
//
//
// This task is used as an interface between
// PndFtsHoughTrackFinder
// and PandaRoot
// It was originally modeled after mvd/MvdTracking/PndMvdRiemannTrackFinderTask (among many others)
//
// This task provides functionality for other FTS tracking classes by passing a pointer to itself.
//
// TODO
// Make this task work for time-based simulation as well [have a look at PndMvdRiemannTrackFinderTask::FillHitArray()]
//
// Created: 18.06.2013
//
// *************************************************************************








#ifndef PndFtsHoughTrackerTask_H
#define PndFtsHoughTrackerTask_H

#include "FairTask.h"
#include <cmath>
#include <vector>
#include "Rtypes.h"                     // for Double_t, Int_t, etc
#include <iostream>

#include "TVector3.h"
#include "PndFtsHit.h"
#include "TClonesArray.h"

class PndFtsHoughTrackCand;
class PndFtsHoughSpace;
class TH2;


class PndGeoFtsPar;
class FairField;




class PndFtsHoughTrackerTask : public FairTask
{
public:
	PndFtsHoughTrackerTask(Int_t verbose=0, Bool_t persistence=kTRUE, Bool_t saveDebugInfo=kFALSE);
	~PndFtsHoughTrackerTask();

	/** Load the parameter container from the runtime database **/
	virtual void SetParContainers();
	/** Initiliazation of task at the beginning of a run **/
	virtual InitStatus Init();
	/** ReInitiliazation of task when the runID changes **/
	virtual InitStatus ReInit();


	/** Executed for each event. **/
	virtual void Exec(Option_t* opt);
	virtual void FinishEvent();

	/** Finish task called at the end of the run **/
	virtual void Finish();

	//////////
	// SETTERS
	//////////
	void SetVerbose(Int_t verbose){ fVerbose = verbose;};
	void SetPersistence(Bool_t val){ fPersistence = val;};
	void SetSaveDebugInfo(Bool_t saveDebugInfo){ fSaveDebugInfo = saveDebugInfo;};
	//	void SetTrackOutput(TString name = "FTSTrkHough") { fTracksArrayName = name; };

	//  for writing out histograms for debugging
	void WriteHistogram(PndFtsHoughSpace* houghSpace, Int_t index=-1);

	///////////
	// GETTERS
	//////////
	const Int_t getFtsBranchId() const {  return fFtsBranchId; };
	TClonesArray* getFtsHitArrayPtr() { return fFtsHitArray; }; // Input array of PndFtsHit
	FairField* getMagneticFieldPtr() { return fField; };
	const Int_t GetVerbose() const { return fVerbose; };
	const Bool_t GetSaveDebugInfo() const { return fSaveDebugInfo; };
	const UInt_t GetEventNr() const { return fEventNr; };
	TVector3 GetHitPositionError(UInt_t hitId) const; // get the position error for hit with index hitId based on FTS straw geometry
	const PndFtsHit* GetFtsHit(UInt_t hitId) const {
		if ( hitId >= fFtsHitArray->GetEntriesFast() ) {
			Warning("GetFtsHit","hitId is too large.");
			return 0;
		}
		//		TClonesArray *ftsHitArray= (TClonesArray *)FairRootManager::Instance()->GetObject("FTSHit");
		const PndFtsHit *myHit = (PndFtsHit*) fFtsHitArray->At(hitId);
		return myHit;
	}

private:
	// for PandaRoot input/output
	PndFtsHoughTrackerTask *fTrackerTask;


	//  for writing out histograms for debugging
	//	void InitOutFileForDebugging();
	//	void AddNewEventToOutFileForDebugging(UInt_t eventNr);
	//	TFile* fOutFile;

	// general
	Bool_t fPersistence;



	// Input
	////////

	//! FTS Branch ID
	Int_t   fFtsBranchId; // detector Id
	TClonesArray *fFtsHitArray; // Input array of PndFtsHit

	PndGeoFtsPar *fFtsParameters;
	/** Input array of PndFtsTube (map of FTS tubes) **/
	TClonesArray *fFtsTubeArray;

	// for B field access
	FairField* fField;



	// Output
	/////////
	/** Output array to  new data level**/
	//  TClonesArray* <OutputDataLevel>;

	TString fTracksArrayName;     // Branch name where to store the Track candidates
	TClonesArray  *fTrackCands;   //! Array of found track candidates in PndTrackCand (for output)
	TClonesArray  *fTracks;       //! Array of found tracks in PndTrack (for output)
	TClonesArray *fHoughTrackCands; // Array of found track cands in PndFtsHoughTrackCand (only for debugging)
	TClonesArray *fHoughSpaces;


	// Debug
	////////
	Bool_t fSaveDebugInfo;
	UInt_t fEventNr;





	// TODO: I don't think I need the copy constructor and the operator=
	PndFtsHoughTrackerTask(const PndFtsHoughTrackerTask&);
	PndFtsHoughTrackerTask operator=(const PndFtsHoughTrackerTask&);



	ClassDef(PndFtsHoughTrackerTask,1);
};

#endif
