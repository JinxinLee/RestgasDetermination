/*
 * GaussianModel2D.h
 *
 *  Created on: Jan 16, 2013
 *      Author: steve
 */

#ifndef GAUSSIANMODEL2D_H_
#define GAUSSIANMODEL2D_H_

#include "MultiModel2D.h"

class GaussianModel2D: public MultiModel2D {
private:
	enum gaussian_model_types_2D {
		SINGLE_GAUSSIAN_2D,
		DOUBLE_GAUSSIAN_2D
	} model_type;

	double num_sigmas;
	double &gauss_var1_sigma;
	double &gauss_var2_sigma;

public:
	GaussianModel2D(gaussian_model_types_2D type);
	virtual ~GaussianModel2D();

	void updateDomain();

	/**
	 * normalized detector response function for 2D fits. Here: 2D-gaussian
	 * @params x pointer to array containing theta and phi value
	 * @returns value of the response function at the specified theta and phi value with the given theta and phi sigma
	 */
	double single_gaussian_2D(double *x) const;

	double double_gaussian_2D(double *x) const;
};

#endif /* GAUSSIANMODEL2D_H_ */
