/*
 * NumericConvolutionModel2D.cxx
 *
 *  Created on: Jan 16, 2013
 *      Author: steve
 */

#include "NumericConvolutionModel2D.h"

NumericConvolutionModel2D::NumericConvolutionModel2D(std::string name_,
		shared_ptr<Model2D> first_, shared_ptr<Model2D> second_) :
		Model2D(name_) {
	first = first_;
	second = second_;

	addModelToList(first);
	addModelToList(second);

	//set some properties of the convolution
	divisions = 10000;
}

NumericConvolutionModel2D::~NumericConvolutionModel2D() {
	// TODO Auto-generated destructor stub
}

double NumericConvolutionModel2D::eval(const double *x) const {
	double xx[2], xprime[2], val = 0.0;
	double division_width_var1 = second->getVar1DomainRange() / divisions;
	double range_low_var1 = second->getVar1DomainLowerBound();
	double division_width_var2 = second->getVar2DomainRange() / divisions;
	double range_low_var2 = second->getVar2DomainLowerBound();
	for (unsigned int i = 0; i < divisions; i++) {
		xprime[0] = (division_width_var1 * (1.0 * i + 0.5) - range_low_var1);
		xx[0] = x[0] - xprime[0];
		for (unsigned int j = 0; j < divisions; j++) {
			xprime[1] = (division_width_var2 * (1.0 * j + 0.5) - range_low_var2);
			xx[1] = x[1] - xprime[1];
			val += first->evaluate(xx) * second->evaluate(xprime);
		}
	}
	val = val * division_width_var1 * division_width_var2;
	return val;
}
