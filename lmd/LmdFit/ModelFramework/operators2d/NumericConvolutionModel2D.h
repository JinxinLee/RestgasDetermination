/*
 * NumericConvolutionModel2D.h
 *
 *  Created on: Jan 16, 2013
 *      Author: steve
 */

#ifndef NUMERICCONVOLUTIONMODEL2D_H_
#define NUMERICCONVOLUTIONMODEL2D_H_

#include "Model2D.h"

class NumericConvolutionModel2D: public Model2D {
private:
	unsigned int divisions;

	shared_ptr<Model2D> first, second;
public:
	NumericConvolutionModel2D(std::string name_, shared_ptr<Model2D> first_,
			shared_ptr<Model2D> second_);
	virtual ~NumericConvolutionModel2D();

	double eval(const double *x) const;
};

#endif /* NUMERICCONVOLUTIONMODEL2D_H_ */
