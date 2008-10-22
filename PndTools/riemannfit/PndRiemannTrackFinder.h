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
	
	void FindTracks();										///< Main function to start the riemann track finding
	void MergeTracks();
	void SetHits(std::vector<CbmHit*> hits){fHits = hits;};	///< Replaces the existing array of hits with a new one
	void AddHits(std::vector<CbmHit*> hits);				///< Appends the new array of hits to the existing one
	void AddHits(TClonesArray* hits);						///< Appends the new array of hits to the existing one
	void AddHit(CbmHit* hit){fHits.push_back(hit);};		///< Adds one new hit to the array of hits
	void SetVerbose (int val){fVerbose = val;}
	int NTracks(){return fTrackCand.size();};					///< Returns the number of found tracks
	PndRiemannTrack GetTrack(int i){return fTracks[i];};	///< Returns the track with the index i
	std::vector<Int_t> GetTrackCandidates(int i){return fHitsInTracks[i];};	///< Returns the hits belonging to track i
	std::vector<TrackCand> GetTrackCand(){return fTrackCand;}
	std::vector<TrackCand> GetMergedTrackCands(){return fMergedTrackCand;}
	TrackCand GetTrackCand(int i) {return fTrackCand[i];}
	double HitDistance(CbmHit* h1, CbmHit* h2);				///< Calculates the distance between two hits
	int HitTooClose(std::vector<Int_t> hitsInUse, CbmHit* newHit, double threshold); ///< returns if and which hit was too close to the hit which is tested
	
	void SetMaxPlaneDistance(double val){fMaxPlaneDist = val;}
	void SetMaxSZDist(double val){fMaxSZDist = val;}
	void SetMaxSZChi2(double val){fMaxSZChi2 = val;}
	void SetMinPointDist(double val){fMinPointDist = val;}
	void SetMaxTheta(double val){fMaxTheta = val;}
	void SetMaxPhi(double val){fMaxPhi = val;}
	void SetUseZeroPos(bool val){fUseZeroPos = val;}
	
	void SetCurvDiff(double val){fCurvDiff = val;}
	void SetDipDiff(double val){fDipDiff = val;}
	
private:
	std::vector<CbmHit*> fHits;							///< Vector of CbmHits used for track finding (fitting)
	std::vector<PndRiemannTrack> fTracks;				///< Resulting Riemann Tracks
	std::vector<std::vector<Int_t> > fHitsInTracks;		///< Vector of indizes which hits where used in which track
	std::vector<TrackCand> fTrackCand;
	std::vector<TrackCand> fMergedTrackCand;
	std::vector< std::vector<int> > fHitsTooClose;		///< matrix of TrackNr and hits which are too close to one of the three starting points
//	std::vector<int> fTooCloseFirst;
//	std::vector<int> fTooCloseSecond;
	std::map<int, std::pair<int,int> > fMapHitToID;		///< map to convert the list of hits back into a detID and hitID
	std::map<std::pair<unsigned int, unsigned int>, int > fMapIDtoHit; ///<map to convert the list of detID/hitID hits into the list of hits for track finding
	double fMaxPlaneDist;								///< Distance cut between new point and riemann plane
	double fMaxSZDist;									///< Distance cut between s-z coordinate of a new point and the sz-fit of the hits in the track
	double fMinPointDist;								///< Minimum distance between two points to use them as point for the base plane
	double fMaxSZChi2;									///< Maximum allowed Chi2 in an sz fit
	double fMaxTheta;									///< Maximum theta difference between the points
	double fMaxPhi;										///< Maximum phi difference between the points
	int    fMinNumberOfHits;							///< Minimum number of hits in track necessary for a match
	double fCurvDiff;									///< TrackMerger parameter
	double fDipDiff;									///< TrackMerger parameter
	
	int fVerbose;
	bool fUseZeroPos;
	
	std::vector<std::vector<Int_t> >  GetStartTracks();
	bool CheckThetaPhi(int hit1, int hit2);				///< Tests if the theta, phi difference between the two hits is within the boundaries
	bool CheckHitDistance(int hit1, int hit2);	///< Tests if the distance is larger than fMinPointDistance
	bool CheckSZ(PndRiemannTrack aTrack);				///< Tests the results of the sz fit
	bool CheckRiemannHit(PndRiemannTrack* track, PndRiemannHit* hit);
	bool CheckHitInSameSensor(int hit1, int hit2); ///< Tests if hits in the same sensor are selected
	
	PndRiemannTrack CreateRiemannTrack(std::vector<Int_t> aHits); ///< Creates a PndRiemannTrack from an array of indices of Hits
		
	bool TrackExists(std::vector<Int_t> hitsInTrack);
	std::vector<int> FindTracksWithSimilarParameters(int TrackInd, std::vector<int>& TracksToTest, double curvDiff, double dipDiff);
	std::vector<int> FindTracksWithSimilarHits(std::vector<int>& TracksToTest);
	void RemoveTrack(int TrackInd, std::vector<int>& TrackList);
	void PrintTrackCand(TrackCand* cand, bool shortOutput = false);
	TrackCand CreateOneTrackCand(std::vector<int> tracks);
	void RefitTrackCand(TrackCand& cand);
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
