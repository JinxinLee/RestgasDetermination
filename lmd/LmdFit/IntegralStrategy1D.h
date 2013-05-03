/*
 * IntegralStrategy1D.h
 *
 *  Created on: Apr 30, 2013
 *      Author: steve
 */

#ifndef INTEGRALSTRATEGY1D_H_
#define INTEGRALSTRATEGY1D_H_

class Model1D;

class IntegralStrategy1D {
public:
	IntegralStrategy1D();
	virtual ~IntegralStrategy1D();

	virtual double Integral(Model1D *model1d, double xlow, double xhigh,
			double precision) =0;
};

#endif /* INTEGRALSTRATEGY1D_H_ */
