#ifndef UNBINNEDLOGLIKELIHOODESTIMATOR_H_
#define UNBINNEDLOGLIKELIHOODESTIMATOR_H_

#include "fit/ModelEstimator.h"

class UnbinnedLogLikelihoodEstimator: public ModelEstimator {
private:
	double last_integral;
	bool skip_integral;

	int integral_skip_counter;
	int integral_skip_amount;
public:
	UnbinnedLogLikelihoodEstimator();
	virtual ~UnbinnedLogLikelihoodEstimator();

	// the likelihood function
	double eval();
};

#endif /* UNBINNEDLOGLIKELIHOODESTIMATOR_H_ */
