#ifndef PNDMVDGEMRIEMANNTRACKFINDER_H_
#define PNDMVDGEMRIEMANNTRACKFINDER_H_

#include "TClonesArray.h"
#include "PndRiemannTrackFinder.h"
#include "PndGeoHandling.h"

#include "TH2F.h"
#include <vector>

class PndMvdGemRiemannTrackFinder : public  PndRiemannTrackFinder
{
public:
	PndMvdGemRiemannTrackFinder();
	virtual ~PndMvdGemRiemannTrackFinder();

	void FindTracks();										///< Main function to start the riemann track finding
	void AddHits(TClonesArray* hits, Int_t branchId);


	//void SetGeoH(PndGeoHandling geoH){fGeoH=geoH;};
	void SetCutDistH(TH2F* hist) {fCutDistH=hist;}
	void SetCutChi2H(TH2F* hist) {fCutChi2H=hist;}

private:
	PndGeoHandling* fGeoH;

	std::vector<std::vector<Int_t> >  GetStartTracks();
	bool CheckSZ(PndRiemannTrack aTrack);				///< Tests the results of the sz fit
	bool CheckRiemannHit(PndRiemannTrack* track, PndRiemannHit* hit, FairHit* fairHit);
	bool CheckRiemannHitMvd(PndRiemannTrack* track, PndRiemannHit* hit, FairHit* fairHit);
	bool CheckRiemannHitGem(PndRiemannTrack* track, PndRiemannHit* hit, FairHit* fairHit);



	int GetLayerGem(FairHit* hit);
	int GetLayerMvd(FairHit* hit);
	int GetLayer(TString identifier);
	void InitLayerMap();
	void InitLayerMapMvd();
	void InitLayerMapGem();


	TH2F* fCutDistH;  ///< cutting histograms
	TH2F* fCutChi2H;

	double fZClosePar;	///< parameter to separate  forward and backward tracks

	std::vector< std::vector<int> >fLayers; ///< contains layer information of hits
	std::map<TString, int> fLayerMap; ///< identifier string, assigned layer id
	int fLastLayerId; ///< last layer Id assigned
	/*       b - barrel layer;   d - disk layer
	0 - zero position
	1 - 1b
	2 - 1d
	3 - 2d
	4 - 2b
	5 - 3d
	6 - 4d
	7 - 3b
	8 - 5d
	9 - 4b
   10 - 6d
*/
	int fNLayers; ///< number of Layers
	double GetMaxPlaneDist(double radius, double dip, bool sign); //getting cut distance
	double GetMaxSZChi2(double radius, double dip, bool sign);    //getting cut Chi2
	std::vector<int> GetTooCloseHitsInLayer(int LayerNumber , int HitNumber ); // searching for too close hits to HitNumber-Hit in layer LayerNumber

public:
  ClassDef(PndMvdGemRiemannTrackFinder,1)
};

#endif /*PNDMVDGEMRIEMANNTRACKFINDER_H_*/
