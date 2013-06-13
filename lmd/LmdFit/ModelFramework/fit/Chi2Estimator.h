/*
 * Chi2Estimator.h
 *
 *  Created on: Jun 5, 2013
 *      Author: steve
 */

#ifndef CHI2ESTIMATOR_H_
#define CHI2ESTIMATOR_H_

#include "ModelEstimator.h"

class Chi2Estimator: public ModelEstimator {
public:
	Chi2Estimator();
	virtual ~Chi2Estimator();

	// the chisquare function
	double eval() const;
};

#endif /* CHI2ESTIMATOR_H_ */
