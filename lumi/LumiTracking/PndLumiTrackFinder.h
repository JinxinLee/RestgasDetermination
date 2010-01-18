/*
 * PndLumiTrackFinder.h
 *
 *       Author: tsito
 */

#ifndef PNDLUMITRACKFINDER_H_
#define PNDLUMITRACKFINDER_H_
#include "FairTask.h"
#include "FairRootManager.h"

#include "PndLumiHit.h"
#include "PndLumiPoint.h"
#include "PndMCTrack.h"
#include "PndLumiTrack.h"

#include <map>
#include <vector>
#include <algorithm>
#include <string>

#include "TClonesArray.h"
#include "TMath.h"
#include "TString.h"
#include "TVector2.h"
#include "TVector3.h"

using namespace std;

class PndLumiTrackFinder: public FairTask
{
public:

	PndLumiTrackFinder();

	PndLumiTrackFinder(Double_t dth, Double_t dph, Int_t iverb);

	virtual ~PndLumiTrackFinder();

	virtual InitStatus Init();

	virtual void Exec(Option_t* opt);

	void Print() const;

private:

	vector<vector<Int_t> >GetHitsOnTrack(map<Int_t,TVector2> anglemap);

	bool IsGoodTrack(vector<TVector3> oneCandidate);

	void GetErrorsPerHit(Int_t mcindex, TVector3& Err);


	Double_t fDTheta, fDPhi; //Cell dimension in (theta,phi) plane
	Int_t iVerbose;

	//Input Array
	TClonesArray *fLumiPoint;
	TClonesArray *fLumiMCTrack;
	TClonesArray *fLumiHit;

	//Output Array
	TClonesArray* fLumiTrack;
	//std::map<Int_t, TrackCand*> fTrackCandMap;

	ClassDef(PndLumiTrackFinder,1);

};

#endif /* PNDLUMITRACKFINDER_H_ */
