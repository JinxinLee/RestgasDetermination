/*
 * ExponentialModel1D.cxx
 *
 *  Created on: Apr 5, 2013
 *      Author: steve
 */

#include "ExponentialModel1D.h"
#include "core/ModelPar.h"

#include <cmath>
#include <limits>

ExponentialModel1D::ExponentialModel1D(std::string name_) : Model1D(name_) {
  initModelParameters();
}

ExponentialModel1D::~ExponentialModel1D() {
  // TODO Auto-generated destructor stub
}

void ExponentialModel1D::initModelParameters() {
  amplitude = getModelParameterSet().addModelParameter("exp_amplitude");
  exp_factor = getModelParameterSet().addModelParameter("exp_exp_factor");
}

double ExponentialModel1D::eval(const double *x) const {
  return amplitude->getValue() * exp(exp_factor->getValue() * x[0]);
}

void ExponentialModel1D::updateDomain() {
  if (exp_factor->getValue() > 0.0)
    setDomain(std::log(1e-06 / amplitude->getValue()) / exp_factor->getValue(),
        std::numeric_limits<double>::max());
  else
    setDomain(std::numeric_limits<double>::min(),
        std::log(1e-06 / amplitude->getValue()) / exp_factor->getValue());
}
