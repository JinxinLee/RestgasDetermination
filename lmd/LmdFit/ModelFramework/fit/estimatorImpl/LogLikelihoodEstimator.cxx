#include "LogLikelihoodEstimator.h"
#include "core/Model.h"
#include "fit/data/Data.h"

#include <cmath>

LogLikelihoodEstimator::LogLikelihoodEstimator() {
	// TODO Auto-generated constructor stub
}

LogLikelihoodEstimator::~LogLikelihoodEstimator() {
	// TODO Auto-generated destructor stub
}

double LogLikelihoodEstimator::eval() const {
	//calculate loglikelihood
	// poisson sum_i(y_i * ln (f(x_i)) - f(x_i))

	double loglikelihood = 0.0;
	double delta;

	std::vector < DataPointProxy > &data_points = data->getData();
	// loop over data
	for (unsigned int i = 0; i < data_points.size(); i++) {
		shared_ptr < DataStructs::binned_data_point > data_point;
		if (data_points[i].isPointUsed()) {
			data_point = data_points[i].getBinnedDataPoint();
			double model_value = data->getBinningFactor()
					* fit_model->evaluate(data_point->bin_center_value);
			// if model is zero at this point should be removed otherwise log(0)!!!
			if(model_value <= 0.0)
				continue;
			delta = model_value - data_point->z * log(model_value);
			loglikelihood += delta;
		}
	}
	return loglikelihood;
}
