#ifndef PNDRIEMANNTRACKFINDER_H_
#define PNDRIEMANNTRACKFINDER_H_

#include "PndRiemannTrack.h"
#include "PndRiemannHit.h"
#include "CbmHit.h"

#include "TClonesArray.h"
#include "TrackCand.h"

#include <vector>
#include <map>

class PndRiemannTrackFinder
{
public:
	PndRiemannTrackFinder();
	virtual ~PndRiemannTrackFinder();
	
	void FindTracks(double maxdist, double maxszdist = 1);						///< Main function to start the riemann track finding
	void SetHits(std::vector<CbmHit*> hits){fHits = hits;};	///< Replaces the existing array of hits with a new one
	void AddHits(std::vector<CbmHit*> hits);				///< Appends the new array of hits to the existing one
	void AddHits(TClonesArray* hits);						///< Appends the new array of hits to the existing one
	void AddHit(CbmHit* hit){fHits.push_back(hit);};		///< Adds one new hit to the array of hits
	
	int NTracks(){return fTracks.size();};					///< Returns the number of found tracks
	PndRiemannTrack GetTrack(int i){return fTracks[i];};	///< Returns the track with the index i
	std::vector<Int_t> GetTrackCandidates(int i){return fHitsInTracks[i];};	///< Returns the hits belonging to track i
	std::vector<TrackCand> GetTrackCand(){return fTrackCand;}
	TrackCand GetTrackCand(int i) {return fTrackCand[i];}
	double HitDistance(CbmHit* h1, CbmHit* h2);				///< Calculates the distance between two hits
	int HitTooClose(std::vector<Int_t>* hitsInUse, CbmHit* newHit, double threshold); ///< returns if and which hit was too close to the hit which is tested
	
private:
	std::vector<CbmHit*> fHits;							///< Vector of CbmHits used for track finding (fitting)
	std::vector<PndRiemannTrack> fTracks;				///< Resulting Riemann Tracks
	std::vector<std::vector<Int_t> > fHitsInTracks;		///< Vector of indizes which hits where used in which track
	std::vector<TrackCand> fTrackCand;
	std::vector<int> fHitsTooClose; 					///< vector of size 4 which contains the hits which are too close to either one of the three base points of a circle or to the fourth hit
	std::map<int, std::pair<int,int> > fMapHitToID;		///< map to convert the list of hits back into a detID and hitID
	
	double fMaxPlaneDist;								///< Distance cut between new point and riemann plane
	double fMaxSZDist;									///< Distance cut between s-z coordinate of a new point and the sz-fit of the hits in the track
	double fMinPointDist;								///< Minimum distance between two points to use them as point for the base plane
	double fMaxSZChi2;									///< Maximum allowed Chi2 in an sz fit
	double fMaxTheta;									///< Maximum theta difference between the points
	double fMaxPhi;										///< Maximum phi difference between the points
	
	int fVerbose;
	bool fUseZeroPos;
	
	bool TrackExists(std::vector<Int_t> hitsInTrack);

	// Fast method to figure out if a combination of hits is already used as a track
	//Problem: It does not work for more than 32 hits
//	std::vector<Int_t> fHitsInTracks; ///< one int stands for the hits used in one track. The hits are encoded as bits in an int32
	
	//Solution: use a vector<vector <Int_t> > fHitsInTracks instead. This is not so fast but works for more than 32 hits

//	void SetHitInTrack(Int_t position, Int_t& value);
//	std::vector<int> GetHitsInTrack(Int_t value);
//	bool TrackExists(Int_t hitsInTrack);


//	std::vector<int> GetHitsInTrack(Int_t value);


public:
  ClassDef(PndRiemannTrackFinder,1)
};

#endif /*PNDRIEMANNTRACKFINDER_H_*/
