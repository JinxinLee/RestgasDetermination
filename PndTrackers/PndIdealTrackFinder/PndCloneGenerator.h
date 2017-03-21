/*
 * PndCloneGenerator.h
 *
 *  Created on: Apr 12, 2010
 *      Author: stockman
 */

#ifndef PndCloneGenerator_H_
#define PndCloneGenerator_H_


// framework includes
#include "FairTask.h"
#include "FairMCPoint.h"
#include "PndTrackCand.h"
#include "TClonesArray.h"
#include "TDatabasePDG.h"
#include <math.h>
#include <PndIdealTrackFinder.h>

class PndCloneGenerator : public PndIdealTrackFinder {
public:
	PndCloneGenerator();
	virtual ~PndCloneGenerator();

	  /** Virtual method Exec **/
	  virtual void Exec(Option_t* opt);




protected:
	  virtual void CreateTrackCandClones();
	  virtual void SetFirstLastHit(std::map<FairLink, PndTrackCand>& trackCandMap);


private:

	  std::map<FairLink, PndTrackCand> fTrackCandCloneMap;



	  ClassDef(PndCloneGenerator,2);
};

#endif /* PndCloneGenerator_H_ */
