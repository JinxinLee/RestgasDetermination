/*
 * PndMCIdealTrackFinderNewLinks.h
 *
 *  Created on: Apr 12, 2010
 *      Author: stockman
 */

#ifndef PndMCIdealTrackFinderNewLinks_H_
#define PndMCIdealTrackFinderNewLinks_H_


// framework includes
#include "FairTask.h"
#include "PndTrackCand.h"

#include "TClonesArray.h"

class PndMCIdealTrackFinderNewLinks : public FairTask {
public:
	PndMCIdealTrackFinderNewLinks();
	virtual ~PndMCIdealTrackFinderNewLinks();

	  /** Virtual method Init **/
//	  virtual void SetParContainers();
	  virtual InitStatus Init();
	  virtual void AddBranchName(TString name){ fBranchNames.push_back(name);}


	  /** Virtual method Exec **/
	  virtual void Exec(Option_t* opt);

	//  virtual void Finish();
protected:
	  virtual void CreateTrackCands();

private:
	  TClonesArray* fTrackCand;
	  TClonesArray* fMCTrack;
	  std::map<TString, TClonesArray*> fBranchMap;
	  std::vector<TString> fBranchNames;
	  std::map<FairLink, PndTrackCand> fTrackCandMap;
	  Int_t fHitCount;

	  ClassDef(PndMCIdealTrackFinderNewLinks,1);
};

#endif /* PndMCIdealTrackFinderNewLinks_H_ */
