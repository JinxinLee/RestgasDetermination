#include "PndRiemannTrackFinder.h"
#include <iostream>
#include <math.h>
#include "PndMvdHit.h"


ClassImp(PndRiemannTrackFinder);

PndRiemannTrackFinder::PndRiemannTrackFinder(): fMaxPlaneDist(0.7), fMaxSZDist(1), fMaxSZChi2(1),
fMinPointDist(0.5), fMaxTheta(0.4), fMaxPhi(0.4), fUseZeroPos(true), fCurvDiff(0.02), fDipDiff(0.02),
fMinNumberOfHits(4), fVerbose(1)
{
	if (fUseZeroPos){
		TVector3 pos(0.0,0.0,0.0);
		TVector3 dpos(0.1,0.1,0.1);
		PndMvdHit* ZeroHit = new PndMvdHit(1, "", pos, dpos, -1, 0, 0,-1); // this is not very nice (one should create a neutral FairHit here
		fHits.push_back(ZeroHit);
	}
}

PndRiemannTrackFinder::~PndRiemannTrackFinder()
{
	if (fUseZeroPos)
		delete(fHits[0]);
}

void PndRiemannTrackFinder::AddHits(std::vector<FairHit*> hits)
{
	for (int i = 0; i < hits.size(); i++){
		fHits.push_back(hits[i]);
	}
}

void PndRiemannTrackFinder::AddHits(TClonesArray* hits)
{
	for (int i = 0; i < hits->GetEntries(); i++){
		FairHit* myHit = (FairHit*)(hits->At(i));
		fHits.push_back(myHit);
		std::pair<int,int> myID(myHit->GetDetectorID(), i);
		fMapHitToID[fHits.size()-1]=myID;
		fMapIDtoHit[myID] = fHits.size()-1;
	}
}

