/*
 * SmearingConvolutionModel1D.h
 *
 *  Created on: Jul 9, 2013
 *      Author: steve
 */

#ifndef SMEARINGCONVOLUTIONMODEL1D_H_
#define SMEARINGCONVOLUTIONMODEL1D_H_

#include "Model1D.h"

class SmearingConvolutionModel1D: public Model1D {
private:
	unsigned int divisions;

	shared_ptr<Model1D> first, second;
public:
	SmearingConvolutionModel1D(std::string name_, shared_ptr<Model1D> first_,
			shared_ptr<Model1D> second_);
	virtual ~SmearingConvolutionModel1D();

	void initModelParameters();

	double eval(const double *x) const;

	void updateDomain();
};

#endif /* SMEARINGCONVOLUTIONMODEL1D_H_ */
