#include "PndRiemannTrackFinder.h"
#include <iostream>
#include <math.h>
#include "../../mvd/MvdData/PndMvdHit.h"

ClassImp(PndRiemannTrackFinder);

PndRiemannTrackFinder::PndRiemannTrackFinder(): fMaxPlaneDist(0.7), fMaxSZDist(1), fMaxSZChi2(1),
fMinPointDist(0.5), fMaxTheta(0.4), fMaxPhi(0.4), fUseZeroPos(true), fVerbose(1)
{
	if (fUseZeroPos){
		TVector3 pos(0.0,0.0,0.0);
		TVector3 dpos(0.1,0.1,0.1);
		PndMvdHit* ZeroHit = new PndMvdHit(1, "", pos, dpos, 0, 0, 0);
		fHits.push_back(ZeroHit);
	}
	fHitsTooClose.resize(3,-1);
}

PndRiemannTrackFinder::~PndRiemannTrackFinder()
{
	if (fUseZeroPos)
		delete(fHits[0]);
}

void PndRiemannTrackFinder::AddHits(std::vector<CbmHit*> hits)
{
	for (int i = 0; i < hits.size(); i++){
		fHits.push_back(hits[i]);
	}
}

void PndRiemannTrackFinder::AddHits(TClonesArray* hits)
{
	for (int i = 0; i < hits->GetEntries(); i++){
		CbmHit* myHit = (CbmHit*)(hits->At(i));
		fHits.push_back(myHit);
		std::pair<int,int> myID(myHit->GetDetectorID(), i);
		fMapHitToID[fHits.size()-1]=myID;
	}
}

