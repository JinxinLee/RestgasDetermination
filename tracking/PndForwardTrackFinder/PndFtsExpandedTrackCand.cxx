/*
 * PndFtsExpandedTrackCand.cpp
 *
 *  Created on: Jun 2, 2016
 *      Author: kibellus
 */

#include <PndFtsExpandedTrackCand.h>

PndFtsExpandedTrackCand::~PndFtsExpandedTrackCand() {
	// TODO Auto-generated destructor stub
}

PndFtsExpandedTrackCand::PndFtsExpandedTrackCand(vector<PndFtsHit*> h1, vector<PndFtsHit*> h2, vector<PndLineApproximation> l){
	vector<PndFtsHit*> hits;
	for(size_t i=0;i<h1.size();i++) hits.push_back(h1[i]);
	for(size_t i=0;i<h2.size();i++) hits.push_back(h2[i]);
	for(size_t i=0;i<hits.size();i++){
		fPndTrackCand.AddHit(hits[i]->GetEntryNr(),i);
	}
	fSourceHits = hits;
	fApproximations = l;
}
