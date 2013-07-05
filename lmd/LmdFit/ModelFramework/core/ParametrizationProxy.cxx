/*
 * ParametrizationProxy.cxx
 *
 *  Created on: Apr 4, 2013
 *      Author: steve
 */

#include "ParametrizationProxy.h"

ParametrizationProxy::ParametrizationProxy() {
  // TODO Auto-generated constructor stub
  state = NONE;
}

ParametrizationProxy::~ParametrizationProxy() {
  // TODO Auto-generated destructor stub
}

bool ParametrizationProxy::hasParametrization() const {
  if(state == PARAMETRIZATION)
    return true;
  return false;
}

bool ParametrizationProxy::hasParametrizationModel() const {
  if(state == PARAMETRIZATION_MODEL)
    return true;
  return false;
}

void ParametrizationProxy::setParametrization(shared_ptr<Parametrization> parametrization_) {
  parametrization_model.reset();
  parametrization = parametrization_;
  state = PARAMETRIZATION;
}

void ParametrizationProxy::setParametrizationModel(shared_ptr<ParametrizationModel> parametrization_model_) {
  parametrization.reset();
  parametrization_model = parametrization_model_;
  state = PARAMETRIZATION_MODEL;
}

const shared_ptr<Parametrization>& ParametrizationProxy::getParametrization() const {
  return parametrization;
}

const shared_ptr<ParametrizationModel>& ParametrizationProxy::getParametrizationModel() const {
  return parametrization_model;
}
