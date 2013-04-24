/*
 * NumericConvolutionModel1D.h
 *
 *  Created on: Jan 11, 2013
 *      Author: steve
 */

#ifndef NUMERICCONVOLUTIONMODEL1D_H_
#define NUMERICCONVOLUTIONMODEL1D_H_

#include "Model1D.h"

class NumericConvolutionModel1D : public Model1D {
private:
	unsigned int divisions;

	std::shared_ptr<Model1D> first, second;
public:
	NumericConvolutionModel1D(std::shared_ptr<Model1D> first_, std::shared_ptr<Model1D> second_);

    void initModelParameters();

	double eval(double *x) const;

	void updateDomain();
};

#endif /* NUMERICCONVOLUTIONMODEL1D_H_ */