void PndRiemannTrackFinder::FindTracks(double maxdist, double maxszdist)
{
	fMaxPlaneDist = maxdist;
	fMaxSZDist = maxszdist;
	
//	PndRiemannTrack* actTrack;
	Int_t hitsInTrack = 0;
	PndRiemannHit zeroPos;
	zeroPos.setXYZ(0,0,0);
	zeroPos.setDXYZ(0.1,0.1,0.1);
	std::vector<Int_t> hitsTooClose;
	
	TVector3 vec1, vec2, vec3;
	
	fHitsTooClose.resize(3,-1);
	
	if (fVerbose > 0) std::cout << "fHits.size(): " << fHits.size() << std::endl;
	std::vector<Int_t> actCandidates;
	
	if (fHits.size() > 4){
		for (int l = 0; l < fHits.size()-3; l++){				//take three points and create a riemann plane
			for (int i = l+1; i < fHits.size()-2; i++){
				
				if (HitDistance(fHits[l], fHits[i]) < fMinPointDist)
				{
					if (fVerbose > 0)
						std::cout << "Hits: " << l << " " << i << " too close" << std::endl;
					if (fVerbose > 1)
						std::cout << "HitsTooClose at " << 0 << " set to: "	<< i << std::endl;

					fHitsTooClose[0] = i;
					continue;
				}
				fHits[l]->Position(vec1);
				fHits[i]->Position(vec2);
				if (l > 0){
					if (fabs(vec1.Theta()-vec2.Theta()) > fMaxTheta && fabs(vec1.Phi()-vec2.Phi()) > fMaxPhi) continue;
				}
				for (int j = i+1; j < fHits.size()-1; j++){
					if (fVerbose > 0) std::cout << "Checking Points: " << l << " " << i << " " << j << std::endl;
					fHits[j]->Position(vec3);
					if (fabs(vec3.Theta()-vec2.Theta()) > fMaxTheta && fabs(vec3.Phi()-vec2.Phi()) > fMaxPhi){
						if (fVerbose > 0){
							std::cout << "Theta: " << vec3.Theta() - vec2.Theta() << " > max: " << fMaxTheta << " or ";
							std::cout << "Phi: " << vec3.Phi() - vec2.Phi() << " > max: " << fMaxPhi << std::endl;
						}
						continue;
					}
					actCandidates.clear();
					actCandidates.push_back(l);
					actCandidates.push_back(i);				
					
					int tooCloseIndex = HitTooClose(actCandidates, fHits[j], fMinPointDist);
					if (tooCloseIndex > -1 && tooCloseIndex < 3){
						fHitsTooClose[tooCloseIndex]=j;
						if (fVerbose > 0) std::cout << "Hit: " << j << " too close to hit: " << tooCloseIndex << std::endl;
						if (fVerbose > 1) std::cout << "HitsTooClose at " << tooCloseIndex << " set to: " << j << std::endl;
														
						continue;
					}
					actCandidates.push_back(j);
					
					if (!TrackExists(actCandidates)){			//checks if the combination of three points was used before in a track
						PndRiemannTrack actTrack;// = new PndRiemannTrack();
						PndRiemannHit hit1(fHits[l]);
						PndRiemannHit hit2(fHits[i]);
						PndRiemannHit hit3(fHits[j]);
						actTrack.addHit(&hit1);
						actTrack.addHit(&hit2);
						actTrack.addHit(&hit3);
						actTrack.refit();
						TVectorT<double> orig = actTrack.orig();
						if (fVerbose > 0) std::cout << "Base plane from Points: " << l << " " << i << " " << j << " r: " << actTrack.r() << " orig: " << orig[0] << " " << orig[1] << std::endl;
						actTrack.szFit();
						if (actTrack.szChi2() > fMaxSZChi2){
							if (fVerbose > 0) std::cout << "sz-Fit does not match, Chi2: " << actTrack.szChi2() << std::endl;
							//delete(actTrack);
							continue;
						}
						for (int k = j+1; k < fHits.size(); k++){
							tooCloseIndex = HitTooClose(actCandidates, fHits[k], fMinPointDist);
							if ( tooCloseIndex > -1 && tooCloseIndex < 3){
								if (fVerbose > 0) std::cout << "Hit: " << k << " too close to hit: " << fHitsTooClose[tooCloseIndex] << std::endl;
								fHitsTooClose[tooCloseIndex] = k;
								if (fVerbose > 1) std::cout << "HitsTooClose at " << tooCloseIndex << " set to: " << k << std::endl;
								//delete(actTrack);
								continue;
							}
							PndRiemannHit actHit(fHits[k]);
							double szDist = actTrack.szDist(&actHit);
							//double szError = actTrack.szError(&actHit);
							actTrack.szFit(&actHit);
							double szChi2 = actTrack.szChi2();
							
							if (fVerbose > 0) std::cout << "Point " << k << ": dist " << actTrack.dist(&actHit) << " szDist " << szDist << " szChi2 " << szChi2 << std::endl;
							if (fVerbose > 0){
								bool reject = false;
								if (fabs(actTrack.dist(&actHit)) > fMaxPlaneDist){
									std::cout << "dist larger than " << fMaxPlaneDist;
									reject = true;
								}
								if (szChi2 > fMaxSZChi2){
									std::cout << " SZ Chi2 too big! Cut at: " << fMaxSZChi2;		
									reject = true;
								}
								if (reject) std::cout << std::endl;
							}
							//if (fVerbose > 0) std::cout << "sz-error: " << szError << std::endl;
							if ((fabs(actTrack.dist(&actHit)) < fMaxPlaneDist) && (szChi2 < fMaxSZChi2)){
								actTrack.addHit(&actHit);
								actTrack.refit();
								orig = actTrack.orig();
								actCandidates.push_back(k);
								//SetHitInTrack(k, hitsInTrack);
								if (fVerbose > 0) std::cout << "actHit added: " << k  << " r: " << actTrack.r() << " orig: " << orig[0] << " " << orig[1] << std::endl;
							}
						}
						if (actTrack.getNumHits() > 3)
						{
							for (int m = 0; m < fHitsTooClose.size(); m++){
								if (fHitsTooClose[m] > -1){
									if (fVerbose > 0) std::cout << "HitsTooClose Array " << m << ": " << fHitsTooClose[m] << std::endl;
									PndRiemannHit actHit2(fHits[fHitsTooClose[m]]);
									actTrack.szFit(&actHit2);
									double szChi2 = actTrack.szChi2();
									if (fVerbose > 0) std::cout << "HitsTooClose Point: " << fHitsTooClose[m] << ": dist " << actTrack.dist(&actHit2) << " szDist " << actTrack.szDist(&actHit2) << " szChi2 " << szChi2 << std::endl;
																
									if ((fabs(actTrack.dist(&actHit2)) < fMaxPlaneDist) && (szChi2 < fMaxSZChi2)){
										actTrack.addHit(&actHit2);
										actTrack.refit();
										orig = actTrack.orig();
										actCandidates.push_back(fHitsTooClose[m]);
										if (fVerbose > 0) std::cout << "TooCloseHit added: " << fHitsTooClose[m] << " at index: " << m << std::endl;
									}
								}
								
							}
							actTrack.refit();
							actTrack.szFit();
							fTracks.push_back(actTrack);
							fHitsInTracks.push_back(actCandidates);
							if (fVerbose > 0) std::cout << "Track added! " << l << " " << i
									<< " " << j << " r: " << actTrack.r()
									<< " orig: " << orig[0] << " " << orig[1]
									<< " sz-m: " << actTrack.getSZm() << " sz-t: " << actTrack.getSZt()
									<< " dip: " << actTrack.dip()
									<< std::endl;

							if (fVerbose > -1) std::cout << "Hits in Track: ";
							for (int i = 0; i < actCandidates.size(); i++)
							{
								if (fVerbose > -1)
									std::cout << " " << fMapHitToID[actCandidates[i]].first << "/" << fMapHitToID[actCandidates[i]].second;
									//std::cout << " " << actCandidates[i];
							}
							if (fVerbose > -1) std::cout << std::endl;
						}
						//delete(actTrack);
					}
					hitsInTrack = 0;
					fHitsTooClose[2] = -1;
				}
				fHitsTooClose[1] = -1;
			}
			fHitsTooClose[0] = -1;
		}	
	}
	for (int n = 0; n < fHitsInTracks.size(); n++){
//		fTracks[n].szfit();
		TrackCand myTrackCand;
		myTrackCand.setCurv(1/fTracks[n].r());
		myTrackCand.setDip(fTracks[n].dip());
		myTrackCand.setInverted(fTracks[n].sign());
		for (int o = 0; o < fHitsInTracks[n].size(); o++){
			myTrackCand.addHit(fMapHitToID[fHitsInTracks[n][o]].first, fMapHitToID[fHitsInTracks[n][o]].second);
		}
		fTrackCand.push_back(myTrackCand);
	}
}