void PndRiemannTrackFinder::FindTracks()
{
	std::vector<std::vector<Int_t> > Tracks = GetStartTracks();				//Get the possible track seeds

	for (int trackId = 0; trackId < Tracks.size(); trackId++){				//Go through all track seeds and search for additional points

		if (Tracks[trackId].size() != 3)
			std::cout << "-E- PndRiemannTrackFinder::FindTracks: Start points: " << Tracks[trackId].size()
			          << " in Track: " << trackId << std::endl;

		std::vector<Int_t> StartTrack = Tracks[trackId];

		if (fVerbose > 1){
			std::cout << "------------------------------------" << std::endl;
			std::cout << "Start Plane from Points: " << StartTrack[0] << " "
					  << StartTrack[1] << " " << StartTrack[2] << std::endl;
		}
		if (TrackExists(StartTrack) == true){
			if (fVerbose > 1) std::cout << "Track exists already!" << std::endl;
			continue;
		}
		PndRiemannTrack actTrack = CreateRiemannTrack(StartTrack);
		int startHit = StartTrack[2];
		for (int testHit = startHit+1; testHit < fHits.size(); testHit++){

			if (CheckHitDistance(StartTrack[0], testHit)!= true){fHitsTooClose[trackId].push_back(testHit); continue;}
			if (CheckHitDistance(StartTrack[1], testHit)!= true){fHitsTooClose[trackId].push_back(testHit); continue;}
			if (CheckHitDistance(StartTrack[2], testHit)!= true){fHitsTooClose[trackId].push_back(testHit); continue;}

			PndRiemannHit actHit(fHits[testHit]);

			if (fVerbose > 1) std::cout << "Point " << testHit ;
			if (CheckRiemannHit(&actTrack, &actHit) != true) continue;


			StartTrack.push_back(testHit);

			actTrack.addHit(actHit);
			actTrack.refit();
			actTrack.szFit();

			TVectorD orig = actTrack.orig();
			if (fVerbose > 1) std::cout << "actHit added: " << testHit  << " r: " << actTrack.r()
										<< " orig: " << orig[0] << " " << orig[1] << std::endl;
		}

		if (actTrack.getNumHits() > (fMinNumberOfHits-1))		//if you have a track match
		{
			std::vector<int> hits = fHitsTooClose[trackId];
			for (int ind = 0; ind < hits.size(); ind++){
				PndRiemannHit actHit(fHits[hits[ind]]);
				if (CheckRiemannHit(&actTrack, &actHit)!= true) continue;
				StartTrack.push_back(hits[ind]);

				actTrack.addHit(actHit);
				actTrack.refit();
				actTrack.szFit();
			}

			fTracks.push_back(actTrack);
			fHitsInTracks.push_back(StartTrack);
			if (fVerbose > 1){
				TVectorD orig = actTrack.orig();
				std::cout << "Track added! " << StartTrack[0] << " " << StartTrack[1]
					<< " " << StartTrack[2] << " r: " << actTrack.r()
					<< " orig: " << orig[0] << " " << orig[1]
					<< " sz-m: " << actTrack.getSZm() << " sz-t: " << actTrack.getSZt()
					<< " dip: " << actTrack.dip()
					<< std::endl;
			}

			if (fVerbose > 0) std::cout << "Hits in Track: ";
			for (int i = 0; i < StartTrack.size(); i++)
			{
				if (fVerbose > 0)
					std::cout << " " << fMapHitToID[StartTrack[i]].first << "/" << fMapHitToID[StartTrack[i]].second;
			}
			if (fVerbose > 0) {
				TVectorD myOrig = actTrack.orig();
				std::cout << " numHits: " << actTrack.getNumHits() << std::endl;
				std::cout << " curv: " << 1/actTrack.r() << "+/-" << actTrack.dR()/(actTrack.r() * actTrack.r())
						  << " dip: " << actTrack.dip() << "+/-" << actTrack.dDip()
						  << " orig: " << myOrig[0] << "+/-" << actTrack.dX()
						  << " " << myOrig[1] << "+/-" << actTrack.dY() << std::endl;
			}
		}
	}
	for (int n = 0; n < fHitsInTracks.size(); n++)
	{
		//		fTracks[n].szfit();
		TrackCand myTrackCand;
		myTrackCand.setCurv(1/fTracks[n].r());
		//myTrackCand.setCurvError(fTracks[n].dR()/(fTracks[n].r() * fTracks[n].r()));
		myTrackCand.setDip(fTracks[n].dip());
		//myTrackCand.setDipError(fTracks[n].dDip());
		myTrackCand.setInverted(fTracks[n].sign());
		for (int o = 0; o < fHitsInTracks[n].size(); o++)
		{
			myTrackCand.addHit(fMapHitToID[fHitsInTracks[n][o]].first,
					fMapHitToID[fHitsInTracks[n][o]].second);
		}
		fTrackCand.push_back(myTrackCand);
	}

	MergeTracks();
	fTrackCand.clear();
	fTrackCand = fMergedTrackCand;
	if (fVerbose > 0) {
		std::cout << "Tracks after merging:" << std::endl;
		for (int p = 0; p < fTrackCand.size(); p++){
			PrintTrackCand(&fTrackCand[p], true);
		}
	}

}

