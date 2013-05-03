/*
 * GaussianModel1D.h
 *
 *  Created on: Dec 17, 2012
 *      Author: steve
 */

#ifndef GAUSSIANMODEL1D_H_
#define GAUSSIANMODEL1D_H_

#include "Model1D.h"

class GaussianModel1D: public Model1D {
private:
	double num_sigmas;
	shared_ptr<ModelPar> gauss_sigma;
	shared_ptr<ModelPar> gauss_mean;

public:
	/**
	 * The default constructor which will be indirectly used by the user through
	 * #PndLmdLumiModelROOT
	 * @param plab required for a full model description
	 * */
	GaussianModel1D();

	virtual ~GaussianModel1D();

	void initModelParameters();

	/**
	 * normalized detector response function for 1D fits. Here: 1D-gaussian
	 * @params x pointer to array containing theta value
	 * @returns value of the response function at the specified theta value with
	 * the given theta sigma
	 */
	double eval(double *x) const;

	void updateDomain();
};

#endif /* GAUSSIANMODEL1D_H_ */
