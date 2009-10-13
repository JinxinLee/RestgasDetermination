#ifndef PNDMVDRIEMANNTRACKFINDER_H_
#define PNDMVDRIEMANNTRACKFINDER_H_

#include "TClonesArray.h"
#include "PndRiemannTrackFinder.h"
#include "PndMvdGeoHandling.h"

#include "TH2F.h"

class PndMvdRiemannTrackFinder : public  PndRiemannTrackFinder
{
public:
	PndMvdRiemannTrackFinder();
	virtual ~PndMvdRiemannTrackFinder();

	void FindTracks();										///< Main function to start the riemann track finding
	void AddHits(TClonesArray* hits);

	void SetGeoH(PndMvdGeoHandling geoH){fGeoH=geoH;};
	void SetCutDistH(TH2F* hist) {fCutDistH=hist;}
	void SetCutChi2H(TH2F* hist) {fCutChi2H=hist;}

private:
	PndMvdGeoHandling fGeoH;

	std::vector<std::vector<Int_t> >  GetStartTracks();
	bool CheckSZ(PndRiemannTrack aTrack);				///< Tests the results of the sz fit
	bool CheckRiemannHit(PndRiemannTrack* track, PndRiemannHit* hit);

	TH2F* fCutDistH;  ///< cutting histograms
	TH2F* fCutChi2H;

	double fZClosePar;	///< parameter to separate  forward and backward tracks

	std::vector<int> fLayers[11]; ///< contains layer information of hits
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

	double GetMaxPlaneDist(double radius, double dip, bool sign); //getting cut distance
	double GetMaxSZChi2(double radius, double dip, bool sign);    //getting cut Chi2
	std::vector<int> GetTooCloseHitsInLayer(int LayerNumber , int HitNumber ); // searching for too close hits to HitNumber-Hit in layer LayerNumber

public:
  ClassDef(PndMvdRiemannTrackFinder,1)
};

#endif /*PNDMVDRIEMANNTRACKFINDER_H_*/
