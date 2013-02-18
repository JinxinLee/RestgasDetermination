#include "PndRiemannTrackFinder.h"
#include <iostream>
#include <math.h>
#include "PndSdsHit.h"

ClassImp(PndRiemannTrackFinder);

PndRiemannTrackFinder::PndRiemannTrackFinder(): fMaxPlaneDist(1), fMaxSZDist(1), fMaxSZChi2(1),
fMinPointDist(1), fUseZeroPos(true), fCurvDiff(0.05), fDipDiff(0.05),fMinNumberOfHits(4), fVerbose(0), fMagField(2.0)
{
	if (fUseZeroPos){
		TVector3 pos(0.0,0.0,0.0);
		TVector3 dpos(0.1,0.1,0.1);
		FairHit* ZeroHit = new FairHit(-1, pos, dpos, -1);
		fHits.push_back(ZeroHit);
		FairLink myID;
		fMapHitToID[fHits.size()-1]=myID;
		fMapIDtoHit[myID] = fHits.size()-1;
	}
}

PndRiemannTrackFinder::~PndRiemannTrackFinder()
{
	if (fUseZeroPos)
		delete(fHits[0]);
}

void PndRiemannTrackFinder::AddHits(std::vector<FairHit*> hits, Int_t branchId)
{
	Int_t startSize = 0; //= fHits.size();
	for (int i = 0; i < hits.size(); i++){
		fHits.push_back(hits[i]);
		FairHit* myHit = fHits[i];
		FairLink myID;
		if (myHit->GetEntryNr().GetIndex() < 0){
			myID = FairLink(branchId, startSize + i);
		}
		else
			myID = myHit->GetEntryNr();
		fMapHitToID[fHits.size()-1]=myID;
		fMapIDtoHit[myID] = fHits.size()-1;
	}
}

void PndRiemannTrackFinder::AddHits(TClonesArray* hits, Int_t branchId)
{
	Int_t startSize = 0;//= fHits.size();
	for (int i = 0; i < hits->GetEntries(); i++){
		FairHit* myHit = (FairHit*)(hits->At(i));
		fHits.push_back(myHit);
		FairLink myID;
		if (myHit->GetEntryNr().GetIndex() < 0){
			myID = FairLink(-1, -1, branchId, startSize + i, 1);
		}
		else
			myID = myHit->GetEntryNr();
		fMapHitToID[fHits.size()-1]=myID;
		fMapIDtoHit[myID] = fHits.size()-1;
	}
}

void PndRiemannTrackFinder::FindTracks()
{
	std::vector<std::vector<Int_t> > Tracks = GetStartTracks();				//Get the possible track seeds
    std::vector<int> tooClose;
	for (int trackId = 0; trackId < Tracks.size(); trackId++){				//Go through all track seeds and search for additional points

		if (Tracks[trackId].size() != 3)
			std::cout << "-E- PndRiemannTrackFinder::FindTracks: Start points: " << Tracks[trackId].size()
			          << " in Track: " << trackId << std::endl;

		std::vector<Int_t> StartTrack = Tracks[trackId];					//Take a start track

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
		int startHit = StartTrack[2];										//StartTrack always has three points, the last is the one with the largest index
		for (int testHit = startHit+1; testHit < fHits.size(); testHit++){
			if (CheckHitDistance(StartTrack[0], testHit)!= true){fHitsTooClose[trackId].push_back(testHit); continue;}
			if (CheckHitDistance(StartTrack[1], testHit)!= true){fHitsTooClose[trackId].push_back(testHit); continue;}
			if (CheckHitDistance(StartTrack[2], testHit)!= true){fHitsTooClose[trackId].push_back(testHit); continue;}

			PndRiemannHit actHit(fHits[testHit], testHit);							//Create a Riemann Hit from a FairHit

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

			if (fVerbose > 0){
				std::cout << "Hits in Track: ";
				for (int i = 0; i < StartTrack.size(); i++)
				{
					std::cout << " " << fMapHitToID[StartTrack[i]];
				}
				TVectorD myOrig = actTrack.orig();
				std::cout << " numHits: " << actTrack.getNumHits() << std::endl;
				std::cout << " curv: " << 1/actTrack.r() << "+/-" << actTrack.dR()/(actTrack.r() * actTrack.r())
						  << " dip: " << actTrack.dip() << "+/-" << actTrack.dDip()
						  << " orig: " << myOrig[0] << "+/-" << actTrack.dX()
						  << " " << myOrig[1] << "+/-" << actTrack.dY() << std::endl;
			}
		}
	}

	for (unsigned int n = 0; n < fTracks.size(); n++){
		PndTrackCand myTrackCand;
		std::vector<PndRiemannHit> TrackHits = fTracks[n].getHits();

		for (unsigned int p = 0; p < TrackHits.size(); p++){
			myTrackCand.AddHit(fMapHitToID[TrackHits[p].hitID()], TrackHits[p].s());
		}
		fTrackCand.push_back(myTrackCand);

		std::pair<double,double> CurvDip(1/fTracks[n].r(),fTracks[n].dip());
		fCurvAndDipOfCand.push_back(CurvDip);
	}


//	MergeTracks();
//	fTrackCand.clear();
//	fTrackCand = fMergedTrackCand;
//	if (fVerbose > 0) {
//		std::cout << "Tracks after merging:" << std::endl;
//		for (int p = 0; p < fTrackCand.size(); p++){
//			fTrackCand[p].Print();
//		}
//	}

}

