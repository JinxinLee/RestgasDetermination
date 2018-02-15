/*
 * PndGiBuuJPsiEE.h
 *
 *  Created on: Aug 9, 2012
 *      Author: stockman
 */

#ifndef PNDGIBUUJPSIEE_H_
#define PNDGIBUUJPSIEE_H_

#include <PndGiBuuDecayerAbs.h>

class PndGiBuuJPsiEE: public PndGiBuuDecayerAbs {
public:
	PndGiBuuJPsiEE();
	virtual ~PndGiBuuJPsiEE();

	virtual std::vector<PndGiBuuTrack> DecayTrack(PndGiBuuTrack track);

ClassDef(PndGiBuuJPsiEE, 1);
};

#endif /* PNDGIBUUJPSIEE_H_ */
