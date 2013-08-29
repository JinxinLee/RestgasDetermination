/*
 * PndTrackingQualityData.h
 *
 *  Created on: Aug 23, 2013
 *      Author: stockman
 */

#ifndef PNDTRACKINGQUALITYDATA_H_
#define PNDTRACKINGQUALITYDATA_H_

#include "FairMultiLinkedData.h"
#include "FairRootManager.h"

#include "PndMCMatch.h"
#include "PndMCResult.h"
#include "PndTrack.h"
#include "PndTrackCand.h"

#include <TObject.h>
#include <TString.h>
#include <TClonesArray.h>

#include <vector>
#include <map>

class PndTrackingQualityData : public TObject
{
public:
	PndTrackingQualityData();
	PndTrackingQualityData(std::vector<TString>& branchNames);

	virtual ~PndTrackingQualityData();

	virtual void Init();
	void SetVerbose(Int_t val){fVerbose = val;}

	//! Adds branch names of detector data which should be taken into account in the analysis
	void AddHitsBranchName(TString name){ fBranchNames.push_back(name);}
	std::map<TString, FairMultiLinkedData> AnalyseTrack(PndTrack* track);
	std::map<TString, FairMultiLinkedData> AnalyseTrackCand(PndTrackCand* trackCand);


private:

	FairMultiLinkedData GetMCInfoForBranch(TString branchName, PndTrackCand* trackCand);

	Int_t fVerbose;
	FairRootManager* ioman;

	TString fTrackBranchName;

	std::vector<TString> fBranchNames;
//	PndMCResult fIdealTrackData;

	PndMCMatch* fMCMatch;




	ClassDef(PndTrackingQualityData, 1)
};

#endif /* PNDTRACKINGQUALITY_H_ */