std::vector< std::vector<Int_t> > PndRiemannTrackFinder::GetStartTracks()
{
	std::vector<Int_t> actCandidates;
	std::vector<std::vector<Int_t> > Tracks;
	std::vector<int> tooCloseFirst;
	std::vector<int> tooCloseSecond;
	if (fHits.size() > 4){
		for (int first = 0; first < fHits.size()-3; first++){				//take three points and create a riemann plane - get first hit
			tooCloseFirst.clear();
			for (int second = first+1; second < fHits.size()-2; second++){				// get second hit
				tooCloseSecond.clear();
				if (CheckHitDistance(first, second) != true){ tooCloseFirst.push_back(second); continue;}
				//if (CheckThetaPhi(first, second) != true) continue;
				if (CheckHitInSameSensor(first, second) == true) continue;

				for (int third = second+1; third < fHits.size()-1; third++){
					if (fVerbose > 1) std::cout << "Checking Points: " << first << " " << second << " " << third << std::endl;
					//if (CheckThetaPhi(second, third) != true) continue;
					if (CheckHitDistance(first, third)!= true){tooCloseFirst.push_back(third); continue;}
					if (CheckHitDistance(second, third)!= true){tooCloseFirst.push_back(third); continue;}
					if (CheckHitInSameSensor(first, third)==true)continue;
					if (CheckHitInSameSensor(second, third)==true)continue;

					actCandidates.clear();
					actCandidates.push_back(first);
					actCandidates.push_back(second);
					actCandidates.push_back(third);

					if (!TrackExists(actCandidates)){			//checks if the combination of three points was used before in a track
						PndRiemannTrack actTrack;// = new PndRiemannTrack();
						PndRiemannHit hit1(fHits[first]);
						PndRiemannHit hit2(fHits[second]);
						PndRiemannHit hit3(fHits[third]);
						actTrack.addHit(hit1);
						actTrack.addHit(hit2);
						actTrack.addHit(hit3);
						actTrack.refit();
						if (CheckSZ(actTrack)!= true) continue;
						TVectorT<double> orig = actTrack.orig();
						if (fVerbose > 1) std::cout << "Base plane from Points: " << first << " " << second << " " << third << " r: " << actTrack.r() << " orig: " << orig[0] << " " << orig[1] << std::endl;
						Tracks.push_back(actCandidates);
						std::vector<int> combHits = tooCloseFirst;
						combHits.insert(combHits.begin(), tooCloseSecond.begin(), tooCloseSecond.end());
						fHitsTooClose.push_back(combHits);
					}
				}
			}
		}
	}
	if (fVerbose > 1) {
		std::cout << "Start Tracks are: " << std::endl;
		for (int i = 0; i < Tracks.size(); i++){
			std::vector<int> aTrack = Tracks[i];
			for (int j = 0; j < aTrack.size(); j++){
				std::cout << aTrack[j] << " ";
			}
			std::cout << std::endl;
		}
	}
	return Tracks;
}

bool PndRiemannTrackFinder::CheckThetaPhi(int hit1, int hit2)
{
	TVector3 vec1, vec2;
	fHits[hit1]->Position(vec1);
	fHits[hit2]->Position(vec2);

	if (vec1.Theta() > 0 && vec2.Theta() > 0)
		if (fabs(vec1.Theta()-vec2.Theta()) > fMaxTheta && fabs(vec1.Phi()-vec2.Phi()) > fMaxPhi){
			if (fVerbose > 1){
				std::cout << "Theta: " << vec1.Theta() - vec2.Theta() << " > max: " << fMaxTheta << " or ";
				std::cout << "Phi: " << vec1.Phi() - vec2.Phi() << " > max: " << fMaxPhi << std::endl;
			}
			return false;
		}
	return true;
}

bool PndRiemannTrackFinder::CheckHitDistance(int hit1, int hit2)
{
	double dist = HitDistance(fHits[hit1], fHits[hit2]);
	if (dist < fMinPointDist)
	{
		if (fVerbose > 1) std::cout << "Hits: " << hit1 << " " << hit2 << " too close " << dist << std::endl;
		return false;
	}
	return true;
}

bool PndRiemannTrackFinder::CheckSZ(PndRiemannTrack aTrack)
{
	aTrack.szFit();
	if (aTrack.szChi2() > fMaxSZChi2){
		if (fVerbose > 1) std::cout << "sz-Fit does not match, Chi2: " << aTrack.szChi2() << std::endl;
		return false;
	}
	return true;
}

