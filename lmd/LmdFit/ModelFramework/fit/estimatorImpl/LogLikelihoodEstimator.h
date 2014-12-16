#ifndef LOGLIKELIHOODESTIMATOR_H_
#define LOGLIKELIHOODESTIMATOR_H_

#include "fit/ModelEstimator.h"

class LogLikelihoodEstimator: public ModelEstimator {
public:
		LogLikelihoodEstimator();
	virtual ~LogLikelihoodEstimator();

	// the likelihood function
	double eval();
};

#endif /* LOGLIKELIHOODESTIMATOR_H_ */
