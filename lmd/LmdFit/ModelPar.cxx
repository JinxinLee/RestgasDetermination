/*
 * ModelPar.cxx
 *
 *  Created on: Jan 12, 2013
 *      Author: steve
 */

#include "ModelPar.h"

ModelPar::ModelPar() {
  name = "";
  value = 0.0;
  fixed = true;
}

ModelPar::ModelPar(std::string name_, double value_, bool fixed_) :
    name(name_), value(value_), fixed(fixed_) {
  set = false;
  locked = true; // variable is immediately locked which sounds strange
                 // but it can be set once because it is not set yet
                 // the lock should not be fiddled with and is automatically
                 // opened within the registerUpdater method of #ModelParameterHandler
  superior = false;
}

const std::string& ModelPar::getName() const {
  return name;
}

void ModelPar::setLocked(bool locked_) {
  locked = locked_;
}

bool ModelPar::isParameterFixed() const {
  return fixed;
}

void ModelPar::setParameterFixed(bool fixed_) {
  fixed = fixed_;
}

void ModelPar::setValue(double value_) {
  // there are some requirements to allow the setting of the parameter
  // if it wasn't set at all before...
  if (!set) {
    value = value_;
    set = true;
  } else {
    // usually if its set you don't want it to change again
    // there is an exception:
    // is fixed and is connected to another parameter
    // and that connection partner has changed
    if (!locked) {
      value = value_;
    }
  }
}

const double& ModelPar::getValue() const {
  return value;
}

bool ModelPar::isSet() const {
  return set;
}
bool ModelPar::isConnected() const {
  if (connections.size() > 0)
    return true;
  else
    return false;
}
bool ModelPar::isSuperior() const {
  return superior;
}

void ModelPar::setSuperior(bool superior_) {
  superior = superior_;
}

void ModelPar::setConnectionTo(const std::shared_ptr<ModelPar> &model_par) {
  connections.insert(model_par);
}

std::set<std::shared_ptr<ModelPar> >& ModelPar::getParameterConnections() {
  return connections;
}
