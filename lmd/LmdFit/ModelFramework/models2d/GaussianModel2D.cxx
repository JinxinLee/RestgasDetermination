/*
 * GaussianModel2D.cxx
 *
 *  Created on: Jan 16, 2013
 *      Author: steve
 */

#include "GaussianModel2D.h"

#include <cmath>

GaussianModel2D::GaussianModel2D(std::string name_) :
		Model2D(name_) {
	initModelParameters();
	num_sigmas = 5.0;
}

GaussianModel2D::~GaussianModel2D() {
	// TODO Auto-generated destructor stub
}

void GaussianModel2D::initModelParameters() {
	gauss_sigma_var1 = getModelParameterSet().addModelParameter(
			"gauss_sigma_var1");
	gauss_sigma_var2 = getModelParameterSet().addModelParameter(
			"gauss_sigma_var2");
	gauss_mean_var1 = getModelParameterSet().addModelParameter("gauss_mean_var1");
	gauss_mean_var2 = getModelParameterSet().addModelParameter("gauss_mean_var2");
	// as default the amplitude of the gauss is set to 1,
	// which makes it a normal distribution
	gauss_amplitude = getModelParameterSet().addModelParameter("gauss_amplitude");
	gauss_amplitude->setValue(1.0);
	gauss_amplitude->setParameterFixed(true);
}

double GaussianModel2D::eval(const double *x) const {
	return gauss_amplitude->getValue()
			* exp(
					-(pow(x[0] - gauss_mean_var1->getValue(), 2.0)
							/ (2.0 * gauss_sigma_var1->getValue()
									* gauss_sigma_var1->getValue())
							+ pow(x[0] - gauss_mean_var2->getValue(), 2.0)
									/ (2.0 * gauss_sigma_var2->getValue()
											* gauss_sigma_var2->getValue())))
			/ (gauss_sigma_var1->getValue() * gauss_sigma_var2->getValue() * 2.0
					* M_PI);
}

void GaussianModel2D::updateDomain() {
	double temp = num_sigmas * gauss_sigma_var1->getValue();
	setVar1Domain(-temp + gauss_mean_var1->getValue(),
			temp + gauss_mean_var1->getValue());
	temp = num_sigmas * gauss_sigma_var2->getValue();
	setVar2Domain(-temp + gauss_mean_var2->getValue(),
			temp + gauss_mean_var2->getValue());
}

