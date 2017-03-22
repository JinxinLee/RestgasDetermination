/*
 * PndMissingPzCleanerTask.h
 *
 * \brief Task to flag or to remove tracks with missing Pz information
 *
 * This task takes as an input any PndTrack branch. It goes through all the tracks. For those tracks only consisting of STTHits
 * and with a ParamFirst::pz value between 0.015 and 0.025 it either sets the PndTrack::fFlag to -30 or deletes the track.
 * What action is taken is selected by SetRemoveTrack(). If SetRemoveTrack is set to kTRUE the track is deleted otherwise flaged.
 * The filtered PndTracks are written in an output branch either with a given name by SetOutputTrackBranch() of, if no branch name is
 * given by InputTrackBranchName + "_filtered".
 *
 *  Created on: 22.3.2017
 *      Author: t.stockmanns
 */

#ifndef PndMissingPzCleanerTask_H_
#define PndMissingPzCleanerTask_H_

#include "FairTask.h"
#include "TString.h"

class TClonesArray;

class PndMissingPzCleanerTask: public FairTask {
public:
	PndMissingPzCleanerTask()
		: FairTask("Missing Pz Track Cleaner Task"), fInputTrackBranch(""), fOutputTrackBranch(""),
		  fRemoveTrack(kFALSE), fTracks(0), fFilteredTracks(0), fPersistence(kTRUE){ };
	PndMissingPzCleanerTask(const TString& branchName)
		: FairTask("Missing Pz Track Cleaner Task"), fInputTrackBranch(branchName), fOutputTrackBranch(""),
		  fRemoveTrack(kFALSE), fTracks(0), fFilteredTracks(0), fPersistence(kTRUE){ };
	virtual ~PndMissingPzCleanerTask(){};

	/** Virtual method Init **/
	virtual void SetParContainers();
	virtual InitStatus Init();

	/** Virtual method Exec **/
	virtual void Exec(Option_t* opt);
	virtual void FinishEvent();
	virtual void Finish();

    void SetInputTrackBranch(const TString& name) { fInputTrackBranch = name; }
    void SetOutputTrackBranch(const TString& name) { fOutputTrackBranch = name; }
    void SetRemoveTrack(const Bool_t& remove) { fRemoveTrack = remove; }
    void SetPersistence(const Bool_t& val) { fPersistence = val; }

private:
    TClonesArray* fTracks;
    TClonesArray* fFilteredTracks;

    TString fInputTrackBranch;
    TString fOutputTrackBranch;

    Bool_t fRemoveTrack; ///< if this flag is set than the track will not be written to the output branch. Otherwise the flag -30 will be set.
    Bool_t fPersistence; ///< sets if the track is stored in the output root tree or not.

	ClassDef(PndMissingPzCleanerTask,1);


};

#endif /* PndMissingPzCleanerTask_H_ */
