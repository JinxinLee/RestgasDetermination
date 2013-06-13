/*
 * ModelMinimizer.h
 *
 *  Created on: Jun 5, 2013
 *      Author: steve
 */

#ifndef MODELMINIMIZER_H_
#define MODELMINIMIZER_H_

#include "ModelControlParameter.h"

#include <tr1/memory>

using std::tr1::shared_ptr;

/**
 * Abstract class for constructing minimizer objects. Concrete implementations
 * (for example ROOTMinimizer using ROOT's Minuit Minimizer) need to derive
 * from this class and implement the #minimize() function that interfaces to
 * the minimizer.
 */
class ModelMinimizer {
protected:
	// control parameter used for the minimization
	ModelControlParameter &control_param;

public:
	ModelMinimizer(ModelControlParameter &control_param_);
	virtual ~ModelMinimizer();

	virtual int minimize() =0;

	int doMinimization();
};

#endif /* MODELMINIMIZER_H_ */