/*void PndRiemannTrackFinder::SetHitInTrack(Int_t position, Int_t& value)
{
	if (position < 32)
		value |= (1 << position);
	else if (fVerbose > 0) std::cout << "-E- PndRiemannTrackFinder::SetHitInTrack: position to long: " << position << std::endl;
			
}*/

bool PndRiemannTrackFinder::TrackExists(std::vector<Int_t> hitsInTrack){
	//if (fVerbose > 0) std::cout << "TrackExists: hitsInTrack " << hitsInTrack << std::endl;
	bool result = true;
	bool oneNumberEqual = false;
	
	if (fVerbose > 1) std::cout << "TrackExists: fHitsInTrack.size: " << fHitsInTracks.size() << std::endl;
	for (int i = 0; (i < fHitsInTracks.size()); i++){														//run through tracks in trackList
		for (int k = 0; (k < hitsInTrack.size()&&(result == true)); k++){									//run through all hits in test track	
			for (int j = 0; (j < fHitsInTracks[i].size()) && (oneNumberEqual == false); j++){				//run through all hits in selected track
				if (fVerbose > 1) std::cout << hitsInTrack[k] << " ?= " << fHitsInTracks[i][j] << std::endl;
				if (hitsInTrack[k] == fHitsInTracks[i][j])
					oneNumberEqual = true;
				else oneNumberEqual = false;
			}
			result &= oneNumberEqual;
			oneNumberEqual = false;
		}
		if (result == true) {
			if (fVerbose > 0) std::cout << "Track exists already!" << std::endl;
			return true;
		}
		result = true;
	}
	return false;
}

double PndRiemannTrackFinder::HitDistance(CbmHit* h1, CbmHit* h2)
{
	TVector3 vH1, vH2, result;
	h1->Position(vH1);
	h2->Position(vH2);
	
	result = vH1 -vH2;
	return result.Mag();
}

int PndRiemannTrackFinder::HitTooClose(std::vector<Int_t> hitsInUse, CbmHit* newHit, double threshold)
{
	for (int i = 0; i < hitsInUse.size(); i++){
		CbmHit* h1 = fHits[hitsInUse.at(i)];
		if (fVerbose > 1) std::cout << "Point Distance: " << HitDistance(h1, newHit) << std::endl;
		if (fabs(HitDistance(h1, newHit)) < threshold)
			return i;
	}
	return -1;
}
/*std::vector<int> PndRiemannTrackFinder::GetHitsInTrack(Int_t value)
{
	int bit = 0;
	std::vector<int> result;
	for (int i = 0 ; i < 32; i++){
		bit = ((value >> i) & 1);
		if (bit == 1){
			result.push_back(i);
		}
	}
	return result;
}*/
