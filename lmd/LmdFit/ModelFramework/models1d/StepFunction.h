/*
 * StepFunction.h
 *
 *  Created on: Jun 14, 2013
 *      Author: steve
 */

#ifndef STEPFUNCTION_H_
#define STEPFUNCTION_H_

#include "Model1D.h"

class StepFunction: public Model1D {
private:
  shared_ptr<ModelPar> amplitude;
  shared_ptr<ModelPar> edge;
  bool falling_edge;

public:
	StepFunction(bool falling_edge_);
	virtual ~StepFunction();

  double eval(const double *x) const;

  void initModelParameters();

  void updateDomain();
};

#endif /* STEPFUNCTION_H_ */
