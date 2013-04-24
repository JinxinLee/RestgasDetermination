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

    std::shared_ptr<Model2D> first, second;
public:
	NumericConvolutionModel2D(std::shared_ptr<Model2D> first_, std::shared_ptr<Model2D> second_);
	virtual ~NumericConvolutionModel2D();

	double eval(double *x) const;
};

#endif /* NUMERICCONVOLUTIONMODEL2D_H_ */
