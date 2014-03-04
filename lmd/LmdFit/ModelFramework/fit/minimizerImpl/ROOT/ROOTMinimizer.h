/*
 * ROOTMinimizer.h
 *
 *  Created on: Jan 21, 2013
 *      Author: steve
 */

#ifndef ROOTMINIMIZER_H_
#define ROOTMINIMIZER_H_

#include "ModelMinimizer.h"
#include "ModelFitResult.h"

#include "Math/Minimizer.h"

class ROOTMinimizer: public ModelMinimizer {
private:
	ROOT::Math::Minimizer* min;

	int minimize();

public:
	ROOTMinimizer();
	virtual ~ROOTMinimizer();

	const ROOT::Math::Minimizer* getROOTMinimizer() const;

	virtual ModelFitResult createModelFitResult() const;
};

#endif /* ROOTMINIMIZER_H_ */
