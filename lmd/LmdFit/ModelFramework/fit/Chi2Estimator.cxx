/*
 * Chi2Estimator.cxx
 *
 *  Created on: Jun 5, 2013
 *      Author: steve
 */

#include "Chi2Estimator.h"
#include "Model.h"

Chi2Estimator::Chi2Estimator() {
	// TODO Auto-generated constructor stub
	binning_factor = 1.0;
}

Chi2Estimator::~Chi2Estimator() {
	// TODO Auto-generated destructor stub
}

double Chi2Estimator::eval() const {
	//calculate chisquare
	double chisq = 0.0;
	double delta;

	// loop over data
	for (unsigned int i = 0; i < data_points.size(); i++) {
		delta = (data_points[i].z
				- binning_factor * fit_model->evaluate(data_points[i].x))
				/ data_points[i].z_error;
		chisq += delta * delta;
	}
	return chisq;
}
