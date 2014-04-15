/*
 * IntegralStrategyGSL1D.h
 *
 *  Created on: Apr 30, 2013
 *      Author: steve
 */

#ifndef INTEGRALSTRATEGYGSL1D_H_
#define INTEGRALSTRATEGYGSL1D_H_

#include "IntegralStrategy1D.h"

#include "core/Model1D.h"
#include <iostream>

class IntegralStrategyGSL1D: public IntegralStrategy1D {
private:
	static Model1D *current_model;

  static double gsl_func_wrapper(double x, void *p) {
  	Model1D *current_model_temp = current_model;
  	double value = current_model->eval(&x);
  	current_model = current_model_temp;
    return value;
  }
public:
	IntegralStrategyGSL1D();
	virtual ~IntegralStrategyGSL1D();

  double Integral(Model1D *model1d, double xlow, double xhigh, double precision);
};
#endif /* INTEGRALSTRATEGYGSL1D_H_ */
