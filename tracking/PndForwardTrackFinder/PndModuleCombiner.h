/*
 * PndModuleCombiner.h
 *
 *  Created on: Jul 18, 2016
 *      Author: kibellus
 */

#ifndef PNDTOOLS_PNDFORWARDTRACKFINDER_PNDMODULECOMBINER_H_
#define PNDTOOLS_PNDFORWARDTRACKFINDER_PNDMODULECOMBINER_H_

#include <vector>
#include <iostream>

#include "PndLineApproximation.h"
#include "PndLine.h"
#include "PndFtsLineComparator.h"
#include "PndTrackCollection.h"

using namespace std;

class PndModuleCombiner {
public:
	static Double_t compDist;
	static Double_t compAngle;
	static Double_t hitAddT;
	PndModuleCombiner() : fLineComparator(compDist,compAngle){}
	virtual ~PndModuleCombiner();
	vector<PndLineApproximation> combineModules(vector<PndLineApproximation> m1, vector<PndLineApproximation> m2);
	void combineModules(vector<PndTrackCollection> &c,vector<PndLineApproximation> );
	void addUnusedHits(vector<PndFtsHit*> hits,vector<PndTrackCollection> &c);
private:
	PndFtsLineComparator fLineComparator;
};

#endif /* PNDTOOLS_PNDFORWARDTRACKFINDER_PNDMODULECOMBINER_H_ */