std::vector< std::vector<Int_t> > PndRiemannTrackFinder::GetStartTracks()
{
	std::vector<Int_t> actCandidates;
	std::vector<std::vector<Int_t> > Tracks;
	std::vector<int> tooCloseFirst;
	std::vector<int> tooCloseSecond;

	if (fHits.size() > 3){
		for (int first = 0; first < fHits.size()-3; first++){				//take three points and create a riemann plane - get first hit
			tooCloseFirst.clear();
			for (int second = first+1; second < fHits.size()-2; second++){// get second hit
				tooCloseSecond.clear();
				if (CheckHitDistance(first, second) != true){ tooCloseFirst.push_back(second); continue;}
				if (CheckHitInSameSensor(first, second) == true) continue;
				for (int third = second+1; third < fHits.size()-1; third++){
					if (fVerbose > 1) std::cout << "Checking Points: " << first << " " << second << " " << third << std::endl;
					if (CheckHitDistance(first, third)!= true){tooCloseFirst.push_back(third); continue;}
					if (CheckHitDistance(second, third)!= true){tooCloseSecond.push_back(third); continue;}///<---------

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
	if (aTrack.szChi2() > fMaxSZChi2 || aTrack.szChi2() < 0){
		if (fVerbose > 1) std::cout << "sz-Fit does not match, Chi2: " << fMaxSZChi2 << std::endl;
		return false;
	}
	return true;
}

bool PndRiemannTrackFinder::CheckZeroPassing(std::vector<int> hitIds, int hit)
{
	int zeroPresent = -1;
	for (int i = 0; i < hitIds.size(); i++){
		if (fMapHitToID[hitIds[i]].GetIndex() < 0){
			zeroPresent = i;
		}
	}
	if (zeroPresent > -1) {
		FairHit* testHit = fHits[hit];
		FairHit* myHit;
		if (zeroPresent + 1 < hitIds.size()) {
			myHit = fHits[hitIds[zeroPresent + 1]];
		} else {
			myHit = fHits[hitIds[zeroPresent - 1]];
		}
		TVector3 point1, point2, result;
		//std::cout << " TestHit: " << *testHit << " BaseHit: " << *myHit << std::endl;
		myHit->Position(point1);
		testHit->Position(point2);
		if ((point1 * point2) < 0){
			//std::cout << "product " << point1 * point2 << " negative: ZeroPassing" << std::endl;
			if (fVerbose > 1) std::cout << "ZeroPassing!" << std::endl;
			return true;
		}
	}
	return false;
}

PndRiemannTrack PndRiemannTrackFinder::CreateRiemannTrack(std::vector<Int_t> aHits)
{
	PndRiemannTrack result;
	for (int i = 0; i < aHits.size(); i++)
	{
		PndRiemannHit hit(fHits[aHits[i]], aHits[i]);
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

	if(((PndSdsHit*)first)->GetSensorID() == ((PndSdsHit*)second)->GetSensorID()){
		std::cout << hit1 << " " << hit2 <<" in Same Sensor: " << ((PndSdsHit*)first)->GetSensorID() << std::endl;
		return true;
	}
	return false;
}

bool PndRiemannTrackFinder::CheckHitInTrack(std::vector<int> hitIds, int hit)
{
	for (int i = 0; i < hitIds.size(); i++){
		if (hitIds[i] == hit){
			if (fVerbose > 1) std::cout << "-I- PndRiemannTrackFinder::CheckHitInTrack - Hit: " << hit << " already in track!" << std::endl;
			return true;
		}
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
		if (fVerbose > 1) std::cout << " SZ Chi2 too big! Cut at: " << fMaxSZChi2<< std::endl;
		return false;
	}
	if (fabs(szDist) > fMaxSZDist){ ////////fabs  added
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
			std::vector<PndTrackCand> tempTrCnd;
			std::vector<int> tempST;
			std::vector<int> tempKillAfter;

			   for(int k=0;k<SelectedTracks.size();k++){
				   tempTrCnd.push_back(fTrackCand[SelectedTracks[k]]);
				   tempST.push_back(k);
			   }
            int tempSize0=tempTrCnd.size();
			int tempSize=tempTrCnd.size();
            bool checkMerge=true;
			while (checkMerge){
				int counter=0;
				for(int j=0;j<tempSize;j++){
					TracksToMerge = FindTracksWithSimilarHits(tempST,tempTrCnd,tempKillAfter);
					selectedTracksSize = SelectedTracks.size();
					PndTrackCand newCand = CreateOneTrackCand(TracksToMerge,tempTrCnd);
					tempTrCnd.push_back(newCand);
					tempST.push_back(tempTrCnd.size()-1);
					if (TracksToMerge.size()>1) counter++;
				}
				if (counter>0)
				checkMerge=true;
				else checkMerge=false;

				for(int k=0;k<tempKillAfter.size();k++){
					for(int i=0;i<tempST.size();i++){
						if (tempST[i]==tempKillAfter[k]){
							RemoveTrack(tempKillAfter[k], tempST);
						}
					}
				}
			}
			for(int k=0;k<tempST.size();k++){
				fMergedTrackCand.push_back(tempTrCnd[tempST[k]]);
			}
		}
		else fMergedTrackCand.push_back(fTrackCand[SelectedTracks[0]]);
	}
}

std::vector<int> PndRiemannTrackFinder::FindTracksWithSimilarParameters(int TrackInd, std::vector<int>& TracksToTest, double curvDiff, double dipDiff)
{
	std::vector<int> result;
	Double_t testCurv=fCurvAndDipOfCand[TrackInd].first;
	Double_t testDip=fCurvAndDipOfCand[TrackInd].second;
	result.push_back(TrackInd);
	RemoveTrack(TrackInd, TracksToTest);
	int tracksToTestSize = TracksToTest.size();
	if(fVerbose > 1){
		std::cout << std::endl;
		std::cout << "TrackInd: " << TrackInd << " tracksToTest: " << tracksToTestSize << std::endl;
	}

	for (int i = 0; i < tracksToTestSize; i++){             ////////////////// !!!!!!!!!!!!!!!!!!!!!!
		Double_t checkCurv=fCurvAndDipOfCand[TracksToTest[i]].first;
		Double_t checkDip=fCurvAndDipOfCand[TracksToTest[i]].second;
		if ((fabs(2*(testCurv - checkCurv)/(testCurv + checkCurv)) < curvDiff) &&
			(fabs(2*(testDip - checkDip)/(testDip + checkDip )) < dipDiff))
		{
			result.push_back(TracksToTest[i]);
			RemoveTrack(TracksToTest[i], TracksToTest);
			tracksToTestSize = TracksToTest.size();
			i--;
		}
	}
	if (fVerbose > 1 && result.size() > 1){
		std::cout << "Tracks with similar parameters: curv: " << testCurv <<
				  " dip: " << testDip << std::endl;
		for(int j = 0; j < result.size();  j++){
			fTrackCand[result[j]].Print();

		}
	}
	return result;
}

std::vector<int> PndRiemannTrackFinder::FindTracksWithSimilarHits(std::vector<int>& TracksToTest,std::vector<PndTrackCand> tempTrCnd, std::vector<int>& tempKillAfter)
{
	std::vector<int> result;
	std::map<std::pair<unsigned int,unsigned int>, int > hitCount;
	int TrackInd = TracksToTest[0];

	PndTrackCand testTrack = tempTrCnd[TrackInd];

	result.push_back(TrackInd);
	RemoveTrack(TrackInd, TracksToTest);
	unsigned int detId, hitId;
	for (int i = 0; i < testTrack.GetNHits(); i++)
	{
		detId=testTrack.GetSortedHit(i).GetDetId();
		hitId=testTrack.GetSortedHit(i).GetHitId();
		std::pair<unsigned int, unsigned int> hitPair(detId, hitId);
		hitCount[hitPair] = 1;
	}
	int tracksToTestSize = TracksToTest.size();
	for (int j = 0; j < tracksToTestSize; j++){
		PndTrackCand myTrack = tempTrCnd[TracksToTest[j]];

		int NHits = 0;
		unsigned int myTrackDetId, myTrackHitId;
		for (int k = 0; k < myTrack.GetNHits(); k++){
			myTrackDetId=myTrack.GetSortedHit(k).GetDetId();
			myTrackHitId=myTrack.GetSortedHit(k).GetHitId();
			std::pair<unsigned int, unsigned int> testPair(myTrackDetId, myTrackHitId);
			if (hitCount[testPair] > 0){
				hitCount[testPair]++;
				NHits++;
			}
		}
		if (NHits > 2){
			result.push_back(TracksToTest[j]);
            tempKillAfter.push_back(TracksToTest[j]);
		}
	}
	if (fVerbose > 1){
		std::cout << "Tracks with similar hits: " << std::endl;
		for (int m = 0; m < result.size(); m++){
			tempTrCnd[result[m]].Print();
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
	bool result = true;
	bool oneNumberEqual = false;

	//if (fVerbose > 2) std::cout << "TrackExists: fHitsInTrack.size: " << fHitsInTracks.size() << std::endl;
	for (int i = 0; (i < fHitsInTracks.size()); i++){														//run through tracks in trackList
		for (int k = 0; (k < hitsInTrack.size()&&(result == true)); k++){									//run through all hits in test track
			for (int j = 0; (j < fHitsInTracks[i].size()) && (oneNumberEqual == false); j++){				//run through all hits in selected track
				//if (fVerbose > 2) std::cout << hitsInTrack[k] << " ?= " << fHitsInTracks[i][j] << std::endl;
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

PndTrackCand PndRiemannTrackFinder::CreateOneTrackCand(std::vector<int> tracks, std::vector<PndTrackCand> tempTrCnd)

{
	PndTrackCand result;

	std::map<std::pair<unsigned int, unsigned int>, int > hits;
	for (int i = 0; i < tracks.size(); i++){
		PndTrackCand myTrackCand = tempTrCnd[tracks[i]];

		unsigned int detId, hitId;
		for (int j = 0; j < myTrackCand.GetNHits(); j++){
			detId=myTrackCand.GetSortedHit(j).GetDetId();
			hitId=myTrackCand.GetSortedHit(j).GetHitId();
			hits[std::pair<unsigned int, unsigned int>(detId, hitId)]++;
		}
	}
	for (std::map<std::pair<unsigned int, unsigned int>, int >::const_iterator it = hits.begin(); it!= hits.end(); it++){
		std::pair<unsigned int, unsigned int> dethit = it->first;
		result.AddHit(dethit.first, dethit.second,0);
	}
	return result;
}

