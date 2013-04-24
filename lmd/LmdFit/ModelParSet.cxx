/*
 * ModelParSet.cxx
 *
 *  Created on: Feb 9, 2013
 *      Author: steve
 */

#include "ModelParSet.h"

#include <iostream>

ModelParSet::ModelParSet() {
  // TODO Auto-generated constructor stub
}

ModelParSet::~ModelParSet() {
  // TODO Auto-generated destructor stub
}

unsigned int ModelParSet::getNumberOfParameters() const {
  return model_par_map.size();
}

unsigned int ModelParSet::getNumberOfFreeParameters() const {
  unsigned int nfree = 0;
  for (std::map<std::string, std::shared_ptr<ModelPar>, stringcomp>::const_iterator it =
      model_par_map.begin(); it != model_par_map.end(); it++) {
    if (!it->second->isParameterFixed())
      nfree++;
  }
  return nfree;
}

void ModelParSet::printInfo() const {
  std::cout << "Set contains " << model_par_map.size() << " entries"
      << std::endl;
  int counter = 0;
  std::cout << "************************************************************"
      << std::endl;
  for (std::map<std::string, std::shared_ptr<ModelPar>, stringcomp>::const_iterator it =
      model_par_map.begin(); it != model_par_map.end(); it++) {
    std::cout << "------------------------------------------------------------"
        << std::endl;
    std::cout << "parameter " << counter << "  (" << it->first << ")"
        << std::endl;
    std::cout << "name: " << it->second->getName() << std::endl;
    std::cout << "value: " << it->second->getValue() << std::endl;
    std::cout << "is fixed: " << it->second->isParameterFixed() << std::endl;
    std::cout << "is set: " << it->second->isSet() << std::endl;
    std::cout << "------------------------------------------------------------"
        << std::endl;
    counter++;
  }
  std::cout << "************************************************************"
      << std::endl;
}

const std::shared_ptr<ModelPar> ModelParSet::addModelParameter(
    std::string name_, double value_, bool fixed_) {
  if (modelParameterExists(name_)) {
    std::cout << "Warning: This model parameter " << name_
        << " already exists. Returning existing value reference!" << std::endl;
  } else {
    model_par_map[name_] = std::shared_ptr<ModelPar>(
        new ModelPar(name_, value_, fixed_));
  }
  return model_par_map[name_];
}

void ModelParSet::addModelParameter(std::shared_ptr<ModelPar> model_par) {
  model_par_map[model_par->getName()] = model_par;
}

int ModelParSet::setModelParameterValue(const std::string &name_,
    double value_) {
  if (modelParameterExists(name_)) {
    model_par_map[name_]->setValue(value_);
    return 0;
  } else {
    std::cout << "Error: Parameter " << name_
        << " was not found in the parameter set!" << std::endl;
    return 1;
  }
}

void ModelParSet::reassignParameter(std::shared_ptr<ModelPar> model_par) {
  model_par_map.erase(model_par->getName());
  addModelParameter(model_par);
}

bool ModelParSet::modelParameterExists(
    const std::shared_ptr<ModelPar> &model_par) const {
  return modelParameterExists(model_par->getName());
}

bool ModelParSet::modelParameterExists(const std::string &name_) const {
  if (model_par_map.find(name_) != model_par_map.end())
    return true;
  else {
    return false;
  }
}

int ModelParSet::addModelParameters(ModelParSet &daughter_model_par_set) {
  int num_pars_reassigned = 0;
  // loop over all parameters to be added
  for (std::map<std::string, std::shared_ptr<ModelPar>, stringcomp>::const_iterator it =
      daughter_model_par_set.getModelParameterMap().begin();
      it != daughter_model_par_set.getModelParameterMap().end(); it++) {
    // only superior/global parameters are passed onto the parent models
    if (it->second->isSuperior()) {
      // check if this element already exists in this map and if so
      // take that element and reassign the one which is supposed to be added
      if (modelParameterExists(it->first)) {
        daughter_model_par_set.reassignParameter(model_par_map[it->first]);
        num_pars_reassigned++;
      } else { // otherwise just add it
        addModelParameter(it->second);
      }
    }
  }
  return num_pars_reassigned;
}

const double& ModelParSet::getModelParameterValue(
    const std::string &name_) const {
  return model_par_map.at(name_)->getValue();
}

std::shared_ptr<ModelPar> ModelParSet::getModelParameter(
    const std::string &name_) {
  if (!modelParameterExists(name_)) {
    addModelParameter(name_);
  }
  return model_par_map.at(name_);
}

const std::shared_ptr<ModelPar> ModelParSet::getFreeModelParameter(
    const int index) {
  int counter = 0;
  for (std::map<std::string, std::shared_ptr<ModelPar>, stringcomp>::const_iterator it =
      model_par_map.begin(); it != model_par_map.end(); it++) {
    if (!it->second->isParameterFixed()) {
      counter++;
      if (counter == index) {
        return it->second;
      }
    }
  }
  return model_par_map.end()->second;
}

int ModelParSet::checkParameters() const {
  for (std::map<std::string, std::shared_ptr<ModelPar>, stringcomp>::const_iterator it =
      model_par_map.begin(); it != model_par_map.end(); it++) {
    if (!it->second->isSet()) {
      return 1;
    }
  }
  return 0;
}

bool ModelParSet::checkSuperiorParameters() const {
  for (std::map<std::string, std::shared_ptr<ModelPar>, stringcomp>::const_iterator it =
      model_par_map.begin(); it != model_par_map.end(); it++) {
    if (it->second->isSuperior() && !it->second->isSet())
      return false;
  }
  return true;
}

void ModelParSet::getModelParameters(double *pars) const {
  int counter = 0;
  for (std::map<std::string, std::shared_ptr<ModelPar>, stringcomp>::const_iterator it =
      model_par_map.begin(); it != model_par_map.end(); it++) {
    if (!it->second->isParameterFixed()) {
      pars[counter] = it->second->getValue();
      counter++;
    }
  }
}

void ModelParSet::freeModelParameter(const std::string &name_) {
  if (modelParameterExists(name_)) {
    model_par_map[name_]->setParameterFixed(false);
  }
}

void ModelParSet::freeAllModelParameter() {
  for (std::map<std::string, std::shared_ptr<ModelPar>, stringcomp>::const_iterator it =
      model_par_map.begin(); it != model_par_map.end(); it++) {
    if (!it->second->isSuperior()) {
      it->second->setParameterFixed(false);
    }
  }
}

std::map<std::string, std::shared_ptr<ModelPar>, ModelParSet::stringcomp>& ModelParSet::getModelParameterMap() {
  return model_par_map;
}

void ModelParSet::updateModelParameters(const double *pars) {
  int counter = 0;
  // first overwrite the corresponding parameter values
  // loop over the parameter set and update all the free parameters with these values
  for (std::map<std::string, std::shared_ptr<ModelPar>, stringcomp>::iterator it =
      model_par_map.begin(); it != model_par_map.end(); it++) {
    // if parameter is free
    if (!it->second->isParameterFixed()) {
      it->second->setValue(pars[counter]);
      counter++;
    }
  }
}
