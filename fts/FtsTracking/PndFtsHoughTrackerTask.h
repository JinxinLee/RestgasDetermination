/**
 @class PndFtsHoughTrackerTask

 @author Martin J. Galuska <martin [dot] j [dot] galuska (at) physik [dot] uni (minus) giessen [dot] de>

 @brief Interface between PandaRoot (data input and output) and PndFtsHoughTrackFinder (implementation of PR algorithm).

 This task provides functionality / data for the FTS PR classes by passing a pointer to itself as an argument in the object's constructor.

 This class was originally modeled after
 mvd/MvdTracking/PndMvdRiemannTrackFinderTask
 (among many others)

 TODO
 Make this task work for time-based simulation as well [have a look at PndMvdRiemannTrackFinderTask::FillHitArray()]

 Created: 18.06.2013
 */





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
class PndGeoFtsPar;
class FairField;




class PndFtsHoughTrackerTask : public FairTask
{
public:
	/** @brief Constructor with flags. Can also be used as standard constructor.
	 * @param[in] verbose Verbosity level: 0 least, higher -> more output.
	 * @param[in] persistence kFALSE does not write track candidates from PR to output root file.
	 * @param[in] saveDebugInfo kTRUE will write internal representation of track(let) candidates to output root file.
	 */
	PndFtsHoughTrackerTask(Int_t verbose=0, Bool_t persistence=kTRUE, Bool_t saveDebugInfo=kFALSE);
	/** @brief Destructor.	 */
	~PndFtsHoughTrackerTask();

	/** @brief Loads the parameter container from the runtime database. */
	virtual void SetParContainers();
	/** @brief Initialization of task at the beginning of a run. */
	virtual InitStatus Init();
	/** @brief ReInitiliazation of task when the runID changes. */
	virtual InitStatus ReInit();


	/** @brief Executed for each event.
	 * @param opt Not used.
	 */
	virtual void Exec(Option_t* opt);
	/** @brief When is this executed? After each event? */
	virtual void FinishEvent();

	/** @brief Writes output to root file, I guess. Called at the end of the run. */
	virtual void Finish();

	//----------
	// SETTERS
	//----------
	void SetVerbose(Int_t verbose){ fVerbose = verbose;};
	void SetPersistence(Bool_t val){ fPersistence = val;};
	void SetSaveDebugInfo(Bool_t saveDebugInfo){ fSaveDebugInfo = saveDebugInfo;};
	//	void SetTrackOutput(TString name = "FTSTrkHough") { fTracksArrayName = name; };

	//----------
	// GETTERS
	//----------
	const Int_t GetVerbose() const { return fVerbose; }; ///< @brief Returns the verbosity level.
	const Bool_t GetSaveDebugInfo() const { return fSaveDebugInfo; }; ///< @brief Returns the save debug flag.
	const UInt_t GetEventNr() const { return fEventNr; }; ///< @brief Returns the event number.

	//-----------
	//DATA ACCESS
	//-----------
	/**@brief Returns the number of FTS hits.
	 * @return Number of FTS hits.
	 */
	const Int_t GetNFtsHits() const { return fFtsHitArray->GetEntriesFast(); };
	/** @brief Returns pointer to the hit with index hitId in the FTS hit array.
	 * @param[in] hitId Index (in FTS hit array) of the hit which should be returned.
	 * @return Pointer to hit with index hitId in FTS hit array.
	 */
	const PndFtsHit *const GetFtsHit(UInt_t hitId) const {
		if ( hitId >= GetNFtsHits() ) {
			Warning("GetFtsHit","hitId is too large.");
			return 0;
		}
		//		TClonesArray *ftsHitArray= (TClonesArray *)FairRootManager::Instance()->GetObject("FTSHit");
		const PndFtsHit *myHit = (PndFtsHit*) fFtsHitArray->At(hitId);
		return myHit;
	};
	/** @brief Returns the position error (based on FTS straw geometry) for the hit with index hitId in the FTS hit array.
	 * @param[in] hitId Index (in fFtsHitArray) of the hit for which the error should be returned.
	 * @return Error in cm.
	 */
	const TVector3 GetHitPositionError(UInt_t hitId) const;
	FairField *const getMagneticFieldPtr() const { return fField; }; ///< @brief Returns pointer to the B field.
	/**@brief Returns detector Id of FTS. Try not to use it.
	 *
	 * @see GetFtsHit
	 * @see GetNHits
	 * @return Detector Id of FTS.
	 */
	const Int_t getFtsBranchId() const {  return fFtsBranchId; };
	/**@brief Returns pointer to the hit array in which FTS hits are saved as PndFtsHit. Try not to use it.
	 *
	 * @see GetFtsHit
	 * @see GetNHits
	 * @return Pointer to the hit array in which hits are saved as PndFtsHit.
	 */
	TClonesArray *const getFtsHitArrayPtr() const { return fFtsHitArray; };

	//------
	//DEBUG
	//-----
	/** @brief For writing out Hough spaces as histograms (for debugging purposes).
	 * @param[in] houghSpace To be written as histogram.
	 * @param[in] index Use this parameter in case the same kind of Hough space needs to be written out multiple times per event.
	 */
	void WriteHistogram(const PndFtsHoughSpace *const houghSpace, Int_t index=-1) const;
	FairLogger* fLogger; ///< @brief For output handling.


private:
	//  for writing out histograms for debugging
	//	void InitOutFileForDebugging();
	//	void AddNewEventToOutFileForDebugging(UInt_t eventNr);
	//	TFile* fOutFile;

	//--------
	// General
	//--------
	Bool_t fPersistence;

	//--------
	// Input
	//--------
	Int_t   fFtsBranchId; ///< @brief Detector Id of FTS.
	TClonesArray *fFtsHitArray; ///< @brief Input array of PndFtsHit

	/** @brief Needed for FTS map creator.
	 *
	 * I don't really know what that does...
	 */
	PndGeoFtsPar *fFtsParameters;
	/** @brief Input array of PndFtsTube (map of FTS tubes).
	 *
	 * Is filled by map creator
	 */
	TClonesArray *fFtsTubeArray;

	/// @brief For B field access.
	FairField* fField;


	//--------
	// Output
	//--------
	TString fTracksArrayName;     ///< @brief Branch name where to store the Track candidates
	TClonesArray  *fTrackCands;   ///< @brief Array of found track candidates in PndTrackCand (for output)
	TClonesArray  *fTracks;       ///< @brief Array of found tracks in PndTrack (for output)
	TClonesArray *fHoughTrackCands; ///< @brief Array of found track cands in PndFtsHoughTrackCand (only for debugging)
	/* @brief Not used.
	 *
	 * The idea was to use this in order to write out the Hough spaces to the root file.
	 * Due to bad performance this is not used. Instead, I write it out as picture directly.
	 */
	//TClonesArray *fHoughSpaces;

	//-------
	// Debug
	//-------
	Bool_t fSaveDebugInfo; ///< @brief Debug information will be created iif kTRUE.
	UInt_t fEventNr; ///< @brief Event number for debugging purposes.


	// TODO: I don't think I need the copy constructor and the operator=
	PndFtsHoughTrackerTask(const PndFtsHoughTrackerTask&);
	PndFtsHoughTrackerTask operator=(const PndFtsHoughTrackerTask&);

	ClassDef(PndFtsHoughTrackerTask,1);
};

#endif
