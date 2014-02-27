// *************************************************************************
// Author: Martin Galuska
// martin (dot) j [dot] galuska <at> physik {dot} uni (minus) giessen <dot> de
//
//
// TODO
// Next work on PndFtsHoughTrackCand
// After work on PndFtsHoughTrackFinder
//
// This task is used as an interface between
// PndFtsHoughTrackFinder
// and PandaRoot
// It was modeled after mvd/PndMvdRiemannTrackFinderTask (among many others)
//
// TODO
// Make this task work for time-based simulation as well [have a look at PndMvdRiemannTrackFinderTask::FillHitArray()]
//
// Created: 18.06.2013
// Modified: 12.02.2014
//
// *************************************************************************








#ifndef PndFtsTrackerTaskHough_H
#define PndFtsTrackerTaskHough_H

#include "FairTask.h"
#include <cmath>
#include <vector>


class PndFtsHoughTrackCand;

class TClonesArray;
class PndGeoFtsPar;
class FairField;




class PndFtsTrackerTaskHough : public FairTask
{
public:
	PndFtsTrackerTaskHough();
	~PndFtsTrackerTaskHough();

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

	void SetVerbose(Int_t verbose){ fVerbose = verbose;};
	void SetPersistence(Bool_t val){ fPersistence = val;};



	void SetSaveDebugInfo(Bool_t saveDebugInfo){ fSaveDebugInfo = saveDebugInfo;};
	void SetTrackOutput(TString name = "FTSTrkHough") { fTracksArrayName = name; };



private:
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


	// Debug
	////////
	Bool_t fSaveDebugInfo;
	int fEventNr;





	// helper method (handy for multiple constructors)
	void Initialization_ClassVariables();

	// TODO: I don't think I need the copy constructor and the operator=
	PndFtsTrackerTaskHough(const PndFtsTrackerTaskHough&);
	PndFtsTrackerTaskHough operator=(const PndFtsTrackerTaskHough&);



	ClassDef(PndFtsTrackerTaskHough,1);
};

#endif
