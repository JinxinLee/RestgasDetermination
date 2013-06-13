/*
 * GaussianModel.cxx
 *
 *  Created on: Dec 17, 2012
 *      Author: steve
 */

#include "GaussianModel1D.h"

#define _USE_MATH_DEFINES
#include "math.h"

GaussianModel1D::GaussianModel1D(std::string name_) :
		Model1D(name_) {
	initModelParameters();
	num_sigmas = 5.0;
}

GaussianModel1D::~GaussianModel1D() {

}

void GaussianModel1D::initModelParameters() {
	gauss_sigma = getModelParameterSet().addModelParameter("gauss_sigma");
	gauss_mean = getModelParameterSet().addModelParameter("gauss_mean");
	// as default the amplitude of the gauss is set to 1,
	// which makes it a normal distribution
	gauss_amplitude = getModelParameterSet().addModelParameter("gauss_amplitude");
	gauss_amplitude->setValue(1.0);
	gauss_amplitude->setParameterFixed(true);
}

double GaussianModel1D::eval(const double *x) const {
	// single gauss smearing function
	/*std::cout << "x = " << x[0] << " -> " << gauss_mean->getValue() << " "
	 << gauss_sigma->getValue() << std::endl;*/
	return gauss_amplitude->getValue()
			* exp(
					-pow(x[0] - gauss_mean->getValue(), 2.0)
							/ (2.0 * gauss_sigma->getValue() * gauss_sigma->getValue()))
			/ (gauss_sigma->getValue() * sqrt(2.0 * M_PI));
}

void GaussianModel1D::updateDomain() {
	double temp = num_sigmas * gauss_sigma->getValue();
	setDomain(-temp + gauss_mean->getValue(), temp + gauss_mean->getValue());
}
