/*
 * PndGiBuuDecayerAbs.h
 *
 *  Created on: Aug 7, 2012
 *      Author: stockman
 *      Abstract base class for decaying particles in the PndGiBuuGenerator
 */

#ifndef PNDGIBUUDECAYERABS_H_
#define PNDGIBUUDECAYERABS_H_

#include "PndGiBuuTrack.h"

#include <vector>

class PndGiBuuDecayerAbs {
public:
	PndGiBuuDecayerAbs();
	virtual ~PndGiBuuDecayerAbs();

	virtual std::vector<PndGiBuuTrack> DecayTrack(PndGiBuuTrack track) = 0;

	ClassDef(PndGiBuuDecayerAbs, 1);
};

#endif /* PNDGIBUUDECAYERABS_H_ */
