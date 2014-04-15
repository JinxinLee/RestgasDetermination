/*
 * AdditionModel1D.h
 *
 *  Created on: Apr 10, 2013
 *      Author: steve
 */

#ifndef ADDITIONMODEL1D_H_
#define ADDITIONMODEL1D_H_

#include "core/Model1D.h"

class AdditionModel1D: public Model1D {
private:
  shared_ptr<Model1D> first;
  shared_ptr<Model1D> second;

public:
  AdditionModel1D(std::string name_, shared_ptr<Model1D> first_, shared_ptr<Model1D> second_);
  virtual ~AdditionModel1D();

  void initModelParameters();

  double eval(const double *x) const;

  void updateDomain();
};

#endif /* ADDITIONMODEL1D_H_ */
