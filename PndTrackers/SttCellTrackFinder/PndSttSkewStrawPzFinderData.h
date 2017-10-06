/*
 * PndSttSkewStrawPzFinderData.h
 *
 *  Created on: Jun 29, 2016
 *      Author: walan603
 */

#ifndef PNDSTTSKEWSTRAWPZFINDERDATA_H
#define PNDSTTSKEWSTRAWPZFINDERDATA_H

// Base Class Headers
#include "TObject.h"
#include "TVector2.h"

#include <iostream>

using namespace std;

class PndSttSkewStrawPzFinderData : public TObject{
public:
	PndSttSkewStrawPzFinderData();
	virtual ~PndSttSkewStrawPzFinderData();

	double getLineIntercept() const {
		return LineIntercept;
	}

	void setLineIntercept(double lineIntercept) {
		LineIntercept = lineIntercept;
	}

	double getLineSlope() const {
		return LineSlope;
	}

	void setLineSlope(double lineSlope) {
		LineSlope = lineSlope;
	}

	const vector<vector<TVector2> >& getPhiPairVector() const {
		return ZPhiPairVector;
	}

	void setPhiPairVector(const vector<vector<TVector2> >& phiPairVector) {
		ZPhiPairVector = phiPairVector;
	}

	const vector<TVector2>& getTrueZPhi() const {
		return TrueZPhi;
	}

	void setTrueZPhi(const vector<TVector2>& trueZPhi) {
		TrueZPhi = trueZPhi;
	}

private:
	unsigned int method; //0 = Line Combi, 1 = Hough transform, 2 = Iterative linear regression
	double LineSlope;
	double LineIntercept;

	vector<vector< TVector2> > ZPhiPairVector;
	vector<TVector2> TrueZPhi;

	ClassDef(PndSttSkewStrawPzFinderData,1)


};

#endif /* PNDTOOLS_STTCELLTRACKFINDER_PNDSTTSKEWSTRAWPZFINDERDATA_H_ */
