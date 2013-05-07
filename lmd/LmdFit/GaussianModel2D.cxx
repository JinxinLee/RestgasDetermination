/*
 * GaussianModel2D.cxx
 *
 *  Created on: Jan 16, 2013
 *      Author: steve
 */

#include "GaussianModel2D.h"

GaussianModel2D::GaussianModel2D(std::string name_,
		gaussian_model_types_2D type) :
		Model2D(name_) {
	num_sigmas = 5.0;
	model_type = type;

	if (model_type == SINGLE_GAUSSIAN_2D) {
		this(&single_gaussian_2D);
		gauss_var1_sigma = getModelParameterSet().addModelParameter(
				"gauss_var1_sigma");
		gauss_var2_sigma = getModelParameterSet().addModelParameter(
				"gauss_var2_sigma");

	} else if (model_type == DOUBLE_GAUSSIAN_2D) {
		this(&double_gaussian_2D);
	} else { // should never happen
		std::cout << "ERROR: The requested smearing model " << model_type
				<< " does not exist!" << std::endl;
	}
}

GaussianModel2D::~GaussianModel2D() {
	// TODO Auto-generated destructor stub
}

void GaussianModel2D::updateDomain() {
	if (model_type == SINGLE_GAUSSIAN_2D) {
		double temp = num_sigmas * gauss_var1_sigma / 2.0;
		setVar1Domain(-temp, temp);
		temp = num_sigmas * gauss_var2_sigma / 2.0;
		setVar2Domain(-temp, temp);
	} else if (model_type == DOUBLE_GAUSSIAN_2D) {
		// change this.... stuff
	}
}

double GaussianModel2D::single_gaussian_2D(double *x) const {
	return exp(
			-(x[0] * x[0] / (2.0 * gauss_var1_sigma * gauss_var1_sigma)
					+ x[1] * x[1] / (2.0 * gauss_var2_sigma * gauss_var2_sigma)))
			/ (gauss_var1_sigma * gauss_var2_sigma * 2.0 * C_PI);
}

double GaussianModel2D::double_gaussian_2D(double *x) const {
	// double gauss smearing function
	/*return par[0] / (par[0] + 1.0) / (par[1] * sqrt(2.0 * C_PI))
	 * exp(-(x[0] * x[0] / (2.0 * par[1] * par[1])))
	 + 1.0 / (par[0] + 1.0) / (par[2] * sqrt(2.0 * C_PI))
	 * exp(-(x[0] * x[0] / (2.0 * par[2] * par[2])));*/
	return 0;
}
