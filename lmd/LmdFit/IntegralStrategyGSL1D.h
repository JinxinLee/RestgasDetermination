/*
 * IntegralStrategyGSL1D.h
 *
 *  Created on: Apr 30, 2013
 *      Author: steve
 */

#ifndef INTEGRALSTRATEGYGSL1D_H_
#define INTEGRALSTRATEGYGSL1D_H_

#include "IntegralStrategy1D.h"

#include "Model1D.h"

class IntegralStrategyGSL1D: public IntegralStrategy1D {
private:
	static Model1D *current_model;

  static double gsl_func_wrapper(double x, void *p) {
    return current_model->evaluate(&x);
  }
public:
	IntegralStrategyGSL1D();
	virtual ~IntegralStrategyGSL1D();

  double Integral(Model1D *model1d, double xlow, double xhigh, double precision);
};
#endif /* INTEGRALSTRATEGYGSL1D_H_ */