PndRiemannTrack PndRiemannTrackFinder::CreateRiemannTrack(std::vector<Int_t> aHits)
{
	PndRiemannTrack result;
	for (int i = 0; i < aHits.size(); i++)
	{
		PndRiemannHit hit(fHits[aHits[i]]);
		result.addHit(hit);
	}
	result.refit();
	result.szFit();
	return result;
}

bool PndRiemannTrackFinder::CheckHitInSameSensor(int hit1, int hit2)
{
	FairHit* first = fHits[hit1];
	FairHit* second = fHits[hit2];

	if (first->GetDetectorID() != 1 || first->GetDetectorID() != 2)
		return false;

	if(((PndMvdHit*)first)->GetDetName() == ((PndMvdHit*)second)->GetDetName()){
		std::cout << hit1 << " " << hit2 <<" in Same Sensor: " << ((PndMvdHit*)first)->GetDetName() << std::endl;
		return true;
	}
	return false;
}

bool PndRiemannTrackFinder::CheckRiemannHit(PndRiemannTrack* track, PndRiemannHit* hit)
{
	double dist = track->dist(hit);
	double szDist = track->szDist(hit);
	double szChi2 = track->calcSZChi2(hit);

	if (fVerbose > 1) std::cout << ": dist " << dist << " szDist " << szDist << " szChi2 " << szChi2 << std::endl;

	if (fabs(dist) > fMaxPlaneDist){
		if (fVerbose > 1) std::cout << "dist larger than " << fMaxPlaneDist << std::endl;
		return false;
	}
	if (szChi2 > fMaxSZChi2){
		if (fVerbose > 1) std::cout << " SZ Chi2 too big! Cut at: " << fMaxSZChi2 << std::endl;
		return false;
	}
	if (szDist > fMaxSZDist){
		if (fVerbose > 1) std::cout << " SZ Dist too big! Cut at: " << fMaxSZDist << std::endl;
		return false;
	}
	return true;
}

void PndRiemannTrackFinder::MergeTracks()
{
	std::vector<int> RemainingTracks;
	std::vector<int> SelectedTracks;
	std::vector<int> TracksToMerge;
	if (fVerbose > 1) std::cout << "fTrackCand size: " << fTrackCand.size() << std::endl;
	for (int i = 0; i < fTrackCand.size(); i++){
		RemainingTracks.push_back(i);
	}
	int remainingTracksSize = RemainingTracks.size();
	if (fVerbose > 1) std::cout << "RemainingTracks: " << RemainingTracks.size() << std::endl;
	while(remainingTracksSize != 0){
		SelectedTracks = FindTracksWithSimilarParameters(RemainingTracks[0], RemainingTracks, fCurvDiff, fDipDiff);
		remainingTracksSize=RemainingTracks.size();
		if (fVerbose > 1) std::cout << "RemainingTracks: " << RemainingTracks.size() << std::endl;
		if (SelectedTracks.size() > 1){
			int selectedTracksSize = SelectedTracks.size();
			while (selectedTracksSize != 0){
				TracksToMerge = FindTracksWithSimilarHits(SelectedTracks);
				selectedTracksSize = SelectedTracks.size();
				TrackCand newCand = CreateOneTrackCand(TracksToMerge);
				fMergedTrackCand.push_back(newCand);
				if (fVerbose > 1){
					std::cout << "New TrackCand:" << std::endl;
					PrintTrackCand(&newCand);
				}
			}
		}
		else fMergedTrackCand.push_back(fTrackCand[SelectedTracks[0]]);
	}
}

