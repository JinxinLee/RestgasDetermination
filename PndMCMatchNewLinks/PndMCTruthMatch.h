/*
 * PndMCTruthMatch.h
 *
 *  Created on: Apr 12, 2010
 *      Author: stockman
 */

#ifndef PndMCTruthMatch_H_
#define PndMCTruthMatch_H_


// framework includes
#include "FairTask.h"

#include "TClonesArray.h"
#include <math.h>

/// Prints the MCTrack info for all data objects in a branch
///
/// This class prints for all data objects of a given branch the matching MCTrack information
/// as long as the data derives from FairMultiLinkedData_Interface
/// \author Tobias Stockmanns
/// \date Sep. 2015

class PndMCTruthMatch : public FairTask {
public:
	PndMCTruthMatch();
	PndMCTruthMatch(TString inBranchName);
	virtual ~PndMCTruthMatch();

	  /** Virtual method Init **/
//	  virtual void SetParContainers();
	  virtual InitStatus Init();
	  virtual void SetInBranchName(TString name){ fInBranchName = name;}

	  /** Virtual method Exec **/
	  virtual void Exec(Option_t* opt);

private:
	  TClonesArray* fMCTrack;
	  TClonesArray* fInBranch;

	  TString fInBranchName;


	  ClassDef(PndMCTruthMatch,1);
};

#endif /* PndMCTruthMatch_H_ */
