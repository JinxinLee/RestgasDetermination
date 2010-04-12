/*
 * PndMCIdealTrackFinder.h
 *
 *  Created on: Apr 12, 2010
 *      Author: stockman
 */

#ifndef PNDMCIDEALTRACKFINDER_H_
#define PNDMCIDEALTRACKFINDER_H_


// framework includes
#include "FairTask.h"
#include "PndMCMatch.h"
#include "PndDetectorList.h"

#include "TClonesArray.h"

class PndMCIdealTrackFinder : public FairTask {
public:
	PndMCIdealTrackFinder();
	virtual ~PndMCIdealTrackFinder();

	  /** Virtual method Init **/
//	  virtual void SetParContainers();
	  virtual InitStatus Init();


	  /** Virtual method Exec **/
	  virtual void Exec(Option_t* opt);

	//  virtual void Finish();

private:
	  PndMCMatch* fMCMatch;
	  TClonesArray* fTrackCand;
	  TClonesArray* fMCTrack;
};

#endif /* PNDMCIDEALTRACKFINDER_H_ */
