/*
 * AdditionModel1D.cxx
 *
 *  Created on: Apr 10, 2013
 *      Author: steve
 */

#include "AdditionModel1D.h"

#include <iostream>

AdditionModel1D::AdditionModel1D(shared_ptr<Model1D> first_,
    shared_ptr<Model1D> second_) {
  first = first_;
  second = second_;

  addModelToList(first);
  addModelToList(second);
}

AdditionModel1D::~AdditionModel1D() {
  // TODO Auto-generated destructor stub
}

void AdditionModel1D::initModelParameters() {

}

double AdditionModel1D::eval(double *x) const {
  return add(first, second, x);
}

void AdditionModel1D::updateDomain() {
  // first we need to check if user defined a domain for his models
  if (first->getDomainRange() == 0 || second->getDomainRange() == 0) {
    std::cout << "Warning: Some of the models used for the addition have not"
        " defined any domains!" << std::endl;
  } else {
    setDomain(std::min(first->getDomain().first, second->getDomain().first),
        std::max(first->getDomain().second, second->getDomain().second));
  }
}
