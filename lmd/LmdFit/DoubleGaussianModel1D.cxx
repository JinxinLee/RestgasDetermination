/*
 * DoubleGaussianModel1D.cxx
 *
 *  Created on: Feb 19, 2013
 *      Author: steve
 */

#include "DoubleGaussianModel1D.h"
#include "PndLmdConstants.h"

#include <cmath>
#include <iostream>
DoubleGaussianModel1D::DoubleGaussianModel1D() {
  num_sigmas = 5.0;
  initModelParameters();
}

DoubleGaussianModel1D::~DoubleGaussianModel1D() {
  // TODO Auto-generated destructor stub
}

void DoubleGaussianModel1D::initModelParameters() {
  gauss_mean = getModelParameterSet().addModelParameter("gauss_mean");
  gauss_sigma_1 = getModelParameterSet().addModelParameter("gauss_sigma_1");
  gauss_sigma_2 = getModelParameterSet().addModelParameter("gauss_sigma_2");
  gauss_ratio_12 = getModelParameterSet().addModelParameter("gauss_ratio_12");
}

double DoubleGaussianModel1D::eval(double *x) const {
  // double gauss smearing function
  return gauss_ratio_12->getValue() / (gauss_ratio_12->getValue() + 1.0)
      / (gauss_sigma_1->getValue() * sqrt(2.0 * C_PI))
      * exp(
          -(pow(x[0] - gauss_mean->getValue(), 2.0)
              / (2.0 * gauss_sigma_1->getValue() * gauss_sigma_1->getValue())))
      + 1.0 / (gauss_ratio_12->getValue() + 1.0)
          / (gauss_sigma_2->getValue() * sqrt(2.0 * C_PI))
          * exp(
              -(pow(x[0] - gauss_mean->getValue(), 2.0)
                  / (2.0 * gauss_sigma_2->getValue() * gauss_sigma_2->getValue())));
}

void DoubleGaussianModel1D::updateDomain() {
  double temp = num_sigmas * gauss_sigma_1->getValue();
  if (gauss_sigma_1->getValue() < gauss_sigma_2->getValue())
    temp = num_sigmas * gauss_sigma_2->getValue();
  setDomain(-temp + gauss_mean->getValue(), temp + gauss_mean->getValue());
}
