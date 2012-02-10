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
#include "PndLumiTransposition.h"

#include "PndDetectorList.h"

#include <map>
#include <vector>
#include <algorithm>
#include <string>

#include "TClonesArray.h"
#include "TMath.h"
#include "TString.h"
#include "TVector2.h"
#include "TVector3.h"

//class PndLumiHitErrors;

using namespace std;

class PndLumiTrackFinder: public FairTask
{
public:

	PndLumiTrackFinder();

	PndLumiTrackFinder(Double_t nsig, Double_t mom, Double_t thick,
			Double_t pow, Int_t iverb, bool usehough);

	virtual ~PndLumiTrackFinder();

	virtual InitStatus Init();

	virtual void Exec(Option_t* opt);
	void Finish(){;}

	void Print() const;

private:

	std::vector<std::vector<Int_t> > gethitsontrackbyangle(std::map<Int_t,Int_t> detIdmap, std::map<Int_t,TVector3> hitmap);

	std::vector<std::vector<Int_t> >gethitsontrackbycoord(std::map<Int_t,Int_t> detIdmap, std::map<Int_t,TVector3> hitmap);

	bool IsGoodCollection(std::map<Int_t,Int_t>);

	TVector3 doHitTransf(TVector3 &);

	Double_t getMultiScatterAngle();

	Double_t fn; // multiple of sigma for hit searcher
	Double_t fMom; //beam momentum
	Double_t fEps; //Silicon sensor thickness
	Double_t fPow; //bending power of the dipole if off fPow=0, if on fPow = 40 mrad
	Int_t fVerbose;
	bool fUseHough;

	//Input Array
	TClonesArray *fLumiHit;

	//Output Array
	TClonesArray* fLumiTrack;

	PndLumiTransposition* fGeoH;

	ClassDef(PndLumiTrackFinder,2);

};

#endif /* PNDLUMITRACKFINDER_H_ */
