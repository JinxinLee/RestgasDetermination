/*
 * Tuple.h
 *
 *  Created on: Jun 2, 2016
 *      Author: kibellus
 */
#include <iostream>
#include "PndLineApproximation.h"

#ifndef PNDTOOLS_PNDFORWARDTRACKFINDER_TUPLE_H_
#define PNDTOOLS_PNDFORWARDTRACKFINDER_TUPLE_H_

class Tuple {
public:
	Tuple(PndLineApproximation xp, Int_t yp) : x(xp),y(yp){}
	virtual ~Tuple();

	PndLineApproximation x;
	Int_t y;
};

#endif /* PNDTOOLS_PNDFORWARDTRACKFINDER_TUPLE_H_ */