std::vector<int> PndRiemannTrackFinder::FindTracksWithSimilarParameters(int TrackInd, std::vector<int>& TracksToTest, double curvDiff, double dipDiff)
{
	std::vector<int> result;
	TrackCand testTrack = fTrackCand[TrackInd];
	result.push_back(TrackInd);
	RemoveTrack(TrackInd, TracksToTest);
	int tracksToTestSize = TracksToTest.size();
	if(fVerbose > 1){
		std::cout << std::endl;
		std::cout << "TrackInd: " << TrackInd << " tracksToTest: " << tracksToTestSize << std::endl;
	}
	for (int i = 0; i < tracksToTestSize; i++){
		TrackCand myTrack = fTrackCand[TracksToTest[i]];
		if ((fabs(testTrack.getCurv() - myTrack.getCurv()) < curvDiff) &&
			(fabs(testTrack.getDip()- myTrack.getDip()) < dipDiff))
		{
			result.push_back(TracksToTest[i]);
			RemoveTrack(TracksToTest[i], TracksToTest);
			tracksToTestSize = TracksToTest.size();
			i--;
		}
	}
	if (fVerbose > 1 && result.size() > 1){
		std::cout << "Tracks with similar parameters: curv: " << testTrack.getCurv() <<
				  " dip: " << testTrack.getDip() << std::endl;
		for(int j = 0; j < result.size();  j++){
			TrackCand printTrack = fTrackCand[result[j]];
			PrintTrackCand(&printTrack);
		}
	}
	return result;
}

std::vector<int> PndRiemannTrackFinder::FindTracksWithSimilarHits(std::vector<int>& TracksToTest)
{
	std::vector<int> result;
	std::map<std::pair<unsigned int,unsigned int>, int > hitCount;
	int TrackInd = TracksToTest[0];
	TrackCand testTrack = fTrackCand[TrackInd];
	result.push_back(TrackInd);
	RemoveTrack(TrackInd, TracksToTest);
	unsigned int detId, hitId;
	for (int i = 0; i < testTrack.getNHits(); i++)
	{
		testTrack.getHit(i, detId, hitId);
		std::pair<unsigned int, unsigned int> hitPair(detId, hitId);
		hitCount[hitPair] = 1;
	}
	int tracksToTestSize = TracksToTest.size();
	for (int j = 0; j < tracksToTestSize; j++){
		TrackCand myTrack = fTrackCand[TracksToTest[j]];
		int NHits = 0;
		unsigned int myTrackDetId, myTrackHitId;
		for (int k = 0; k < myTrack.getNHits(); k++){
			myTrack.getHit(k, myTrackDetId, myTrackHitId);
			std::pair<unsigned int, unsigned int> testPair(myTrackDetId, myTrackHitId);
			if (hitCount[testPair] > 0){
				hitCount[testPair]++;
				NHits++;
			}
		}
		if (NHits > 2){
			result.push_back(TracksToTest[j]);
			RemoveTrack(TracksToTest[j], TracksToTest);
			tracksToTestSize = TracksToTest.size();
			j--;
		}
	}
	if (fVerbose > 1){
		std::cout << "Tracks with similar hits: " << std::endl;
		for (int m = 0; m < result.size(); m++){
			PrintTrackCand(&fTrackCand[result[m]], true);
		}
	}
	return result;
}

void PndRiemannTrackFinder::RemoveTrack(int TrackInd, std::vector<int>& TrackList)
{
	int trackListSize = TrackList.size();
	for(int i = 0; i < trackListSize; i++){
		if (TrackList[i] == TrackInd){
			TrackList.erase(TrackList.begin()+i);
			trackListSize = TrackList.size();
		}
	}
}

