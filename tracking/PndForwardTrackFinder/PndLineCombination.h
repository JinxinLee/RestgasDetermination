/*
 * PndLineCombination.h
 *
 *  Created on: Jul 12, 2016
 *      Author: kibellus
 */

#ifndef PNDTOOLS_PNDFORWARDTRACKFINDER_PNDLINECOMBINATION_H_
#define PNDTOOLS_PNDFORWARDTRACKFINDER_PNDLINECOMBINATION_H_

#include "PndLineApproximation.h"


class PndLineCombination {
public:
	PndLineCombination(PndLineApproximation a1,PndLineApproximation *a2,Double_t q)
		: approx1(a1),approx2(a2), quality(q){}
	PndLineCombination(PndLineApproximation a1)
		: approx1(a1),approx2(0), quality(-1){}
	virtual ~PndLineCombination();
	PndLineApproximation getApprox1(){ return approx1;}
	PndLineApproximation *getApprox2(){ return approx2;}
	Double_t getQuality(){
		if(approx2==0) return 99999999999;
		else return quality;
	}
    void setApprox1(PndLineApproximation a1){ approx1 = a1;}
    void setApprox2(PndLineApproximation *a2){approx2 = a2;} //TODO: speicher Freigeben
    void setQuality(Double_t q){quality = q;}
private:
	PndLineApproximation approx1;
	PndLineApproximation *approx2;
	Double_t quality;
};

#endif /* PNDTOOLS_PNDFORWARDTRACKFINDER_PNDLINECOMBINATION_H_ */
