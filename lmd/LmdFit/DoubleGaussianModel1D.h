/*
 * DoubleGaussianModel1D.h
 *
 *  Created on: Feb 19, 2013
 *      Author: steve
 */

#ifndef DOUBLEGAUSSIANMODEL1D_H_
#define DOUBLEGAUSSIANMODEL1D_H_

#include "Model1D.h"

class DoubleGaussianModel1D: public Model1D {
private:
	double num_sigmas;
    std::shared_ptr<ModelPar> gauss_mean;
	std::shared_ptr<ModelPar> gauss_sigma_1;
	std::shared_ptr<ModelPar> gauss_sigma_2;
	std::shared_ptr<ModelPar> gauss_ratio_12;
public:
	DoubleGaussianModel1D();
	virtual ~DoubleGaussianModel1D();

    void initModelParameters();

	double eval(double *x) const;

	void updateDomain();
};

#endif /* DOUBLEGAUSSIANMODEL1D_H_ */