bool PndRiemannTrackFinder::TrackExists(std::vector<Int_t> hitsInTrack){
	//if (fVerbose > 1) std::cout << "TrackExists: hitsInTrack " << hitsInTrack << std::endl;
	bool result = true;
	bool oneNumberEqual = false;

	if (fVerbose > 2) std::cout << "TrackExists: fHitsInTrack.size: " << fHitsInTracks.size() << std::endl;
	for (int i = 0; (i < fHitsInTracks.size()); i++){														//run through tracks in trackList
		for (int k = 0; (k < hitsInTrack.size()&&(result == true)); k++){									//run through all hits in test track
			for (int j = 0; (j < fHitsInTracks[i].size()) && (oneNumberEqual == false); j++){				//run through all hits in selected track
				if (fVerbose > 2) std::cout << hitsInTrack[k] << " ?= " << fHitsInTracks[i][j] << std::endl;
				if (hitsInTrack[k] == fHitsInTracks[i][j])
					oneNumberEqual = true;
				else oneNumberEqual = false;
			}
			result &= oneNumberEqual;
			oneNumberEqual = false;
		}
		if (result == true) {
			if (fVerbose > 1) std::cout << "Track exists already!" << std::endl;
			return true;
		}
		result = true;
	}
	return false;
}

double PndRiemannTrackFinder::HitDistance(FairHit* h1, FairHit* h2)
{
	TVector3 vH1, vH2, result;
	h1->Position(vH1);
	h2->Position(vH2);

	result = vH1 -vH2;
	return result.Mag();
}

int PndRiemannTrackFinder::HitTooClose(std::vector<Int_t> hitsInUse, FairHit* newHit, double threshold)
{
	for (int i = 0; i < hitsInUse.size(); i++){
		FairHit* h1 = fHits[hitsInUse.at(i)];
		if (fVerbose > 2) std::cout << "Point Distance: " << HitDistance(h1, newHit) << std::endl;
		if (fabs(HitDistance(h1, newHit)) < threshold)
			return i;
	}
	return -1;
}

void PndRiemannTrackFinder::PrintTrackCand(TrackCand* cand, bool shortOutput)
{
  unsigned int det, hit;
     if(shortOutput == false) std::cout << "TrackCand: " << cand->getCurv() <<" curv, " //<< " +/- " << cand->getCurvError() <<" curv, "
     									<< cand->getDip() << " +/- " << " dip, " //<< cand->getDipError() << " dip, "
     									<< (int)(cand->inverted()) << " inverted." << "\n";
     for (int i = 0; i < cand->getNHits(); i++){
    	 cand->getHit(i, det, hit);
    	 std::cout << det << "/" << hit << " ";
     }
     std::cout << "\n";
}

TrackCand PndRiemannTrackFinder::CreateOneTrackCand(std::vector<int> tracks)
{
	TrackCand result;
	std::map<std::pair<unsigned int, unsigned int>, int > hits;
	for (int i = 0; i < tracks.size(); i++){
		TrackCand myTrackCand = fTrackCand[tracks[i]];
		unsigned int detId, hitId;
		for (int j = 0; j < myTrackCand.getNHits(); j++){
			myTrackCand.getHit(j, detId, hitId);
			hits[std::pair<unsigned int, unsigned int>(detId, hitId)]++;
		}
	}
	for (std::map<std::pair<unsigned int, unsigned int>, int >::const_iterator it = hits.begin(); it!= hits.end(); it++){
		std::pair<unsigned int, unsigned int> dethit = it->first;
		result.addHit(dethit.first, dethit.second);
	}
	RefitTrackCand(result);
	return result;
}

void PndRiemannTrackFinder::RefitTrackCand(TrackCand& cand)
{
	PndRiemannTrack myTrack;
	unsigned int detId, hitId;
	for (int i = 0; i < cand.getNHits(); i++){
		cand.getHit(i, detId, hitId);
		FairHit* myHit = fHits[fMapIDtoHit[std::pair<unsigned int, unsigned int>(detId, hitId)]];
		PndRiemannHit hit(myHit);
		myTrack.addHit(hit);
	}
	myTrack.refit();
	myTrack.szFit();
	cand.setCurv(1/myTrack.r());
	//cand.setCurvError(myTrack.dR()/(myTrack.r()*myTrack.r()));
	cand.setDip(myTrack.dip());
	//cand.setDipError(myTrack.dDip());
	cand.setInverted(myTrack.sign());
}

