/*
 * NumericConvolutionModel1D.h
 *
 *  Created on: Jan 11, 2013
 *      Author: steve
 */

#ifndef NUMERICCONVOLUTIONMODEL1D_H_
#define NUMERICCONVOLUTIONMODEL1D_H_

#include "Model1D.h"

class NumericConvolutionModel1D: public Model1D {
private:
	unsigned int divisions;

	shared_ptr<Model1D> first, second;
public:
	NumericConvolutionModel1D(std::string name_, shared_ptr<Model1D> first_,
			shared_ptr<Model1D> second_);

	void initModelParameters();

	double eval(const double *x) const;

	void updateDomain();
};

#endif /* NUMERICCONVOLUTIONMODEL1D_H_ */
