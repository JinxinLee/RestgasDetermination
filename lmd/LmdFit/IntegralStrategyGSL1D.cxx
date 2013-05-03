/*
 * IntegralStrategyGSL1D.cxx
 *
 *  Created on: Apr 30, 2013
 *      Author: steve
 */

#include "IntegralStrategyGSL1D.h"

#include <gsl/gsl_integration.h>

Model1D* IntegralStrategyGSL1D::current_model = 0;

IntegralStrategyGSL1D::IntegralStrategyGSL1D() {
	// TODO Auto-generated constructor stub

}

IntegralStrategyGSL1D::~IntegralStrategyGSL1D() {
	// TODO Auto-generated destructor stub
}

double IntegralStrategyGSL1D::Integral(Model1D *model1d, double xlow, double xhigh, double precision) {
	current_model = model1d;
	double result, error;
	size_t neval;
	gsl_function F;
	F.function = &gsl_func_wrapper;
	F.params = 0;
	if (gsl_integration_qng(&F, xlow, xhigh, precision, precision, &result,
			&error, &neval)) {
		// in principle we could do something if we are not precise enough
		// default will be just to return the estimate.
	}
	return result;
}
