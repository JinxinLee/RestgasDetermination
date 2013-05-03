/*
 * PndLmdSmearingGaussianModelParametrization1D.cxx
 *
 *  Created on: Jan 22, 2013
 *      Author: steve
 */

#include "PndLmdSmearingGaussianModelParametrization1D.h"

PndLmdSmearingGaussianModelParametrization1D::PndLmdSmearingGaussianModelParametrization1D(
    ModelParSet &model_par_set_) :
    Parametrization(model_par_set_) {
  initParameters();
}

void PndLmdSmearingGaussianModelParametrization1D::initParameters() {
  p_lab = model_par_set.addModelParameter("p_lab");
  p_lab->setSuperior(true);
  gauss_sigma = model_par_set.getModelParameter("gauss_sigma");
  gauss_mean = model_par_set.getModelParameter("gauss_mean");

  // add the lab momentum parameter from outside to the list
  dependency_parameters.push_back(p_lab);

  p_lab->setConnectionTo(gauss_sigma);
  p_lab->setConnectionTo(gauss_mean);
}

PndLmdSmearingGaussianModelParametrization1D::~PndLmdSmearingGaussianModelParametrization1D() {
  // TODO Auto-generated destructor stub
}

void PndLmdSmearingGaussianModelParametrization1D::parametrize() {
  // if we have a single gaussian
  gauss_sigma->setValue(
      0.122 + 1.378 / p_lab->getValue() / p_lab->getValue());
  if(p_lab->getValue() < 2.0)
    gauss_mean->setValue(0.1);
  else
    gauss_mean->setValue(0.0);
  //phi resolution sigma: 1.344e-02 + 0.2944 / pow(parameters[0], 2.0) (only needed for 2d fit)
}
