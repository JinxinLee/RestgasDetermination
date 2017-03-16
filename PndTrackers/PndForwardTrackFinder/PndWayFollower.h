/*
 * PndWayFollower.h
 *
 *  Created on: Jul 26, 2016
 *      Author: kibellus
 */

#ifndef PNDTOOLS_PNDFORWARDTRACKFINDER_PNDWAYFOLLOWER_H_
#define PNDTOOLS_PNDFORWARDTRACKFINDER_PNDWAYFOLLOWER_H_

#include <map>
#include <vector>
#include <iostream>
#include "PndFtsExpandedTrackCand.h"
#include "PndLineApproximation.h"
#include "PndFtsLineComparator.h"
#include "PndTrackCollection.h"
#include "PndLine.h"
#include "Tuple.h"
#include "TClonesArray.h"

using namespace std;

class PndWayFollower {
public:
	static Double_t compDist;
	static Double_t compAngle;
	static Double_t qualiT;
	PndWayFollower(TClonesArray *p,TClonesArray *p2) : fLineComp(compDist,compAngle), planes(p),planesHit(p2){}
	virtual ~PndWayFollower();
	vector<PndTrackCollection> followLines(Int_t begin);
	void init(vector<PndLineApproximation> b, map<Int_t, vector<PndFtsExpandedTrackCand>> c){
		fBefore = b;
		cands = c;
	}
	PndLineApproximation getBestApproxTo(PndLine l1, Int_t layer);
	PndLineApproximation getBestSkewedApproxTo(PndLine l2, Int_t layer);
private:
	vector<PndLineApproximation> fBefore;
	map<Int_t, vector<PndFtsExpandedTrackCand>> cands;
	PndFtsLineComparator fLineComp;
	TClonesArray *planes;
	TClonesArray *planesHit;

	int asd = 0;
};

#endif /* PNDTOOLS_PNDFORWARDTRACKFINDER_PNDWAYFOLLOWER_H_ */
